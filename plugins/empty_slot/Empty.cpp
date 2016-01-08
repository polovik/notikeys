#include <QDebug>
#include <QQmlContext>
#include "Empty.h"
#include "device/Settings.h"

EmptySlot::EmptySlot(QObject *parent) : QObject(parent)
{
}

EmptySlot::~EmptySlot()
{
}

void EmptySlot::loadPlugin()
{
}

void EmptySlot::exportToQML(QQmlContext *context)
{
    Q_UNUSED(context);
//    context->setContextProperty("EmptySlot", this);
}

void EmptySlot::handleButtonPressing()
{
    qWarning() << "Empty button is pure virtual. It can't be pressed by real button";
}

void EmptySlot::start()
{
}

void EmptySlot::stop()
{
}
