#ifndef _S_DATE_
#define _S_DATE_

/* VARIABLES */
extern char     SCR_TIME_FMT[];
extern char     SCR_DATE_FMT[];
extern int      SCR_Y2K_PIVOT;          /* JMP 28-11-98 */

/* FUNCTIONS */

#ifndef SCRPROTO
extern long     SCR_current_date() ;
extern long     SCR_current_time();
extern long     _SCR_current_date() ;
extern long     _SCR_current_time();
extern char     *SCR_long_to_date();
extern char     *SCR_long_to_time();
extern char     *SCR_long_to_fdate();
extern char     *SCR_long_to_ftime();
extern long     SCR_date_to_long();
extern long     SCR_time_to_long();
extern long     SCR_fdate_to_long();
extern long     SCR_ftime_to_long();
extern long     DT_date_num();
extern long     DT_time_num();
extern long     DT_num_date();
extern long     DT_add_months();
extern long     DT_add_days();
extern long     DT_dmy_date();
extern long     DT_diff_dates();
extern long     DT_date_julian();
extern long     DT_julian_date();
extern long     DT_diff_date_time();
extern long     DT_bus();
extern long     DT_diff_30();
extern long     SCR_cvt_time(), SCR_cvt_date();
extern long     DT_dmy_date2000();
extern long     DT_current_sec();
extern long     DT_secs_1180();

#else
#include <s_prodt.h>
#endif

#endif


