#ifndef SRC_AUTO_COMMANDS_PIVOTCOMMAND_H_
#define SRC_AUTO_COMMANDS_PIVOTCOMMAND_H_

#include "WPILib.h"
#include <AHRS.h>
#include "AutoCommand.h"
#include "Auto/PIDSource/PIDInputSource.h"
#include "Auto/PIDSource/PIDOutputSource.h"
#include "RobotModel.h"
#include "Profiler.h"

/**
 * A class implementing Pivot PID the WPILibrary PID Controller
 */
class PivotCommand : public AutoCommand {

public:
	/**
	 * PivotCommand a constructor
	 * @param robot a RobotModel
	 * @param desiredAngle a double that is the angle of the turn
	 * @param isAbsolutePosition a bool that represents whether the angle is absolute position of deta angle
	 * @param navXSource a NavXPIDSource
	 */
	PivotCommand(RobotModel *robot, double desiredAngle, bool isAbsolutePosition, NavXPIDSource* navXSource);

	/**
	 * PivotCommand a destructor
	 */
	virtual ~PivotCommand();

	/**
	 * gets Yaw from navX, sets Setpoint, continuous to false, output range, and absolute tolerance
	 */
	void Init();

	/**
	 * resets PID, sets isDone_ to true
	 */
	void Reset();

	/**
	 * if PivotPID is on target more than three times then timeout
	 * pivotPID resets, disable, isDone sets to true
	 */
	void Update(double currTimeSec, double deltaTimeSec);

	/**
	 * @return isDone_
	 */
	bool IsDone();

	/**
	 * gets PID values from ini file, sets to 0 if not present
	 */
	void GetIniValues();

private:

	double pFac_, iFac_, dFac_;
	double desiredAngle_;
	double initYaw_;

	bool isDone_;

	int numTimesOnTarget_;

	RobotModel *robot_;
	PIDController *pivotPID_;
	NavXPIDSource * navXSource_;
	PivotPIDTalonOutput *talonOutput_;

	double maxOutput_;
	double tolerance_;

	/**
	 * Minimum output to correct for, less would be considered done
	 */
	double minDrivePivotOutput_;

	double pivotCommandStartTime_;

	double pivotTimeoutSec_, actualTimeoutSec_;
};

#endif /* SRC_AUTO_COMMANDS_PIVOTCOMMAND_H_ */
