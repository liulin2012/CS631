#define DAYS_YEAR 365
#define SECOND_DAY 86400
#define SIX_MONTH   ((DAYS_YEAR / 2) * SECOND_DAY)


void print_entry(FTSENT *p);
void print_long(FTSENT *p);
void print_col_across(FTSENT *p);
void print_col(FTSENT *p);

int print_name(FTSENT *p, struct stat *st);
void print_time(time_t time);
int print_one_name(FTSENT *p, FORMAT *format);
int print_human(LL size, int length);
int print_kil(LL size, int length);
char print_type(mode_t mode);
