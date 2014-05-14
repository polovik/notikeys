@echo off
SET PATH=c:\Qt\Qt5.1.1_mingw\5.1.1\mingw48_32\bin;%PATH%

lrelease.exe gui\langs\lang_ru_RU.ts gui\langs\lang_ru_RU.qm
for /d %%i in (plugins\*) do (
    lrelease.exe %%i\langs\lang_ru_RU.ts %%i\langs\lang_ru_RU.qm
)

PAUSE