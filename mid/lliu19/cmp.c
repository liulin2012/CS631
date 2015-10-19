#include "ls.h"
#include "cmp.h"

int
name_cp(const FTSENT *a, const FTSENT *b)
{
    return (strcmp(a->fts_name, b->fts_name));
}

int
r_name_cp(const FTSENT *a, const FTSENT *b)
{
    return name_cp(b ,a);
}

int
acc_time_cp(const FTSENT *a, const FTSENT *b)
{
    if (a->fts_statp->st_atime < b->fts_statp->st_atime)
        return 1;
    else if (a->fts_statp->st_atime > b->fts_statp->st_atime)
        return -1;
    else
        return (name_cp(a,b));
}

int
r_acc_time_cp(const FTSENT *a, const FTSENT *b)
{
    return acc_time_cp(b, a);
}

int
stat_time_cp(const FTSENT *a, const FTSENT *b)
{
    if (a->fts_statp->st_ctime < b->fts_statp->st_ctime)
        return 1;
    else if (a->fts_statp->st_ctime > b->fts_statp->st_ctime)
        return -1;
    else
        return (name_cp(a,b));
}

int
r_stat_time_cp(const FTSENT *a, const FTSENT *b)
{
    return stat_time_cp(b, a);
}


int
mod_time_cp(const FTSENT *a, const FTSENT *b)
{
    if (a->fts_statp->st_mtime < b->fts_statp->st_mtime)
        return 1;
    else if (a->fts_statp->st_mtime > b->fts_statp->st_mtime)
        return -1;
    else
        return (name_cp(a,b));
}

int
r_mod_time_cp(const FTSENT *a, const FTSENT *b)
{
    return mod_time_cp(b, a);
}

int
size_cp(const FTSENT *a, const FTSENT *b)
{
    if (a->fts_statp->st_size < b->fts_statp->st_size)
        return 1;
    else if (a->fts_statp->st_size > b->fts_statp->st_size)
        return -1;
    else
        return (name_cp(a,b));
}

int
r_size_cp(const FTSENT *a, const FTSENT *b)
{
    return size_cp(b, a);
}
