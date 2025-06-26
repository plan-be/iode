#include <time.h>

#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"
#include <process.h>

/* ========================================================================
Crée un groupe Windows et y ajoute un élément. Cette version du programme
est compatible Windows 3.x. Sous Windows 95, elle crée un raccourci
pour l'élément ajouté et ajoute un point dans le menu Démarrer/Programmes.

La fonction WscrAddGroupItem() exploite les fonctionnalités DDE de
PROGMAN.

&EN groupname = nom du groupe (à ouvrir ou créer)
&EN cmd = commande à exécuter, y compris ses paramètres éventuels.
    Cette commande ne peut contenir de virgules ni de parenthèses.

&EN title = titre de l'icône de l'élément ajouté
&EN startdir = directory de départ. A préciser si possible. Si cette
    valeur n'est pas fixée (""), le directory est extrait (par Windows)
    de la valeur de cmd. Si cmd contient un argument, le nom extrait est
    incorrect.

&EN show = affichage du groupe
&EN2 0 pour une fenêtre minimisée
&EN2 1 pour une fenêtre normale

Il est à noter que la fenêtre du groupe reste apparente après création
du groupe. L'utilisateur doit lui-même la quitter s'il le souhaite.

D'autre part, aucune valeur de retour ne permet de véifier la bonne
exécution des commandes.

Sous Windows 95, un point est ajouté par cette commande dans le menu
Démarrer/Programmes avec le contenu du groupe.

Sous Windows NT, la commande peut être inopérante en raison des
prérogatives insuffisante de l'utilisateur.

&EX
    WscrAddGroupItem("IODE", "\\iode\\iodew32.exe", "IODE for Win32",
			     "\\usr\\iode", 1);
    WscrAddGroupItem("IODE", "\\windows\\notepad.exe readme.ode",
			     "Lisez-moi", "\\iode", 1);
    WscrAddGroupItem("IODE", "\\iode\\iodew.exe", "IODE for Win16",
			     "\\usr\\iode", 1);
&TX

&SA WscrDeleteGroup()
================================================================== */

WscrAddGroupItem(U_ch *groupname, U_ch *cmd, U_ch *title,
				  U_ch *startdir, int show)
{
    U_ch    buf[256];

    sprintf(buf,"[CreateGroup(\"%s\")]", groupname);
    WscrDdeExecute("PROGMAN", "PROGMAN", buf);
    if(show) sprintf(buf,"[ShowGroup(\"%s\",1)]", groupname);
    else     sprintf(buf,"[ShowGroup(\"%s\",2)]", groupname);
//    Sleep(50);
    WscrDdeExecute("PROGMAN", "PROGMAN", buf);

    if(startdir == 0) startdir = "";
    if(title == 0)    title = cmd;
    sprintf(buf, "[AddItem(%s,\"%s\",,,,,%s)]", cmd, title, startdir);
//    Sleep(50);
    WscrDdeExecute("PROGMAN", "PROGMAN", buf);
    return(0);
}

/* ========================================================================
Détruit un groupe Windows. Cette version du programme
est compatible Windows 3.x.

La fonction WscrAddGroupItem() permet de créer des groupes et d'y ajouter des éléments.

&EN groupname = nom du groupe (à ouvrir ou créer)

Aucune valeur de retour ne permet de véifier la bonne exécution des
commandes.

Sous Windows NT, la commande peut être inopérante en raison des
prérogatives insuffisante de l'utilisateur.

&EX
    WscrDeleteGroup("IODE");
&TX
&SA WscrAddGroupItem()

================================================================== */

WscrDeleteGroup(U_ch *groupname)
{
    U_ch    buf[256];

    sprintf(buf,"[DeleteGroup(\"%s\")]", groupname);
//    Sleep(50);
    WscrDdeExecute("PROGMAN", "PROGMAN", buf);
    return(0);
}

