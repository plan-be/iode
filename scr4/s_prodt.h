#ifdef SCRCPP
extern "C" {
#endif

/* s_dt*.c */
extern int DT_year(long date);
extern int DT_month(long date);
extern int DT_day(long date);
extern long DT_date_num(long date);
extern long DT_num_date(long day);
extern int DT_bix(long year);
extern int DT_dmy(long date,long d[]);
extern long DT_dmy_date(long d[]);
extern long DT_dmy2000(long date,long d[]);
extern long DT_dmy_date2000(long d[]);
extern int DT_day_w(long date);
extern long DT_add_months(long date,int nbm);
extern long DT_add_days(long date,int nbd);
extern long DT_bus(long date,int dayr);
extern int DT_leave(long date);
extern int DT_diff_months(long d1,long d2);
extern long DT_diff_dates(long d1,long d2);
extern int DT_tsf_30(long vd1[]);
extern long DT_diff_30(long d2,long d1);
extern long DT_date_julian(long date);
extern long DT_julian_date(long jul);
extern int DT_nb_2902(long d1,long d2);
extern int DT_nb_n2902(long d1,long d2);
extern int DT_month_days(long date);
extern int DT_calendar(long date,int bline,int bcol);
extern long SCR_date_to_long(char *string);
extern long SCR_fdate_to_long(char *str,char *fmt);
extern int SCR_check_date(long date);
extern int SCR_check_fmt_date(char *str,char *fmt);
extern long SCR_time_to_long(char *string);
extern long SCR_ftime_to_long(char *str,char *fmt);
extern int SCR_check_time(long hour);
extern int SCR_check_fmt_time(char *str,char *fmt);
/* s_date.c */
extern long SCR_current_date(void);
extern long SCR_current_time(void);
extern char *SCR_long_to_date(long val,char *string);
extern char *SCR_long_to_time(long val,char *string);
extern char *SCR_long_to_fdate(long date,char *str,char *fmt);
extern char *SCR_long_to_ftime(long hour,char *str,char *fmt);
extern int SCR_fmt_dt(char *fmto,char *fmti);
extern int SCR_date_upper_char(int ch);

#ifdef SCRCPP
}
#endif
