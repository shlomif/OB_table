#include <OB_table/table.h>
#ifdef SHLOMIFY
#include <OB_table/util.h>
#include <assert.h>
#else
#include <OB_table/assert.h>
#endif

#define START_N      16
#define EXPAND_RATIO 2

/* Sentinel for previously occupied table entries */
extern void *const OB_deleted;

SHRET OB_table_init(struct OB_table *t, size_t n_hint)
{
	if (n_hint) t->cap = n_hint * EXPAND_RATIO;
	if (t->cap < START_N) t->cap = START_N;
#ifdef SHLOMIFY
	t->table = calloc(t->cap, sizeof *t->table);
#else
	ASSERT(t->table = calloc(t->cap, sizeof *t->table));
	return t;
#endif
}

/* Like OB_table_find, but returns first empty on failure (or NULL if saturated)
*/
static inline struct OB_item*
find(struct OB_table *t, struct OB_item pivot)
{
	size_t tries, i;
	struct OB_item *result = NULL, *first_empty = NULL;
    const size_t hash = pivot.hash_value;
	i = hash % t->cap;
	for (tries = 0; !result && tries < t->cap; tries++) {
		/* Remember first empty during search, if any */
		if (!first_empty && (!t->table[i].item || t->table[i].item == OB_deleted))
			first_empty = &t->table[i];
		/* NULL implies element does not exist */
		if (!t->table[i].item) {
			result = first_empty ? first_empty : &t->table[i];
            result->hash_value = hash;
		/* If there is a non-empty slot check it is actually what we're
		   searching for */
		} else if (   t->table[i].item != OB_deleted
                && (hash == t->table[i].hash_value)
		           && t->comp(t->table[i].item, pivot.item))
		{
			result = &t->table[i];
		}
		/* Next slot */
		if (++i == t->cap) i = 0;
	}
	return result ? result : (first_empty ? ({first_empty->hash_value = hash; first_empty; }) : NULL);
}

void ** OB_table_find(struct OB_table *t, void *el)
{
	struct OB_item pivot = {.item=el, .hash_value=t->hash(el)};
	struct OB_item*result = find(t, pivot);
	return (!result->item || result->item == OB_deleted) ? NULL : (&result->item);
}

static void ** OB_table_insert_loc__pivot(struct OB_table *t, struct OB_item pivot)
{
	struct OB_table tc;
	struct OB_item*loc = find(t, pivot);
	/* Element not exist? */
	if (!loc || !loc->item || loc->item == OB_deleted) {
		/* Not enough capacity? */
		if (EXPAND_RATIO * (t->n + 1) > t->cap) {
			tc = *t;
			tc.n = 0;
			OB_table_init(&tc, MAX(EXPAND_RATIO, 2) * (t->n + 1));
			for (size_t i = 0; i < t->cap; i++) {
				if (t->table[i].item && t->table[i].item != OB_deleted)
					OB_table_insert_loc__pivot(&tc, t->table[i]);
			}
			free(t->table);
			*t = tc;
			loc = find(t, pivot);
		}
		loc->item = pivot.item;
		t->n++;
	}
	return &(loc->item);
}

void ** OB_table_insert_loc(struct OB_table *t, void *el)
{
	struct OB_item pivot = {.item=el, .hash_value=t->hash(el)};
    return OB_table_insert_loc__pivot(t, pivot);
}

void * OB_table_remove_loc(struct OB_table *t, void **loc)
{
	void *result = *loc;
	*loc = OB_deleted;
	t->n--;
	return result;
}

struct OB_item * OB_table_step(struct OB_table *t, struct OB_item *loc)
{
	struct OB_item *result = NULL;
	size_t i = 0;
	if (loc) i = loc - t->table + 1;
	for (; !result && i < t->cap; i++) {
		if (t->table[i].item && t->table[i].item != OB_deleted) {
			result = &t->table[i];
		}
	}
	return result;
}

size_t OB_table_len(const struct OB_table *t)
{
	return t->n;
}

SHRET OB_table_clear(struct OB_table *t)
{
	free(t->table);
	t->cap = 0;
	t->n = 0;
#ifndef SHLOMIFY
	return t;
#endif
}

void * OB_table_insert(struct OB_table *t, void *el)
{
	void **loc, *result = NULL;
	loc = OB_table_insert_loc(t, el);
	if (loc && *loc && *loc != OB_deleted) result = *loc;
	return result;
}

void * OB_table_remove(struct OB_table *t, void *el)
{
	void *result = NULL, **loc = OB_table_find(t, el);
	if (loc) result = OB_table_remove_loc(t, loc);
	return result;
}