/* ========================================================================
Cette fonction généralise par WscrRegisterFileType().

Enregistre un Mime type et l'extension d'un fichier dans la base de
registres de Windows 95 ou NT. Associe éventuellement une icône et/ou un
programme pour ouvrir le fichier dans l'explorateur Windows.

&EN mime = mime type ("application/x-iodevar"). Si nul, pas
	   d'enregistrement de mime type.
&EN ext = extension du fichier (".var")
&EN code = code associé au type de fichier ("IODE.VAR")
&EN descr = description affiché dans l'explorateur ("Iode Variables")
&EN icon = localisation de l'icône associée à ce type de fichier
    ("C:\\WINDOWS\\SYSTEM\\cool.dll,24")
&EN opencmd = programme à lancer pour ouvrir le fichier avec des
    paramètres éventuels ("C:\\iode\\iode.exe %1")
&EN newcmd = programme à lancer pour créer le fichier avec des
    paramètres éventuels ("C:\\iode\\iode.exe -c %1")
&EN printcmd = programme à lancer pour imprimer le fichier avec des
    paramètres éventuels ("C:\\iode\\iode.exe -p %1")

&RT -1 en cas d'erreur et 0 en cas de succès

&NO L'existence des associations n'est pas vérifiée.
================================================================== */

WscrRegisterMimeType(char *mime, char *ext, char *code, char *descr,
	    char *icon, char *opencmd, char *newcmd, char *printcmd)
{
    int     rc;
    HKEY    phk;
    char    buf[256];

    if(code == 0) return(-1);

Debug("1");
    if(rc = RegCreateKey(HKEY_CLASSES_ROOT, code, &phk)) goto err;
Debug("2");
    if(rc = RegSetValue(phk, "", REG_SZ, descr, strlen(descr) + 1)) goto err;
Debug("3");
    RegCloseKey(phk);
Debug("4");

    // Create a Root entry
    if(rc = RegCreateKey(HKEY_CLASSES_ROOT, ext, &phk)) goto err;
    if(rc = RegSetValue(phk, "", REG_SZ, code, strlen(code) + 1)) goto err;
    if(mime && mime[0]) {
//        if(rc = RegSetValue(phk, "Content Type", REG_SZ, mime, strlen(mime) + 1))
//        if(rc = RegCreateKeyEx(phk, "Content Type", 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &sphk, &dwdummy))
//            goto err;
	if(rc = RegSetValueEx(phk, "Content Type", 0, REG_SZ, mime, strlen(mime) + 1))
	    goto err;
	}
    RegCloseKey(phk);

    if(icon && icon[0]) {
	sprintf(buf, "%s\\DefaultIcon", code);
	if(rc = RegCreateKey(HKEY_CLASSES_ROOT, buf, &phk)) goto err;
	if(rc = RegSetValue(phk, "", REG_SZ,
			 icon, strlen(icon) + 1)) goto err;
	RegCloseKey(phk);
	}

    if(opencmd && opencmd[0]) {
	sprintf(buf, "%s\\shell\\open\\command", code);
	if(rc = RegCreateKey(HKEY_CLASSES_ROOT, buf, &phk)) goto err;
	if(rc = RegSetValue(phk, "", REG_SZ,
			 opencmd, strlen(opencmd) + 1)) goto err;

	RegCloseKey(phk);
	}

    if(printcmd && printcmd[0]) {
	sprintf(buf, "%s\\shell\\print\\command", code);
	if(rc = RegCreateKey(HKEY_CLASSES_ROOT, buf, &phk)) goto err;
	if(rc = RegSetValue(phk, "", REG_SZ,
			 printcmd, strlen(printcmd) + 1)) goto err;
	RegCloseKey(phk);
	}

    if(newcmd && newcmd[0]) {
	sprintf(buf, "%s\\shell\\new\\command", code);
	if(rc = RegCreateKey(HKEY_CLASSES_ROOT, buf, &phk)) goto err;
	if(rc = RegSetValue(phk, "", REG_SZ,
			 newcmd, strlen(newcmd) + 1)) goto err;
	RegCloseKey(phk);
	}

    if(ext && mime && mime[0]) {
	sprintf(buf, "MIME\\Database\\Content Type\\%s", mime);
	if(rc = RegCreateKey(HKEY_CLASSES_ROOT, buf, &phk)) goto err;
	if(rc = RegSetValueEx(phk, "Extension", 0, REG_SZ,
			 ext, strlen(ext) + 1)) goto err;
	RegCloseKey(phk);
	}

    return(0);
err:
    sprintf(buf, "Error : rc=%d, buf= '%s'", rc);
    MessageBox(0, buf, "Error",
		  MB_OK | MB_ICONSTOP | MB_APPLMODAL);
    return(-1);
}

