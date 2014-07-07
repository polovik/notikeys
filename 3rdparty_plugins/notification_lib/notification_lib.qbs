import qbs 1.0
import qbs.Process

Product {
    //  Project type and dependencies
	type: "dynamiclibrary"
	name: "notification_lib"
    Depends { name: "cpp" }
	Depends { name: "Qt"; submodules: ["core", "network"]  }

    //  Sources
	files: ["NotificationLib.cpp", "NotificationLib.h", "README.txt"]
    cpp.includePaths: ["../../gui/"]
    cpp.linkerFlags: ["-static"]
    cpp.libraryPaths: ["e:/Qt_own/mingw32/i686-w64-mingw32/lib"]
    cpp.staticLibraries: ["ole32", "uuid", "wsock32"]

    //  Store build in separate folder with all depends. Executed by "qbs install"
    Group {
        name: "Generated dll"
        fileTagsFilter: "dynamiclibrary"
        qbs.install: true
//        qbs.installDir: "./plugins/gmail_atom"
    }
}
