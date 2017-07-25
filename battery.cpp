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

#include <QStandardItemModel>
#include <QTimer>
#include <QTime>

#include "battery.h"
extern "C" {
#include <udevhelper.h>

namespace battudev {
int numbatteries;
BatteryStruct **batteries;
BatteryStruct *currentBattery;
}
}

Battery::Battery(QObject *parent) : QObject(parent)
{
    initializeVariables();
}

Battery::Battery(int battery, QObject *parent) : QObject(parent)
{
    initializeVariables();

    setBatteryNumber(battery);
}

int Battery::batteryNumber() const
{
    return m_batteryNumber;
}

void Battery::setBatteryNumber(int value)
{
    m_batteryNumber = value;

    createModel();
    updateUdev();

    emit batteryNumberChanged(value);
}

void Battery::calculateTimeRemaining()
{
    m_timeLeft = calculateTimeRemaining(energy, usage);
    QStandardItem *item = m_model->item(0, 12);

    if (item != Q_NULLPTR)
        item->setText(m_timeLeft->toString());
}

QTime *Battery::calculateTimeRemaining(int energy, int usage)
{
    if (usage == 0)
        return m_timeLeft;

    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    if (status == BatteryStatus::Discharging) {
        hours = energy / usage;
        minutes = energy % usage;
        seconds = minutes % 60;
        minutes = minutes / 60;
    } else if(status == BatteryStatus::Charging) {
        int chargeNeeded = m_maxEnergy - energy;
        hours = chargeNeeded / usage;
        minutes = chargeNeeded % usage;
        seconds = minutes % 60;
        minutes = minutes / 60;
    }

    return new QTime(hours, minutes, seconds);
}

void Battery::initializeVariables()
{
    int maxbatteries = 5;
    battudev::batteries = initializeHelper(&maxbatteries);
    battudev::numbatteries = maxbatteries;

    if(maxbatteries > 0) {
        updateTimer = new QTimer;

        battudev::currentBattery = battudev::batteries[0];
        connect(updateTimer, &QTimer::timeout, this, &Battery::updateUdev);
    }
    else {
        lastError = BatteryError::NoBattery;
        emit batteryError("No batteries found", lastError);
        return;
    }

    updateTimer->setInterval(1000);
    updateTimer->start();
}

void Battery::setBatteryCapacity(int value)
{
    QString capacityString = QString::number(value) + "%";
    QStandardItem *item = m_model->item(0, 5);
    m_batteryCapacityNumber = value;

    item->setText(capacityString);
    emit batteryCapacityChanged(value);
}

BatteryStatus Battery::getStatus() const
{
    return status;
}

void Battery::setStatus(const BatteryStatus &value)
{
    QStandardItem *item = m_model->item(0, 4);
    QString statusStr;
    status = value;

    switch (value) {
    case BatteryStatus::Full:
        statusStr = "Full";
        break;
    case BatteryStatus::Charging:
        statusStr = "Charging";
        break;
    case BatteryStatus::Discharging:
        statusStr = "Discharging";
        break;
    default:
        break;
    }

    updateUdev();

    item->setText(statusStr);
    item->setData(static_cast<int>(status), Qt::UserRole);
    emit batteryStatusChanged(value);
}

