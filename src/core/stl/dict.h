#ifndef __DICT_H__
#define __DICT_H__

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef struct { char *k, *v; unsigned h; } kp_t;
typedef struct { unsigned fill, used, size; kp_t *t; } dict_t;

unsigned hash_murmur(const char *k);
char *xstrdup(const char *s);

dict_t *dict_new(void);
void    dict_free(dict_t *d);
int     dict_add(dict_t *d, const char *k, const char *v);
const char *dict_get(dict_t *d, const char *k, const char *def);
int     dict_del(dict_t *d, const char *k);
int     dict_enum(dict_t *d, int pos, const char **k, const char **v);
void    dict_dump(dict_t *d, FILE *fp);
typedef void (*dict_foreach_cb)(const char *k, const char *v, void *ctx);
void    dict_foreach(dict_t *d, dict_foreach_cb cb, void *ctx);

kp_t *dict_slot(dict_t *d, const char *k, unsigned h);
int   dict_resize(dict_t *d);

#ifdef DICT_IMPLEMENTATION

unsigned hash_murmur(const char *k)
{
    const unsigned m = 0x5bd1e995, r = 24, seed = 0x0badcafe;
    int len = (int)strlen(k);
    unsigned h = seed ^ len;
    const uint8_t *d = (const uint8_t *)k;

    while (len >= 4) {
        unsigned v = *(const uint32_t *)d;
        v *= m; v ^= v >> r; v *= m;
        h *= m; h ^= v;
        d += 4; len -= 4;
    }
    if (len & 3) {
        unsigned tail = 0;
        switch (len & 3) {
            case 3: tail |= d[2] << 16; /* fallthrough */
            case 2: tail |= d[1] << 8;  /* fallthrough */
            case 1: tail |= d[0]; h ^= tail * m;
        }
    }
    h ^= h >> 13; h *= m; h ^= h >> 15;
    return h;
}

char *xstrdup(const char *s)
{
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

kp_t *dict_slot(dict_t *d, const char *k, unsigned h)
{
    if (!d || !k) return NULL;
    unsigned mask = d->size - 1, i = h & mask, perturb = h;
    kp_t *slot = NULL;

    for (;;) {
        kp_t *e = d->t + i;
        if (!e->k)            return slot ? slot : e;
        if (e->k == (void *)-1 && !slot) slot = e;
        if (e->h == h && e->k != (void *)-1 && !strcmp(e->k, k)) return e;
        i = (i * 5 + perturb + 1) & mask;
        perturb >>= 5;
    }
}

int dict_resize(dict_t *d)
{
    unsigned old = d->size, newsize = old;
    while (newsize < d->used * 2) newsize <<= 1;
    if (newsize == old) return 0;

    kp_t *oldt = d->t;
    d->t = (kp_t *)calloc(newsize, sizeof(kp_t));
    if (!d->t) { d->t = oldt; return -1; }
    d->size = newsize; d->used = d->fill = 0;

    for (unsigned i = 0; i < old; ++i)
        if (oldt[i].k && oldt[i].k != (void *)-1) {
            kp_t *s = dict_slot(d, oldt[i].k, oldt[i].h);
            *s = oldt[i]; d->used++; d->fill++;
        }
    free(oldt);
    return 0;
}

dict_t *dict_new(void)
{
    dict_t *d = (dict_t *)calloc(1, sizeof(dict_t));
    if (!d) return NULL;
    d->size = 8;
    d->t = (kp_t *)calloc(d->size, sizeof(kp_t));
    if (!d->t) { free(d); return NULL; }
    return d;
}

void dict_free(dict_t *d)
{
    if (!d) return;
    for (unsigned i = 0; i < d->size; ++i) {
        if (d->t[i].k && d->t[i].k != (void *)-1) {
            free(d->t[i].k);
            free(d->t[i].v);
        }
    }
    free(d->t); free(d);
}

int dict_add(dict_t *d, const char *k, const char *v)
{
    if (!d || !k) return -1;
    unsigned h = hash_murmur(k);
    kp_t *e = dict_slot(d, k, h);
    if (!e) return -1;

    if (e->k == NULL || e->k == (void *)-1) {
        e->k = xstrdup(k);
        e->v = v ? xstrdup(v) : NULL;
        if (!e->k || (v && !e->v)) return -1;
        e->h = h; d->used++; d->fill++;
    } else {
        free(e->v);
        e->v = v ? xstrdup(v) : NULL;
        if (v && !e->v) return -1;
    }
    if (d->fill * 3 >= d->size * 2) dict_resize(d);
    return 0;
}

const char *dict_get(dict_t *d, const char *k, const char *def)
{
    if (!d || !k) return def;
    kp_t *e = dict_slot(d, k, hash_murmur(k));
    return (e && e->k && e->k != (void *)-1) ? e->v : def;
}

int dict_del(dict_t *d, const char *k)
{
    if (!d || !k) return -1;
    kp_t *e = dict_slot(d, k, hash_murmur(k));
    if (!e || !e->k || e->k == (void *)-1) return -1;
    free(e->k); free(e->v);
    e->k = (char *)-1; e->v = NULL;
    d->used--;
    return 0;
}

int dict_enum(dict_t *d, int pos, const char **k, const char **v)
{
    if (!d || !k || !v || pos < 0) return -1;
    while ((unsigned)pos < d->size &&
           (!d->t[pos].k || d->t[pos].k == (void *)-1)) ++pos;
    if ((unsigned)pos >= d->size) { *k = *v = NULL; return -1; }
    *k = d->t[pos].k; *v = d->t[pos].v;
    return pos + 1;
}

void dict_dump(dict_t *d, FILE *fp)
{
    const char *k, *v; int pos = 0;
    while ((pos = dict_enum(d, pos, &k, &v)) != -1)
        fprintf(fp, "%20s: %s\n", k, v ? v : "UNDEF");
}

void dict_foreach(dict_t *d, dict_foreach_cb cb, void *ctx)
{
    const char *k, *v; int pos = 0;
    while ((pos = dict_enum(d, pos, &k, &v)) != -1)
        cb(k, v, ctx);
}

#endif /* DICT_IMPLEMENTATION */
#endif /* __DICT_H__ */