#include "AlarmManager.h"

void AlarmManager::begin()
{
    pinMode(Config::PIN_BUZZER, OUTPUT);
    pinMode(Config::PIN_LED_INDICATOR, OUTPUT);

    digitalWrite(Config::PIN_BUZZER, LOW);
    digitalWrite(Config::PIN_LED_INDICATOR, LOW);

    patternStep_ = 0;
}

void AlarmManager::update(SystemState currentState)
{
    if (currentState != SystemState::ALARM)
    {
        digitalWrite(Config::PIN_BUZZER, LOW);
        digitalWrite(Config::PIN_LED_INDICATOR, LOW);

        patternStep_ = 0;
        return;
    }

    if (!patternTimer_.isDue())
        return;

    patternStep_ ^= 1;

    digitalWrite(Config::PIN_BUZZER, patternStep_);
    digitalWrite(Config::PIN_LED_INDICATOR, patternStep_);
}
