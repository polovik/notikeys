For correct using this lib, it must contain minimum dependencies.
E.g., Thunderbird's plugin may open library, which have only dependencies with libs in folder c:\windows\system32\ This is restriction of js-ctypes library.

Therefore, needs statically build Qt framework.

set PATH=e:\Qt_own\Qt5.3.0\5.3\mingw482_32\bin;%PATH%
set PATH=e:\Qt_own\Qt5.3.0\Tools\mingw482_32\bin;%PATH%
qbs build --log-level trace release profile:Qt5_2_1_static


cd E:\Qt_own\qbs-1.2.1.src\qbs-1.2.1
set PATH=e:\Qt_own\mingw32\bin\;%PATH%
set PATH=e:\Qt_own\qt_lib_static\bin\;%PATH%
qmake -recursive qbs.pro
mingw32-make