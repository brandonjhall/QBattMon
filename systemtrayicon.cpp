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

#include <QtNetwork/QLocalServer>
#include <QStandardItemModel>
#include <QApplication>
#include <QTimer>
#include <QMenu>
#include <QDir>

#include "systemtrayicon.h"
//#include "mainwidget.h"

//extern MainWidget *mainWidget();

SystemTrayIcon::SystemTrayIcon(const QIcon &icon, QObject *parent)
    :QSystemTrayIcon(icon, parent)
{
//    mWidget = mainWidget();
//    mainWidget()->show();
    createMenu();
}

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    :QSystemTrayIcon(QIcon::fromTheme("battery-full"), parent)
{
//    mWidget = mainWidget();
//    mainWidget()->show();
    createMenu();
}

void SystemTrayIcon::onBatteryError(QString error, BatteryError batteryError)
{
    switch (batteryError) {
    case BatteryError::NoBattery:
    {
        showMessage("No Battery", error, QSystemTrayIcon::Critical, 3000);
        setIcon(QIcon::fromTheme("battery-missing"));
    }
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
    QString iconName = icon().name();
    QString newIconName;
    BatteryStatus status = static_cast<BatteryStatus>(model->item(0, 4)->data(Qt::UserRole).toInt());
    BatteryLevel level = static_cast<BatteryLevel>(model->item(0, 6)->data(Qt::UserRole).toInt());

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
            if(capacity >= 90)
            {
                newIconName = "battery-full-charging";
            }
            else if(capacity >= 45)
            {
                newIconName = "battery-good-charging";
            }
            else if(capacity <= 44)
            {
                newIconName = "battery-low-charging";
            }
            break;
        case BatteryLevel::Low:
            if(capacity >= 20)
            {
                newIconName = "battery-low-charging";
            }
            else if(capacity < 20)
            {
                newIconName = "battery-caution-charging";
            }
            break;
        }
        break;
    case BatteryStatus::Discharging:
        switch (level) {
        case BatteryLevel::Full:
            setIcon(QIcon::fromTheme("battery-full"));
            break;
        case BatteryLevel::Normal:
            if(capacity >= 90)
            {
                newIconName = "battery-full";
            }
            else if(capacity >= 45)
            {
                newIconName = "battery-good";
            }
            else if(capacity <= 44)
            {
                newIconName = "battery-low";
            }
            break;
        case BatteryLevel::Low:
            if(capacity >= 7)
            {
                newIconName = "battery-caution";
            }
            else if(capacity < 6)
            {
                newIconName = "battery-empty";
            }
            break;
        }
        break;
    default:
        break;
    }

    if(newIconName != iconName)
    {
        QString title;
        QString message;
        QSystemTrayIcon::MessageIcon msgIcon;
        QStringList warnings = QStringList() << "battery-low" << "battery-caution" << "battery-empty";

        setIcon(QIcon::fromTheme(newIconName));

        if(newIconName == "battery-low")
        {
            title = "Caution:";
            message = "Battery is low";
            msgIcon = QSystemTrayIcon::Information;
        }

        if(newIconName == "battery-caution")
        {
            title = "Warning:";
            message = "Battery is almost empty";
            msgIcon = QSystemTrayIcon::Warning;
        }

        if(newIconName == "battery-empty")
        {
            title = "CRITICAL:";
            message = "Battery is empty!";
            msgIcon = QSystemTrayIcon::Critical;
        }

        if(warnings.contains(newIconName))
            showMessage(title, message, msgIcon, 3000);
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
        showMessage(statusStr, "Battery is full", QSystemTrayIcon::Information, 3000);
        break;
    case BatteryStatus::Charging:
        statusStr = "Charging battery";
        showMessage(statusStr, "Charging battery", QSystemTrayIcon::Information, 3000);
        break;
    case BatteryStatus::Discharging:
        statusStr = "Discharging battery";
        showMessage(statusStr, QString("Time Remaining: " + model->item(0, 12)->text()), QSystemTrayIcon::Information, 3000);
        break;
    default:
        break;
    }

    toolTipStr = statusStr + "\nBattery Level: " + capacityStr;
    setToolTip(toolTipStr);
}

bool SystemTrayIcon::setupServer(QString serverName)
{
    server = new QLocalServer;

    server->setSocketOptions(QLocalServer::UserAccessOption);
    return server->listen(serverName);
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
