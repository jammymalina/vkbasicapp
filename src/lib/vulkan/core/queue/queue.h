#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_MAX_COUNT 16

#include <stdint.h>

typedef enum QueueType { QUEUE_PRESENT, QUEUE_GRAPHICS, QUEUE_COMPUTE, QUEUE_TRANSFER } QueueType;

typedef struct QueueDescriptor {
    uint32_t family_index;
    uint32_t count;
    float priorities[QUEUE_MAX_COUNT];
} QueueDescriptor;

static inline void queue_descriptor_clear(QueueDescriptor* descriptor) {
    descriptor->family_index = UINT32_MAX;
    descriptor->count = 0;
}

void queue_descriptor_copy(const QueueDescriptor* src, QueueDescriptor* dest);
void queue_descriptor_create_1p(QueueDescriptor* descriptor, uint32_t family_index, uint32_t count, float priority);
void queue_descriptor_create_np(QueueDescriptor* descriptor, uint32_t family_index, uint32_t count, float* priorities);

#endif