/* ========================================================================
Cette fonction est généralisée par WscrRegisterMimeType().

Enregistre l'extension d'un nom de fichier dans la base de
registres de Windows 95 ou NT. Associe éventuellement une icône et/ou un
programme pour ouvrir le fichier dans l'explorateur Windows.

&EN mime = mime type ("application/x-iodevar"). Si nul, pas
	   d'enregistrement de mime type.
&EN ext = extension du fichier (".var")
&EN code = code associé au type de fichier ("IODE.VAR")
&EN descr = description affiché dans l'explorateur ("Iode Variables")
&EN icon = localisation de l'icône associée à ce type de fichier
    ("C:\\WINDOWS\\SYSTEM\\cool.dll,24")
&EN opencmd = programme à lancer pour ouvrir le fichier avec des
    paramètres éventuels ("C:\\iode\\iode.exe %1")
&EN newcmd = programme à lancer pour créer le fichier avec des
    paramètres éventuels ("C:\\iode\\iode.exe -c %1")
&EN printcmd = programme à lancer pour imprimer le fichier avec des
    paramètres éventuels ("C:\\iode\\iode.exe -p %1")

&RT -1 en cas d'erreur et 0 en cas de succès

&NO L'existence des associations n'est pas vérifiée.
================================================================== */
WscrRegisterFileType(char *ext, char *code, char *descr,
	    char *icon, char *opencmd, char *newcmd, char *printcmd)
{
    return(WscrRegisterMimeType((char *)0, ext, code, descr,
	    icon, opencmd, newcmd, printcmd));
}

/* ========================================================================
Désenregistre un type de fichier dans la base de registres de Windows 95.

&EN ext = extension du fichier (".var")
&EN code = code associé au type de fichier ("IODE.VAR")

Si ext est NULL ou vide, l'action n'a pas lieu et la fonction retourne
0. Si code est NULL ou vide, l'action n'a pas lieu et la fonction
retourne 0.

&RT -1 en cas d'erreur et 0 en cas de succès
================================================================== */

WscrRegisterDeleteFileType(char *ext, char *code)
{
    return(WscrRegisterDeleteMimeType(ext, code, (char *)0));
}

/* ========================================================================
Désenregistre un mime type dans la base de registres de Windows 95 ou NT.

&EN ext = extension du fichier (".var")
&EN code = code associé au type de fichier ("IODE.VAR")
&EN mime = mime type ("application/x-iodevar")

Si ext est NULL ou vide, l'action n'a pas lieu et la fonction retourne
0. Si code est NULL ou vide, l'action n'a pas lieu et la fonction
retourne 0.

&RT -1 en cas d'erreur et 0 en cas de succès
================================================================== */

WscrRegisterDeleteMimeType(char *ext, char *code, char *mime)
{
    char    buf[256];
    int     rc = 0;

    if(WscrRegDeleteKey(ext)) rc = -1;
    if(code && code[0]) {
	sprintf(buf, "%s\\DefaultIcon", code);
	WscrRegDeleteKey(buf);
	sprintf(buf, "%s\\shell\\open\\command", code);
	WscrRegDeleteKey(buf);
	sprintf(buf, "%s\\shell\\open", code);
	WscrRegDeleteKey(buf);
	sprintf(buf, "%s\\shell\\print\\command", code);
	WscrRegDeleteKey(buf);
	sprintf(buf, "%s\\shell\\print", code);
	WscrRegDeleteKey(buf);
	sprintf(buf, "%s\\shell\\new\\command", code);
	WscrRegDeleteKey(buf);
	sprintf(buf, "%s\\shell\\new", code);
	WscrRegDeleteKey(buf);
	sprintf(buf, "%s\\shell", code);
	WscrRegDeleteKey(buf);
	if(WscrRegDeleteKey(code)) rc = -1;
	}
    if(mime && mime[0]) {
	sprintf(buf, "MIME\\Database\\Content Type\\%s", mime);
	if(WscrRegDeleteKey(buf)) rc = -1;
	}
    return(rc);
}

