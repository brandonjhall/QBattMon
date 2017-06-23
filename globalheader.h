#ifndef POWER_DIR
#define POWER_DIR "/sys/class/power_supply"
#endif

#ifndef BATTERY_MESSAGES
#define BATTERY_MESSAGES

enum class BatteryStatus {
    Full,
    Charging,
    Discharging
};

enum class BatteryLevel {
    Full,
    Normal,
    Low
};

enum class BatteryError {
    NoBattery,
    NoError
};

#endif
