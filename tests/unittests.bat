set compiler=vc64
if /I [%1] == [-vc32] set compiler=vc32
if /I [%1] == [-bc32] set compiler=bc32

set iodecmd=..\cmd\%compiler%\iodecmd
set alloclog=-alloclog test.log
::set alloclog=

%iodecmd% -y %alloclog% args.rep 10
%iodecmd% -y %alloclog% calc_ratio.rep
%iodecmd% -y %alloclog% Collectifs_inputs.rep
%iodecmd% -y %alloclog% .cmp.rep
%iodecmd% -y %alloclog% eviews.rep hermreg_5.txt
%iodecmd% -y %alloclog% example.bug.rep
%iodecmd% -y %alloclog% example.convert.rep
%iodecmd% -y %alloclog% example.csv.rep
::%iodecmd% -y %alloclog% example.excel.rep
%iodecmd% -y %alloclog% example.foreach.rep
%iodecmd% -y %alloclog% example.foreach2.rep
%iodecmd% -y %alloclog% example.proc.rep
%iodecmd% -y %alloclog% hh25.rep
%iodecmd% -y %alloclog% hhidt.rep
%iodecmd% -y %alloclog% ldo.rep
%iodecmd% -y %alloclog% PROJECTION_HH_2013.rep
::%iodecmd% -y %alloclog% resfun.rep
%iodecmd% -y %alloclog% sh_dec.rep
%iodecmd% -y %alloclog% test.rep
%iodecmd% -y %alloclog% test2.rep
%iodecmd% -y %alloclog% test3.rep
%iodecmd% -y %alloclog% test4.rep
%iodecmd% -y %alloclog% test5.rep
%iodecmd% -y %alloclog% testapi.rep
%iodecmd% -y %alloclog% testlong.rep
%iodecmd% -y %alloclog% testrtf.rep                    