/*NH*/
WscrRegDeleteKey(char *key)
{
    if(key == 0 || key[0] == 0) return(0);
//    Debug("Delete Key : '%s' ... ", key);
    if(RegDeleteKey(HKEY_CLASSES_ROOT, key)) {
//        Debug("Error\n");
	return(-1);
	}
    else {
//        Debug("Ok\n");
	return(0);
	}
}

/* ========================================================================
Enregistre le programme de désinstallation dans le Control Panel de
Windows 95.

&EN code = code associé au logiciel ("IODE")
&EN title = titre à faire apparaître lors de la désinstallation
    ("IODE version 5")
&EN prog= programme de désinstallation (+ paramètres)
    ("C:\\IODE\\uninstall.exe")

&RT -1 en cas d'erreur et 0 en cas de succès
================================================================== */

WscrRegisterUninstall(char *code, char *title, char *exe)
{
    char    buf[512];
    HKEY    phk;

    sprintf(buf, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s", code);
    if(RegCreateKey(HKEY_LOCAL_MACHINE, buf, &phk)) return(-1);
    if(RegSetValueEx(phk, "DisplayName", 0, REG_SZ, title, strlen(title) + 1)) return(-1);
    if(RegSetValueEx(phk, "UninstallString", 0, REG_SZ, exe, strlen(exe) + 1)) return(-1);
    RegCloseKey(phk);

    return(0);
}

/* ========================================================================
Désenregistre les entrées pour la désinstallation dans le Control Panel
de Windows 95.

&EN code = code associé au logiciel ("IODE")

&RT -1 en cas d'erreur et 0 en cas de succès
================================================================== */

WscrRegisterDeleteUninstall(char *code)
{
    char    buf[512];

    if(code == 0 || code[0] == 0) return(0);
    sprintf(buf, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s", code);
    if(RegDeleteKey(HKEY_LOCAL_MACHINE, buf)) return(-1);
    return(0);
}

/* ========================================================================
Enregistre des données propres à une application dans la base de
registres de Windows 95.

&EN ApplCode = code de l'application ("IODE")
&EN ApplVar = nom de la variable ("LastDirectory")
&EN VarValue = valeur de la variable ("c:\\usr\\iode\\maribel")

&RT -1 en cas d'erreur et 0 en cas de succès
&SA WscrRegisterGetApplData(), WscrRegisterDeleteApplData()
================================================================== */

WscrRegisterSetApplData(char *ApplCode, char *ApplVar, char *VarValue)
{
    char    buf[512];
    HKEY    phk;

    sprintf(buf, "SOFTWARE\\SCR-AL1\\%s", ApplCode);
    if(RegCreateKey(HKEY_LOCAL_MACHINE, buf, &phk)) return(-1);
    if(RegSetValueEx(phk, ApplVar, 0, REG_SZ, VarValue, strlen(VarValue) + 1)) return(-1);
    RegCloseKey(phk);
    return(0);
}

/* ========================================================================
Recherche des données propres à une application dans la base de
registres de Windows 95.

&EN ApplCode = code de l'application ("IODE")
&EN ApplVar = nom de la variable ("LastDirectory")
&EN VarValue = valeur de la variable ("c:\\usr\\iode\\maribel"). Le buffer
    doit être assez grand pour contenir la valeur

&RT -1 en cas d'erreur et 0 en cas de succès
&SA WscrRegisterSetApplData(), WscrRegisterDeleteApplData()
================================================================== */

WscrRegisterGetApplData(char *ApplCode, char *ApplVar, char *VarValue)
{
    char    buf[512];
    HKEY    phk;
    DWORD   len;

    sprintf(buf, "SOFTWARE\\SCR-AL1\\%s", ApplCode);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, buf, 0, KEY_QUERY_VALUE, &phk)) return(-1);
    if(RegQueryValueEx(phk, ApplVar, 0, NULL, NULL, &len)) return(-1);
    if(RegQueryValueEx(phk, ApplVar, 0, NULL, VarValue, &len)) return(-1);
    RegCloseKey(phk);
    return(0);
}

