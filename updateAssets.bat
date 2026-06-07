@echo off

xcopy /y assets build\assets\ > nul
xcopy /y /e assets\audio build\assets\audio\ > nul

pushd build\assets
del .gitignore NotoSansJP.ttf
popd
