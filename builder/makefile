prog = "c:\Program Files (x86)\nsis\makensis.exe"
files = \
	..\man\iode.chm ..\man\readme.htm ..\man\iode.hlp \
	..\dos\iode.exe ..\man\iode.pdf ..\dos\iode.ini

all : PlanningCornerInstaller.exe WinTramoSeatsInstaller.exe

inst : iode663.exe iode663upd.exe

PlanningCornerInstaller.exe: ..\pc\Release\Win32\PlanningCorner.exe iodepc_mui.nsi ..\pc\ml2\pc.parms
	$(prog) iodepc_mui.nsi
	DigiCertUtil.exe sign /noinput /sha1 "c8c28b4300e274f9cf437244f913cc99bfef109b" "PlanningCornerInstaller.exe"

iode663.exe : ..\cmd\iodecmd.exe ..\dos\iode.exe iode_mui.nsi iode.rtf $(files)
	$(prog) iode_mui.nsi
	DigiCertUtil.exe sign /noinput /sha1 "c8c28b4300e274f9cf437244f913cc99bfef109b" "iode663.exe"

iode663upd.exe : ..\cmd\iodecmd.exe ..\dos\iode.exe iode_mui_upd.nsi iode_upd.rtf $(files)
	$(prog) iode_mui_upd.nsi
	DigiCertUtil.exe sign /noinput /sha1 "c8c28b4300e274f9cf437244f913cc99bfef109b" "iode663upd.exe"

WinTramoSeatsInstaller.exe: ..\tramoseats\WinTramoSeats\WinTramoSeats\bin\Release\WinTramoSeats.exe iode_tramoseats.nsi
	$(prog) iode_tramoseats.nsi
	DigiCertUtil.exe sign /noinput /sha1 "c8c28b4300e274f9cf437244f913cc99bfef109b" "WinTramoSeatsInstaller.exe"
