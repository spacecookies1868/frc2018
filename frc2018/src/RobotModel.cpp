#include <WPILib.h>
#include <RobotModel.h>

const double WHEEL_DIAMETER = 6.0 / 12.0; 			// TODO CHANGE
const double ENCODER_COUNT_PER_ROTATION = 256.0;
const int EDGES_PER_ENCODER_COUNT = 4;

RobotModel::RobotModel() {
	// Initializing ini
	pini_ = new Ini("home/lvuser/robot.ini");
	// Initialzing Pivot PID vals
	pivotPFac_ = 0.0;
	pivotIFac_ = 0.0;
	pivotDFac_ = 0.0;
	// Initializing DriveStraight PID vals
	driveDPFac_ = 0.0;
	driveDIFac_ = 0.0;
	driveDDFac_ = 0.0;
	driveRPFac_ = 0.0;
	driveRIFac_ = 0.0;
	driveRDFac_ = 0.0;

	// Initializing timer
	timer_= new Timer();
	timer_->Start();

	// Initializing pdp
	pdp_ = new PowerDistributionPanel();

	// Initializing Encoders
	leftDriveEncoder_ = new Encoder(LEFT_DRIVE_ENCODER_A_PWM_PORT, LEFT_DRIVE_ENCODER_B_PWM_PORT, true);		// TODO check if true or false
	leftDriveEncoder_->SetDistancePerPulse(((WHEEL_DIAMETER) * M_PI) / ENCODER_COUNT_PER_ROTATION);

	rightDriveEncoder_ = new Encoder(RIGHT_DRIVE_ENCODER_A_PWM_PORT, RIGHT_DRIVE_ENCODER_B_PWM_PORT, false);
	rightDriveEncoder_->SetDistancePerPulse(((WHEEL_DIAMETER) * M_PI) / ENCODER_COUNT_PER_ROTATION);

	// Initializing Drive Talons
	isLeftInverted_ = false;
	leftMaster_ = new WPI_TalonSRX(LEFT_DRIVE_MASTER_ID);
	rightMaster_ = new WPI_TalonSRX(RIGHT_DRIVE_MASTER_ID);
	leftSlave_ = new WPI_TalonSRX(LEFT_DRIVE_SLAVE_ID);
	rightSlave_ = new WPI_TalonSRX(RIGHT_DRIVE_SLAVE_ID);

	rightMaster_->SetInverted(!isLeftInverted_);
	rightSlave_->SetInverted(!isLeftInverted_);
	leftMaster_->SetInverted(isLeftInverted_);
	leftSlave_->SetInverted(isLeftInverted_);

	leftMaster_->Set(ControlMode::PercentOutput, 0.0);
	rightMaster_->Set(ControlMode::PercentOutput, 0.0);
	leftSlave_->Set(ControlMode::Follower, LEFT_DRIVE_MASTER_ID);
	rightSlave_->Set(ControlMode::Follower, RIGHT_DRIVE_MASTER_ID);

	// Initializing NavX
	navX_ = new AHRS(SPI::kMXP);
	Wait(1.0); // NavX takes a second to calibrate

	// Initializing pneumatics
	compressor_ = new Compressor(PNEUMATICS_CONTROL_MODULE_ID);
//	gearShiftSolenoid_ = new DoubleSolenoid(GEAR_SHIFT_FORWARD_SOLENOID_PORT, GEAR_SHIFT_REVERSE_SOLENOID_PORT);
}

void RobotModel::ResetTimer() {
	timer_->Reset();
}

double RobotModel::GetTime() {
	return timer_->Get();
}

void RobotModel::SetDriveValues(RobotModel::Wheels wheel, double value) {
	switch(wheel) {
		case (kLeftWheels):
			leftMaster_->Set(value);
			break;
		case (kRightWheels):
			rightMaster_->Set(value);
			break;
		case (kAllWheels):
			rightMaster_->Set(value);
			leftMaster_->Set(value);
	}
}

void RobotModel::SetHighGear() {
//	gearShiftSolenoid_->Set(DoubleSolenoid::kForward); // TODO Check if right
}

void RobotModel::SetLowGear() {
//	gearShiftSolenoid_->Set(DoubleSolenoid::kReverse); // TODO Check if right
}

double RobotModel::GetDriveEncoderValue(RobotModel::Wheels wheel) { // TODO check if encoders are working
	switch(wheel) {
		case (kLeftWheels):
				return leftDriveEncoder_->Get();
		case (kRightWheels):
				return rightDriveEncoder_->Get();
		case (kAllWheels):
				return 0;
		default:
			return 0;
	}
}

double RobotModel::GetLeftDistance() {
	return leftDriveEncoder_->GetDistance();
}

double RobotModel::GetRightDistance() {
	return rightDriveEncoder_->GetDistance();
}

double RobotModel::GetNavXYaw() {
	return navX_->GetYaw();
}

void RobotModel::ZeroNavXYaw() {
	navX_->ZeroYaw();
}

void RobotModel::RefreshIni() {
	delete pini_;
	const char* usbPath = "insert path here"; // TODO fix
	if(FILE *file = fopen(usbPath, "r")) {
		fclose(file);
		pini_ = new Ini(usbPath);
	} else {
		pini_ = new Ini("/home/lvuser/robot.ini");
	}

	RefreshIniVals();
}

void RobotModel::RefreshIniVals() {
	printf("Refreshing ini vals\n");
	pivotPFac_ = pini_->getf("PIVOT PID", "pFac", 0.0);
	pivotIFac_ = pini_->getf("PIVOT PID", "iFac", 0.0);
	pivotDFac_ = pini_->getf("PIVOT PID", "dFac", 0.0);

	driveDPFac_ = pini_->getf("DRIVESTRAIGHT PID", "dPFac", 0.0);
	driveDIFac_ = pini_->getf("DRIVESTRAIGHT PID", "dIFac", 0.0);
	driveDDFac_ = pini_->getf("DRIVESTRAIGHT PID", "dDFac", 0.0);
	driveRPFac_ = pini_->getf("DRIVESTRAIGHT PID", "rPFac", 0.0);
	driveRIFac_ = pini_->getf("DRIVESTRAIGHT PID", "rIFac", 0.0);
	driveRDFac_ = pini_->getf("DRIVESTRAIGHT PID", "rDFac", 0.0);

	printf("Refreshed ini vals succesfully\n");
}

RobotModel::~RobotModel() {
	// TODO Auto-generated destructor stub
}

