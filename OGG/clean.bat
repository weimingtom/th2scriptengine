cd ogg_static
rmdir /S /Q Debug Release
del /Q /A:H *.suo 
del /Q *.user *.ncb

cd ..\vorbis_static
rmdir /S /Q Debug Release
del /Q /A:H *.suo 
del /Q *.user *.ncb

cd ..\oggDec
rmdir /S /Q Debug Release
del /Q /A:H *.suo 
del /Q *.user *.ncb

