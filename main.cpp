#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QSettings>
#include <QIcon>

#include "globalheader.h"
#include "systemtrayicon.h"
#include "mainwidget.h"
#include "battery.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

static bool showMainWidget = true;
static MainWidget *w;
static Battery *battery;

void readConfig();
void writeConfig();
void configureApplication(const QApplication &app);
void configureCommandLine(const QApplication &app);
void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
void readBatteryError(QString error, BatteryError errorType);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w = new MainWidget;
    SystemTrayIcon *tray = new SystemTrayIcon;
    battery = new Battery;

    configureApplication(a);
    configureCommandLine(a);
    tray->setModel(battery->getModel());
    w->setModel(battery->getModel());

    QObject::connect(battery, &Battery::batteryError, readBatteryError);
    QObject::connect(w, &MainWidget::selectedBatteryChanged, battery, &Battery::setBatteryNumber);
    QObject::connect(battery, &Battery::batteryError, tray, &SystemTrayIcon::onBatteryError);
    QObject::connect(battery, &Battery::batteryStatusChanged, tray, &SystemTrayIcon::onStatusChanged);
    QObject::connect(battery, &Battery::batteryCapacityChanged, tray, &SystemTrayIcon::setCapacity);
    QObject::connect(battery, &Battery::filesUpdated, tray, &SystemTrayIcon::updateIcon);
    QObject::connect(tray, &SystemTrayIcon::activated, onTrayActivated);

    if(showMainWidget)
    {
        tray->show();
        w->show();
    }
    else
    {
        tray->show();
    }

    readConfig();
    int exitCode = a.exec();

    writeConfig();

    return exitCode;
}

void readConfig()
{
    bool ok;
    QSettings settings;
    int batteryNumber = settings.value("battery/number", 0).toInt(&ok);

    if(ok)
        battery->setBatteryNumber(batteryNumber);
}

void writeConfig()
{
    QSettings settings;
    int batteryNumber = battery->getBatteryNumber();

    settings.setValue("battery/number", batteryNumber);
}

void configureApplication(const QApplication &app)
{
    QIcon::setThemeName("Adwaita");
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName("QBattMon");
    app.setOrganizationName("BrandonSoft");
    app.setApplicationVersion("1.1.1");
}

void configureCommandLine(const QApplication &app)
{
    QCommandLineParser parser;

    parser.setApplicationDescription("CLI usage for QBattMon");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption trayOnly(QStringList() << "t" << "tray",
                                   QCoreApplication::translate("main", "Start application in the tray only."));
    parser.addOption(trayOnly);

    parser.process(app);

    if(parser.isSet(trayOnly))
    {
        showMainWidget = false;
    }
}

void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if(w->isVisible())
            w->close();
        else
            w->show();
        break;
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

void readBatteryError(QString error, BatteryError errorType)
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
