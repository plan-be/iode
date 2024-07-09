@echo %*
@echo off                            > nul
@echo ============================   >> unittests.log
@echo %*                             >> unittests.log
@echo ----------------------------   >> unittests.log

iodecmd -y -alloclog test.log %*     >> unittests.log
