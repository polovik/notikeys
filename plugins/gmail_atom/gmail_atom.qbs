import qbs 1.0
import qbs.Process

Product {
    //  Precompile procedures
    Probe {
        id: updateTranslationsList
        configure: {
            var process = new Process();
            process.exec("linguist_up", ["-verbose", "-no-recursive", "plugins/gmail_atom", "-ts", "plugins/gmail_atom/langs/lang_ru_RU.ts"]);
            print("lupdate: " + process.readStdOut() + process.readStdErr());
            process.close();
            found = false;
        }
    }

    //  Project type and dependencies
	type: "dynamiclibrary"
	name: "gmail_atom"
    Depends { name: "cpp" }
	Depends { name: "Qt"; submodules: ["core", "network", "qml", "quick"]  }
    Depends { name: "libdevice" }

    //  Sources
	files: ["GmailAtom.cpp", "GmailAtom.h"]
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
    destinationDirectory: "../build/plugins/gmail_atom"
    Group {
        name: "QML resources"
        files: ["AccountSettings.qml", "gmail.png", "checking.gif", "license.txt"]
        qbs.install: true
        qbs.installDir: "../../build/plugins/gmail_atom"
    }
    Group {
        name: "Language resources"
        fileTagsFilter: "qm"
        qbs.install: true
        qbs.installDir: "../../build/plugins/gmail_atom/langs"
    }
}
