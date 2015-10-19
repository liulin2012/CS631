#include "ls.h"
#include "print.h"


void 
print_entry(FTSENT *p)
{
    LEN *len = (LEN *)p->fts_pointer;
    FTSENT *cur;
    if(f_size && p && !IS_NOPRINT(p))
        printf("total %d\n", len->cnt_total);
    for (cur = p; cur; cur = cur->fts_link) {
        if (IS_NOPRINT(cur))
            continue;
        (void)print_one_name(cur, len);
        (void)putchar('\n');
    }
}

void
print_long(FTSENT *p)
{
    LEN *len = (LEN *)p->fts_pointer;
    FTSENT *cur;
    if (!IS_NOPRINT(p))
        printf("total %d\n", len->cnt_total);
    for (cur = p; cur; cur = cur->fts_link) {
        if (IS_NOPRINT(cur))
            continue;
        struct stat *st;
        st = cur->fts_statp;

        char perm_mode[25];
        if (f_inode)
            printf("%*lld ", len->l_inode, (LL)st->st_ino);

        if (f_size) {
            if (f_humanize)
                print_human((LL)st->st_size, len->l_human);
            else if (f_kbyte)
                print_kil((LL)st->st_size, len->l_kb);
            else 
                printf("%*lld ", len->l_blocks, (LL)st->st_blocks); 
        }
        strmode(st->st_mode, perm_mode);
        printf("%s ", perm_mode);
        printf("%*d ", len->l_links, (int)st->st_nlink);
        if (f_numericonly) {
            printf("%*d ", len->l_uid, (int)st->st_uid);
            printf("%*d ", len->l_gid, (int)st->st_gid);
            
        }
        else {
            struct passwd *u_pass;
            struct group *g_pass;
            u_pass = getpwuid(st->st_uid);
            g_pass = getgrgid(st->st_gid);
            printf("%*s ", len->l_uname, u_pass->pw_name);
            printf("%*s ", len->l_gname, g_pass->gr_name);
        }
        if(f_humanize)
            print_human((LL)st->st_size, len->l_human);
        else
            printf("%*d ", len->l_size, (int)st->st_size);

        if(f_accesstime)
            print_time(st->st_atime);
        else if(f_statustime)
            print_time(st->st_ctime);
        else 
            print_time(st->st_mtime);


        print_name(cur, st);
        if((st->st_mode & S_IFMT) == S_IFLNK){
            char lnk_tmp[1024], tmp[1024];
            int cn;
            snprintf(lnk_tmp, sizeof(lnk_tmp), "%s/%s", 
                    cur->fts_parent->fts_accpath, cur->fts_name);
            if((cn = readlink(lnk_tmp, tmp, sizeof(lnk_tmp) - 1)) == -1)
                sys_err("readlink err");
            else{
                tmp[cn] = '\0';
                printf(" -> %s", tmp);
            }
        }
        putchar('\n');
    }
}

void
print_col_across(FTSENT *p)
{
    FTSENT *cur = p;
    LEN *len = (LEN *)p->fts_pointer;
    if(f_size && p && !IS_NOPRINT(p))
        printf("total %d\n", len->cnt_total);
    while (cur) {
        if (IS_NOPRINT(cur)) {
            cur = cur->fts_link;
            continue;
        }
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); 
        int terminal_width, colwidth, colnums, nums, col;
        
        colwidth = len->l_name + 1;
        terminal_width = w.ws_col;
        if (f_inode)
            colwidth += len->l_inode + 1;
        if (f_size) {
                if (f_humanize)
                    colwidth += len->l_human + 1;
                else if (f_kbyte)
                    colwidth += len->l_kb + 1;
                else 
                    colwidth += len->l_blocks + 1;
        }
        if (f_type)
            colwidth += 1;

        if (terminal_width < 2* colwidth) {
            print_entry(p);
            return ;
        }
        colnums = terminal_width / colwidth;
        colwidth = terminal_width / colnums;

        nums = col = 0;
        for (cur = p; cur; cur = cur->fts_link) {
            if (IS_NOPRINT(cur))
                continue;
            if (col == colnums) {
                col = 0;
                putchar('\n');
            }
            nums = print_one_name(cur, len);
            while (nums++ < colwidth)
                putchar(' ');
            col++;
        }
        putchar('\n');
    }
}

