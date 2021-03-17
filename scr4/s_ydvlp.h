#ifndef _SCR_YY_H_
#define _SCR_YY_H_

#include "s_yy.h"
#include <stdlib.h>

#define DDEFS       (yy->yy_dyndefs)
#define SDEFS       (yy->yy_stdefs)
#define NBDDEFS     (yy->yy_nb_dyndefs)
#define NBSDEFS     (yy->yy_nb_stdefs)
#define NBIF        (yy->yy_nb_if)

#ifndef POWERPC
/*extern int      strcmp(); /* JMP 29-12-97 */
#endif

extern double   SCR_atof(); /* JMP_M 4.21 29-09-95 */

#endif  /* _SCR_YY_H_ */

