#include "Auto/Commands/PivotCommand.h"
#include "WPILib.h"

PivotCommand::PivotCommand(RobotModel *robot, double desiredAngle, bool isAbsoluteAngle, NavXPIDSource* navXSource) {
	navXSource_ = navXSource;

	initYaw_ = navXSource_->PIDGet();

	if (isAbsoluteAngle){
		desiredAngle_ = desiredAngle;
	} else {
		desiredAngle_ = initYaw_ + desiredAngle;
		if (desiredAngle_ > 180) {
			desiredAngle_ -= -360;
		} else if (desiredAngle_ < -180) {
			desiredAngle_ += 360;
		}
	}

	isDone_ = false;
	robot_ = robot;
	talonOutput_ = new PivotPIDTalonOutput(robot_);

	pivotCommandStartTime_ = robot_->GetTime();
	pivotTimeoutSec_ = 0.0;
	GetIniValues();
	pivotPID_ = new PIDController(pFac_, iFac_, dFac_, navXSource_, talonOutput_);

	maxOutput_ = 0.9;
	tolerance_ = 1.0;

	numTimesOnTarget_ = 0;

}

void PivotCommand::Init() {
	//Profiler profiler(robot_, "Pivot Init");
	// Setting PID values (in case they changed)
	GetIniValues();
	pivotPID_->SetPID(pFac_, iFac_, dFac_);

	initYaw_ = navXSource_->PIDGet();

	pivotPID_->SetSetpoint(desiredAngle_);
	pivotPID_->SetContinuous(true);
	pivotPID_->SetInputRange(-180, 180);
	pivotPID_->SetOutputRange(-maxOutput_, maxOutput_);     //adjust for 2018
	pivotPID_->SetAbsoluteTolerance(tolerance_);	 //adjust for 2018
	pivotPID_->Enable();

	printf("Initial NavX Angle: %f\n", initYaw_);
	printf("Desired NavX Angle: %f\n", initYaw_ + desiredAngle_);

	isDone_ = false;
	numTimesOnTarget_ = 0;
	pivotCommandStartTime_ = robot_->GetTime();
}

void PivotCommand::Reset() {
	printf("Disabling pivotcommand\n");
	pivotPID_->Reset();
	isDone_ = true;
	printf("DONE FROM RESET \n");
}

void PivotCommand::Update(double currTimeSec, double deltaTimeSec) {
	SmartDashboard::PutNumber("Pivot Error", pivotPID_->GetError());
	SmartDashboard::PutNumber("Setpoint", pivotPID_->GetSetpoint());
	SmartDashboard::PutNumber("Delta setpoint", pivotPID_->GetDeltaSetpoint());

	double timeDiff = robot_->GetTime() - pivotCommandStartTime_;
	bool timeOut = (timeDiff > pivotTimeoutSec_);								//test this value

	SmartDashboard::PutNumber("Pivot time diff", timeDiff);

	if (pivotPID_->OnTarget()) {
		numTimesOnTarget_++;
	} else {
		numTimesOnTarget_ = 0;
	}
	if ((pivotPID_->OnTarget() && numTimesOnTarget_ > 3) || timeOut) {
		printf("Final NavX Angle: %f\n", navXSource_->PIDGet());
		printf("Angle NavX Error %f\n", pivotPID_->GetError());
		pivotPID_->Reset();
		isDone_ = true;
		robot_->SetDriveValues(RobotModel::kLeftWheels, 0.0);
		robot_->SetDriveValues(RobotModel::kRightWheels, 0.0);
		printf("PIVOT IS DONE \n");
		if (timeOut) {
			printf("FROM TIME OUT\n");
		}
	} else {
		double output = talonOutput_->GetOutput();
//		double output = 0.0;
		robot_->SetDriveValues(RobotModel::kLeftWheels, output);
		robot_->SetDriveValues(RobotModel::kRightWheels, -output);

//		output = talonOutput_->GetOutput();
		SmartDashboard::PutNumber("left output", output);
		SmartDashboard::PutNumber("right output", -output);
	}
}

bool PivotCommand::IsDone() {
	return isDone_;
}

void PivotCommand::GetIniValues() {

	pFac_ = robot_->pivotPFac_;
	iFac_ = robot_->pivotIFac_;
	dFac_ = robot_->pivotDFac_;
	pivotTimeoutSec_ = robot_->pivotTimeoutSec_;
//	minDrivePivotOutput_ = robot_->pini_->getf("PIVOT PID", "minDrivePivotOutput", 0.0);
	printf("PIVOT COMMAND p: %f, i: %f, d: %f\n", pFac_, iFac_, dFac_);
}

PivotCommand::~PivotCommand() {
	Reset();
	printf("IS DONE FROM DECONSTRUCTOR\n");
}
