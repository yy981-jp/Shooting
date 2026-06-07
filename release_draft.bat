@echo off


set bin=c:\msys64\mingw64\bin
set rel=release

if not exist %rel% md %rel%

xcopy /y /e build\assets %rel%\assets\
copy /y build\st.exe %rel%\


rem dep dll

copy /y "%bin%\SDL2.dll" %rel%\
copy /y "%bin%\SDL2_image.dll" %rel%\
copy /y "%bin%\SDL2_mixer.dll" %rel%\
copy /y "%bin%\SDL2_ttf.dll" %rel%\

copy /y "%bin%\libgcc_s_seh-1.dll" %rel%\
copy /y "%bin%\libstdc++-6.dll" %rel%\
copy /y "%bin%\libwinpthread-1.dll" %rel%\
copy /y "%bin%\libfreetype-6.dll" %rel%\
copy /y "%bin%\libharfbuzz-0.dll" %rel%\

copy /y "%bin%\libavif-16.dll" %rel%\
copy /y "%bin%\libjpeg-8.dll" %rel%\
copy /y "%bin%\libjpeg-8.dll" %rel%\
copy /y "%bin%\libjxl.dll" %rel%\
copy /y "%bin%\libpng16-16.dll" %rel%\
copy /y "%bin%\libmpg123-0.dll" %rel%\
copy /y "%bin%\libtiff-6.dll" %rel%\
copy /y "%bin%\libwebp-7.dll" %rel%\
copy /y "%bin%\libwebpdemux-2.dll" %rel%\
copy /y "%bin%\libbrotlidec.dll" %rel%\

copy /y "%bin%\libFLAC.dll" %rel%\
copy /y "%bin%\libopusfile-0.dll" %rel%\
copy /y "%bin%\libvorbisfile-3.dll" %rel%\
copy /y "%bin%\libwavpack-1.dll" %rel%\

copy /y "%bin%\libxmp.dll" %rel%\
copy /y "%bin%\libbz2-1.dll" %rel%\
copy /y "%bin%\zlib1.dll" %rel%\
copy /y "%bin%\libaom.dll" %rel%\
copy /y "%bin%\libdav1d-7.dll" %rel%\
copy /y "%bin%\librav1e.dll" %rel%\
copy /y "%bin%\libsharpyuv-0.dll" %rel%\
copy /y "%bin%\libdeflate.dll" %rel%\
copy /y "%bin%\libjbig-0.dll" %rel%\