/* ========================================================================
Détruit toutes les données propres à une application dans la base de
registres de Windows 95.

&EN ApplCode = code de l'application ("IODE")
&EN ApplVar = nom de la variable ("LastDirectory")
&EN VarValue = valeur de la variable ("c:\\usr\\iode\\maribel")

&RT -1 en cas d'erreur et 0 en cas de succès
&SA WscrRegisterGetApplData(), WscrRegisterSetApplData()
================================================================== */

WscrRegisterDeleteApplData(char *ApplCode)
{
    char    buf[512];

    if(ApplCode == 0 || ApplCode[0] == 0) return(0);
    sprintf(buf, "SOFTWARE\\SCR-AL1\\%s", ApplCode);
    if(RegDeleteKey(HKEY_LOCAL_MACHINE, buf)) return(-1);
    return(0);
}

/* ========================================================================
Recherche une clé REGSZ dans la partie HKEY_LOCAL_MACHINE de la base de
registres de Windows.

&EN KeyName = Nom de la clé "Software\\Microsoft\\Windows\\CurrentVersion\\...\\WinWord.exe")
&EN VarValue = valeur de la variable ("c:\\Program Files\\Microsoft Office\\winword.exe").

Le buffer doit être assez grand pour contenir la valeur de VarValue.

&RT -1 en cas d'erreur et 0 en cas de succès
&SA WscrRegisterSetApplData(), WscrRegisterDeleteApplData()
================================================================== */

WscrRegisterGetValue(char *KeyName, char *VarValue)
{
    HKEY    phk;
    DWORD   len;
    char    path[255], name[255];
    int     i;

    for(i = strlen(KeyName) - 1; i > 0 ; i--)
	if(KeyName[i] == '\\') break;
    strcpy(path, KeyName);
    strcpy(name, KeyName + i + 1);
    path[i] = 0;

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_QUERY_VALUE, &phk)) return(-1);
    if(RegQueryValueEx(phk, name, 0, NULL, NULL, &len)) return(-1);
    if(RegQueryValueEx(phk, name, 0, NULL, VarValue, &len)) return(-1);
    RegCloseKey(phk);
    return(0);
}


/* ===============================================================
Exécute une commande via WinExec sans attendre de retour
================================================================== */

WscrWinExec(char *cmd)
{
    return(WinExec(cmd, SW_SHOW));
}

/* ===============================================================
Exécute une commande via WinExec sans attendre de retour (MINIMIZE)
================================================================== */

WscrWinExecMin(char *cmd)
{
    return(WinExec(cmd, SW_MINIMIZE));
}

/* ===============================================================
Exécute une commande via spawn en bloquant le process appelant
================================================================== */

WscrSystem(char *cmd)
{
    char    **argv;

    argv = SCR_vtom(cmd, ' ');
    spawnv(P_WAIT, argv[0], argv);
    SCR_free_tbl(argv);
}


/* ===============================================================
Retourne le nom du directory de Windows. Le buffer windir doit être au
moins de 128 caractères.

&RT 0 en cas de succès, une valeur non nulle sinon.
================================================================== */

WscrGetWindowsDir(char *windir)
{
    return(GetWindowsDirectory(windir, 127));
}

/* ===============================================================
Retourne le nom du directory system de Windows. Le buffer sysdir doit
être au moins de 128 caractères.

&RT 0 en cas de succès, une valeur non nulle sinon.
================================================================== */

WscrGetSystemDir(char *sysdir)
{
    return(GetSystemDirectory(sysdir, 127));
}


/* ===============================================================
Exécute une commande via CreateProcess en bloquant le process appelant.
Tous les évènements destinés au process appelant sont bloqués pendant la
période d'exécution du sous-programme.

Cette commande fonctionne pour les fichiers .bat, .exe, ..., y compris
en 16 bits. Des paramètres peuvent être passés dans cmd.

Cette crée un nouveau VDM (Virtual DOS Machine), ce qui permet
de gérer correctement les portes COM:, LPT1: sous Win95 et Win98.


&RT 0 en cas de succès, -1 si le process n'a pu être créé.

&EX
    WscrCreateProcess("mytest.bat DIR1 c:\\tmp");

&TX
================================================================== */

