
<Le fichier printcap>
    Le fichier printcap
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le fichier printcap contient la d‚finition de toutes les imprimantes
d‚finies sur le systŠme. Lorsqu'une impression doit avoir lieu dans le
cadre d'une session IODE, le fichier printcap est consult‚ et les
informations qui s'y trouvent permettent de d‚terminer le mode
d'impression ad‚quat.

Le fichier printcap contient trois types de lignes :

&EN des lignes de description commen‡ant par un + et contenant un
    ensemble de champs s‚par‚s par des :

&EN des lignes de commandes suivant la ligne de d‚finition et contenant
    un paramŠtre indiqu‚ par %1 et ‚ventuellement un second %2.
    Lors de l'ex‚cution du programme, le premier paramŠtre est remplac‚
    par le nom du fichier … imprimer et le second, s'il existe, par le
    nom du fichier output.

&EN des lignes de commentaires commen‡ant par un ##

Les champs des lignes de description sont au nombre de quatre :

&EN le nom logique de l'imprimante
&EN le langage de l'imprimante qui indique au programme d'impression
    le langage … utiliser pour envoyer les informations d'impression.
    Seule la premiŠre lettre est prise en compte. Les langages reconnus
    sont :

&EN2 ASCII formatt‚
&EN2 HP Deskjet500
&EN2 Maker Interchange Format (Frame Maker 3.0)
&EN2 Postscript
&EN2 Ritch Text Format (MS Word, AmiPro, etc)
&EN2 Unformatted ASCII

&EN le nombre d'arguments requis pour cette imprimante

&EN la description de l'imprimante en texte libre. C'est ce texte qui
    est pr‚sent‚ … l'utilisateur pour lui permettre de choisir une
    destination d'impression.


Par exemple, un fichier de d‚finition des imprimantes install‚es sur le
systŠme peut ˆtre le suivant :

&CO
    +ps402:Postscript:1:Agfa P400PS laserprinter 2th floor
	net use lpt3: /d
	net use lpt3: suncalc:ps402
	net print %1 lpt3
	net use lpt3: /d
    +lw3:Postscript:1:Apple Laserwriter 3th floor
	net use lpt3: /d
	net use lpt3: suncalc:lw3
	net print %1 lpt3
	net use lpt3: /d
    +lpt1:HP:1:HP Deskjet 500 on lpt1:
	copy %1 lpt1
    +psfile:Postscript:2:Postscript file
	copy %1 %2
    +lpt1:ASCII:1:Standard DOS Printer on lpt1:
	copy %1 lpt1
    ##+lpt2:FASCII:1:Standard DOS Printer on lpt2:
    ##    copy %1 lpt1
&TX

Diff‚rents fichiers printcap peuvent ˆtre d‚finis sur le systŠme, chacun
correspondant … une configuration donn‚e. Ainsi, un fichier peut ne
contenir que les imprimantes locales, un second les imprimantes locales
et les imprimantes r‚seau. Il suffit de copier dans le fichier printcap
le fichier correspondant … la configuration actuelle avant de lancer IODE,
TODE ou GODE.
>




