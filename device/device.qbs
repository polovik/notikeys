import qbs 1.0

Product {
	type: "staticlibrary"
	name: "libdevice"
    files: ["Device.cpp", "Device.h", "Settings.cpp", "Settings.h"]
	Depends { name: "Qt"; submodules: ["core"]  }
}
