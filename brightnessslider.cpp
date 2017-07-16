#include <QDir>
#include <QFile>
#include "brightnessslider.h"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

BrightnessSlider::BrightnessSlider(QWidget *parent) : QSlider(parent)
{
    brightnessFile = new QFile;

    getMaxBrightness();
    setMaximum(100);
    setMinimum(10);

    getBrightness();

    connect(this, &BrightnessSlider::valueChanged, this, &BrightnessSlider::onValueChanged);
}

BrightnessSlider::BrightnessSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent)
{
    brightnessFile = new QFile;

    getMaxBrightness();
    setMaximum(100);
    setMinimum(10);

    getBrightness();

    connect(this, &BrightnessSlider::valueChanged, this, &BrightnessSlider::onValueChanged);
}

void BrightnessSlider::setPosition(QString brightness)
{
    QString percent = brightness.replace("%", "");

    bool ok;
    int position = percent.toInt(&ok);

    if (ok)
        setValue(position);
}

void BrightnessSlider::onChangeBrightness(double brightnessPercent)
{
    QString brightness = QString::number(brightnessPercent) + "%";

    setBrightness(brightnessPercent);
    setBrightnessString(brightness);
    setValue(brightnessPercent);
}

void BrightnessSlider::setMaximum(int max)
{
    QSlider::setMaximum(max);
}

void BrightnessSlider::getBrightness()
{
    setFileName("actual_brightness");
    if (brightnessFile->open(QFile::ReadOnly)) {
        QString brightnessStr = brightnessFile->readAll().replace("\n", "");

        bool ok;
        double brightness = brightnessStr.toDouble(&ok);

        if (ok) {
            brightness = (brightness / maxBrightness) * 100;
            setValue(brightness);

            QString percent = QString::number(value()) + "%";
            setBrightnessString(percent);
        }

        brightnessFile->close();
    }
}

void BrightnessSlider::getMaxBrightness()
{
    setFileName("max_brightness");
    if (brightnessFile->open(QFile::ReadOnly)) {
        QString brightnessStr = brightnessFile->readAll().replace("\n", "");

        bool ok;
        double brightness = brightnessStr.toDouble(&ok);

        if (ok) {
            maxBrightness = brightness;
        }

        brightnessFile->close();
    }
}

void BrightnessSlider::setFileName(QString fileName)
{
    QDir backlightDir("/sys/class/backlight");

    QStringList backlights = backlightDir.entryList(/*QDir::NoDotAndDotDot*/);
    int dotIndex = backlights.indexOf(".");

    while (backlights.count() > 1) {
        if (dotIndex == 0)
            backlights.removeFirst();
        else
            backlights.removeLast();
    }

    if (backlights.count() == 1) {
        backlightDir.cd(backlights.first());
        QString dirPath = backlightDir.path();

        brightnessFile->setFileName(QString(dirPath + "/" + fileName));
    }
}

void BrightnessSlider::setBrightness(double brightness)
{
    XRRScreenResources *resources;
    XRRPropertyInfo *info;
    static Atom backlight;
    RROutput output;
    Display *dpy;
    Window root;

    int screen = 0;
    long value;
    double min;
    double max;
    int o = 0;

    dpy = XOpenDisplay(NULL);
    root = RootWindow(dpy, screen);

    resources = XRRGetScreenResources(dpy, root);
    output = resources->outputs[o];

    backlight = XInternAtom (dpy, "Backlight", True);
    info = XRRQueryOutputProperty(dpy, output, backlight);

    min = info->values[0];
    max = info->values[1];

    XFree(info);
    XRRFreeScreenResources(resources);

    value = brightness * (max - min) + min;

    XRRChangeOutputProperty(dpy, output, backlight, XA_INTEGER,
                    32, PropModeReplace, (unsigned char *) &value, 1);

    XFlush(dpy);
    XSync(dpy, False);
    XCloseDisplay(dpy);
}

void BrightnessSlider::incBrightness(float inc)
{
    setFileName("actual_brightness");
    if (brightnessFile->open(QFile::ReadOnly)) {
        QString brightnessStr = brightnessFile->readAll().replace("\n", "");

        bool ok;
        float brightness = brightnessStr.toFloat(&ok);

        if (ok) {
            float newBrightness;

            brightness = brightness / maxBrightness;
            newBrightness = brightness + inc;
            int sliderValue = newBrightness * 100;
            setValue(sliderValue);
        }

        brightnessFile->close();
    }
}

void BrightnessSlider::decBrightness(float dec)
{
    setFileName("actual_brightness");
    if (brightnessFile->open(QFile::ReadOnly)) {
        bool ok;
        QString brightnessStr = brightnessFile->readAll().replace("\n", "");
        float brightness = brightnessStr.toFloat(&ok);

        if (ok) {
            float newBrightness;

            brightness = brightness / maxBrightness;
            newBrightness = brightness - dec;
            int sliderValue = newBrightness * 100;
            setValue(sliderValue);
        }

        brightnessFile->close();
    }
}

QString BrightnessSlider::brightnessString() const
{
    return m_brightnessString;
}

void BrightnessSlider::setBrightnessString(const QString &value)
{
    m_brightnessString = value;
    emit brightnessStringChanged(value);
}

void BrightnessSlider::onValueChanged(double value)
{
    QString brightness = QString::number(value) + "%";
    double dbl = value / 100.0;
    setBrightness(dbl);

    setBrightnessString(brightness);
    emit brightnessStringChanged(brightness);
}
