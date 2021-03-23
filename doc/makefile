#############################################################
##                      MANUELS                            ##
#############################################################
MFLAGS = -e ; -f . -wm
HFLAGS = -e . -f ; -wm -s

MFILES = macros.h iode.m objs.m lec.m \
	file.m wspace.m data.m compute.m print.m report.m misc.m \
	licence.m1 intro.m1 organ.m lexique.m rep1.m rep2.m ascii.m \
	inter.m sample.m printcap.m syntax.m rep0.m \
	reports.m repn.m methods.m

userman.mif : userman.m $(MFILES)
	scr4w_h $(MFLAGS) -c userman.m -o userman.hlp
	scr4w_hm -c ~ -i userman -t usersman
	del userman.hlp
	scr4w_a2f -i userman.a2m -o userman.mif
#        del userman.a2m
	move userman.mif rf

#iode.hlp : help.m $(MFILES)
#        echo scr4w_h $(HFLAGS) -c help.m -o iode.hlp
#        scr4w_h $(HFLAGS) -c help.m -o iode.hlp
#        move iode.hlp ..\iode.hlp

refman.mif : refman.m $(MFILES)
	scr4w_h $(MFLAGS) -c refman.m -o refman.hlp
	scr4w_hm -c ~ -i refman -t refman
	del refman.hlp
#        scr4w_a2m -lf_off -gif -noref refman.a2m -o refman.mif
	scr4w_a2f -noref -i refman.a2m -o refman.mif
	del refman.a2m
	move refman.mif rf

refman.rtf : refman.m $(MFILES)
	scr4w_h $(MFLAGS) -c refman.m -o refman.hlp
	scr4w_hm -c ~ -i refman -t refman
	del refman.hlp
	qode -d rtffile -o refman.rtf refman.a2m
	del refman.a2m
	move refman.rtf rf

refman.htm : refman.m $(MFILES)
	scr4w_h $(MFLAGS) -c refman.m -o tmp.hlp
	scr4w_hm -c ~ -i tmp -t refman
	del tmp.hlp
	qode -parb 0 -pari 0 -d htmlfile -o tmp.htm tmp.a2m
	del tmp.a2m
	scr4_spl -b "fonddoc.gif" -u "backw.gif" -i tmp.htm -o ref -l 3 -t "Manuel de reference de IODE"
	del tmp.htm
	scr4_toc -i ref.htm -o toc.htm -l 4
	copy refman.ht1 ref.htm
	scr4_sbs -s "<!STARTTOC>" "<!ENDTOC>" toc.htm ref.htm
	scr4_sbs -s "<!STARTHEADER>" "<!ENDHEADER>" header.ht1 ref*.htm
	scr4_sbs -s "<!STARTFOOTER>" "<!ENDFOOTER>" footer.ht1 ref*.htm

userman.htm : userman.m $(MFILES)
	scr4w_h $(MFLAGS) -c userman.m -o tmp.hlp
	scr4w_hm -c ~ -i tmp -t usersman
	del tmp.hlp
	qode -parb 0 -pari 0 -d htmlfile -o tmp.htm tmp.a2m
	del tmp.a2m
	scr4_spl  -b "fonddoc.gif" -u "backw.gif" -i tmp.htm -o user -l 4 -t "Manuel de l'utilisateur de IODE"
	del tmp.htm
	scr4_toc -i user.htm -o toc.htm -l 4
	copy userman.ht1 user.htm
	scr4_sbs -s "<!STARTTOC>" "<!ENDTOC>" toc.htm user.htm
	scr4_sbs -s "<!STARTHEADER>" "<!ENDHEADER>" header.ht1 user*.htm
	scr4_sbs -s "<!STARTFOOTER>" "<!ENDFOOTER>" footer.ht1 user*.htm

repman.htm : repman.m $(MFILES)
	scr4w_h $(MFLAGS) -c repman.m -o tmp.hlp
	scr4w_hm -c ~ -i tmp -t repman
	del tmp.hlp
	qode -parb 0 -pari 0 -d htmlfile -o tmp.htm tmp.a2m
	del tmp.a2m
	scr4_spl -b "fonddoc.gif" -u "backw.gif" -i tmp.htm -o rep -l 4 -t "Rapports de IODE"
	del tmp.htm
	scr4_toc -i rep.htm -o toc.htm -l 4
	copy repman.ht1 rep.htm
	scr4_sbs -s "<!STARTTOC>" "<!ENDTOC>" toc.htm rep.htm
	scr4_sbs -s "<!STARTHEADER>" "<!ENDHEADER>" header.ht1 rep*.htm
	scr4_sbs -s "<!STARTFOOTER>" "<!ENDFOOTER>" footer.ht1 rep*.htm

