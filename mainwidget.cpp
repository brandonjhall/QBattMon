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

#include <QDataWidgetMapper>
#include <QStandardItemModel>
#include <QMetaProperty>
#include <QSettings>

#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    mapper = new QDataWidgetMapper;
}

MainWidget::~MainWidget()
{
    delete ui;
}

QStandardItemModel *MainWidget::getModel() const
{
    return model;
}

void MainWidget::setModel(QStandardItemModel *value)
{
    model = value;

    for(int i = 0; i < model->rowCount(); i++) {
        QString itemString = model->item(i)->data(Qt::DisplayRole).toString();
        ui->comboBox->addItem(itemString);
    }

    mapper->clearMapping();
    mapper->setModel(value);
    mapper->setItemDelegate(new ComboBoxDelegate(this));
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

void MainWidget::selectBattery()
{
    bool ok = false;
    QString batteryName = ui->comboBox->currentText();
    QString batteryNumberStr = batteryName.at(batteryName.size() - 1);
    int batteryNumber = batteryNumberStr.toInt(&ok);

    if(ok)
    {
        QSettings settings;

        settings.setValue("battery/number", batteryNumber);
        emit selectedBatteryChanged(batteryNumber);
    }
}

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
{

}

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
