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
    connect(this, &BrightnessSlider::valueChanged, this,&BrightnessSlider::onValueChanged);
}

BrightnessSlider::BrightnessSlider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent)
{
    brightnessFile = new QFile;
    getMaxBrightness();
    setMaximum(100);
    setMinimum(10);
    getBrightness();
    connect(this, &BrightnessSlider::valueChanged, this,&BrightnessSlider::onValueChanged);
}

void BrightnessSlider::setPosition(QString brightness)
{
    QString percent = brightness.replace("%", "");
    int position = percent.toInt();

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
    if(brightnessFile->open(QFile::ReadOnly))
    {
        bool ok;
        QString brightnessStr = brightnessFile->readAll().replace("\n", "");
        double brightness = brightnessStr.toDouble(&ok);

        if(ok)
        {
            brightness = (brightness / maxBrightness) * 100;
            setValue(brightness);

            QString percent = QString::number(value()) + "%";
            setBrightnessString(percent);
            emit brightnessStringChanged(percent);
        }

        brightnessFile->close();
    }
}

void BrightnessSlider::getMaxBrightness()
{
    setFileName("max_brightness");
    if(brightnessFile->open(QFile::ReadOnly))
    {
        bool ok;
        QString brightnessStr = brightnessFile->readAll().replace("\n", "");
        double brightness = brightnessStr.toDouble(&ok);

        if(ok)
        {
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

    while(backlights.count() > 1)
    {
        if(dotIndex == 0)
        {
            backlights.removeFirst();
        }
        else
        {
            backlights.removeLast();
        }
    }

    if(backlights.count() == 1)
    {
        backlightDir.cd(backlights.first());
        QString dirPath = backlightDir.path();

        brightnessFile->setFileName(QString(dirPath + "/" + fileName));
    }
}

void BrightnessSlider::setBrightness(double brightness)
{
    Display *dpy;
    static Atom backlight;
    int screen = 0, o = 0;
    Window root;
    XRRScreenResources *resources;
    RROutput output;
    XRRPropertyInfo *info;
    double min, max;
    long value;

    dpy = XOpenDisplay(NULL);
    backlight = XInternAtom (dpy, "Backlight", True);
    root = RootWindow(dpy, screen);
    resources = XRRGetScreenResources(dpy, root);
    output = resources->outputs[o];
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

void BrightnessSlider::incBrightness(double inc)
{
    double newBrightness;
    setFileName("actual_brightness");
    if(brightnessFile->open(QFile::ReadOnly))
    {
        bool ok;
        QString brightnessStr = brightnessFile->readAll().replace("\n", "");
        double brightness = brightnessStr.toDouble(&ok);

        if(ok)
        {
            brightness = brightness / maxBrightness;
            newBrightness = brightness + inc;
            setBrightness(newBrightness);
        }

        brightnessFile->close();
    }
}

void BrightnessSlider::decBrightness(double dec)
{
    double newBrightness;
    setFileName("actual_brightness");
    if(brightnessFile->open(QFile::ReadOnly))
    {
        bool ok;
        QString brightnessStr = brightnessFile->readAll().replace("\n", "");
        double brightness = brightnessStr.toDouble(&ok);

        if(ok)
        {
            brightness = brightness / maxBrightness;
            newBrightness = brightness - dec;
            setBrightness(newBrightness);
        }

        brightnessFile->close();
    }
}

QString BrightnessSlider::getBrightnessString() const
{
    return brightnessPercent;
}

void BrightnessSlider::setBrightnessString(const QString &value)
{
    brightnessPercent = value;
}

void BrightnessSlider::onValueChanged(double value)
{
    QString brightness = QString::number(value) + "%";
    double dbl = value / 100.0;
    setBrightness(dbl);

    setBrightnessString(brightness);
    emit brightnessStringChanged(brightness);
}
