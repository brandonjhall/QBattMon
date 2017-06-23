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

#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>

#include "globalheader.h"

class QTime;
class QTimer;
class QFile;
class QStandardItemModel;

class Battery : public QObject
{
    Q_OBJECT
public:
    explicit Battery(QObject *parent = nullptr);
    Battery(int battery, QObject *parent = nullptr);

    int getBatteryNumber() const;
    BatteryStatus getStatus() const;
    QStandardItemModel *getModel() const;
    QString getLevel() const;
    QString getCurrentPower() const;
    QString getCurrentEnergy() const;
    QTime *getTimeLeft() const;

signals:
    void batteryError(QString error, BatteryError batteryError);
    void batteryNumberChanged(int number);
    void batteryCapacityChanged(int level);
    void batteryStatusChanged(BatteryStatus status);
    void filesUpdated();

public slots:
    void setBatteryNumber(int value);

private:
    void calculateTimeRemaining();
    QTime *calculateTimeRemaining(int energy, int usage);
    void initializeVariables();
    void setBatteryCapacity(int value);
    void setStatus(const BatteryStatus &value);
    void setLevel(const QString &value);
    void setCurrentEnergy(const QString &value);
    void setCurrentPower(const QString &value);
    void createModel();

    int batteryNumber;
    int batteryCapacityNumber;
    int maxEnergy;
    int energy;
    int usage;
    QString level;
    QString currentPower;
    QString currentEnergy;
    QFile *batteryCapacity;
    QFile *batteryLevel;
    QFile *batteryStatus;
    QFile *batteryCurrentPower;
    QFile *batteryCurrentEnergy;
    QTime *timeLeft;
    QTimer *updateTimer;
    QStringList batteryFolders;
    BatteryError lastError = BatteryError::NoError;
    BatteryStatus status;
    QStandardItemModel *model;

private slots:
    void updateFiles();
};

#endif // BATTERY_H
