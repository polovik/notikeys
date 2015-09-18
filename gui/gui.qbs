import qbs 1.0
import qbs.Process

Product {
    //  Precompile procedures
    Probe {
        id: updateTranslationsList
        configure: {
            var process = new Process();
            process.exec("lupdate", ["-verbose", "-recursive", "gui", "-ts", "gui/langs/lang_ru_RU.ts"]);
            print("lupdate: " + process.readStdOut() + process.readStdErr());
            process.close();
            found = false;
        }
    }

    //  Project type and dependencies
    type: "application"
    name: "gui"
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "widgets", "network", "qml", "quick", "serialport"] }
    Depends { name: "libdevice" }
    
    //  Sources
    files: ["main.cpp", "PluginsManager.cpp", "ExternalPluginServer.cpp", "QtQuick2ApplicationViewer.cpp", "PluginsManager.h", "PluginInterface.h", "ExternalPluginServer.h", "ExternalPluginProtocol.h", "QtQuick2ApplicationViewer.h" ]
    Group {
        name: "QML files"
        condition: true
        files: "qml/*.qml"
    }
    Group {
        name: "Images"
        condition: true
        files: "qml/images/*"
    }
    Group {
        name: "Language packs"
        condition: true
        files: "langs/*.ts"
    }
    cpp.includePaths: ['.']

    //  Store build in separate folder with all depends. Executed by "qbs install"
    Group {
        name: "GUI application"
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "."
    }
    Group {
        name: "QML resources"
        files: "qml"
        qbs.install: true
        qbs.installDir: "."
    }
    Group {
        name: "Language resources"
        fileTagsFilter: "qm"
        qbs.install: true
        qbs.installDir: "./langs"
    }
}
