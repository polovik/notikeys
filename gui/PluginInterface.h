#ifndef INTERFACES_H
#define INTERFACES_H

#include <QString>

namespace NS_PLUGIN_INFO {
const QString fieldTitle = "name";
const QString fieldVersion = "version";
const QString fieldDescription = "description";
const QString fieldDependencies = "dependencies";
const QString fieldID = "UID";
const QString fieldLogoFile = "logo";
const QString fieldSettingsFile = "settings";
const QString fieldLanguagePacks = "translations";
}

class QQmlContext;

typedef enum {
    LED_OFF = 0,
    LED_ON  = 1,
    LED_SINGLE_BLINK    = 2,
    LED_RARE_BLINK      = 3,
    LED_FREQUENT_BLINK  = 4,
    LED_BRIGHTNESS_GLIDING  = 5
} LedMode_e;
typedef void (*LedControlFunction)(QString, LedMode_e);

class PluginInterface
{
public:
    virtual ~PluginInterface() {}
    virtual void loadPlugin() = 0;
    virtual void exportToQML(QQmlContext *context) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void analizeExternalEvents(qint32 eventsCount) = 0;
    virtual void handleButtonPressing() = 0;
    void setLedControlFunction(LedControlFunction func, QString pluginUid) { m_controlLed = func; m_uid = pluginUid; }
    void setLedMode(LedMode_e mode) { m_controlLed(m_uid, mode); }

private:
    QString m_uid;
    LedControlFunction m_controlLed;
};


QT_BEGIN_NAMESPACE

#define PluginInterface_iid "notikeys.gui.qt.PluginInterface"

Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)
QT_END_NAMESPACE


#endif
