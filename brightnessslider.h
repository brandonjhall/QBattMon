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

    QString getBrightnessPercent() const;

signals:
    void brightnessChanged(QString brightness);

public slots:
    void setPosition(QString brightness);

private:
    void setBrightnessPercent(const QString &value);
    void setMaximum(int max);
    void getBrightness();
    void getMaxBrightness();
    void setFileName(QString fileName);
    void setBrightness(double brightness);

    double dblBrightness;
    double maxBrightness;
    QFile *brightnessFile;
    QString brightnessPercent;

private slots:
    void onValueChanged(double value);
};

#endif // BRIGHTNESSSLIDER_H
