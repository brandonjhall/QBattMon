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

class QStandardItemModel;
class QTimer;
class QTime;
class QFile;

class Battery : public QObject
{
    Q_OBJECT
public:
    Battery(int battery, QObject *parent = nullptr);
    explicit Battery(QObject *parent = nullptr);

    QStandardItemModel *model() const;

    QString currentEnergy() const;
    QString currentPower() const;

    BatteryStatus getStatus() const;
    int batteryNumber() const;
    QTime *timeLeft() const;
    QString level() const;

    int batteryCapacityNumber() const;

signals:
    void batteryError(QString error, BatteryError batteryError);
    void batteryStatusChanged(BatteryStatus status);
    void batteryCapacityChanged(int m_level);
    void batteryNumberChanged(int number);
    void filesUpdated();

public slots:
    void setBatteryNumber(int value);

private:
    QTime *calculateTimeRemaining(int energy, int usage);
    void setCurrentEnergy(const QString &value);
    void setCurrentPower(const QString &value);
    void setStatus(const BatteryStatus &value);
    void setLevel(const QString &value);
    void setBatteryCapacity(int value);
    void calculateTimeRemaining();
    void initializeVariables();
    void createModel();

    int m_batteryCapacityNumber;
    int m_batteryNumber;
    int m_maxEnergy;
    int energy;
    int usage;

    QString m_currentEnergy;
    QString m_currentPower;
    QString m_level;

    QFile *batteryCurrentEnergy;
    QFile *batteryCurrentPower;
    QFile *batteryCapacity;
    QFile *batteryStatus;
    QFile *batteryLevel;

    QStandardItemModel *m_model;

    BatteryError lastError = BatteryError::NoError;
    QStringList batteryFolders;
    BatteryStatus status;
    QTimer *updateTimer;
    QTime *m_timeLeft;

private slots:
    void updateFiles();
};

#endif // BATTERY_H
