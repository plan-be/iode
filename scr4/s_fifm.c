#include "s_filein.h"
#include "s_file.h"

int FI_fsmagic(fn, cmt, sb)
char    *cmt, *fn;
struct stat *sb;
{
    int type = FI_UNKNOWN;

    if (stat(fn, sb)) {
	type = FI_UNFOUND;
	FI_strcat(cmt,"Not Found ");
    }
    else {
	switch (sb->st_mode & S_IFMT) {
	case S_IFDIR:
	    type = FI_DIR;
	    FI_strcat(cmt, "Directory ");
	    return(type);
	case S_IFREG:
	    break;
	}
    }

    /*
     * regular file, check next possibility
     */
    if (type != FI_UNFOUND && sb->st_size == 0) {
	type = FI_EMPTY;
	FI_strcat(cmt, "Empty ");
    }

    return(type);
}






