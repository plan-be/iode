#include "macros.h"

<IODE: source organisation>
IODE: source organisation
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯

Two versions: 
&EN <32 bits IODE version>
&EN <64 bits IODE version>

>
<32 bits IODE version>
32 bits IODE version
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&IT Borland (currently Embarcadero) 32 bits
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
~uLocation~U: c:/usr/iode_src

~uVersionalisation tool~U: SVN

~uSVN server~U: svn.plan.be/iode

..tb
| Directory                | Target                            | Contents 
| ~c./api                  | iode_c_api.lib (32 bits)             | api sources
| ~c./tests/test_c_api     | test1[cpp].exe                    | test1 sources + makefile
| ~c./tests/data           |                                   | data for test1.exe and test1cpp.exe 
| ~c./tests/output         |                                   | results of test1.exe and test1cpp.exe 
| ~c./cmd                  | iodecmd.exe                       | iodecmd sources
| ~c./dos                  | iode.exe                          | GUI DOS sources
| ~c./iodecom              | iodecom.exe                       | sources and make64.bat to generate iodecom.exe
| ~c./man                  | iode.chm, iode.plan.be, readme.htm, iode.pdf  | makechm.bat, makewiki.bat, makereadme.bat, makemif.bat, makedev.bat, makeall.bat
| ~c./man/src              |                                   | manual sources (.m, .m1, .png, .jpg, .gif...)
| ~c./man/src/dev          | DESCRIPTION.md, LEC.md, etc       | developper's manual sources (*.m) 
| ~c./man/bin              | manuals                           | scr4 executables to compile manual pages
| ~c./fun                  |                                   | example model 
| ~c./nsis_installer       | iode{vers}.exe, iodeupd{vers}.exe | sources (*.nsi) + remakeiode.bat to regenerate the whole project
| ~bExternal sources
| ~c../scr4_src            | iode_scr4.lib, s32wo.lib           | scr4 sources files + makefile
..te


&IT Iode for python (MSVC 64 bits)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Location: ~cc:/usr/iode_src~C

..tb
| Directory          | Target                        | Contents 
| ~c./pyiode         | py{vers}/iode.pyd             | makepy.bat, cythonize_iode.py, test_iode.py, .pyx and .pxi sources
| ~c./api/vc64       | iode_c_api.lib (64 bits MSVC)    | 64 bits objects + VC makefile
| ~cscr4_src/vc64    | s4iode.lib (64 bits MSVC)     | 64 bits objects + VC makefile
..te
>


<64 bits IODE version>
64 bits IODE version
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&IT MSVC 64 bits.
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Location: ~c%USERPROFILE%\\source\\repos\\iode~C

Versionalisation tool: ~cgit~C

GitHub repository: ~chttps://github.com/plan-be/iode~C

..tb
| Directory                | Target                            | Contents 
| ~c./api                  | iode_c_api.lib (64 bits MSVC)        | api sources 
| ~c./tests/test_c_api     | test1[cpp].exe, test_c_api.exe    | test1 sources, convert_tests.bat, borland_to_google_test.py
| ~c./tests/data           |                                   | data for test1.exe and test1cpp.exe 
| ~c./tests/output         |                                   | results of test1.exe and test1cpp.exe 
| ~c./cpp_api              | iodecppapi.lib (64 bits MSVC)     | api sources 
| ~c./cmd                  | iodecmd.exe                       | iodecmd sources
| ~c./pyiode               | py{vers}/iode.pyd                 | *.pxi, *.pyx + makepy.bat + cythonize_iode.py + test_iode.py
| ~c./gui                  | iode_gui.exe                      | Qt sources
| ~c./scr4                 | iode_scr4.lib                      | scr4 sources files for iode 
| ~c./man                  | iode.chm, iode.plan.be, readme.htm, iode.pdf  | makechm.bat, makewiki.bat, makereadme.bat, makemif.bat, makedev.bat, makeall.bat
| ~c./man/src              |                                   | manual sources (.m, .m1, .png, .jpg, .gif...)
| ~c./man/src/dev          | DESCRIPTION.md, LEC.md, etc       | developper's manual sources (*.m) 
| ~c./man/bin              | manuals                           | scr4 executables to compile manual pages
..te
>


