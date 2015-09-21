import qbs 1.0

Product {
	type: "staticlibrary"
	name: "libdevice"
    files: [
        "Device.cpp",
        "Device.h",
        "Settings.cpp",
        "Settings.h",
        "UartPort.cpp",
        "UartPort.h",
        "FSM.cpp",
        "FSM.h",
        "protocol_pc_to_device.h",
    ]
    Depends { name: "Qt"; submodules: ["core", "serialport"]  }
    Depends { name: "cpp" }
//    cpp.defines: ["--std=c++0x"]
    cpp.cxxFlags: ["--std=c++0x"]
}
