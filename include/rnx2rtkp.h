#ifndef RNX2RTKP_OPTION
#define RNX2RTKP_OPTION

#define MAXFILE 8

typedef struct {
    char *k;
    char *o;
    char *ts;
    char *te;
    char *ti;
    char *p;
    char *f;
    char *m;
    char *v;
    char *s;
    char *d;
    char *b;
    char *c;
    char *i;
    char *h;
    char *t;
    char *u;
    char *e;
    char *a;
    char *n;
    char *g;
    char *r;
    char *l;
    char **y;
    char **x;
    char **filename;
}rnx_opt_t;

int rnx2rtkp_func(rnx_opt_t *opt);

#endif