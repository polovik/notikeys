#ifndef INTERFACES_H
#define INTERFACES_H

#include <QString>

namespace NS_PLUGIN_INFO {
const QString fieldTitle = "name";
const QString fieldVersion = "version";
const QString fieldDescription = "description";
const QString fieldDependencies = "dependencies";
}

class PluginInterface
{
public:
    virtual ~PluginInterface() {}
};


QT_BEGIN_NAMESPACE

#define PluginInterface_iid "notikeys.gui.qt.PluginInterface"

Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)
QT_END_NAMESPACE


#endif
