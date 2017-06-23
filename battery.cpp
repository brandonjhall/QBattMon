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
#include <QFile>
#include <QDir>

#include "battery.h"

Battery::Battery(QObject *parent) : QObject(parent)
{
    initializeVariables();
    createModel();
}

Battery::Battery(int battery, QObject *parent) : QObject(parent)
{
    initializeVariables();
    setBatteryNumber(battery);
    createModel();
}

int Battery::getBatteryNumber() const
{
    return batteryNumber;
}

void Battery::setBatteryNumber(int value)
{
    QString level;
    QString capacity;
    QString status;
    QString powerNow;
    QString energyNow;
    batteryNumber = value;

    if(batteryFolders.contains(QString("BAT%1").arg(value))) {
        QString batteryPath = POWER_DIR + QString("/BAT%1").arg(value);
        capacity = batteryPath + "/capacity";
        status = batteryPath + "/status";
        level = batteryPath + "/capacity_level";
        powerNow = batteryPath + "/power_now";
        energyNow = batteryPath + "/energy_now";
        lastError = BatteryError::NoError;
    }
    else {
        emit batteryError("No battery", BatteryError::NoBattery);
        lastError = BatteryError::NoBattery;
    }

    if(QFile(level).exists())
        batteryLevel->setFileName(level);

    if(QFile(capacity).exists())
        batteryCapacity->setFileName(capacity);

    if(QFile(status).exists())
        batteryStatus->setFileName(status);

    if(QFile(powerNow).exists())
        batteryCurrentPower->setFileName(powerNow);

    if(QFile(energyNow).exists())
        batteryCurrentEnergy->setFileName(energyNow);

    emit batteryNumberChanged(value);
}

void Battery::calculateTimeRemaining()
{
    QStandardItem *item = model->item(0, 12);
    timeLeft = calculateTimeRemaining(energy, usage);

    if(item != Q_NULLPTR)
        item->setText(timeLeft->toString());
}

QTime *Battery::calculateTimeRemaining(int energy, int usage)
{
    if(usage == 0)
        return timeLeft;

    int hours = 0, minutes = 0, seconds = 0;

    if(status == BatteryStatus::Discharging)
    {
        hours = energy / usage;
        minutes = energy % usage;
        seconds = minutes % 60;
        minutes = minutes / 60;
    }
    else if(status == BatteryStatus::Charging)
    {
        int chargeNeeded = maxEnergy - energy;
        hours = chargeNeeded / usage;
        minutes = chargeNeeded % usage;
        seconds = minutes % 60;
        minutes = minutes / 60;
    }

    return new QTime(hours, minutes, seconds);
}

void Battery::initializeVariables()
{
    batteryLevel = new QFile;
    batteryCapacity = new QFile;
    batteryStatus = new QFile;
    batteryCurrentPower = new QFile;
    batteryCurrentEnergy = new QFile;
    updateTimer = new QTimer;
    batteryFolders = QDir(POWER_DIR).entryList(QStringList() << "BAT*");

    updateTimer->setInterval(1000);
    connect(updateTimer, &QTimer::timeout, this, &Battery::updateFiles);
    updateTimer->start();
}

void Battery::setBatteryCapacity(int value)
{
    QString capacityString = QString::number(value) + "%";
    QStandardItem *item = model->item(0, 5);
    batteryCapacityNumber = value;

    item->setText(capacityString);
    emit batteryCapacityChanged(value);
}

BatteryStatus Battery::getStatus() const
{
    return status;
}

void Battery::setStatus(const BatteryStatus &value)
{
    QString statusStr;
    QStandardItem *item = model->item(0, 4);
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
    item->setText(statusStr);
    item->setData(static_cast<int>(status), Qt::UserRole);
    emit batteryStatusChanged(value);
}

void Battery::createModel()
{
    model = new QStandardItemModel;
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

    model->setHorizontalHeaderLabels(headers);

    foreach (QString battery, batteryFolders) {
        QStandardItem *batteryItem = new QStandardItem(battery);
        QString batteryPath = QString(POWER_DIR) + QString("/") + battery;
        QList<QStandardItem*> row;

        row.append(batteryItem);

        foreach (QString fileName, files) {
            QString contents;
            QFile file(batteryPath + "/" + fileName);

            if(file.open(QFile::ReadOnly))
                contents = file.readAll();

            contents.replace("\n", "");

            if(fileName == "capacity")
                contents += "%";

            if(fileName == "energy_full" || fileName == "energy_now" || fileName == "power_now")
            {
                bool ok = false;
                int energy = contents.toInt(&ok);

                if(ok)
                {
                    energy = energy / 10000;
                    contents = QString("%1 mAh").arg(energy);
                }

                if(fileName == "energy_now")
                    this->energy = energy;

                if(fileName == "power_now")
                    this->usage = energy;

                if(fileName == "energy_full")
                    this->maxEnergy = energy;
            }

            row.append(new QStandardItem(contents));
        }

        calculateTimeRemaining();
        row.append(new QStandardItem(timeLeft->toString()));
        model->insertRow(model->rowCount(), row);
    }
}

