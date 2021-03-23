set wikiiode=\\192.168.10.21\disk-c\data\apache-docs\html\w-iode

net use x: /d
net use x: %wikiiode% /user:cic

:: Installer
copy iode663*.exe %wikiiode%\data\media\download

:: Manuals
copy ..\man\iode.pdf %wikiiode%\data\media\download
copy ..\man\iode.chm  %wikiiode%\data\media\download
copy ..\man\iode.hlp  %wikiiode%\data\media\download

:: Goodies
copy ..\goodies\* %wikiiode%\data\media\download

:: Wiki pages
copy /Y ..\man\wiki\*.txt %wikiiode%\data\pages
copy /Y ..\man\*.jpg %wikiiode%\data\media\wiki
copy /Y ..\man\*.gif %wikiiode%\data\media\wiki
copy /Y ..\man\*.png %wikiiode%\data\media\wiki

:: Version
copy /Y ..\dos\version.txt %wikiiode%\version.txt

