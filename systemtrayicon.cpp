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
#include <QApplication>
#include <QDir>
#include <QMenu>

#include "systemtrayicon.h"

SystemTrayIcon::SystemTrayIcon(const QIcon &icon, QObject *parent)
    :QSystemTrayIcon(icon, parent)
{
    createMenu();
}

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    :QSystemTrayIcon(QIcon::fromTheme("battery-full"), parent)
{
    createMenu();
}

void SystemTrayIcon::onBatteryError(QString error, BatteryError batteryError)
{
    switch (batteryError) {
    case BatteryError::NoBattery:
    {
//        QTimer *timer = new QTimer;

        showMessage("No Battery", error, QSystemTrayIcon::Critical);
//        connect(timer, &QTimer::timeout, &QApplication::quit);
//        timer->start(5000);
    }
        break;
    case BatteryError::NoError:
        break;
    default:
        break;
    }
}

void SystemTrayIcon::createMenu()
{
    QMenu *menu = new QMenu(QApplication::applicationName());

    // Uses the lamda expression [=](){ QApplication::quit(); } in place of a slot
    menu->addAction(QIcon(), "Exit", [=](){ QApplication::quit(); });
    setContextMenu(menu);
}

void SystemTrayIcon::updateIcon()
{
    BatteryStatus status = static_cast<BatteryStatus>(model->item(0, 4)->data(Qt::UserRole).toInt());
    BatteryLevel level = static_cast<BatteryLevel>(model->item(0, 6)->data(Qt::UserRole).toInt());

    switch (status) {
    case BatteryStatus::Full:
        switch (level) {
        case BatteryLevel::Full:
            setIcon(QIcon::fromTheme("battery-full"));
            break;
        case BatteryLevel::Normal:
            setIcon(QIcon::fromTheme("battery-full-charged"));
            break;
        default:
            break;
        }
        break;
    case BatteryStatus::Charging:
        switch (level) {
        case BatteryLevel::Full:
            setIcon(QIcon::fromTheme("battery-full-charging"));
            break;
        case BatteryLevel::Normal:
            setIcon(QIcon::fromTheme("battery-good-charging"));
            break;
        case BatteryLevel::Low:
            setIcon(QIcon::fromTheme("battery-low-charging"));
            break;
        }
        break;
    case BatteryStatus::Discharging:
        switch (level) {
        case BatteryLevel::Full:
            setIcon(QIcon::fromTheme("battery-full"));
            break;
        case BatteryLevel::Normal:
            setIcon(QIcon::fromTheme("battery-good"));
            break;
        case BatteryLevel::Low:
            setIcon(QIcon::fromTheme("battery-low"));
            break;
        }
        break;
    default:
        break;
    }
}

int SystemTrayIcon::getCapacity() const
{
    return capacity;
}

void SystemTrayIcon::onStatusChanged(BatteryStatus status)
{
    switch (status) {
    case BatteryStatus::Full:
        statusStr = "Battery full";
        showMessage(statusStr, "Battery is full");
        break;
    case BatteryStatus::Charging:
        statusStr = "Charging battery";
        showMessage(statusStr, "Charging battery");
        break;
    case BatteryStatus::Discharging:
        statusStr = "Discharging battery";
        showMessage(statusStr, QString("Time Remaining: " + model->item(0, 12)->text()));
        break;
    default:
        break;
    }

    toolTipStr = statusStr + "\nBattery Level: " + capacityStr;
    setToolTip(toolTipStr);
}

void SystemTrayIcon::setCapacity(int value)
{
    capacity = value;
    capacityStr = QString::number(value) + "%";
    toolTipStr = statusStr + "\nBattery Level: " + capacityStr;

    setToolTip(toolTipStr);
}

QStandardItemModel *SystemTrayIcon::getModel() const
{
    return model;
}

void SystemTrayIcon::setModel(QStandardItemModel *value)
{
    model = value;
}
