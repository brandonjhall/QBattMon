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

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

#include "powermanagement.h"
#include <unistd.h>
#include <QDebug>
#include <pty.h>

PowerManagement::PowerManagement(QObject *parent) :
    QObject(parent)
{
}

PowerManagement::~PowerManagement()
{
}

void PowerManagement::hibernate()
{
    dbusHibernate();
}

void PowerManagement::suspend()
{
    dbusSuspend();
}

void PowerManagement::dbusSuspend()
{
    QDBusConnection connect = QDBusConnection::systemBus();
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UPower",
                                                          "/org/freedesktop/UPower",
                                                          "org.freedesktop.UPower",
                                                          "Suspend");

    connect.call(message, QDBus::NoBlock);
}

void PowerManagement::dbusHibernate()
{
    QDBusConnection connect = QDBusConnection::systemBus();
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UPower",
                                                          "/org/freedesktop/UPower",
                                                          "org.freedesktop.UPower",
                                                          "Hibernate");

    connect.call(message, QDBus::NoBlock);
}