readme.htm : readme.m $(MFILES) readme.ht1
	scr4w_h -e ; -f . -wm -c readme.m -o tmp.hlp
	scr4w_hm -c ~ -i tmp
	del tmp.hlp
	echo .pardef parb html=2 bold=yes > tmp1.a2m
	echo .pardef pari html=3 bold=yes >> tmp1.a2m
#        echo .pardef tit_0 justify=L html=1 size=12 color=red underline=yes>> tmp1.a2m
	type tmp.a2m >> tmp1.a2m
	scr4w_ah -i tmp1.a2m -o tmp1.htm -fs 8 -title "IODE Last Updates" -style stylereadme.css -font T -strip 1
	scr4w_toc -i tmp1.htm -o toc.htm -l 1
#        echo \<FONT SIZE=-2 FACE="ARIAL"> > tmp1.htm
#        type toc.htm >> tmp1.htm
#        echo </FONT> >> tmp1.htm
	copy readme.ht1 readme.htm
# Suppression de la toc trop grande
#        s4sbs -s "<!STARTTOC>" "<!ENDTOC>" toc.htm readme.htm
	s4sbs -s "<!STARTTEXT>" "<!ENDTEXT>" tmp1.htm readme.htm
#        s4sbs -s "<!STARTHEADER>" "<!ENDHEADER>" readme.ht1 read*.htm
	copy readme.htm ..

readme2.htm : readme.m $(MFILES) readme.ht1
	scr4w_h $(MFLAGS) -c readme.m -o tmp.hlp
	scr4w_hm -c ~ -i tmp
	del tmp.hlp
	echo .pardef parb html=2 color=blue > tmp1.a2m
	echo .pardef pari html=3  >> tmp1.a2m
	echo .pardef tit_0 justify=L html=1 size=12 color=red underline=yes>> tmp1.a2m
	type tmp.a2m >> tmp1.a2m
	scr4w_ah -i tmp1.a2m -o readme2.htm -fs 8 -font Times -title "IODE Last Updates"
	s4spl -b "fonddoc.gif" -i readme2.htm -o readm -l 1 -t "Last Updates"
	scr4w_toc -i readm.htm -o toc.htm -l 1
#        echo \<FONT SIZE=-2 FACE="ARIAL"> > tmp1.htm
#        type toc.htm >> tmp1.htm
#        echo </FONT> >> tmp1.htm
	copy readme.ht1 readtoc.htm
	s4sbs -s "<!STARTTOC>" "<!ENDTOC>" toc.htm readtoc.htm
#        scr4_sbs -s "<!STARTHEADER>" "<!ENDHEADER>" toc.htm tmp.htm
#        scr4_sbs -s "<!STARTFOOTER>" "<!ENDFOOTER>" footer.ht1 read*.htm

repsyn.mif : repsyn.m
	scr4w_h $(MFLAGS) -c repsyn.m -o repsyn.hlp
	scr4w_hm -c ~ -i repsyn -t syntaxedesrapports
	del repsyn.hlp
	scr4w_a2m -lf_off -gif -noref repsyn.a2m -o repsyn.mif
	move repsyn.mif rf
#       del repsyn.a2m


cannevas:
	scr4w_h -e ; -f . -c iode_ab.can
	scr4w_hm -e ; -i iode_ab.hlp
	scr4w_h -e ; -f . -c gode_ab.can
	scr4w_hm -e ; -i gode_ab.hlp
	scr4w_h -e ; -f . -c tode_ab.can
	scr4w_hm -e ; -i tode_ab.hlp
	scr4w_h -e ; -f . -c xode_ab.can
	scr4w_hm -e ; -i xode_ab.hlp




#############################################################
##                    BACKUP MANUELS                       ##
#############################################################

odeman.pak :
	-del odeman.pak
	pak A odeman.pak *.m *.m1 macros.h *. *.can

backa : odeman.pak
	copy odeman.pak a:

backb : odeman.pak
	copy odeman.pak b:

back : odeman.pak
	copy odeman.pak back

#############################################################
##                       SCR3_LP                           ##
#############################################################



repman.mif : repman.m $(MFILES)
	scr4w_h $(MFLAGS) -c repman.m -o repman.hlp
	scr4w_hm -c ~ -i repman -t repman
	del repman.hlp
