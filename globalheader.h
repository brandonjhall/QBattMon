/***************************************************************************
 *   Copyright 2017 by Brandon Hall                                        *
 *   brandon.james.hall2@gmail.com                                         *
 *                                                                         *
 *   This file is part of QBattMon                                         *
 *   QBattMon is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   QBattMon is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with QBattMon. If not, see <http://www.gnu.org/licenses/>.      *
 **************************************************************************/

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

enum class MessageType {
    BrightnessUp,
    BrightnessDown,
    BrightnessSet
};

struct LocalMSG
{
    LocalMSG() {}
    LocalMSG(MessageType mType, double percent);
    void setVersion(QString mVersion) {version = mVersion;}

    MessageType type;
    QString version;
    double percentOfBrightness;
};

QDataStream &operator<<(QDataStream &out, const LocalMSG &message);
QDataStream &operator>>(QDataStream &in, LocalMSG &message);


#endif
