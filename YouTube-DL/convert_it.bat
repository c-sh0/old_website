@echo off
rem stupid youtube flv/video downloader/converter
rem
rem
rem Free FLV Player:
rem http://www.martijndevisser.com/blog/flv-player/
rem
rem Free Video format converters
rem mencoder: http://www.mplayerhq.hu/design7/dload.html
rem
rem ffmpeg:   http://arrozcru.no-ip.org/ffmpeg_builds/
rem ffmpeg -i %2 -ab 64 -ar 22050 -b 500 -s 320x240 -ac 1 %3.mov
rem ffmpeg -i %2 -ab 64 -ar 22050 -b 500 -s 320x240 -ac 1 %3.m4v
rem ffmpeg -i %2 -ab 64 -ar 22050 -b 500 -s 320x240 -ac 1 %3.mp4
rem ffmpeg -i %2 -ab 12 -ar 8000 -b 30 -s 176x144 -ac 1 %3.3gp
rem ffmpeg -i %2 -ab 56 -ar 22050 -b 50 -s 320x240 -ac 1 %3.wmv
rem ffmpeg -i %2 -vcodec xvid -ab 64 -ar 22050 -b 200 -s 320x240 -ac 1 %3.avi
rem ffmpeg -i %2 -ab 128 -ar 22050 -b 500 -s 320x240 -ac 2 %3.mp3
rem
rem youstream.tv files
rem ffmpeg -y -i %2 -vcodec mpeg4 -b 250k -acodec libmp3lame -ar 44100 %3.avi
rem - nocon
rem 10/17/2008

setlocal

set PATH=%CD%\lib;%CD%\bin;%CD%;%PATH%

IF '%1'=='' GOTO HELP
IF '%2'=='' GOTO HELP
IF '%3'=='' GOTO HELP
IF '%1'=='/mpg' GOTO TOMPG
IF '%1'=='/avi' GOTO TOAVI


:TOMPG
IF '%2'=='' GOTO HELP
IF '%3'=='' GOTO HELP
rem ffmpeg -i %2 -ab 56 -ar 22050 -b 500 -s 320x240 -y %3.mpg
ffmpeg -i %2 -y %3.mpg

goto END

:TOAVI
IF '%2'=='' GOTO HELP
IF '%3'=='' GOTO HELP
ffmpeg -i %2 -vcodec msmpeg4v2 -acodec wmav2 -y %3.avi
rem ffmpeg -i %2 -ab 64 -ar 22050 -b 500 -s 320x240 -vcodec msmpeg4v2 -acodec libmp3lame -y %3.avi

goto END

:HELP
echo "Syntax: %0 [/mpg|/avi] infile.flv outfile"
goto END

:END

endlocal


