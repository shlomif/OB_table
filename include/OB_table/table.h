#ifndef VV_OB_TABLE_INCLUDED
#define VV_OB_TABLE_INCLUDED

#define SHLOMIFY
#include <OB_table/util.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

struct OB_item {
    void * item;
    size_t hash_value;
};

struct OB_table {
	size_t (*hash)(const void *);
	bool (*comp)(const void *, const void *);
	struct OB_item *table;
#ifndef SHLOMIFY
	union param p;
#endif
	size_t n, cap;
};

/* Pass t with hash_fn and comp_fn set, zero other members */
#ifdef SHLOMIFY
void
#else
struct OB_table *
#endif
OB_table_init(struct OB_table *t, size_t n_hint);

/* Find element */
void **
OB_table_find(struct OB_table *t, void *el);

/* Store the given element if not already exists, return locator of new element
 * or existing element if already exists */
void **
OB_table_insert_loc(struct OB_table *t, void *el);

/* Remove the given element (returns removed ptr - or NULL if none found) */
void *
OB_table_remove_loc(struct OB_table *t, void **loc);

/* Step through all locators (loc = previous locator or NULL to get first
 * locator) */
struct OB_item *
OB_table_step(struct OB_table *t, struct OB_item *loc);

/* Deallocates table */
#ifdef SHLOMIFY
void
#else
struct OB_table *
#endif
OB_table_clear(struct OB_table *t);

/* Get number of stored entries in table */
size_t
OB_table_len(const struct OB_table *t);


/* Store the given element if not already exists, return element or NULL if
 * fail */
void *
OB_table_insert(struct OB_table *t, void *el);

/* Remove an element (returns removed ptr - or NULL if none found) */
void *
OB_table_remove(struct OB_table *t, void *el);

#endif /* VV_OB_TABLE_INCLUDED */
