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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QItemDelegate>
#include <QWidget>

class QStandardItemModel;
class QDataWidgetMapper;
class QLocalServer;

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

    const QStandardItemModel *getModel() const;
    void setModel(QStandardItemModel *value);

    bool setupServer(QString serverName);

    void setCurrentBrightness(double brightnessPercent);
    void setBrightness(double brightnessPercent);
    void incBrightness(double inc);
    void decBrightness(double dec);

signals:
    void selectedBatteryChanged(int selectedBattery);
    void changeBrightness(double brightnessPercent);

private:
    const QStandardItemModel *model;
    QDataWidgetMapper *mapper;
    QLocalServer *server;
    Ui::MainWidget *ui;

private slots:
    void onNewConnection();
    void selectBattery();
    void onReadyRead();
};

#endif // MAINWIDGET_H

#ifndef COMBOBOX_DELEGATE
#define COMBOBOX_DELEGATE

class ComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject *parent = 0);

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
};

#endif
