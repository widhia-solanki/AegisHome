#include "DoorController.h"

void DoorController::begin()
{
    doorServo_.attach(Config::PIN_SERVO_DOOR);

    doorServo_.write(Config::SERVO_ANGLE_CLOSED);

    isOpen_ = false;

    servoFault_ = ErrorCode::NONE;

    doorTimer_.reset();
}

void DoorController::update(const SensorSnapshot& snapshot)
{
    // Doorbell pressed -> open door
    if (snapshot.doorbellPressed)
    {
        doorServo_.write(Config::SERVO_ANGLE_OPEN);

        isOpen_ = true;

        doorTimer_.reset();
    }

    // Auto-close after dwell time
    if (isOpen_ && doorTimer_.isDue())
    {
        doorServo_.write(Config::SERVO_ANGLE_CLOSED);

        isOpen_ = false;
    }
}

bool DoorController::isOpen() const
{
    return isOpen_;
}

ErrorCode DoorController::getServoFault() const
{
    return servoFault_;
}
