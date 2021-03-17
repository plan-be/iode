#include <scr4.h>

char *M_mem(char *txt)
{
    SWSTAT  st;

    SW_status(&st);
    sprintf(txt,"NBA=%ld NBB=%ld ALLOC=%ld FREE=%ld USED=%ld",
	    st.st_nballoc - st.st_nbfree,
	    st.st_nbblks,
	    (st.st_convalloc)/(1024 * 1024),
	    (st.st_convafree + st.st_convfree)/(1024L*1024L),
	    (st.st_convalloc - st.st_convafree)/(1024L*1024L));
    return(txt);
}

M_debug(txt, a1, a2, a3, a4, a5, a6)
char    *txt, *a1, *a2, a3, *a4, *a5, *a6;
{
    FILE 	*fd = 0;
    char	buf[512];

    fd = fopen("memdbg.txt", "a+");
    if(fd) {
	fprintf(fd, txt, a1, a2, a3, a4, a5, a6);
	fprintf(fd, "\n+++%s\n", M_mem(buf));
	}
    fclose(fd);
}
