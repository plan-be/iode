/* A2M */

<Le programme QODE>
    Le programme QODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le programme QODE convertit des textes ASCII comprenant ou non des
commandes A2M en divers autres formats et permet de lancer l'impression
de fichiers sur divers types d'imprimantes selon des proc‚dures d‚finies
par l'utilisateur. Le programme QODE agit comme le programme TODE … la
diff‚rence qu'il n'offre pas d'interface ‚cran, et donc que les
paramŠtres du filtre doivent ˆtre pass‚s en argument au programme QODE.

A noter que le programme QODE utilise le sous-programme PODE et exige que
les imprimantes du systŠme soient d‚finies dans le fichier PRINTCAP.

Le programme QODE est appel‚ par IODE pour imprimer des fichiers dans les
options "QUICK PRINT" du menu "Transfer":

&EN pour imprimer des textes avec commandes A2M avec les options
&CO
    qode.exe -iode -d prname -o ofile filename [-asis] [-l]

    o—  prname est une imprimante d‚finie dans le fichier PRINTCAP
	ofile  est le fichier output
	filename est le fichier A2M … imprimer
&TX
&EN pour imprimer des textes sans commandes A2M avec les options
&CO
    qode.exe -iode -d prname -ia filename lands - font font - size size
	-linesp linesp -margv margv - margh margh

&TX

Les diff‚rentes options sont d‚finies ci-dessous.

La syntaxe du programme QODE est la suivante (on l'obtient par la
commande "qode -h") :

&CO

     A2m interpretor parameters
     --------------------------
       -d printer   : printer name (from printcap database)
       -c catalog   : use alternate catalog
			(use with .mif printer output only)
       -o filename  : for pseudo printer (sending to file)
       -asis        : tables condensed (instead of splitted)
       -l           : paper orientation (instead of portrait)

     No interpretation
     -----------------
       -ia filename : print file asis (do not interpret a2m cmds)
       -size n.n    : font size (-ia and postscript output)
       -margv n.n   : vertical margins (-ia and postscript output)
       -margh n.n   : horizontal margins (-ia and postscript output)
       -linesp n.n  : line spacing (-ia and postscript output)
       -font name   : font name (-ia and postscript output)

     Miscellaneous
     -------------
       -iode        : restart iode
       -h           : this help message
&TX
>
