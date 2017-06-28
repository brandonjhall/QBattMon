#ifndef BRIGHTNESSSLIDER_H
#define BRIGHTNESSSLIDER_H

#include <QSlider>

class QFile;

class BrightnessSlider : public QSlider
{
    Q_OBJECT
public:
    BrightnessSlider(Qt::Orientation orientation, QWidget *parent);
    explicit BrightnessSlider(QWidget *parent = nullptr);

    QString brightnessString() const;

    void setBrightness(double brightness);
    void incBrightness(double inc);
    void decBrightness(double dec);

signals:
    void brightnessStringChanged(QString brightness);

public slots:
    void onChangeBrightness(double brightnessPercent);
    void setPosition(QString brightness);

private:
    void setBrightnessString(const QString &value);
    void setFileName(QString fileName);
    void setMaximum(int max);
    void getMaxBrightness();
    void getBrightness();

    double dblBrightness;
    double maxBrightness;

    QString m_brightnessString;
    QFile *brightnessFile;

private slots:
    void onValueChanged(double value);
};

#endif // BRIGHTNESSSLIDER_H
