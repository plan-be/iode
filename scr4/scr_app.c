#include <stdio.h>

main(argc, argv)
int     argc;
char    *argv[];
{
    FILE    *fd, *fd1;
    int     ch;
    long    pos;

    SCR_version();
    if(argc !=3) {
	printf("SCR4_APP : appends a file to a program\n");
	printf("Syntax : scr4_app progname.exe filename\n");
	exit(1);
	}
#ifdef DOS
    fd = fopen(argv[1], "rb+");
#else
    fd = fopen(argv[1], "r+");
#endif
    if(fd == 0) {
	fprintf(stderr, "Cannot open %s\n", argv[1]);  /* JMP 16-01-96 */
	exit(1);
	}
#ifdef DOS
    fd1 = fopen(argv[2], "rb");
#else
    fd1 = fopen(argv[2], "r");
#endif
    if(fd1 == 0) {
	fprintf(stderr, "Cannot open %s\n", argv[2]);  /* JMP 16-01-96 */
	exit(1);
	}
    fseek(fd, 0L, 2);
    pos = ftell(fd);
    while(1) {
	ch = getc(fd1);
	if(ch == EOF) break;
	putc(ch, fd);
	}
    fwrite(&pos, sizeof(long), 1, fd);
    fwrite("\002\003\004\005", 4, 1, fd);       /* JMP_M 4.19 08-05-95 */

    fclose(fd);
    fclose(fd1);
    exit(0);
}



