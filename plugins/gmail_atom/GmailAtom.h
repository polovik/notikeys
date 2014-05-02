#ifndef GMAILATOM_H
#define GMAILATOM_H

#include <QObject>
#include <QtPlugin>
#include <gui/PluginInterface.h>

class GmailAtom : public QObject, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "notikeys.gui.qt.PluginInterface" FILE "gmail_atom.json")
    Q_INTERFACES(PluginInterface)

public:
    QImage buttonImage() const;
    QByteArray settingsScreenQml() const;
};

#endif // GMAILATOM_H
