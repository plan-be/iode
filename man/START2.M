
/* START 2 */


<D‚marrer IODE et ses accessoires>
    D‚marrer IODE et ses accessoires
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

IODE fonctionne sous certaines versions de UNIX et sous DOS. Nous nous
concentrons dans ce manuel sur la version DOS de IODE.

Deux versions diff‚rentes existent en DOS : un programme DOS
"classique" et une version "32 bits". Nous les distinguerons en les
nommant respectivement version 16 bits (V16) et version 32 bits (V32).

&TI Version 16 bits

Cette version peut fonctionner sur n'importe quel type de PC, y compris
sur un PC dot‚ d'un processeur 8086. Malheureusement, seuls de petits
ensembles de donn‚es pourront ˆtre g‚r‚s dans cette version, car la
m‚moire est exploit‚e de fa‡on habituelle en DOS, c'est-…-dire en
limitant l'espace du programme … 640 KB. Malgr‚ les efforts constants
pour limiter la m‚moire n‚cessaire, les extensions continuelles de IODE
rendent la version 16 bits caduque.

Pour exploiter IODE V16, il faut :

&EN maximiser la m‚moire disponible du DOS en limitant au maximum les
    drivers r‚sidents

&EN installer un driver de m‚moire ‚tendue EMM (par exemple QEMM.SYS).


&TI Version 32 bits

Les ex‚cutables de ces versions du logiciel ont dor‚navant un nom qui se
termine par 32 : iode32, xode32, ... Ceci permet de placer les deux
directories IODE (16 et 32 bits) dans le path et de lancer par exemple
(sans changer de directory ni de path):

	c:\> iode32 (version 32 bits pour les traitements)
	c:\> iode   (version 16 bits pour les impressions ou les graphiques)

    Note : pour exploiter au mieux les possibilit‚s de la m‚moire en DOS,
    ----   il faut placer les lignes suivantes dans le config.sys :

	    DEVICE=C:\DOS\HIMEM.SYS
	    DEVICE=C:\DOS\EMM386.EXE

	   Ceci offre pour le gestionnaire EMM toute la m‚moire du PC, et
	   pour le driver VCPI de DOS4GW ‚galement toute la m‚moire. Ces
	   deux lignes optimisent donc la configuration du PC … la fois
	   pour IODE et IODE32.
>
