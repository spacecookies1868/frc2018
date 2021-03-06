#ifndef SRC_CONTROLLERS_DRIVECONTROLLER_H_
#define SRC_CONTROLLERS_DRIVECONTROLLER_H_

#include "WPILib.h"
#include "RobotModel.h"
#include "DriverStation/ControlBoard.h"
#include "Auto/PIDSource/PIDInputSource.h"
#include "Auto/PIDSource/PIDOutputSource.h"
#include "Auto/Commands/PivotToCubeCommand.h"

class DriveController {
public:
	/**
	 * Initializes all variables
	 * takes in RobotModel and ControlBoard
	 */
	DriveController(RobotModel *robot, ControlBoard *humanControl);

	/**
	 * Destructor
	 */
	~DriveController();

	void Reset();

	void Update(double currTimeSec, double deltaTimeSec);

	/**
	 * Checks if align with cube command is done (if we use)
	 */
	bool IsDone();

	/**
	 * Prints direction, state, angle, etc.
	 */
	void PrintDriveValues();

	enum DriveState {
		kTeleopDrive, kAlignWithCube
	};
private:
	/**
	 * Drives robot in Arcade
	 */
	void ArcadeDrive(double myX, double myY, double thrustSensitivity, double rotateSensitivity);

	/**
	 * Drives robot in Tank
	 */
	void TankDrive(double myLeft, double myRight);

	/**
	 * Quick Turn
	 */
	void QuickTurn(double myRight, double turnConstant);

	/**
	 * Returns -1 for reverse drive, 1 otherwise
	 */
	int GetDriveDirection();

	/**
	 * Adjusts joystick value if too small
	 */
	double HandleDeadband(double value, double deadband);

	/**
	 * Adjusts sensitivity for turn
	 */
	double GetCubicAdjustment(double value, double adjustmentConstant);

	RobotModel *robot_;
	ControlBoard *humanControl_;

	uint32_t currState_;
	uint32_t nextState_;

	double thrustSensitivity_, rotateSensitivity_, quickTurnSensitivity_;
	double LOW_THRUST_SENSITIVITY;
	double LOW_ROTATE_SENSITIVITY;
	double MAX_ELEVATOR_HEIGHT_THRESHOLD;

	NavXPIDSource *navXSource_;
	TalonEncoderPIDSource *talonEncoderSource_;
	PivotToCubeCommand *cubeCommand_;
	bool alignWithCubeStarted_;

	bool isDone_;
};

#endif /* SRC_CONTROLLERS_DRIVECONTROLLER_H_ */
