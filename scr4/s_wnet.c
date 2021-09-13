#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"
#include <winnetwk.h>

/*================================================================
Connect a remote drive. Same as net use F: \\JMP\public for example.

&EN local : local resource name (e.g.: "F:")
&EN remote : remote resource (e.g.: "\\JMP\public")
&EN user : NULL or username for this connection
&EN passwd : NULL or password for user

&RT 0 if successfull, error code if error

&EX
    main(argc, argv)
    int     argc;
    char    *argv[];
    {
	int     rc;
	char    *passwd = 0, *user = 0;

	if(argc < 3) {
	    printf("Usage wnet -d local\n");
	    printf("Usage wnet -c local remote user passwd\n");
	    exit(1);
	    }
	if(argc >= 5) user = argv[4];
	if(argc >= 6) passwd = argv[5];

	if(strcmp(argv[1], "-c") == 0)
	    rc = WscrNetUse(argv[2], argv[3], user, passwd);
	else if(strcmp(argv[1], "-d") == 0)
	    rc = WscrNetDelete(argv[2]);
	printf("rc = %d\n", rc);
    }
&TX
&SA WscrNetDelete()
================================================================ */

WscrNetUse(char *local, char *remote, char *user, char *passwd)
{
    int         rc = 0;
    NETRESOURCE nr;

    memset(&nr, 0, sizeof(nr));
    nr.dwScope = RESOURCE_CONNECTED;
    nr.dwType = RESOURCETYPE_DISK;
    nr.dwDisplayType = 0;
    nr.dwUsage = 0;
    nr.lpLocalName = local;
    nr.lpRemoteName = remote;
    nr.lpComment = 0;
    nr.lpProvider = 0;

    return(WNetAddConnection2(&nr, passwd, user, 0));
};

/*================================================================
Disconnect a network connection. Same as net use f: /delete.

&EN local : resource name to disconnect (e.g.: "F:")

&RT 0 if successfull, error code if error

&EX
    main(argc, argv)
    int     argc;
    char    *argv[];
    {
	int     rc;
	char    *passwd = 0, *user = 0;

	if(argc < 3) {
	    printf("Usage wnet -d local\n");
	    printf("Usage wnet -c local remote user passwd\n");
	    exit(1);
	    }
	if(argc >= 5) user = argv[4];
	if(argc >= 6) passwd = argv[5];

	if(strcmp(argv[1], "-c") == 0)
	    rc = WscrNetUse(argv[2], argv[3], user, passwd);
	else if(strcmp(argv[1], "-d") == 0)
	    rc = WscrNetDelete(argv[2]);
	printf("rc = %d\n", rc);
    }
&TX
&SA WscrNetUse()
================================================================ */

WscrNetDelete(char *local)
{
    return(WNetCancelConnection2(local, 0, 0));
};


/*
main(argc, argv)
int     argc;
char    *argv[];
{
    int     rc;
    char    *passwd = 0, *user = 0;

    if(argc < 3) {
	printf("Usage wnet -d local\n");
	printf("Usage wnet -c local remote user passwd\n");
	exit(1);
	}
    if(argc >= 5) user = argv[4];
    if(argc >= 6) passwd = argv[5];

    if(strcmp(argv[1], "-c") == 0)
	rc = WscrNetUse(argv[2], argv[3], user, passwd);
    else if(strcmp(argv[1], "-d") == 0)
	rc = WscrNetDelete(argv[2]);
    printf("rc = %d\n", rc);
}

*/
#endif /* Win32 */
