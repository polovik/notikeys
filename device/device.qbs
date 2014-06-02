import qbs 1.0

Product {
	type: "staticlibrary"
	name: "libdevice"
	files: ["Settings.cpp", "Settings.h"]
	Depends { name: "Qt"; submodules: ["core"]  }
}