void Battery::createModel()
{
    QStringList files = QStringList() << "technology"
                                      << "type"
                                      << "manufacturer"
                                      << "status"
                                      << "capacity"
                                      << "capacity_level"
                                      << "energy_full"
                                      << "power_now"
                                      << "energy_now"
                                      << "model_name"
                                      << "serial_number";

    QStringList headers = QStringList() << "Technology"
                                        << "Type"
                                        << "Manufacturer"
                                        << "Status"
                                        << "Battery Level"
                                        << "Capacity Level"
                                        << "Maximum Energy"
                                        << "Current Power"
                                        << "Current Energy"
                                        << "Model Name"
                                        << "Serial Number"
                                        << "Time Remaining";

    m_model = new QStandardItemModel;

    m_model->setHorizontalHeaderLabels(headers);

    m_maxEnergy = battudev::currentBattery->energyfull / 10000;
    m_currentEnergy = battudev::currentBattery->energynow / 10000;
    m_currentPower = battudev::currentBattery->powernow / 10000;

    for(int i = 0; i < battudev::numbatteries; i++) {
        QList<QStandardItem*> row;
        BatteryStruct *battery = battudev::batteries[i];

        row.append(new QStandardItem(battery->name));
        row.append(new QStandardItem(battery->technology));
        row.append(new QStandardItem("Battery"));
        row.append(new QStandardItem(battery->manufacturer));
        row.append(new QStandardItem(battery->status));
        row.append(new QStandardItem(battery->level));
        row.append(new QStandardItem(QString::number(battery->capacity) + "%"));
        row.append(new QStandardItem(QString::number(m_maxEnergy) + " mAh"));
        row.append(new QStandardItem(QString::number(battery->powernow)));
        row.append(new QStandardItem(QString::number(battery->energynow)));
        row.append(new QStandardItem(battery->modelname));
        row.append(new QStandardItem(QString::number(battery->serialnumber)));
        row.append(new QStandardItem(""));

        m_model->insertRow(m_model->rowCount(), row);
    }
}

int Battery::batteryCapacityNumber() const
{
    return m_batteryCapacityNumber;
}

QTime *Battery::timeLeft() const
{
    return m_timeLeft;
}

QString Battery::currentEnergy() const
{
    return m_currentEnergy;
}

void Battery::setCurrentEnergy(const QString &value)
{
    bool ok = false;
    int availableEnergy = value.toInt(&ok);

    QStandardItem *item = m_model->item(0, 9);

    if (ok) {
        availableEnergy = availableEnergy / 10000;
        m_currentEnergy = QString("%1 mAh").arg(availableEnergy);
    }

    item->setText(m_currentEnergy);
    energy = availableEnergy;
}

QString Battery::currentPower() const
{
    return m_currentPower;
}

void Battery::setCurrentPower(const QString &value)
{
    bool ok = false;
    int energy = value.toInt(&ok);

    QStandardItem *item = m_model->item(0, 8);

    if (ok) {
        energy = energy / 10000;
        m_currentPower = QString("%1 mAh").arg(energy);
    }

    item->setText(m_currentPower);
    usage = energy;
}

QString Battery::level() const
{
    return m_level;
}

void Battery::setLevel(const QString &value)
{
    BatteryLevel batteryLevel = BatteryLevel::Normal;
    QStandardItem *item = m_model->item(0, 6);
    m_level = value;

    m_level = m_level.replace("\n", "");
    item->setText(m_level);

    if (m_level == "Full")
        batteryLevel = BatteryLevel::Full;
    else if (m_level == "Normal")
        batteryLevel = BatteryLevel::Normal;
    else if (m_level == "Low")
        batteryLevel = BatteryLevel::Low;

    item->setData(static_cast<int>(batteryLevel), Qt::UserRole);
}

QStandardItemModel *Battery::model() const
{
    return m_model;
}

void Battery::updateUdev()
{
    if(checkForUpdates(battudev::currentBattery)) {
        BatteryStatus cstatus = BatteryStatus::Discharging;

        setBatteryCapacity(battudev::currentBattery->capacity);
        setCurrentEnergy(QString::number(battudev::currentBattery->energynow));
        setCurrentPower(QString::number(battudev::currentBattery->powernow));
        setLevel(battudev::currentBattery->level);

        switch (battudev::currentBattery->status[0]) {
        case 'D':
            break;
        case 'C':
            cstatus = BatteryStatus::Charging;
            break;
        case 'F':
            cstatus = BatteryStatus::Full;
            break;
        default:
            break;
        }

        if(status != cstatus)
            setStatus(cstatus);

        calculateTimeRemaining();
        emit updated();
    }
}
