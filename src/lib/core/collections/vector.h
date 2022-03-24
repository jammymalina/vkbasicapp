#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "../memory/memory.h"

#define VECTOR(type)                                                                                                   \
    {                                                                                                                  \
        size_t cap;                                                                                                    \
        size_t size;                                                                                                   \
        type* data;                                                                                                    \
    }

#define VECTOR_INITIALIZER                                                                                             \
    { 0, 0, NULL }

#define vector_init(pv) (void)((pv)->cap = 0, (pv)->size = 0, (pv)->data = NULL)

#define vector_destroy(pv) mem_free((pv)->data)

#define vector_clear(pv) (vector_destroy(pv), vector_init(pv))

#define VECTOR_MINCAP_ ((size_t)10)

static inline size_t vector_min_(size_t a, size_t b) { return a < b ? a : b; }

static inline size_t vector_max_(size_t a, size_t b) { return a > b ? a : b; }

static inline size_t vector_between_(size_t x, size_t min, size_t max) { return vector_max_(min, vector_min_(max, x)); }

static inline size_t vector_enforce_size_t_(size_t value) { return value; }

#define VECTOR_FAILFLAG_ (~(((size_t)-1) >> 1))

static inline void* vector_reallocdata_(
    void* ptr, size_t count, size_t size, size_t* restrict pcap, size_t* restrict psize) {
    void* n = mem_realloc(ptr, count * size);
    if (!n) {
        *pcap |= VECTOR_FAILFLAG_;
        return ptr;
    }
    *pcap = count;
    *psize = vector_min_(*psize, count);
    char* cn = (char*)n;
    const size_t byte_offset = *psize * size;
    size_t extra_empty_part_byte_length = (*pcap - *psize) * size;
    mem_set(cn + byte_offset, 0, extra_empty_part_byte_length);
    return n;
}

static inline bool vector_test_and_reset_failflag_(size_t* pcap) {
    if (*pcap & VECTOR_FAILFLAG_) {
        *pcap &= ~VECTOR_FAILFLAG_;
        return true;
    }
    return false;
}

#define vector_realloc_(pv, new_cap)                                                                                   \
    ((pv)->data = vector_reallocdata_((pv)->data, new_cap, sizeof(*(pv)->data), &(pv)->cap, &(pv)->size),              \
        !vector_test_and_reset_failflag_(&(pv)->cap))

#define vector_resize_(pv, new_cap)                                                                                    \
    ((pv)->cap == (new_cap) || ((new_cap) > 0 ? vector_realloc_(pv, new_cap) : (vector_clear(pv), true)))

static inline size_t vector_growsize_(size_t value) {
    // integer multiplication by 1.5
    return value + (value >> 1);
}

#define vector_max_cap_(pv) (SIZE_MAX / 2 / sizeof(*(pv)->data))

#define vector_reserve(pv, min_cap) vector_reserve_internal_(pv, vector_max_(min_cap, VECTOR_MINCAP_))

#define vector_reserve_internal_(pv, min_cap)                                                                          \
    ((min_cap) <= (pv)->cap ||                                                                                         \
        ((min_cap) <= vector_max_cap_(pv) &&                                                                           \
            vector_realloc_(pv, vector_between_(vector_growsize_((pv)->cap), min_cap, vector_max_cap_(pv)))))

#define vector_shrink_to_fit(pv) (void)vector_resize_(pv, (pv)->size)

#define vector_autoshrink(pv)                                                                                          \
    (void)((pv)->cap <= VECTOR_MINCAP_ || (pv)->cap < vector_growsize_((pv)->size + 5) ||                              \
           vector_resize_(pv, vector_max_((pv)->size + 5, VECTOR_MINCAP_)))

#define vector_check_same_ptr_type_(a, b) (void)((a) == (b))

#define vector_push(pv, item) (vector_reserve(pv, (pv)->size + 1) && ((pv)->data[(pv)->size++] = (item), true))

#define vector_push_all(pv, items, count) vector_push_all_internal_(pv, items, vector_enforce_size_t_(count))

#define vector_push_all_internal_(pv, items, count)                                                                    \
    (vector_check_same_ptr_type_((pv)->data, items),                                                                   \
        vector_reserve(pv, (pv)->size + (count)) &&                                                                    \
            (mem_copy(&(pv)->data[(pv)->size], items, (count) * sizeof(*(pv)->data)), (pv)->size += (count), true))

#define vector_insert_hole(pv, index, count)                                                                           \
    vector_insert_hole_internal_(pv, vector_enforce_size_t_(index), vector_enforce_size_t_(count))

