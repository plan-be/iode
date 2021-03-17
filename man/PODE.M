/* A2M */

<Le programme PODE>
    Le programme PODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le programme PODE est utilis‚ par QODE pour ex‚cuter les commandes li‚es
aux d‚finitions des imprimantes du systŠme dans le fichier PRINTCAP.

Ce programme ne doit normalement pas ˆtre ex‚cut‚ directement par
l'utilisateur de IODE.

&CO
PODE : prints a file (using printcap) and optionnaly restarts a program.
Syntax : pode printername [file [parms ..]] [+ progname arg arg ...]
  where :
    - printername is a valid printer found in the printcap database
    - file is the file to be printed
    - parms are the optionnal parameters for printername
    - + indicates that the subsequent command is to be executed after printing.

&TX
>