WscrCreateProcess(cmd)
char    *cmd;
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    int                 rc;
    DWORD               ret;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    memset(&sa, 0, sizeof(sa));
    si.cb = sizeof(si);
    sa.nLength = sizeof(sa);

    rc = CreateProcess(NULL,
		     cmd,
		     NULL,
		     NULL,
		     FALSE,
		     NORMAL_PRIORITY_CLASS | CREATE_SEPARATE_WOW_VDM,
		     NULL,
		     NULL,
		     &si,
		     &pi);

    if(rc != TRUE) return(-1);
    while(1) {
	ret = WaitForSingleObject(pi.hProcess, 100);
	if(ret != WAIT_TIMEOUT) break;
    }

    return(0);
}

/* ===============================================================
Exécute un programme console (ou autre) 16 ou 32 bits via CreateProcess
en bloquant le process appelant. Tous les évènements destinés au process
appelant sont bloqués pendant la période d'exécution du sous-programme.

Cette commande fonctionne pour les fichiers .bat, .exe, ..., y compris
en 16 bits. Des paramètres peuvent être passés dans cmd.

Ne fonctionne pas pour des commandes interne au shell comme copy ou del.

&EN cmd : commande à exécuter
&EN win : avec (1) ou sans (1) fenêtre

&RT 0 en cas de succès, -1 si le process n'a pu être créé.

&EX
    WscrCreateConsoleProcess("mytest.bat DIR1 c:\\tmp", 0);

&TX
================================================================== */

WscrCreateConsoleProcess(cmd, win)
char    *cmd;
int     win;
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;
    int                 rc;
    DWORD               ret, flags;

    flags = NORMAL_PRIORITY_CLASS;
    if(win == 0) flags |= CREATE_NO_WINDOW;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    memset(&sa, 0, sizeof(sa));
    si.cb = sizeof(si);
    sa.nLength = sizeof(sa);

    rc = CreateProcess(NULL,
		     cmd,
		     NULL,
		     NULL,
		     FALSE,
//                     NORMAL_PRIORITY_CLASS | CREATE_SEPARATE_WOW_VDM,
		     flags,
		     NULL,
		     NULL,
		     &si,
		     &pi);

    if(rc != TRUE) return(-1);
    while(1) {
	ret = WaitForSingleObject(pi.hProcess, 100);
	if(ret != WAIT_TIMEOUT) break;
    }

    return(0);
}

/* ===============================================================
Exécute le programme associé au fichier passé comme argument. Les associations sont
celles de Windows. Par exemple, un fichier .htm démarrera le browser par défaut.

&EN filename : nom du fichier à "éditer"

RT 0 en cas de succès, -1 si le process n'a pu être créé.

&EX
    WscrShellExec("index.htm");   Lance le browser par défaut
    WscrShellExec("readme.txt");  Lance notepad

&TX
================================================================== */


WscrShellExec(filename)
char    *filename;
{
    SHELLEXECUTEINFO    sei;
    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_DDEWAIT;

    sei.hwnd = WscrGetMainWnd();
    sei.lpFile = filename;
    sei.nShow = SW_MAXIMIZE; /* JMP 03-02-99 */
    return(ShellExecuteEx(&sei));
}

/* ===============================================================
Vérifie l'existence de fonctions dans une DLL et retourne le pointeur vers
la fonction le cas échéant.

&EN dll : nom de la dll
&EN fn : fonction recherchée

RT 0 en cas de succès, -1 si une des fonctions manquent

&EX
    WscrGetDllFnPtr("Winspool.lib", "GetDefaultPrinterA");

&TX
================================================================== */

void *WscrGetDllFnPtr(char *dllname, char *fnname)
{
    HINSTANCE   inst;
    void        *fnptr;

    inst = LoadLibrary(dllname);
    if(inst < 0) return(-1);

    return((void *) GetProcAddress(inst, fnname));
}



#endif













