#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QItemDelegate>

class QStandardItemModel;
class QDataWidgetMapper;

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

    QStandardItemModel *getModel() const;
    void setModel(QStandardItemModel *value);

public slots:

signals:
    void selectedBatteryChanged(int selectedBattery);

private:
    Ui::MainWidget *ui;
    QStandardItemModel *model;
    QDataWidgetMapper *mapper;

private slots:
    void selectBattery();
};

#endif // MAINWIDGET_H

#ifndef COMBOBOX_DELEGATE
#define COMBOBOX_DELEGATE

class ComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject *parent = 0);
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
};

#endif
