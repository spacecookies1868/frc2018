#ifndef SRC_DRIVERSTATION_BUTTONREADER_H_
#define SRC_DRIVERSTATION_BUTTONREADER_H_

#include <WPILib.h>

//This file outlines classes that read the states of buttons.
//ButtonReader reads the states of push buttons
class ButtonReader {
public:
	/**
	 * ButtonReader constructor to set joystick, button, get currState, set lastState to currState
	 * @param joy a Joystick
	 */
	ButtonReader(Joystick *joy, int buttonNum);

	/**
	 * Destructor
	 */
	virtual ~ButtonReader();

	/**
	 * Updates lastState and current state, reads the value of the button
	 */
	void ReadValue();

	/**
	 * @return current state of button (whether its currently pressed)
	 */
	bool IsDown();

	/**
	 * @return true if button changed from released to pressed
	 */
	bool WasJustPressed();

	/**
	 * @return true if button changed from pressed to released
	 */
	bool WasJustReleased();

	/**
	 * @return true if state was changed from pressed to released (or vice versa)
	 */
	bool StateJustChanged();

private:
	Joystick *joystick;
	int buttonNum;
	bool lastState;
	bool currState;
};

//ToggleButtonReader reads the states of toggles
class ToggleButtonReader : public ButtonReader {
public:
	/**
	 * ToggleButtonReader constructor to initialize currToggleState
	 * @param joy a Joystick
	 */
	ToggleButtonReader(Joystick *joy, int buttonNum);

	/**
	 * Destructor
	 */
	virtual ~ToggleButtonReader();

	virtual bool GetState();

private:
	bool currToggleState;
};

enum SwitchState {
	kUp = 1,
	kNeutral = 0,
	kDown = -1,
};

//SwitchReaader reads the state of switches
class SwitchReader {
public:
	/**
	 * SwitchReader constructor initializes which joystick the switch is part of
	 * @param myJoy a Joystick
	 * @param upButton an int
	 * @param downButton an int
	 */
	SwitchReader(Joystick *myJoy, int upButton, int downButton);

	/**
	 * @return the state of the switch (up or down)
	 */
	SwitchState GetSwitchState();

private:
	Joystick *joy;
	int upB;
	int downB;
};

#endif /* SRC_DRIVERSTATION_BUTTONREADER_H_ */