#        scr4w_a2m -lf_off -gif -noref repman.a2m -o repman.mif
	scr4w_a2f -noref -i repman.a2m -o repman.mif
#        del repman.a2m

######## WINDOWS ######################

iodew95.hlp : refman.m repman.m readme.m userman.m $(MFILES) \
	refman.m1 repman.m1 userman.m1
	scr4w_h $(MFLAGS) -c iodew95.m -o tmp -log tmp.lg
	scr4w_hm -c ~ -i tmp -dup @iodew95
	del tmp.hlp
	move tmp.a2m iodew95.a2m
	scr4w_a2r -hcw -fs 9 -i iodew95.a2m -compress 1 -title "IODE" -copyr "Federal Planning Bureau"
#        del iodew95.a2m
	c:\scr\hcw\hcw /C/A iodew95

iode.hlp : iodew.m userman.m userman.m1 readme.m $(MFILES)
	scr4w_h $(MFLAGS) -c iodew.m -o tmp -log tmp.lg
	scr4w_hm -c ~ -i tmp -t Manueldelutilisateur Notestechniques
	del tmp.hlp
	echo .pardef parb html=5 color=blue > iode.a2m
	echo .pardef pari html=6 color=blue >> iode.a2m
	echo .topicslink case=no>> iode.a2m
	type tmp.a2m >> iode.a2m
	scr4w_ar -gif2bmp -hcw -fs 9 -i iode.a2m -compress 1 -title "IODE" -copyr "Federal Planning Bureau"
#        del iode.a2m
	c:\scr\hcw\hcw /C/A iode

iode.chm : iodew.m userman.m userman.m1 readme.m $(MFILES) stylechm.css
	scr4w_h $(MFLAGS) -c iodew.m -o tmp -log tmp.lg
	scr4w_hm -c ~ -i tmp -o tmp.a2m -t Manueldelutilisateur Notestechniques -it
#        scr4w_hm -c ~ -i tmp -o tmp.a2m -it
	echo .pardef parb html=5 color=blue > iode.a2m
	echo .pardef pari html=6 color=blue >> iode.a2m
	echo .pardef cmd_1 color=black bold=n >> iode.a2m
	echo .topicslink case=no>> iode.a2m
	type tmp.a2m >> iode.a2m
#        del tmp.hlp
	scr4w_ah -gif2bmp -i iode.a2m -o iode.htm -tcolor 0 -tborder 0 -hcw -title IODE -style stylechm.css -font Times
#
	-"C:\Program Files (x86)\HTML Help Workshop\hhc" iode.hhp
#        -hhc iode.hhp
#       del t*.htm

a2m.hlp : a2m.m a2mman.m
	scr4w_h $(MFLAGS) -c a2mman.m -o tmp -log tmp.lg
	scr4w_hm -c ~ -i tmp -t LeProgrammeA2M Syntaxedesfichiersa2m
	del tmp.hlp
	move tmp.a2m a2m.a2m
	scr4w_a2r -gif2bmp -hcw -fs 9 -i a2m.a2m -compress 1 -title "A2M" -copyr "Federal Planning Bureau"
#        del a2m.a2m
	c:\scr\hcw\hcw /C/A a2m



readpw.htm : readme.m $(MFILES) readpw.ht1
	scr4w_h $(MFLAGS) -c readme.m -o tmp.hlp
	scr4w_hm -c ~ -i tmp
	del tmp.hlp
	echo .pardef parb html=2 color=blue > tmp1.a2m
	echo .pardef pari html=3  >> tmp1.a2m
	echo .pardef tit_0 justify=L html=1 size=12 color=red underline=yes>> tmp1.a2m
	type tmp.a2m >> tmp1.a2m
	scr4w_ah -i tmp1.a2m -o readpwb.htm -fs 8 -title "IODE Last Updates"
	scr4w_spl -b "fonddoc.gif" -u "backw.gif" -i readpwb.htm -o readpw -l 1 -t "Last Updates"
	scr4w_toc -i readpwb.htm -o toc.htm -l 2
#        echo \<FONT SIZE=-2 FACE="ARIAL"> > tmp1.htm
#        type toc.htm >> tmp1.htm
#        echo </FONT> >> tmp1.htm
	copy readpw.ht1 readpwtoc.htm
	s4sbs -s "<!STARTTOC>" "<!ENDTOC>" toc.htm readpwtoc.htm
	s4sbs -s "<!STARTHEADER>" "<!ENDHEADER>" toc.htm tmp.htm
	s4sbs -s "<!STARTFOOTER>" "<!ENDFOOTER>" footer.ht1 readpw*.htm


