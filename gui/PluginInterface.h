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
}

class QQmlContext;

class PluginInterface
{
public:
    virtual ~PluginInterface() {}
    virtual void loadPlugin() = 0;
    virtual void exportToQML(QQmlContext *context) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};


QT_BEGIN_NAMESPACE

#define PluginInterface_iid "notikeys.gui.qt.PluginInterface"

Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)
QT_END_NAMESPACE


#endif
