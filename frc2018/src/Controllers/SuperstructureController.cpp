#include <Controllers/SuperstructureController.h>

const double MAX_ELEVATOR_HEIGHT = 2; // TODO CHANGE THIS

SuperstructureController::SuperstructureController(RobotModel *myRobot, ControlBoard *myHumanControl) {
	robot_ = myRobot;
	humanControl_ = myHumanControl;

	elevatorHeightCommand_ = new ElevatorHeightCommand(robot_);

	currState_ = kInit;
	nextState_ = kIdle;

	intakeMotorOutput_ = 0.7; // TODO test this
	outtakeMotorOutput_ = -0.7; // TODO test this
	elevatorOutput_ = 0.5; // TODO test this

	desiredElevatorHeight_ = 0.0;
}

void SuperstructureController::Reset() {
	currState_ = kInit;
	nextState_ = kInit;

	robot_->SetIntakeOutput(0.0);
	robot_->SetElevatorOutput(0.0);

	elevatorHeightCommand_->Reset();

	desiredElevatorHeight_ = 0.0;
}

void SuperstructureController::Update(double currTimeSec, double deltaTimeSec) {
	switch(currState_) {
	case kInit:
		nextState_ = kIdle;
		printf("kInit\n");
		 robot_->SetIntakeOutput(0.0);
		 robot_->SetElevatorOutput(0.0);
		 desiredElevatorHeight_ = 0.0;
		break;
	case kIdle:
		nextState_ = kIdle;

		if (humanControl_->GetIntakeDesired()) {
			 robot_->SetIntakeOutput(intakeMotorOutput_);
			if (robot_->GetCubeInIntake()) {
				humanControl_->SetIntakeDesired(false);
				robot_->SetIntakeOutput(0.0);
			}
			printf("intake desired \n");
		} else if (humanControl_->GetOuttakeDesired()) {
			robot_->SetIntakeOutput(outtakeMotorOutput_);
			printf("outtake desired \n");
		} else {
			robot_->SetIntakeOutput(0.0);
		}

		if (humanControl_->GetElevatorUpDesired()) {
			robot_->SetElevatorOutput(elevatorOutput_);
			printf("elevator up desired\n");
		} else if (humanControl_->GetElevatorDownDesired()) {
			robot_->SetElevatorOutput(-elevatorOutput_);
			printf("elevator down desired\n");
		} else {
			robot_->SetElevatorOutput(0.0);
		}

		if (humanControl_->GetElevatorHeightDesired()) {
			desiredElevatorHeight_ = (((humanControl_->GetElevatorHeightValue()) + 1) / 2) * MAX_ELEVATOR_HEIGHT;
			elevatorHeightCommand_->SetSetpoint(desiredElevatorHeight_);
			elevatorHeightCommand_->Init();
			nextState_ = kElevatorToHeight;
		}

		if (humanControl_->GetRampDesired()) {
			nextState_ = kRamp;
		}
		printf("in kIdle\n");
		break;
	case kElevatorToHeight:
		printf("in kElevatorToHeight\n");
		if (elevatorHeightCommand_->IsDone()) {
			elevatorHeightCommand_->Reset();
			humanControl_->SetElevatorHeightDesired(false);
			nextState_ = kIdle;
		} else {
			elevatorHeightCommand_->Update(currTimeSec, deltaTimeSec);
		}
		break;
	case kRamp:
		printf("in kRamp\n");
		nextState_ = kRamp;
		break;
	}
	currState_ = nextState_;
}

void RefreshIni() {

}

SuperstructureController::~SuperstructureController() {
	// TODO Auto-generated destructor stub
}

