#ifndef BRIGHTNESSSLIDER_H
#define BRIGHTNESSSLIDER_H

#include <QSlider>

class QFile;

class BrightnessSlider : public QSlider
{
    Q_OBJECT
public:
    explicit BrightnessSlider(QWidget *parent = nullptr);
    BrightnessSlider(Qt::Orientation orientation, QWidget *parent);

    QString getBrightnessString() const;
    void setBrightness(double brightness);
    void incBrightness(double inc);
    void decBrightness(double dec);

signals:
    void brightnessStringChanged(QString brightness);

public slots:
    void setPosition(QString brightness);
    void onChangeBrightness(double brightnessPercent);

private:
    void setBrightnessString(const QString &value);
    void setMaximum(int max);
    void getBrightness();
    void getMaxBrightness();
    void setFileName(QString fileName);

    double dblBrightness;
    double maxBrightness;
    QFile *brightnessFile;
    QString brightnessPercent;

private slots:
    void onValueChanged(double value);
};

#endif // BRIGHTNESSSLIDER_H