void
print_col(FTSENT *p)
{
    FTSENT *cur = p;
    LEN *len = (LEN *)p->fts_pointer;
    if(f_size && p && !IS_NOPRINT(p))
        printf("total %d\n", len->cnt_total);
    while (cur) {
        if (IS_NOPRINT(cur)) {
            cur = cur->fts_link;
            continue;
        }
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); 
        static FTSENT **array;
        int terminal_width, colwidth, colnums, nums, col, numrows, row, chnums, base,count;
        
        colwidth = len->l_name + 1;
        terminal_width = w.ws_col;
        if (f_inode)
            colwidth += len->l_inode + 1;
        if (f_size) {
                if (f_humanize)
                    colwidth += len->l_human + 1;
                else if (f_kbyte)
                    colwidth += len->l_kb + 1;
                else 
                    colwidth += len->l_blocks + 1;
        }

        if (f_type)
            colwidth += 1;

        if (terminal_width < 2* colwidth) {
            print_entry(p);
            return ;
        }
        colnums = terminal_width / colwidth;
        colwidth = terminal_width / colnums;
        

        for (cur = p, count = 0; cur; cur = cur->fts_link) {
             if (!IS_NOPRINT(cur)) {
                count++;
             }
        }
        array = realloc(array, count * sizeof(FTSENT));


        nums = col = 0;
        for (cur = p; cur; cur = cur->fts_link) {
             if (!IS_NOPRINT(cur)) {
                array[nums++] = cur;
             }
        }
        numrows = nums / colnums;
        if (nums % colnums)
            numrows++;

        for (row = 0; row < numrows; row++) {
            for (base = row, col = chnums = 0; col < colnums; col++) {
                chnums = print_one_name(array[base], len);
                if ((base += numrows) >= nums)
                    break;
                while (chnums++ < colwidth)
                    putchar(' ');
            }    
            putchar('\n');
        }

    }
}

void
print_time(time_t tm)
{
    char *time_display;
    int i;
    time_t now = time(NULL);
    if ((time_display = ctime(&tm)) == NULL)
                    /* 012345678901234567890123 */
        time_display= "????????????????????????";
    for (i = 4; i < 11; i++)
        putchar(time_display[i]);
    if (now + SIX_MONTH > tm && now - SIX_MONTH < tm) {
        for (i = 11; i < 16; i++)
            putchar(time_display[i]);
    }
    else {
        putchar(' ');
        for (i = 20; i < 24; i++)
            putchar(time_display[i]);
    }
    putchar(' ');
}

int
print_one_name(FTSENT *p, LEN *len)
{
    int nums = 0;
    struct stat *st;
    st = p->fts_statp;
    
    if (f_inode)
        nums += printf("%*lld ", len->l_inode, (LL)st->st_ino);
    if (f_size) {
        if (f_humanize)
            nums += print_human((LL)st->st_size, len->l_human);
        else if (f_kbyte)
            nums += print_kil((LL)st->st_size, len->l_kb);
        else 
            nums += printf("%*lld ", len->l_blocks, (LL)st->st_blocks); 
    }
    nums += print_name(p, st);
    return nums;
}

int
print_name(FTSENT *p, struct stat *st)
{
    int i, nums;
    char name[1024];
    strcpy(name, p->fts_name);
    nums = 0;
    if (f_nonprint) {
        for (i = 0; i < strlen(name); i++) 
            name[i] = isprint(name[i]) ? name[i] : '?';
    }
    nums += printf("%s", name);

    if (f_type)
        nums += printf("%c", print_type(st->st_mode));

    return nums;
}

int
print_human(LL size, int length)
{
    int nums = 0;
    if (size == 0) {
        nums += printf("%*lld ", length, size);
        return nums;
    } 
    int i = 0;
    char m[5] = "BKMGT";
    char buff[20];
    while (size > 1024) {
        int s = ((size - size % 1024) > 512) ? 1 : 0;
        size = size/1024 + s;
        i++;
    }
    if (size == 0) size = 1;
    sprintf(buff, "%*lld%c", i, size, m[i]);
    nums += printf("%*s ", length, buff);
    return nums;
}

int
print_kil(LL size, int length)
{
    int nums = 0;
    if (size == 0) {
        nums += printf("%*lld ", length, size);
        return nums;
    } 
    int s = ((size - size % 1024) > 512) ? 1 : 0;
    size = size/1024 + s;
    if (size == 0) size = 1;
    nums += printf("%*lld ", length, size);
    return nums;
}

char
print_type(mode_t mode)
{
    switch(mode & S_IFMT) {
        case S_IFDIR:
            return '/';
        case S_IFSOCK:
            return '=';
        case S_IFIFO:
            return '|';
        case S_IFLNK:
            return '@';
        case S_IFWHT:
            return '%';
    }
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        return '*';
    else return ' ';
}

