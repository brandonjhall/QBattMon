#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

#include "globalheader.h"

class QStandardItemModel;

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    SystemTrayIcon(const QIcon &icon, QObject *parent = nullptr);
    SystemTrayIcon(QObject *parent = nullptr);

    QStandardItemModel *getModel() const;
    void setModel(QStandardItemModel *value);

    int getCapacity() const;

public slots:
    void onStatusChanged(BatteryStatus status);
    void setCapacity(int value);
    void updateIcon();
    void onBatteryError(QString error, BatteryError batteryError);

private:
    void createMenu();

    int capacity;
    QString statusStr;
    QString toolTipStr;
    QString capacityStr;
    QStandardItemModel *model;
};

#endif // SYSTEMTRAYICON_H
