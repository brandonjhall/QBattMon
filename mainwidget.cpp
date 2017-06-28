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
#include <QStandardItemModel>
#include <QDataWidgetMapper>
#include <QMetaProperty>
#include <QDataStream>
#include <QSettings>

#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "globalheader.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    mapper = new QDataWidgetMapper;

    ui->setupUi(this);
    ui->brightnessPercent->setText(ui->horizontalSlider->brightnessString());
}

MainWidget::~MainWidget()
{
    delete ui;
}

const QStandardItemModel *MainWidget::getModel() const
{
    return model;
}

void MainWidget::setModel(QStandardItemModel *value)
{
    model = value;

    for (int i = 0; i < model->rowCount(); i++) {
        QString itemString = model->item(i)->data(Qt::DisplayRole).toString();
        ui->comboBox->addItem(itemString);
    }

    mapper->setItemDelegate(new ComboBoxDelegate(this));

    mapper->setModel(value);

    mapper->addMapping(ui->comboBox, 0);
    mapper->addMapping(ui->technology, 1);
    mapper->addMapping(ui->type, 2);
    mapper->addMapping(ui->manufacturer, 3);
    mapper->addMapping(ui->status, 4);
    mapper->addMapping(ui->capacity, 5);
    mapper->addMapping(ui->capacity_level, 6);
    mapper->addMapping(ui->energy_full, 7);
    mapper->addMapping(ui->power_now, 8);
    mapper->addMapping(ui->energy_now, 9);
    mapper->addMapping(ui->model_name, 10);
    mapper->addMapping(ui->serial_number, 11);
    mapper->addMapping(ui->time_remaining, 12);

    mapper->toFirst();
}

void MainWidget::setBrightness(double brightnessPercent)
{
    ui->horizontalSlider->setBrightness(brightnessPercent);
}

void MainWidget::incBrightness(double inc)
{
    ui->horizontalSlider->incBrightness(inc);
}

void MainWidget::decBrightness(double dec)
{
    ui->horizontalSlider->decBrightness(dec);
}

void MainWidget::selectBattery()
{
    QString batteryName = ui->comboBox->currentText();
    QString batteryNumberStr = batteryName.at(batteryName.size() - 1);

    bool ok = false;
    int batteryNumber = batteryNumberStr.toInt(&ok);

    if (ok) {
        QSettings settings;

        settings.setValue("battery/number", batteryNumber);
        emit selectedBatteryChanged(batteryNumber);
    }
}

bool MainWidget::setupServer(QString serverName)
{
    server = new QLocalServer;

    server->setSocketOptions(QLocalServer::UserAccessOption);
    connect(server, &QLocalServer::newConnection, this, &MainWidget::onNewConnection);

    return server->listen(serverName);
}

void MainWidget::setCurrentBrightness(double brightnessPercent)
{
    emit changeBrightness(brightnessPercent);
}

void MainWidget::onNewConnection()
{
    QLocalSocket *soc = server->nextPendingConnection();
    connect(soc, &QLocalSocket::readyRead, this, &MainWidget::onReadyRead);
}

void MainWidget::onReadyRead()
{
    QLocalSocket *soc = (QLocalSocket*)sender();
    QDataStream stream(soc);
    LocalMSG message;

    stream >> message;

    switch (message.type) {
    case MessageType::BrightnessUp:
        incBrightness(message.percentOfBrightness);
        break;
    case MessageType::BrightnessDown:
        decBrightness(message.percentOfBrightness);
        break;
    case MessageType::BrightnessSet:
        decBrightness(message.percentOfBrightness);
        break;
    }
}

QDataStream &operator>>(QDataStream &in, LocalMSG &message)
{
    MessageType type;
    QString version;
    double percent;
    int iType;

    in >> version >> iType >> percent;
    type = static_cast<MessageType>(iType);
    message = LocalMSG(type, percent);
    message.setVersion(version);

    return in;
}

ComboBoxDelegate::ComboBoxDelegate(QObject *parent) : QItemDelegate(parent) {}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!editor->metaObject()->userProperty().isValid()) {
        if (editor->property("currentIndex").isValid()) {
            editor->setProperty("currentIndex", index.data());
            return;
        }
    }

    QItemDelegate::setEditorData(editor, index);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!editor->metaObject()->userProperty().isValid()) {
        QVariant value = editor->property("currentIndex");
        if (value.isValid()) {
            model->setData(index, value);
            return;
        }
    }

    QItemDelegate::setModelData(editor, model, index);
}
