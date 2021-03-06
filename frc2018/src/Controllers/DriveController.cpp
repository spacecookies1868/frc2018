#include <Controllers/DriveController.h>
#include "WPILib.h"

DriveController::DriveController(RobotModel *robot, ControlBoard *humanControl) {
	robot_ = robot;
	humanControl_ = humanControl;

	navXSource_ = new NavXPIDSource(robot_);
	talonEncoderSource_ = new TalonEncoderPIDSource(robot_);

// Set sensitivity to 0
	thrustSensitivity_ = 0.0;
	rotateSensitivity_ = 0.0;
	quickTurnSensitivity_ = 0.0;

	LOW_THRUST_SENSITIVITY= 0.3; // TODO tune this
	LOW_ROTATE_SENSITIVITY = 0.7; // TODO tune this
	MAX_ELEVATOR_HEIGHT_THRESHOLD = 5.0; // TODO tune this


// Default state Teleop Drive
	currState_ = kTeleopDrive;
	nextState_ = kTeleopDrive;

// If using align with cube
	alignWithCubeStarted_ = false;
	cubeCommand_ = NULL;


	isDone_ = false;

}

void DriveController::Reset() {
//	robot_->SetPercentVBusDriveMode(); check robotmodel

//	thrustSensitivity_ = robot_->pini_->getf("TELEOP DRIVING", "thrustSensitivity", 0.3);
//	rotateSensitivity_ = robot_->pini_->getf("TELEOP DRIVING", "rotateSensitivity", 0.5);
//	quickTurnSensitivity_ = robot_->pini_->getf("TELEOP DRIVING", "quickTurnSensitivity", 0.5);
//	check ini
}

void DriveController::Update(double currTimeSec, double deltaTimeSec) {
	PrintDriveValues();

	switch (currState_) {

	case (kTeleopDrive):
//		robot->SetPercentVBusDriveMode(); old talon stuff
		double leftJoyY, leftJoyZ, rightJoyY, rightJoyX, rightJoyZ;

// Takes joystick values
		leftJoyY = -humanControl_->GetJoystickValue(ControlBoard::kLeftJoy, ControlBoard::kY);	// was neg
		leftJoyZ = humanControl_->GetJoystickValue(ControlBoard::kLeftJoy, ControlBoard::kZ);
		rightJoyY = -humanControl_->GetJoystickValue(ControlBoard::kRightJoy, ControlBoard::kY);	// was neg
		rightJoyX = humanControl_->GetJoystickValue(ControlBoard::kRightJoy, ControlBoard::kX);
		rightJoyZ = humanControl_->GetJoystickValue(ControlBoard::kRightJoy, ControlBoard::kZ);

		// so leftJoyZ and rightJoyZ are from -1 to 1
		thrustSensitivity_ = (leftJoyZ + 1.0) / 2.0;
		rotateSensitivity_ = (rightJoyZ + 1.0) / 2.0;

		SmartDashboard::PutNumber("Thrust z", thrustSensitivity_);
		SmartDashboard::PutNumber("Rotate z", rotateSensitivity_);

// Change gear
		if (humanControl_->GetHighGearDesired()) {
			SmartDashboard::PutString("Gear", "High");
			robot_->SetHighGear();
		} else {
			SmartDashboard::PutString("Gear", "Low");
			robot_->SetLowGear();
		}

		if (humanControl_->GetAlignWithCubeDesired()) {
			printf("align with cube desired");
			nextState_ = kAlignWithCube;
		} else {
// Checks quickturn or arcade drive
		SmartDashboard::PutBoolean("Quick turn desired", humanControl_->GetQuickTurnDesired());
		if (humanControl_->GetQuickTurnDesired()) {
			QuickTurn(rightJoyX, 0.0);
			nextState_ = kTeleopDrive;
		} else {
			if (humanControl_->GetArcadeDriveDesired()) {
				ArcadeDrive(rightJoyX, leftJoyY, thrustSensitivity_, rotateSensitivity_);
			} else {
				TankDrive(leftJoyY, rightJoyY);
			}
			nextState_ = kTeleopDrive;
		}

		}
		break;

	case (kAlignWithCube):
		nextState_ = kAlignWithCube;
	    printf("IN K ALIGN WITH CUBE!!!!");
	    if (cubeCommand_ == NULL) {
	    	cubeCommand_ = new PivotToCubeCommand(robot_, navXSource_, talonEncoderSource_, false); //no drive straight
	    	printf("creating cube command without drive straight");
	    	alignWithCubeStarted_ = false;
	    } else if (!alignWithCubeStarted_) {
	    	cubeCommand_->Init();
	    	printf("initializing cube command");
	    	alignWithCubeStarted_ = true;
	    } else if (!cubeCommand_->IsDone()) {
	    	cubeCommand_->Update(currTimeSec, deltaTimeSec);
	    } else {
	    	alignWithCubeStarted_ = false;
	    	nextState_ = kTeleopDrive;
	    }

		break;
		}
	currState_ = nextState_;
}

