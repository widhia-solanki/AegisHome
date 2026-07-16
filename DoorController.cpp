#include "DoorController.h"

void DoorController::begin()
{
    doorServo_.attach(Config::PIN_SERVO_DOOR);

    doorServo_.write(Config::SERVO_ANGLE_CLOSED);

    isOpen_ = false;

    servoFault_ = ErrorCode::NONE;
}

void DoorController::update(const SensorSnapshot& snapshot) {
  // TODO: implement — on snapshot.doorbellPressed, open + flash welcome
  // LED, start dwell timer; on dwell timer expiry, return to closed
}

bool DoorController::isOpen() const
{
    return isOpen_;
}

ErrorCode DoorController::getServoFault() const
{
    return servoFault_;
}
