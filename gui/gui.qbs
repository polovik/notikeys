import qbs 1.0
import qbs.Process

Product {
    //  Precompile procedures
    Probe {
        id: updateTranslationsList
        configure: {
            var process = new Process();
            process.exec("linguist_up", ["-verbose", "-recursive", "gui", "-ts", "gui/langs/lang_ru_RU.ts"]);
            print("lupdate: " + process.readStdOut() + process.readStdErr());
            process.close();
            found = false;
        }
    }

    //  Project type and dependencies
    type: "application"
    name: "gui"
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "widgets", "network", "qml", "quick"] }
    Depends { name: "libdevice" }
    
    //  Sources
    files: ["main.cpp", "PluginsManager.cpp", "ExternalPluginServer.cpp", "qtquick2applicationviewer/qtquick2applicationviewer.cpp", "PluginsManager.h", "PluginInterface.h", "ExternalPluginServer.h", "ExternalPluginProtocol.h", "qtquick2applicationviewer/qtquick2applicationviewer.h" ]
    Group {
        name: "QML files"
        condition: true
        files: "qml/*.qml"
    }
    Group {
        name: "Language packs"
        condition: true
        files: "langs/*.ts"
    }
    cpp.includePaths: ['.', "qtquick2applicationviewer"]

    //  Store build in separate folder with all depends. Executed by "qbs install"
    destinationDirectory: "../build"
    Group {
        name: "QML resources"
        files: "qml"
        qbs.install: true
        qbs.installDir: "../../build"
    }
    Group {
        name: "Language resources"
        fileTagsFilter: "qm"
        qbs.install: true
        qbs.installDir: "../../build/langs"
    }
}