#define vector_insert_hole_internal_(pv, index, count)                                                                 \
    (vector_reserve(pv, (pv)->size + (count)) &&                                                                       \
        ((index) == (pv)->size || (mem_move(&(pv)->data[(index) + (count)], &(pv)->data[index],                        \
                                       ((pv)->size - (index)) * sizeof(*(pv)->data)),                                  \
                                      true)) &&                                                                        \
        ((pv)->size += (count), true))

#define vector_insert(pv, index, item) (vector_insert_hole(pv, index, 1) && ((pv)->data[index] = (item), true))

#define vector_insert_all(pv, index, items, count)                                                                     \
    (vector_check_same_ptr_type_((pv)->data, items),                                                                   \
        vector_insert_hole(pv, index, count) &&                                                                        \
            (mem_copy(&(pv)->data[index], items, (count) * sizeof(*(pv)->data)), true))

static inline void vector_reverse_array_(char* array, size_t len) {
    for (size_t i = 0; i < len / 2; ++i) {
        char c = array[i];
        array[i] = array[len - i - 1];
        array[len - i - 1] = c;
    }
}

static inline void vector_rotate_array_left_(char* array, size_t len, size_t distance) {
    vector_reverse_array_(array, distance);
    vector_reverse_array_(&array[distance], len - distance);
    vector_reverse_array_(array, len);
}

static inline void vector_rotate_array_right_(char* array, size_t len, size_t distance) {
    vector_rotate_array_left_(array, len, len - distance);
}

static inline void vector_move_(char* array, size_t index, size_t count, size_t target) {
    if (index < target) {
        vector_rotate_array_left_(&array[index], target - index + count, count);
    } else {
        vector_rotate_array_right_(&array[target], index - target + count, count);
    }
}

#define vector_move_slice(pv, index, count, target)                                                                    \
    vector_move_slice_internal_(                                                                                       \
        pv, vector_enforce_size_t_(index), vector_enforce_size_t_(count), vector_enforce_size_t_(target))

#define vector_move_slice_internal_(pv, index, count, target)                                                          \
    vector_move_((char *) (pv)->data,                                          \
        (index) * sizeof(*(pv)->data)),                                        \
        (count) * sizeof(*(pv)->data)),                                        \
        (target) * sizeof(*(pv)->data))                                        \
    )

#define vector_move(pv, index, target) vector_move_slice(pv, index, 1, target)

#define vector_remove_slice_noshrink(pv, index, count)                                                                 \
    vector_remove_slice_noshrink_internal_(pv, vector_enforce_size_t_(index), vector_enforce_size_t_(count))

#define vector_remove_slice_noshrink_internal_(pv, index, count)                                                       \
    do {                                                                                                               \
        if ((index) + (count) < (pv)->size)                                                                            \
            mem_move(&(pv)->data[index], &(pv)->data[(index) + (count)],                                               \
                ((pv)->size - (index) - (count)) * sizeof(*(pv)->data));                                               \
        (pv)->size -= (count);                                                                                         \
    } while (0)

#define vector_remove_slice(pv, index, count)                                                                          \
    do {                                                                                                               \
        vector_remove_slice_noshrink(pv, index, count);                                                                \
        vector_autoshrink(pv);                                                                                         \
    } while (0)

#define vector_remove_noshrink(pv, index) vector_remove_slice_noshrink(pv, index, 1)

#define vector_remove(pv, index)                                                                                       \
    do {                                                                                                               \
        vector_remove_noshrink(pv, index);                                                                             \
        vector_autoshrink(pv);                                                                                         \
    } while (0)

#define vector_swap_remove(pv, index) (pv)->data[index] = (pv)->data[--(pv)->size]

#define vector_index_of(pv, item, pidx)                                                                                \
    do {                                                                                                               \
        ssize_t* out = pidx;                                                                                           \
        size_t vector_find_idx_;                                                                                       \
        for (vector_find_idx_ = 0; vector_find_idx_ < (pv)->size && ((pv)->data[vector_find_idx_] != (item));          \
             ++vector_find_idx_)                                                                                       \
            ;                                                                                                          \
        *out = vector_find_idx_ == (pv)->size ? -1 : (ssize_t)vector_find_idx_;                                        \
    } while (0)

#define vector_copy(pdst, psrc)                                                                                        \
    ((vector_init(pdst), vector_clear(pdst), true) && vector_reserve(pdst, (psrc)->cap) &&                             \
        vector_insert_all(pdst, 0, (psrc)->data, (psrc)->size))

#define vector_foreach(item, pv)                                                                                       \
    for (size_t vector_idx_##item = 0;                                                                                 \
         vector_idx_##item < (pv)->size && ((item) = (pv)->data[vector_idx_##item], true); ++vector_idx_##item)

#endif
