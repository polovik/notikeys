notikeys
========

For compile on windows, need build QBS (http://qt-project.org/wiki/qbs)
QBS must be build by compiler, which was used for compile Qt Framework.
Sources prefer get from stable snapshots: http://download.qt-project.org/official_releases/qbs/

For build Notikeys from command line on Windows, it's neccessery to add Qt and toolchain in %PATH%:
set PATH=e:\Qt_own\Qt5.3.0\5.3\mingw482_32\bin;%PATH%
set PATH=e:\Qt_own\Qt5.3.0\Tools\mingw482_32\bin;%PATH%
qbs install --install-root build

And run:
build\gui.exe

When build from QtCreator, all necessary files will be stored in folder "install-root" (in temporary build folder)

