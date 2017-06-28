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

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    SystemTrayIcon(const QIcon &icon, QObject *parent = nullptr);
    SystemTrayIcon(QObject *parent = nullptr);

    void setModel(const QStandardItemModel *value);
    const QStandardItemModel *model() const;

    int capacity() const;

public slots:
    void onBatteryError(QString error, BatteryError batteryError);
    void onStatusChanged(BatteryStatus status);
    void setCapacity(int value);
    void updateIcon();

private:
    void createMenu();

    int m_capacity;

    QString capacityStr;
    QString toolTipStr;
    QString statusStr;

    const QStandardItemModel *m_model;
};
#endif // SYSTEMTRAYICON_H
