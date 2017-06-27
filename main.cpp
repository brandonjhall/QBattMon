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
#include <QtNetwork/QLocalSocket>
#include <QProcessEnvironment>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QApplication>
#include <QSettings>
#include <QIcon>

#include "systemtrayicon.h"
#include "globalheader.h"
#include "mainwidget.h"
#include "battery.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

static bool showMainWidget = true;
static QProcessEnvironment env;
static SystemTrayIcon *tray;
static Battery *battery;
static MainWidget *w;

static void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
static void readBatteryError(QString error, BatteryError errorType);
static void configureApplication(const QApplication &app);
static void configureCommandLine(const QApplication &app);
static void checkForServer();
static void writeConfig();
static void readConfig();

//extern MainWidget *mainWidget()
//{
//    return w;
//}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    w = new MainWidget;
    env = QProcessEnvironment::systemEnvironment();
    tray = new SystemTrayIcon;
    battery = new Battery;
    int exitCode;

    configureApplication(a);
    configureCommandLine(a);

    tray->setModel(battery->getModel());
    w->setModel(battery->getModel());
#ifdef QT_DEBUG
    qDebug() << "User: " << env.value("USER", "qt");
    qDebug() << "Display: " << env.value("DISPLAY", ":0.0");
#endif

    if(showMainWidget)
    {
        tray->show();
        w->show();
    }
    else
        tray->show();

    QObject::connect(battery, &Battery::batteryStatusChanged, tray, &SystemTrayIcon::onStatusChanged);
    QObject::connect(battery, &Battery::batteryCapacityChanged, tray, &SystemTrayIcon::setCapacity);
    QObject::connect(w, &MainWidget::selectedBatteryChanged, battery, &Battery::setBatteryNumber);
    QObject::connect(battery, &Battery::batteryError, tray, &SystemTrayIcon::onBatteryError);
    QObject::connect(battery, &Battery::filesUpdated, tray, &SystemTrayIcon::updateIcon);
    QObject::connect(battery, &Battery::batteryError, readBatteryError);
    QObject::connect(tray, &SystemTrayIcon::activated, onTrayActivated);

    checkForServer();
    readConfig();
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

    if(ok)
        battery->setBatteryNumber(batteryNumber);
}

static void writeConfig()
{
    int batteryNumber = battery->getBatteryNumber();
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

static void configureCommandLine(const QApplication &app)
{
    QCommandLineOption trayOnly(QStringList() << "t" << "tray",
                                QCoreApplication::translate("main", "Start application in the tray only."));
    QCommandLineParser parser;

    parser.setApplicationDescription("CLI usage for QBattMon");
    parser.addOption(trayOnly);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    if(parser.isSet(trayOnly))
        showMainWidget = false;
}

static void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        if(w->isVisible())
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
#ifdef QT_DEBUG
    qDebug() << "Error Text: " << error;
#endif
    switch (errorType) {
    case BatteryError::NoBattery:
#ifdef QT_DEBUG
        qDebug() << "Error Type: " << "NoBattery";
#endif
        w->close();
        break;
    default:
        break;
    }
}

static void checkForServer()
{
    QLocalSocket *soc = new QLocalSocket;

    soc->connectToServer("testing");
    if(soc->waitForConnected())
        qDebug() << "Connected";
    else if(tray->setupServer("testing"))
        qDebug() << "Started server...";
}
