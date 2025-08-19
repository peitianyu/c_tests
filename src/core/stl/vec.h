#ifndef __VEC_H__
#define __VEC_H__

#include <stdlib.h>
#include <string.h>

typedef void      *vec_t;
typedef size_t     vec_type_t;

typedef struct {
    size_t size, capacity;
    unsigned char data[];
} vec_hdr;

vec_hdr *vec_hdr_get(vec_t v);
vec_t    vec_create(void);
void     vec_free(vec_t v);
size_t vec_size(vec_t v);
size_t vec_capacity(vec_t v);

void vec_reserve(vec_t *vp, vec_type_t ts, size_t cap);
void *vec_add_dst(vec_t *vp, vec_type_t ts);
void *vec_insert_dst(vec_t *vp, vec_type_t ts, size_t pos);
void  vec_insert_array(vec_t *vp, vec_type_t ts, size_t pos, const void *arr, size_t cnt);
void  vec_erase(vec_t v, vec_type_t ts, size_t pos, size_t len);
void  vec_pop(vec_t v);
vec_t vec_copy(vec_t v, vec_type_t ts);

#define vec_push(v, T, val)        (*((T*)vec_add_dst(&(v), sizeof(T))) = (val))
#define vec_insert(v, T, pos, val) (*((T*)vec_insert_dst(&(v), sizeof(T), (pos))) = (val))
#define vec_at(v, T, pos)          (*(T*)((char*)(v) + (pos)*sizeof(T)))

#ifdef VEC_IMPLEMENTATION

vec_hdr *vec_hdr_get(vec_t v) { return &((vec_hdr *)v)[-1]; }

vec_t vec_create(void) {
    vec_hdr *h = (vec_hdr *)malloc(sizeof(vec_hdr));
    h->size = h->capacity = 0;
    return h->data;
}

void vec_free(vec_t v)           { free(vec_hdr_get(v)); }

size_t vec_size(vec_t v)     { return vec_hdr_get(v)->size; }

size_t vec_capacity(vec_t v) { return vec_hdr_get(v)->capacity; }

void vec_reserve(vec_t *vp, vec_type_t ts, size_t cap) {
    vec_hdr *h = vec_hdr_get(*vp);
    if (h->capacity >= cap) return;
    h = (vec_hdr *)realloc(h, sizeof(vec_hdr) + cap * ts);
    h->capacity = cap;
    *vp = h->data;
}

void *vec_add_dst(vec_t *vp, vec_type_t ts) {
    vec_hdr *h = vec_hdr_get(*vp);
    if (h->size == h->capacity) {
        size_t newc = h->capacity ? h->capacity * 2 : 1;
        vec_reserve(vp, ts, newc);
        h = vec_hdr_get(*vp);
    }
    return &((unsigned char *)(*vp))[ts * h->size++];
}

void *vec_insert_dst(vec_t *vp, vec_type_t ts, size_t pos) {
    vec_hdr *h = vec_hdr_get(*vp);
    vec_reserve(vp, ts,
                h->size == h->capacity ? (h->capacity ? h->capacity * 2 : 1)
                                       : h->capacity);
    h = vec_hdr_get(*vp);
    memmove((char *)(*vp) + (pos + 1) * ts,
            (char *)(*vp) + pos * ts,
            (h->size - pos) * ts);
    ++h->size;
    return (char *)(*vp) + pos * ts;
}

void vec_insert_array(vec_t *vp, vec_type_t ts, size_t pos, const void *arr, size_t cnt) {
    if (cnt == 0) return;
    vec_hdr *h = vec_hdr_get(*vp);
    size_t new_size = h->size + cnt;
    if (new_size > h->capacity) {
        size_t new_cap = h->capacity;
        while (new_cap < new_size) new_cap = new_cap ? new_cap * 2 : 1;
        vec_reserve(vp, ts, new_cap);
        h = vec_hdr_get(*vp);
    }
    memmove((char *)(*vp) + (pos + cnt) * ts,
            (char *)(*vp) + pos * ts,
            (h->size - pos) * ts);
    memcpy((char *)(*vp) + pos * ts, arr, cnt * ts);
    h->size = new_size;
}

void vec_erase(vec_t v, vec_type_t ts, size_t pos, size_t len) {
    vec_hdr *h = vec_hdr_get(v);
    memmove((char *)v + pos * ts,
            (char *)v + (pos + len) * ts,
            (h->size - pos - len) * ts);
    h->size -= len;
}

void vec_pop(vec_t v) { --vec_hdr_get(v)->size; }

vec_t vec_copy(vec_t v, vec_type_t ts) {
    vec_hdr *h = vec_hdr_get(v);
    vec_hdr *c = (vec_hdr *)malloc(sizeof(vec_hdr) + h->size * ts);
    memcpy(c, h, sizeof(vec_hdr) + h->size * ts);
    c->capacity = c->size;
    return c->data;
}

#endif /* VEC_IMPLEMENTATION */
#endif /* __VEC_H__ */