QTime *Battery::getTimeLeft() const
{
    return timeLeft;
}

QString Battery::getCurrentEnergy() const
{
    return currentEnergy;
}

void Battery::setCurrentEnergy(const QString &value)
{
    bool ok = false;
    int energy = value.toInt(&ok);
    QStandardItem *item = model->item(0, 9);

    if(ok)
    {
        energy = energy / 10000;
        currentEnergy = QString("%1 mAh").arg(energy);
    }

    this->energy = energy;
    item->setText(currentEnergy);
}

QString Battery::getCurrentPower() const
{
    return currentPower;
}

void Battery::setCurrentPower(const QString &value)
{
    bool ok = false;
    int energy = value.toInt(&ok);
    QStandardItem *item = model->item(0, 8);

    if(ok)
    {
        energy = energy / 10000;
        currentPower = QString("%1 mAh").arg(energy);
    }

    usage = energy;
    item->setText(currentPower);
}

QString Battery::getLevel() const
{
    return level;
}

void Battery::setLevel(const QString &value)
{
    BatteryLevel batteryLevel = BatteryLevel::Normal;
    QStandardItem *item = model->item(0, 6);
    level = value;

    level = level.replace("\n", "");
    item->setText(level);

    if(level == "Full")
        batteryLevel = BatteryLevel::Full;
    else if(level == "Normal")
        batteryLevel = BatteryLevel::Normal;
    else if(level == "Low")
        batteryLevel = BatteryLevel::Low;

    item->setData(static_cast<int>(batteryLevel), Qt::UserRole);
}

QStandardItemModel *Battery::getModel() const
{
    return model;
}

void Battery::updateFiles()
{
    if(lastError == BatteryError::NoError){

        if(!batteryCapacity->fileName().isEmpty() && batteryCapacity->open(QFile::ReadOnly)) {
            QString batteryLevelStr = batteryCapacity->readAll();
            int capacity = batteryLevelStr.toInt();

            if(batteryCapacityNumber != capacity)
                setBatteryCapacity(capacity);

            batteryCapacity->close();
        }

        if(!batteryStatus->fileName().isEmpty() && batteryStatus->open(QFile::ReadOnly)) {
            QString batteryStatusStr = batteryStatus->readAll();

            if(batteryStatusStr.contains("Full"))
            {
                if(status != BatteryStatus::Full)
                    setStatus(BatteryStatus::Full);
            }
            else if(batteryStatusStr.contains("Discharging"))
            {
                if(status != BatteryStatus::Discharging)
                    setStatus(BatteryStatus::Discharging);
            }
            else if(batteryStatusStr.contains("Charging"))
            {
                if(status != BatteryStatus::Charging)
                    setStatus(BatteryStatus::Charging);
            }
            batteryStatus->close();
        }

        if(!batteryLevel->fileName().isEmpty() && batteryLevel->open(QFile::ReadOnly)) {
            QString batteryLevelStr = batteryLevel->readAll();

            if(batteryLevelStr != level)
                setLevel(batteryLevelStr);

            batteryLevel->close();
        }

        if(!batteryCurrentPower->fileName().isEmpty() && batteryCurrentPower->open(QFile::ReadOnly)) {
            QString currentPowerStr = batteryCurrentPower->readAll();

            if(currentPower != currentPowerStr)
            {
                setCurrentPower(currentPowerStr);
                calculateTimeRemaining();
            }

            batteryCurrentPower->close();
        }

        if(!batteryCurrentEnergy->fileName().isEmpty() && batteryCurrentEnergy->open(QFile::ReadOnly)) {
            QString currentEnergyStr = batteryCurrentEnergy->readAll();

            if(currentEnergy != currentEnergyStr)
            {
                setCurrentEnergy(currentEnergyStr);
                calculateTimeRemaining();
            }

            batteryCurrentEnergy->close();
        }
    }
    emit filesUpdated();
}
