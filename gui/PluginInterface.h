#ifndef INTERFACES_H
#define INTERFACES_H

#include <QString>

class PluginInterface
{
public:
    virtual ~PluginInterface() {}
    virtual QString title() = 0;
};


QT_BEGIN_NAMESPACE

#define PluginInterface_iid "notikeys.gui.qt.PluginInterface"

Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)
QT_END_NAMESPACE


#endif
