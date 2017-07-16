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
#include <QApplication>
#include <QWheelEvent>
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
        showMessage("No Battery", error, QSystemTrayIcon::Critical, 3000);
        setIcon(QIcon::fromTheme("battery-missing"));
        break;
    default:
        break;
    }
}

void SystemTrayIcon::createMenu()
{
    QMenu *menu = new QMenu(QApplication::applicationName());

    menu->addAction(QIcon(), "Hibernate", [=](){ emit hibernate(); } );
    menu->addAction(QIcon(), "Suspend", [=](){ emit suspend(); } );

    // Uses the lamda expression [=](){ QApplication::quit(); } in place of a slot
    menu->addAction(QIcon(), "Exit", [=](){ QApplication::quit(); } );
    setContextMenu(menu);
}

void SystemTrayIcon::updateIcon()
{
    BatteryStatus status = static_cast<BatteryStatus>(m_model->item(0, 4)->data(Qt::UserRole).toInt());
    BatteryLevel level = static_cast<BatteryLevel>(m_model->item(0, 6)->data(Qt::UserRole).toInt());
    QString newIconName;

    switch (status) {
    case BatteryStatus::Full:
        switch (level) {
        case BatteryLevel::Full:
            newIconName = "battery-full";
            break;
        case BatteryLevel::Normal:
            newIconName = "battery-full-charged";
            break;
        default:
            break;
        }
        break;
    case BatteryStatus::Charging:
        switch (level) {
        case BatteryLevel::Full:
            newIconName = "battery-full-charging";
            break;
        case BatteryLevel::Normal:
            if (m_capacity >= 90)
                newIconName = "battery-full-charging";
            else if (m_capacity >= 45)
                newIconName = "battery-good-charging";
            else
                newIconName = "battery-low-charging";
            break;
        case BatteryLevel::Low:
            if (m_capacity >= 10)
                newIconName = "battery-low-charging";
            else
                newIconName = "battery-caution-charging";
            break;
        }
        break;
    case BatteryStatus::Discharging:
        switch (level) {
        case BatteryLevel::Full:
            newIconName = "battery-full";
            break;
        case BatteryLevel::Normal:
            if (m_capacity >= 90)
                newIconName = "battery-full";
            else if (m_capacity >= 45)
                newIconName = "battery-good";
            else
                newIconName = "battery-low";
            break;
        case BatteryLevel::Low:
            if (m_capacity >= 6)
                newIconName = "battery-caution";
            else
                newIconName = "battery-empty";
            break;
        }
        break;
    }

    if (newIconName != icon().name()) {
        QStringList warnings = QStringList() << "battery-low" << "battery-caution" << "battery-empty";
        QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::Information;
        QString message;
        QString title;

        if (newIconName == "battery-low") {
            message = "Battery is low";
            title = "Caution:";
        } else if(newIconName == "battery-caution") {
            message = "Battery is almost empty";
            msgIcon = QSystemTrayIcon::Warning;
            title = "Warning:";
        } else if(newIconName == "battery-empty") {
            msgIcon = QSystemTrayIcon::Critical;
            message = "Battery is empty!";
            title = "CRITICAL:";
        }

        setIcon(QIcon::fromTheme(newIconName));

        if(warnings.contains(newIconName))
            showMessage(title, message, msgIcon, 3000);
    }
}

int SystemTrayIcon::capacity() const
{
    return m_capacity;
}

void SystemTrayIcon::onStatusChanged(BatteryStatus status)
{
    switch (status) {
    case BatteryStatus::Full:
        statusStr = "Battery full";
        showMessage(statusStr, "Battery is full", QSystemTrayIcon::Information, 3000);
        break;
    case BatteryStatus::Charging:
        statusStr = "Charging battery";
        showMessage(statusStr, "Charging battery", QSystemTrayIcon::Information, 3000);
        break;
    case BatteryStatus::Discharging:
        statusStr = "Discharging battery";
        showMessage(statusStr, QString("Time Remaining: " + m_model->item(0, 12)->text()), QSystemTrayIcon::Information, 3000);
        break;
    }

    toolTipStr = statusStr + "\nBattery Level: " + capacityStr;
    setToolTip(toolTipStr);
}

void SystemTrayIcon::setCapacity(int value)
{
    capacityStr = QString::number(value) + "%";
    m_capacity = value;

    toolTipStr = statusStr + "\nBattery Level: " + capacityStr;
    setToolTip(toolTipStr);
}

const QStandardItemModel *SystemTrayIcon::model() const
{
    return m_model;
}

void SystemTrayIcon::setModel(const QStandardItemModel *value)
{
    m_model = value;
}
