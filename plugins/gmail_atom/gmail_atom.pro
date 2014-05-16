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
licenseSource = $$PWD/license.txt
translationsSource = $$PWD/langs/*.qm
resourcesTarget = $$DESTDIR

win32 {
    settingsSource = $$replace(settingsSource, /, \\)
    logoSource = $$replace(logoSource, /, \\)
    progressSource = $$replace(progressSource, /, \\)
    licenseSource = $$replace(licenseSource, /, \\)
    translationsSource = $$replace(translationsSource, /, \\)
    resourcesTarget = $$replace(resourcesTarget, /, \\)
    system(mkdir $$resourcesTarget)
    system(copy /Y /V $$settingsSource $$resourcesTarget)
    system(copy /Y /V $$logoSource $$resourcesTarget)
    system(copy /Y /V $$progressSource $$resourcesTarget)
    system(copy /Y /V $$licenseSource $$resourcesTarget)
    system(xcopy /Y /V $$translationsSource $$resourcesTarget\\langs\\)
}

CONFIG(debug, debug|release) {
    LIBS += -L../../device/debug -ldeviced
} else {
    LIBS += -L../../device/release -ldevice
}

