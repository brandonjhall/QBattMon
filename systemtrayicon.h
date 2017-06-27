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

#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

#include "globalheader.h"

class QStandardItemModel;
class MainWidget;
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
    void onBatteryError(QString error, BatteryError batteryError);
    void onStatusChanged(BatteryStatus status);
    void setCapacity(int value);
    void updateIcon();

private:
    void createMenu();

    int capacity;
    QString statusStr;
    QString toolTipStr;
    QString capacityStr;
    QStandardItemModel *model;
//    MainWidget *mWidget;

private slots:
};
#endif // SYSTEMTRAYICON_H
