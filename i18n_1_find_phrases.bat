@echo off
SET PATH=c:\Qt\Qt5.1.1_mingw\5.1.1\mingw48_32\bin;%PATH%

lupdate.exe -verbose -recursive gui -ts gui\langs\lang_ru_RU.ts
for /d %%i in (plugins\*) do (
    lupdate.exe -verbose -recursive %%i -ts %%i\langs\lang_ru_RU.ts
)

PAUSE