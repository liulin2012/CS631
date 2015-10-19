#define SIZE_LEN 20
#ifndef S_IFWHT
	#define S_IFWHT  0160000
#endif

#ifdef __linux__
#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#endif

#ifdef __APPLE__
#include <uuid/uuid.h>
#endif


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <ctype.h>
#include <errno.h>
#include <fts.h>
#include <getopt.h>
#include <grp.h>
#include <pwd.h>	/*- user_from_uid -*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define LL long long
#define NO_PRINT 1
#define	IS_NOPRINT(p)	((p)->fts_number == NO_PRINT)

#define WATCH(fmt, ...) \
              if (DEBUG) printf(fmt, __VA_ARGS__);

void sys_err(const char *msg);

/* flags */
int f_accesstime;		/* use time of last access */
int f_column;			/* columnated format */
int f_columnacross;		/* columnated format, sorted across */
int f_humanize;			/* humanize the size field */
int f_inode;			/* print inode */
int f_listdir;			/* list actual directory, not contents */
int f_listdot;			/* list files beginning with . */
int f_longform;			/* long listing format */
int f_nonprint;			/* show unprintables as ? */
int f_nosort;			/* don't sort output */
int f_numericonly;		/* don't convert uid/gid to name */
int f_recursive;		/* ls subdirectories also */
int f_reversesort;		/* reverse whatever sort is used */
int f_singlecol;		/* use single column output */
int f_size;			    /* list size in short listing */
int f_statustime;		/* use time of last mode change */
int f_type;		    	/* add type character for non-regular files */
int f_kbyte;            /* print the k byte */
int fts_opts;           /* fts options */

typedef struct { /* 15 */
    int total;
    int l_name;		/*- useless when single column -*/
    int l_inode;
    int l_links;
    int l_uid;
    int l_gid;
    int l_uname;
    int l_gname;
    int l_size;
    int l_kb;
    int l_human;
    int l_blocks;
    int cnt_total;
    int l_major;
    int l_minor;
} LEN;
