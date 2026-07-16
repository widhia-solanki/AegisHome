#include "SystemStateManager.h"

void SystemStateManager::begin()
{
    state_ = SystemState::BOOT;
}

SystemState SystemStateManager::current() const
{
    return state_;
}

bool SystemStateManager::requestTransition(SystemState next)
{
    if (state_ == next)
        return true;

    switch (state_)
    {
        case SystemState::BOOT:

            if (next == SystemState::NORMAL)
            {
                state_ = next;
                return true;
            }

            break;

        case SystemState::NORMAL:

            if (next == SystemState::SECURITY_ARMED ||
                next == SystemState::ALARM)
            {
                state_ = next;
                return true;
            }

            break;

        case SystemState::SECURITY_ARMED:

            if (next == SystemState::NORMAL ||
                next == SystemState::ALARM)
            {
                state_ = next;
                return true;
            }

            break;

        case SystemState::ALARM:

            if (next == SystemState::NORMAL)
            {
                state_ = next;
                return true;
            }

            break;
    }

    return false;
}
