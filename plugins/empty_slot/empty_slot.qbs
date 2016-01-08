import qbs 1.0
import qbs.Process

Product {
    //  Precompile procedures
    Probe {
        id: updateTranslationsList
        configure: {
            var process = new Process();
            process.exec("lupdate", ["-verbose", "-no-recursive", "plugins/empty_slot", "-ts", "plugins/empty_slot/langs/lang_ru_RU.ts"]);
            print("lupdate: " + process.readStdOut() + process.readStdErr());
            process.close();
            found = false;
        }
    }

    //  Project type and dependencies
	type: "dynamiclibrary"
    name: "empty_slot"
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "qml", "quick"]  }
    Depends { name: "libdevice" }

    //  Sources
    files: ["Empty.cpp", "Empty.h"]
    Group {
        name: "Plugin metadata"
        files: "*.json"
    }
    Group {
        name: "Language packs"
        files: "langs/*.ts"
    }
    cpp.includePaths: ["../../"]

    //  Store build in separate folder with all depends. Executed by "qbs install"
    Group {
        name: "Generated plugin"
        fileTagsFilter: "dynamiclibrary"
        qbs.install: true
        qbs.installDir: "./plugins/empty_slot"
    }
    Group {
        name: "QML resources"
        files: ["Settings.qml", "empty.png", "license.txt"]
        qbs.install: true
        qbs.installDir: "./plugins/empty_slot"
    }
    Group {
        name: "Language resources"
        fileTagsFilter: "qm"
        qbs.install: true
        qbs.installDir: "./plugins/empty_slot/langs"
    }
}
