TEMPLATE      = lib
CONFIG        += plugin
INCLUDEPATH   += ../..
HEADERS       = \
    GmailAtom.h
SOURCES       = \
    GmailAtom.cpp
TARGET        = $$qtLibraryTarget(gmail_atom)
DESTDIR       = ../../build_plugins/gmail_atom

OTHER_FILES += \
    AccountSettings.qml \
    gmail_atom.json

RESOURCES += \
    gmail_atom.qrc

