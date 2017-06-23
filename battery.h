#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>

#include "globalheader.h"

class QTime;
class QTimer;
class QFile;
class QStandardItemModel;

class Battery : public QObject
{
    Q_OBJECT
public:
    explicit Battery(QObject *parent = nullptr);
    Battery(int battery, QObject *parent = nullptr);

    int getBatteryNumber() const;
    BatteryStatus getStatus() const;
    QStandardItemModel *getModel() const;
    QString getLevel() const;
    QString getCurrentPower() const;
    QString getCurrentEnergy() const;
    QTime *getTimeLeft() const;

signals:
    void batteryError(QString error, BatteryError batteryError);
    void batteryNumberChanged(int number);
    void batteryCapacityChanged(int level);
    void batteryStatusChanged(BatteryStatus status);
    void filesUpdated();

public slots:
    void setBatteryNumber(int value);

private:
    void calculateTimeRemaining();
    QTime *calculateTimeRemaining(int energy, int usage);
    void initializeVariables();
    void setBatteryCapacity(int value);
    void setStatus(const BatteryStatus &value);
    void setLevel(const QString &value);
    void setCurrentEnergy(const QString &value);
    void setCurrentPower(const QString &value);
    void createModel();

    int batteryNumber;
    int batteryCapacityNumber;
    int maxEnergy;
    int energy;
    int usage;
    QString level;
    QString currentPower;
    QString currentEnergy;
    QFile *batteryCapacity;
    QFile *batteryLevel;
    QFile *batteryStatus;
    QFile *batteryCurrentPower;
    QFile *batteryCurrentEnergy;
    QTime *timeLeft;
    QTimer *updateTimer;
    QStringList batteryFolders;
    BatteryError lastError = BatteryError::NoError;
    BatteryStatus status;
    QStandardItemModel *model;

private slots:
    void updateFiles();
};

#endif // BATTERY_H
