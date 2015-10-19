/*
 * lin liu
 */

#include "ls.h"
#include "cmp.h"
#include "print.h"

#define MAX(a, b) ((a)>(b) ? (a):(b))
#define UPDATE(b, f, v, l) sprintf((b), (f), (v)); (l) = MAX(strlen(b),(l));
#define BY_NAME 0
#define BY_SIZE 1
#define BY_TIME 2
int sortkey = BY_NAME;

void (*print_fn)(FTSENT *);
int (*sort_fn)(const FTSENT *, const FTSENT *);

void build(int argc, char **argv);
void display(FTSENT *p, FTSENT *chp);
void update_len(FTSENT *cur, LEN *len);
int compare(const FTSENT **a, const FTSENT **b);
void usage(void);

int 
main(int argc, char *argv[]) 
{
    int ch;
    char *default_dir[] = {".", NULL};

    setprogname(argv[0]);

    if (!getuid())
        f_listdot = 1;
    if(isatty(STDOUT_FILENO)) 
        f_column = f_nonprint = 1;
    else
        f_singlecol = 1;
    fts_opts = FTS_PHYSICAL;
    while ((ch = getopt(argc, argv, "1AaBbCcdFfghikLlMmnOoPpqRrSsTtuWwXx"))
        != -1) {
        switch (ch) {
            /*
             * The −w, and −q options override each other; 
             * the last one specified determines the format used for non-print-able characters.
             */
            case 'q':
                f_nonprint = 1;
            case 'w':
                f_nonprint = 0;
            /*
             * The -1, -C, -l, -n and -x options override each other,
             * the last one specified determines the format used
             */
            case '1':
                f_singlecol = 1;
                f_column = f_columnacross = f_longform = 0;
                break;
            case 'C':
                f_column = 1;
                f_singlecol = f_columnacross = f_longform = 0;
                break;
            case 'l':
                f_longform = 1;
                f_column = f_columnacross = f_singlecol= 0;
                break;
            case 'n':
                f_numericonly = 1;
                f_longform = 1;
                f_column = f_columnacross = f_singlecol= 0;
                break;
            case 'x':
                f_columnacross = 1;
                f_column = f_singlecol= f_longform = 0;
                break;
            /*
             * The −c and −u options override each other; 
             * the last one specified determines the file time used.
             */
            case 'c':
                f_statustime = 1;
                f_accesstime = 0;
                break;
            case 'u':
                f_accesstime = 1;
                f_statustime = 0;
                break;
            case 't':
                sortkey = BY_TIME;
                break;
            case 'f':
                f_nosort = 1;
                break;
            case 'a':
                fts_opts |= FTS_SEEDOT;
            case 'A':
                f_listdot = 1;
                break;
            case 'd':
                f_listdir = 1;
                f_recursive = 0;
                break;
            case 'R':
                f_recursive = 1;
                break;
            case 'F':
                f_type = 1;
                break;
            case 'i':
                f_inode = 1;
                break; 
            case 'k':
                f_kbyte = 1;
                f_humanize = 0;
            break;
            case 'h':
                f_humanize = 1;
                f_kbyte = 0;
                break;
            case 'r':
                f_reversesort = 1;
                break;
            case 'S':
                sortkey = BY_SIZE;
                break;
            case 's':
                f_size = 1;
                break;
            default:
            case '?':
            usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (f_singlecol)
        print_fn = print_entry;
    else if (f_columnacross)
        print_fn = print_col_across;
    else if (f_longform)
        print_fn = print_long;
    else if (f_column)
        print_fn = print_col;
    
    if (f_reversesort) {
        switch (sortkey) {
        case BY_NAME:
            sort_fn = r_name_cp;
            break;
        case BY_SIZE:
            sort_fn = r_size_cp;
            break;
        case BY_TIME:
            if (f_accesstime)
                sort_fn = r_acc_time_cp;
            else if (f_statustime)
                sort_fn = r_stat_time_cp;
            else /* Use modification time. */
                sort_fn = r_mod_time_cp;
            break;
        }
    } else {
        switch (sortkey) {
        case BY_NAME:
            sort_fn = name_cp;
            break;
        case BY_SIZE:
            sort_fn = size_cp;
            break;
        case BY_TIME:
            if (f_accesstime)
                sort_fn = acc_time_cp;
            else if (f_statustime)
                sort_fn = stat_time_cp;
            else /* Use modification time. */
                sort_fn = mod_time_cp;
            break;
        }
    }
    if(argc)
        build(argc, argv);
    else    /*- No parameter, cur dir -*/
        build(1, default_dir);
    return(EXIT_SUCCESS);
}


void 
build(int argc, char **argv)
{
    FTS *ftsp;
    FTSENT *p, *chp;
    
    int If_mul = 0;
    if ((ftsp = fts_open(argv, fts_opts, f_nosort ? NULL : compare)) == NULL)
        sys_err("fts_open failed");
    chp = fts_children(ftsp, 0);
    display(NULL, chp);
    if (f_listdir) {
        (void)fts_close(ftsp);
        return ;
    }

    while ((p = fts_read(ftsp)) != NULL) {
        switch(p->fts_info) {
            case FTS_DC:
                (void)fprintf(stderr, "%s: Directory cycle detected %s.",
                  p->fts_name, strerror(errno));
                exit(EXIT_FAILURE);
            case FTS_ERR:
            case FTS_DNR:
                (void)fprintf(stderr, "%s: FTS directory error %s.",
                  p->fts_name, strerror(p->fts_errno));
                break;
            case FTS_D:
                /*- -a and -A option -*/
                if(p->fts_level != FTS_ROOTLEVEL && 
                    p->fts_name[0] == '.' && !f_listdot) 
                    break;

                if(If_mul)
                    (void)printf("\n%s\n", p->fts_path);
                else if (argc > 1){
                    (void)printf("%s\n", p->fts_path);
                }

                display(p, fts_children(ftsp, 0));
                if (!f_recursive) //&& chp != NULL
                    fts_set(ftsp, p, FTS_SKIP);
                If_mul = 1;
                break;  
            default:
                break;
            }
        }
    
    fts_close(ftsp);
    return ;
}

void
display(FTSENT *p, FTSENT *chp)
{
    LEN this_len = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
    FTSENT *cur;
    for(cur = chp; cur; cur = cur->fts_link){
        if (cur->fts_info == FTS_ERR || cur->fts_info == FTS_NS) {
            sys_err("File fail");
            cur->fts_number = NO_PRINT;
            continue;
        }
        if (p == NULL && cur->fts_info ==FTS_D && !f_listdir)
        {
            cur->fts_number = NO_PRINT;
            continue;
        }
        if ( p && cur->fts_name[0] == '.' && !f_listdot){ 
            cur->fts_number = NO_PRINT;
            continue;
        }
        update_len(cur, &this_len);
    }

    print_fn(chp);
}

void 
update_len(FTSENT *cur, LEN *len)
{
    char buf[1024];
    cur->fts_pointer = len;
    struct stat *cs;
    cs = cur->fts_statp;
    
    UPDATE(buf, "%s", cur->fts_name, len->l_name);
    if(f_inode)
        UPDATE(buf, "%lld", (LL)cs->st_ino, len->l_inode);
    if(f_size)
        UPDATE(buf, "%lld", (LL)cs->st_blocks, len->l_blocks);
    
    UPDATE(buf, "%lld", (LL)cs->st_size, len->l_size);
    len->cnt_total += cs->st_blocks;

    UPDATE(buf, "%d", (int)cs->st_nlink, len->l_links);
    
    if(f_numericonly){
        UPDATE(buf, "%d", cs->st_uid, len->l_uid);
        UPDATE(buf, "%d", cs->st_gid, len->l_gid);
    }
    else {  /*- uname/gname -*/
        struct passwd *u_st;
        struct group *g_st;
        u_st = getpwuid(cs->st_uid);
        g_st = getgrgid(cs->st_gid);
        UPDATE(buf, "%s", u_st->pw_name, len->l_uname);
        UPDATE(buf, "%s", g_st->gr_name, len->l_gname);
    }
    
    if(f_kbyte){
        LL kb = cs->st_size;
        kb = (kb % 1024) ? kb/1024 : (kb/1024 + 1);
        UPDATE(buf, "%lld", kb, len->l_kb);
    }
    if (f_humanize){
        int i = 0;
        LL size = (LL) cs->st_size;
        const char units[] = "BKMGT";
        while(size > 1024){
            size /= 1024;
            i++;
        }
        sprintf(buf, "%*lld%c", i, size, units[i]);
        len->l_human = MAX(strlen(buf), len->l_human);
    }
    if(S_ISCHR(cs->st_mode) || S_ISBLK(cs->st_mode)){
        UPDATE(buf, "%d", major(cs->st_rdev), len->l_major);
        UPDATE(buf, "%d", minor(cs->st_rdev), len->l_minor);
    }
 
}

int
compare(const FTSENT **a, const FTSENT **b)
{
    int a_info, b_info;
    a_info = (*a)->fts_info;
    b_info = (*b)->fts_info;
    if(a_info == FTS_ERR || b_info == FTS_ERR || 
       a_info == FTS_DNR || b_info == FTS_DNR)
        return 0;
    if(a_info == FTS_NS && b_info == FTS_NS)
        return name_cp(*a, *b);
    else if(a_info == FTS_NS || b_info == FTS_NS)
        return (a_info==FTS_NS)?1:-1;
    
    return (sort_fn(*a, *b));
}

void 
sys_err(const char *msg){
        perror(msg);
        exit(EXIT_FAILURE);
}

void
usage(void)
{
    (void)fprintf(stderr,
        "usage: ls [ −AacCdFfhiklnqRrSstuwx1] [file ...]]\n");
    exit(EXIT_FAILURE);
    /* NOTREACHED */
}

