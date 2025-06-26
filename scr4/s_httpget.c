#include "scr4.h"
#define HTTP_BUFSIZE 512

/******************************************************** 
Lecture d'un fichier via interface http.

&EN host : nom ou addresse ip du host
&EN port : port 
&EN filename : fichier à rechercher

Cette fonction ne gçre pas les cas autres qu'un fichier texte. 

Les CRLF sont remplacés par des CR dans le string retourné. 
C'est ok pour un fichier de texte normal, mais sans doute pas pour des fichiers binaires.

&EX
	char *contents = SCR_HttpGetFile(host, port, filename);
	if(contents) {
		SCR_printf_file(filename, "%s", contents);
		SCR_free(contents);
		}
&TX
&RT : vecteur contenant le fichier
*********************************************************/

char *SCR_HttpGetFile(char *host, int port, char *filename)
{
	int     sock, rc, pos; 
	char    *res = 0, *ptr, buf[HTTP_BUFSIZE + 2];
    extern int WSOCK_DEBUG ;

    WSOCK_DEBUG = 0;

    sock = WSockConnect(host, port);
    if(sock <= 0) {
		Debug("Cannot connect to %s:%d\n", host, port);
		return(res);
	}

	sprintf(buf, "GET /%s HTTP/1/1\r\n", filename);
    rc = WSockBWrite(sock, buf, strlen(buf));
	ptr = "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*\r\n";
	rc = WSockBWrite(sock, ptr, strlen(ptr));
	ptr = "Accept-Language: fr-be\r\n";
	rc = WSockBWrite(sock, ptr, strlen(ptr));
	//ptr = "Accept-Encoding: gzip, deflate\r\n";
	//rc = WSockBWrite(sock, ptr, strlen(ptr));
	ptr = "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322)\r\n";
	rc = WSockBWrite(sock, ptr, strlen(ptr));
	//ptr = "Connection: Keep-Alive\r\n\r\n";
	rc = WSockBWrite(sock, ptr, strlen(ptr));
	
	// Fin de header \r\n\r\n
	sprintf(buf, "Host: %s\r\n\r\n", host);
	ptr = buf;
	rc = WSockBWrite(sock, ptr, strlen(ptr));
	
	// Lecture de la réponse
	rc = WSockRead(sock, buf, HTTP_BUFSIZE);
	buf[HTTP_BUFSIZE] = 0;
	if(rc < 0) goto fin;
	pos = U_index(buf, "\r\n\r\n");
	if(pos < 0) goto fin;
	if(U_index(buf + pos + 4, "404 Not Found") >= 0 ||
	   U_index(buf + pos + 4, "403 Forbidden") >= 0)
		goto fin;
	res = SCR_stracpy(buf + pos + 4);
	while(1) {
		rc = WSockPeek(sock, buf, HTTP_BUFSIZE);
		if(rc <= 0) break;
		rc = WSockRead(sock, buf, rc);
		buf[rc] = 0;
		res = SCR_strafcat(res, buf);
	}
	SCR_replace(res, "\r\n", "\n");
	/*
	//WSockFlush(sock);
	//SCR_sleep(500);
	if(rc < 0)
	    printf("Connection error.\n");
	else {
	    while(1) {
		rc = WSockPeek(sock, buf, 20450);
		if(rc < 0) {
		    printf("ERROR rc=%d\n", rc);
		    break;
		    }
		if(rc == 0) break;
		rc = WSockRead(sock, buf, 20450);
		if(rc > 0) {
		    lg = strlen(buf);
		    j = 0;
		    //while(j + 5 < lg && memcmp(buf + j, "<HTML>", 6)) j++;
		    printf("%s\n", buf + j);
		    }
		else       printf("!!!! ERROR rc=%d\n", rc);
		}
	    }
	}
*/
fin:
    WSockClose(sock);
    return(res);
}
