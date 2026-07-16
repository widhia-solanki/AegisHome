#include "SecurityManager.h"

void SecurityManager::begin()
{
    armed_ = false;
    armRejected_ = false;
}

void SecurityManager::update(
    const SensorSnapshot& snapshot,
    SystemStateManager& stateManager)
{
    armRejected_ = false;

    if (snapshot.securityButtonPressed)
    {
        if (snapshot.irFault != ErrorCode::NONE)
        {
            armRejected_ = true;
        }
        else
        {
            armed_ = !armed_;

            stateManager.requestTransition(
                armed_
                ? SystemState::SECURITY_ARMED
                : SystemState::NORMAL
            );
        }
    }

    if (armed_ && snapshot.motionDetected)
    {
        stateManager.requestTransition(
            SystemState::ALARM
        );
    }
}

bool SecurityManager::isArmed() const
{
    return armed_;
}

bool SecurityManager::lastArmRequestRejected() const
{
    return armRejected_;
}