void DriveController::ArcadeDrive(double myX, double myY, double thrustSensitivity, double rotateSensitivity) {
	SmartDashboard::PutString("Drive Mode", "Arcade Drive");

	double thrustValue = myY * GetDriveDirection();
	double rotateValue = myX;
//	double rotateValue = myX * GetDriveDirection(); // TODO fix if you want chloemode
	double leftOutput = 0.0;
	double rightOutput = 0.0;

	// Account for small joystick jostles (deadband)
	thrustValue = HandleDeadband(thrustValue, 0.1);	// 0.02 was too low
	rotateValue = HandleDeadband(rotateValue, 0.06);

	// Sensitivity adjustment
	rotateValue = GetCubicAdjustment(rotateValue, rotateSensitivity);
	rotateValue *= fabs(thrustValue);

	thrustValue = GetCubicAdjustment(thrustValue, thrustSensitivity);

	leftOutput = thrustValue + rotateValue;			// CHECK FOR COMP BOT
	rightOutput = thrustValue - rotateValue;

	// Make sure, output values are within range
	if (leftOutput > 1.0) {
		leftOutput = 1.0;
	} else if (rightOutput > 1.0) {
		rightOutput = 1.0;
	} else if (leftOutput < -1.0) {
		leftOutput = -1.0;
	} else if (rightOutput < -1.0) {
		rightOutput = -1.0;
	}

	robot_->SetDriveValues(RobotModel::kLeftWheels, leftOutput);
	robot_->SetDriveValues(RobotModel::kRightWheels, rightOutput);

	SmartDashboard::PutNumber("Left motor output", leftOutput );
	SmartDashboard::PutNumber("Right motor output", rightOutput);
}

void DriveController::TankDrive(double myLeft, double myRight) {
	SmartDashboard::PutString("Drive Mode", "Tank Drive");
	double leftOutput = myLeft * GetDriveDirection();
	double rightOutput = myRight * GetDriveDirection();

	robot_->SetDriveValues(RobotModel::kLeftWheels, leftOutput);
	robot_->SetDriveValues(RobotModel::kRightWheels, rightOutput);
}

void DriveController::QuickTurn(double myRight, double turnConstant) {
	SmartDashboard::PutString("Drive Mode", "Quick Turn");

	double rotateValue = GetCubicAdjustment(myRight, turnConstant);

	robot_->SetDriveValues(RobotModel::kLeftWheels, rotateValue);		// CHECK FOR COMP BOT
	robot_->SetDriveValues(RobotModel::kRightWheels, -rotateValue);		// CHECK FOR COMP BOT
}

int DriveController::GetDriveDirection() {
	if (humanControl_->GetReverseDriveDesired()) {
		return -1;
	} else {
		return 1;
	}
}

double DriveController::HandleDeadband(double value, double deadband) {
	if (fabs(value) < deadband) {
		return 0.0;
	} else {
		return value;
	}
}

// Rotation sensitivity: when z == 0 same output; when z==1, output^3
double DriveController::GetCubicAdjustment(double value, double adjustmentConstant) {
	return adjustmentConstant * std::pow(value, 3.0) + (1 - adjustmentConstant) * value;
}

bool DriveController::IsDone() {
	return isDone_;
}

void DriveController::PrintDriveValues() {
	SmartDashboard::PutNumber("Drive direction", GetDriveDirection());
	SmartDashboard::PutNumber("Get state", currState_);
	SmartDashboard::PutNumber("NavX angle", robot_->GetNavXYaw());
	SmartDashboard::PutNumber("Left drive distance", robot_->GetLeftDistance());
	SmartDashboard::PutNumber("Right drive distance", robot_->GetRightDistance());
//	SmartDashboard::PutNumber("Left drive encoder value", robot_->leftMaster_->GetEncPosition());
//	SmartDashboard::PutNumber("Right drive encoder value", robot_->rightMaster_->GetEncPosition());
	SmartDashboard::PutNumber("Left drive encoder value", robot_->GetLeftEncoderValue());
	SmartDashboard::PutNumber("Right drive encoder value", robot_->GetRightEncoderValue());
}

DriveController::~DriveController() {
}
