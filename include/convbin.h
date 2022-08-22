#ifndef CONVBIN_H
#define CONVBIN_H

typedef struct {
    char *ts;
    char *te;
    char *tr;
    char *ti;
    char *span;
    char *r;
    char *ro;
    char *f;
    char *hc;
    char *hm;
    char *hn;
    char *ht;
    char *ho;
    char *hr;
    char *ha;
    char *hp;
    char *hd;
    char *v;
    char *od;
    char *os;
    char *oi;
    char *ot;
    char *ol;
    char *scan;
    char *mask;
    char *x;
    char *y;
    char *d;
    char *c;
    char *o;
    char *n;
    char *g;
    char *h;
    char *q;
    char *l;
    char *s;
    char *trace;
    char *filename;
}option_t;

int convbin_func(option_t *option);

#endif
