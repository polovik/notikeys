TEMPLATE      = lib
CONFIG        += plugin
QT            += qml

INCLUDEPATH   += ../..
HEADERS       = \
    Thunderbird.h
SOURCES       = \
    Thunderbird.cpp
TARGET        = $$qtLibraryTarget(thunderbird)
DESTDIR       = ../../build/plugins/thunderbird

OTHER_FILES += \
    Settings.qml \
    thunderbird.json

settingsSource = $$PWD/Settings.qml
logoSource = $$PWD/thunderbird.png
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

