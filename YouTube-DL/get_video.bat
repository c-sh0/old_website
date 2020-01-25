@echo off
rem stupid youtube flv/video downloader/converter
rem
rem
rem Free FLV Player:
rem http://www.martijndevisser.com/blog/flv-player/
rem
rem Free Video format converters
rem mencoder: http://www.mplayerhq.hu/design7/dload.html
rem ffmpeg:   http://arrozcru.no-ip.org/ffmpeg_builds/
rem
rem - nocon
rem 10/17/2008



setlocal

set PATH=%CD%\lib;%CD%\bin;%CD%;%PATH%


set GETVIDEO=http://www.youtube.com/get_video.php?
set SCRATCHFILE=youtube_DL.tmp

if '%1'==[] goto HELP
if "%2"=="" goto HELP

wget -O %SCRATCHFILE% %1

FOR /F "tokens=*" %%i in ('grep watch_fullscreen %SCRATCHFILE% ^| cut -d? -f2  ^| sed "s;.*\(video_id.\+\)&title.*;\1;"') do set VIDEOURL=%%i
del %SCRATCHFILE%



rem mp4 format "http://www.youtube.com/get_video.php?fmt=18"
rem HQ FLV format "http://www.youtube.com/get_video.php?fmt=6"
rem
rem wget -O %2.mp4 "%GETVIDEO%fmt=18&%VIDEOURL%"
rem wget -O %2-hq.flv "%GETVIDEO%fmt=6&%VIDEOURL%"

wget -O %2.flv "%GETVIDEO%%VIDEOURL%"

GOTO END


:HELP
echo "Usage: %0 \"youtube_url\" <outfile>"
GOTO END

:END

endlocal
