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

    QString energyNow;
    QString powerNow;
    QString capacity;
    QString status;
    QString level;

    if (batteryFolders.contains(QString("BAT%1").arg(value))) {
        QString batteryPath = POWER_DIR + QString("/BAT%1").arg(value);
        capacity = batteryPath + "/capacity";
        status = batteryPath + "/status";
        level = batteryPath + "/capacity_level";
        powerNow = batteryPath + "/power_now";
        energyNow = batteryPath + "/energy_now";
        lastError = BatteryError::NoError;
    } else {
        emit batteryError("No battery", BatteryError::NoBattery);
        lastError = BatteryError::NoBattery;
    }

    if (QFile(level).exists())
        batteryLevel->setFileName(level);

    if (QFile(capacity).exists())
        batteryCapacity->setFileName(capacity);

    if (QFile(status).exists())
        batteryStatus->setFileName(status);

    if (QFile(powerNow).exists())
        batteryCurrentPower->setFileName(powerNow);

    if (QFile(energyNow).exists())
        batteryCurrentEnergy->setFileName(energyNow);

    createModel();
    updateFiles();
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
    batteryCurrentEnergy = new QFile;
    batteryCurrentPower = new QFile;
    batteryCapacity = new QFile;
    batteryStatus = new QFile;
    batteryLevel = new QFile;

    batteryFolders = QDir(POWER_DIR).entryList(QStringList() << "BAT*");

    updateTimer = new QTimer;

    updateTimer->setInterval(1000);
    connect(updateTimer, &QTimer::timeout, this, &Battery::updateFiles);
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
        updateFiles();
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

    foreach (QString battery, batteryFolders) {
        QString batteryPath = QString(POWER_DIR) + QString("/") + battery;
        QStandardItem *batteryItem = new QStandardItem(battery);
        QList<QStandardItem*> row;

        row.append(batteryItem);

        foreach (QString fileName, files) {
            QFile file(batteryPath + "/" + fileName);
            QString contents;

            if (file.open(QFile::ReadOnly))
                contents = file.readAll();

            contents = contents.replace("\n", "");

            if (fileName == "capacity")
                contents += "%";

            if (fileName == "energy_full" || fileName == "energy_now" || fileName == "power_now") {
                bool ok = false;
                int contentsInt = contents.toInt(&ok);

                if (ok) {
                    contentsInt = contentsInt / 10000;
                    contents = QString("%1 mAh").arg(contentsInt);
                }

                if (fileName == "energy_now")
                    energy = contentsInt;
                else if (fileName == "power_now")
                    usage = contentsInt;
                else if (fileName == "energy_full")
                    m_maxEnergy = contentsInt;
            }

            row.append(new QStandardItem(contents));
        }

        calculateTimeRemaining();
        row.append(new QStandardItem(m_timeLeft->toString()));
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

void Battery::updateFiles()
{
    if (lastError == BatteryError::NoError) {

        if (!batteryCapacity->fileName().isEmpty() && batteryCapacity->open(QFile::ReadOnly)) {
            QString batteryLevelString = batteryCapacity->readAll();
            batteryCapacity->close();
            int capacity = batteryLevelString.toInt();

            if (m_batteryCapacityNumber != capacity)
                setBatteryCapacity(capacity);
        }

        if (!batteryStatus->fileName().isEmpty() && batteryStatus->open(QFile::ReadOnly)) {
            QString batteryStatusString = batteryStatus->readAll();
            batteryStatus->close();

            if (batteryStatusString.contains("Full") && status != BatteryStatus::Full)
                setStatus(BatteryStatus::Full);
            else if (batteryStatusString.contains("Discharging") && status != BatteryStatus::Discharging)
                setStatus(BatteryStatus::Discharging);
            else if (batteryStatusString.contains("Charging") && status != BatteryStatus::Charging)
                setStatus(BatteryStatus::Charging);
        }

        if (!batteryLevel->fileName().isEmpty() && batteryLevel->open(QFile::ReadOnly)) {
            QString batteryLevelString = batteryLevel->readAll();
            batteryLevel->close();

            if (batteryLevelString != m_level)
                setLevel(batteryLevelString);
        }

        if (!batteryCurrentPower->fileName().isEmpty() && batteryCurrentPower->open(QFile::ReadOnly)) {
            QString currentPowerString = batteryCurrentPower->readAll();
            batteryCurrentPower->close();

            if (m_currentPower != currentPowerString) {
                setCurrentPower(currentPowerString);
                calculateTimeRemaining();
            }
        }

        if (!batteryCurrentEnergy->fileName().isEmpty() && batteryCurrentEnergy->open(QFile::ReadOnly)) {
            QString currentEnergyString = batteryCurrentEnergy->readAll();
            batteryCurrentEnergy->close();

            if (m_currentEnergy != currentEnergyString) {
                setCurrentEnergy(currentEnergyString);
                calculateTimeRemaining();
            }
        }
    }

    emit filesUpdated();
}