iode.htm : iode.a2m
	scr4w_ah  -hcw -paranum 0 -i iode.a2m -o iode.htm -tcolor 0 -tborder 0 -title Iode -linkstyle styleweb.css -font Times
	s4sbs -s "<!STARTSTYLE>" "<!ENDSTYLE>" styletoc.htm iode.toc.htm

iodemif1.a2m : iodewmif1.m usermanmif1.m usermanmif1.m1 $(MFILES) style.css
	scr4w_h -e ; -f . -wm -c iodewmif1.m -o tmp -log tmp.lg
	scr4w_hm -c ~ -i tmp -o tmp.a2m -t Manueldelutilisateur -it
#        echo .pardef parb html=5 color=blue > iode.a2m
#        echo .pardef pari html=6 color=blue >> iode.a2m
	echo .pardef parb color=blue > iodemif1.a2m
	echo .pardef pari color=blue >> iodemif1.a2m
	echo .topicslink case=no>> iodemif1.a2m
	type tmp.a2m >> iodemif1.a2m

#GRECT = 15 8.5
#GRECT = 15 11.5
GRECT = 13 9.5
GDPI = 140

iode1.mif : c:\scr\bin\scr4w_af.exe iodemif1.a2m
	scr4w_af -gdpi $(GDPI) -grect $(GRECT) -gif2bmp -i iodemif1.a2m -o iode1.mif 

iodemif2.a2m : usermanmif2.m $(MFILES) style.css
	scr4w_h -e ; -f . -wm -c usermanmif2.m -o tmp -log tmp.lg
	scr4w_hm -c ~ -i tmp -o tmp.a2m -t Annexes1 -it
#        echo .pardef parb html=5 color=blue > iode.a2m
#        echo .pardef pari html=6 color=blue >> iode.a2m
	echo .pardef parb color=blue > iodemif2.a2m
	echo .pardef pari color=blue >> iodemif2.a2m
	echo .topicslink case=no>> iodemif2.a2m
	type tmp.a2m >> iodemif2.a2m

iode2.mif : iodemif2.a2m
	scr4w_af -gdpi $(GDPI) -grect $(GRECT) -gif2bmp -i iodemif2.a2m -o iode2.mif

#iode.mif : iode1.mif iode2.mif

iode.mif : iode.a2m
	scr4w_af -grect 15 8.5 -gif2bmp -i iode.a2m -o iode.mif

PLANDIR =  \\192.168.10.11\C$\data\apache-docs\html\new_planweb\websites\iodeman
EXTRADIR =  \\192.168.10.11\c$\data\apache-docs\html\extra\admin\uploaded

copyplanweb : iode.htm
	net use \\192.168.10.11\C$
	copy frame.htm $(PLANDIR)\index.htm
	copy styleweb.css $(PLANDIR)
	copy styletoc.css $(PLANDIR)
	copy iode.toc.htm $(PLANDIR)
	copy *.gif $(PLANDIR)
	copy *.jpg $(PLANDIR)
	copy T*.htm $(PLANDIR)


copyreadmeplanweb : readme.htm
	copy readme.htm $(PLANDIR)

#  Un- / comment next lines to test locally / publish on wiki.plan.be
#wikiiode = \\192.168.10.11\c$\data\apache-docs\html\w-iode
wikiiode = C:\Apache24\htdocs\w-iode
iode.wiki : iodew.m userman.m userman.m1 readme.m $(MFILES) stylechm.css
	scr4w_h $(MFLAGS) -c iodew.m -o tmp -log tmp.lg
	scr4w_hm -c ~ -i tmp -o tmp.a2m -t Manueldelutilisateur Notestechniques -it
	echo .pardef parb html=5 color=blue > iode.a2m
	echo .pardef pari html=6 color=blue >> iode.a2m
	echo .pardef cmd_1 color=black bold=n >> iode.a2m
	echo .topicslink case=no>> iode.a2m
	type tmp.a2m >> iode.a2m
	del tmp.a2m
	-md wiki
	cd wiki
	copy ..\iode.a2m
	-scr4w_ad -gif2bmp -autolink 1 -paranum 0 -i iode.a2m  -title IODE -ini doku.ini -toc 2
    copy /Y *.txt $(wikiiode)\data\pages
	cd ..
       -copy /Y *.jpg $(wikiiode)\data\media\wiki
       -copy /Y *.gif $(wikiiode)\data\media\wiki
       -copy /Y *.png $(wikiiode)\data\media\wiki




