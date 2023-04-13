@echo %*
@echo off                            > nul
@echo ============================   >> unittests.log
@echo %*                             >>  unittests.log
@echo ----------------------------   >> unittests.log
set exepath=..\..
if exist ..\exedir.txt set /P exepath=<..\exedir.txt
set exepath=%exepath:/=\%\cmd
%exepath%\iodecmd -y -alloclog test.log %*     >> unittests.log
