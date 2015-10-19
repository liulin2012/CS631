#ifdef __linux__
#include <bsd/stdlib.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>	
#include <fts.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#ifndef S_IFWHT
	#define S_IFWHT  0160000
#endif
#define LL long long

#define NO_PRINT 1
#define	IS_NOPRINT(p)	((p)->fts_number == NO_PRINT) /* ignor the noprint file */

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

typedef struct { /* the format for the output */
    int f_total;
    int f_links;
    int f_name;		
    int f_inode;
    int f_uid;
    int f_gid;
    int f_usrname;
    int f_grpname;
    int f_size;
    int f_kb;
    int f_human;
    int f_block;
    int f_major;
    int f_minor;
} FORMAT;
