TEMPLATE      = lib
CONFIG        += plugin
QT            += network qml

INCLUDEPATH   += ../..
HEADERS       = \
    GmailAtom.h
SOURCES       = \
    GmailAtom.cpp
TARGET        = $$qtLibraryTarget(gmail_atom)
DESTDIR       = ../../build/plugins/gmail_atom

OTHER_FILES += \
    AccountSettings.qml \
    gmail_atom.json

settingsSource = $$PWD/AccountSettings.qml
logoSource = $$PWD/gmail.png
progressSource = $$PWD/checking.gif
resourcesTarget = $$DESTDIR

win32 {
    settingsSource = $$replace(settingsSource, /, \\)
    logoSource = $$replace(logoSource, /, \\)
    progressSource = $$replace(progressSource, /, \\)
    resourcesTarget = $$replace(resourcesTarget, /, \\)
    system(copy /Y /V $$settingsSource $$resourcesTarget)
    system(copy /Y /V $$logoSource $$resourcesTarget)
    system(copy /Y /V $$progressSource $$resourcesTarget)
}
