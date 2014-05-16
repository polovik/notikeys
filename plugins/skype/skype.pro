TEMPLATE      = lib
CONFIG        += plugin
QT            += qml

INCLUDEPATH   += ../..
HEADERS       = \
    Skype.h
SOURCES       = \
    Skype.cpp
TARGET        = $$qtLibraryTarget(skype)
DESTDIR       = ../../build/plugins/skype

OTHER_FILES += \
    Settings.qml \
    skype.json

settingsSource = $$PWD/Settings.qml
logoSource = $$PWD/skype.png
licenseSource = $$PWD/license.txt
translationsSource = $$PWD/langs/*.qm
resourcesTarget = $$DESTDIR/

win32 {
    settingsSource = $$replace(settingsSource, /, \\)
    logoSource = $$replace(logoSource, /, \\)
    licenseSource = $$replace(licenseSource, /, \\)
    translationsSource = $$replace(translationsSource, /, \\)
    resourcesTarget = $$replace(resourcesTarget, /, \\)
    system(mkdir $$resourcesTarget)
    system(copy /Y /V $$settingsSource $$resourcesTarget)
    system(copy /Y /V $$logoSource $$resourcesTarget)
    system(copy /Y /V $$licenseSource $$resourcesTarget)
    system(xcopy /Y /V $$translationsSource $$resourcesTarget\\langs\\)
}

CONFIG(debug, debug|release) {
    LIBS += -L../../device/debug -ldeviced
} else {
    LIBS += -L../../device/release -ldevice
}

