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

#include <stdlib.h>

#include <QtNetwork/QLocalSocket>
#include <QProcessEnvironment>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "systemtrayicon.h"
#include "globalheader.h"
#include "mainwidget.h"
#include "battery.h"

#include <QScreen>

static bool showMainWidget = true;
static bool connectedToServer;
static QProcessEnvironment env;
static SystemTrayIcon *tray;
static QLocalSocket *soc;
static Battery *battery;
static MainWidget *w;

static void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
static void readBatteryError(QString error, BatteryError errorType);
static void configureApplication(const QApplication &app);
static void handleArguments(const QApplication &app);
static void sendMessage(LocalMSG message);
static void checkForServer();
static void writeConfig();
static void readConfig();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    env = QProcessEnvironment::systemEnvironment();
    soc = nullptr;
    w = nullptr;

    configureApplication(a);
    checkForServer();
    handleArguments(a);

    if (connectedToServer) {
        qWarning("Application already running.");
        exit(1);
    }

    tray = new SystemTrayIcon;
    battery = new Battery;

    tray->setModel(battery->model());
    w->setModel(battery->model());

    if (showMainWidget) {
        tray->show();
        w->show();
    } else {
        tray->show();
    }

    QObject::connect(battery, &Battery::batteryStatusChanged, tray, &SystemTrayIcon::onStatusChanged);
    QObject::connect(battery, &Battery::batteryCapacityChanged, tray, &SystemTrayIcon::setCapacity);
    QObject::connect(w, &MainWidget::selectedBatteryChanged, battery, &Battery::setBatteryNumber);
    QObject::connect(battery, &Battery::batteryError, tray, &SystemTrayIcon::onBatteryError);
    QObject::connect(battery, &Battery::filesUpdated, tray, &SystemTrayIcon::updateIcon);
    QObject::connect(battery, &Battery::batteryError, readBatteryError);
    QObject::connect(tray, &SystemTrayIcon::activated, onTrayActivated);

    qDebug() << "User: " << env.value("USER", "qt");
    qDebug() << "Display: " << env.value("DISPLAY", ":0.0");

    readConfig();

    int exitCode;
    exitCode = a.exec();

    writeConfig();

    return exitCode;
}

static void readConfig()
{
    QSettings settings;
    int batteryNumber;
    bool ok;

    batteryNumber = settings.value("battery/number", 0).toInt(&ok);

    if (ok)
        battery->setBatteryNumber(batteryNumber);
}

static void writeConfig()
{
    int batteryNumber = battery->batteryNumber();
    QSettings settings;

    settings.setValue("battery/number", batteryNumber);
}

static void configureApplication(const QApplication &app)
{
    QIcon::setThemeName("Adwaita");

    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName("QBattMon");
    app.setOrganizationName("BrandonSoft");
    app.setApplicationVersion("1.1.1");
}

static void handleArguments(const QApplication &app)
{
    QCommandLineOption setBrightness(QStringList() << "s" << "set",
                                     QCoreApplication::translate("main", "Set backlight brightness to <percentage>"),
                                     "percentage");
    QCommandLineOption incBrightness(QStringList() << "i" << "inc",
                                     QCoreApplication::translate("main", "Increment backlight brightness by <percentage>"),
                                     "percentage");
    QCommandLineOption decBrightness(QStringList() << "d" << "dec",
                                     QCoreApplication::translate("main", "Decrement backlight brightness by <percentage>"),
                                     "percentage");
    QCommandLineOption trayOnly(QStringList() << "t" << "tray",
                                QCoreApplication::translate("main", "Start application in the tray only."));

    QCommandLineParser parser;

    parser.setApplicationDescription("CLI usage for QBattMon");
    parser.addOption(setBrightness);
    parser.addOption(incBrightness);
    parser.addOption(decBrightness);
    parser.addOption(trayOnly);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    if (parser.isSet("tray"))
        showMainWidget = false;

    if (parser.isSet("set")) {
        bool ok;
        QString valueStr = parser.value("set");
        double value = valueStr.toDouble(&ok);

        if (ok) {
            if (value > 1) {
                value = value / 100.0;
            }
        }

        if (connectedToServer) {
            sendMessage(LocalMSG(MessageType::BrightnessSet, value));
            exit(0);
        } else {
            w->setBrightness(value);
            exit(0);
        }
    }

    if (parser.isSet("inc")) {
        bool ok;
        QString valueStr = parser.value("inc");
        double value = valueStr.toDouble(&ok);

        if (ok) {
            if (value > 1) {
                value = value / 100.0;
            }
        }

        if (connectedToServer) {
            sendMessage(LocalMSG(MessageType::BrightnessUp, value));
            exit(0);
        } else {
            w->incBrightness(value);
            exit(0);
        }
    }

    if (parser.isSet("dec")) {
        bool ok;
        QString valueStr = parser.value("dec");
        double value = valueStr.toDouble(&ok);

        if (ok) {
            if (value > 1) {
                value = value / 100.0;
            }
        }

        if (connectedToServer) {
            sendMessage(LocalMSG(MessageType::BrightnessDown, value));
            exit(0);
        } else {
            w->decBrightness(value);
            exit(0);
        }
    }
}

static void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        if (w->isVisible())
            w->close();
        else
            w->show();
        break;
    default:
        break;
    }
}

static void readBatteryError(QString error, BatteryError errorType)
{
    qDebug() << "Error Text: " << error;

    switch (errorType) {
    case BatteryError::NoBattery:
        qDebug() << "Error Type: " << "NoBattery";
        w->close();
        break;
    default:
        break;
    }
}

static void checkForServer()
{
    QString serverName = QString(QApplication::applicationName()
                                 + "-"
                                 + QApplication::applicationVersion()
                                 + "-"
                                 + env.value("USER", "qt")
                                 + "-"
                                 + QApplication::primaryScreen()->name());
//                                 + env.value("DISPLAY", ":0.0"));
    soc = new QLocalSocket;

    soc->connectToServer(serverName);
    if (soc->waitForConnected()) {
        connectedToServer = true;
        qDebug() << "Connected";
    } else {
        connectedToServer = false;
        w = new MainWidget;
        if (w->setupServer(serverName)) {
            qDebug() << soc->errorString();
            qDebug() << "Started server...";
            soc->close();
            soc = nullptr;
        }
    }

    qDebug() << "Server name: " << serverName;
}

void sendMessage(LocalMSG message)
{
    QDataStream stream(soc);

    stream << message;
    bool written = soc->waitForBytesWritten();
    qDebug() << "Did the socket write? " << written;

    if (written) {
        soc->disconnectFromServer();
        soc->close();
    }
}

QDataStream &operator<<(QDataStream &out, const LocalMSG &message)
{
    out << message.version << static_cast<int>(message.type) << message.percentOfBrightness;
    return out;
}

LocalMSG::LocalMSG(MessageType mType, double percent)
{
    type = mType;
    percentOfBrightness = percent;
    version = QApplication::applicationVersion();
}
