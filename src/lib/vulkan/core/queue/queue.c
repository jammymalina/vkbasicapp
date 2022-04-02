#include "./queue.h"

void queue_descriptor_copy(const QueueDescriptor* src, QueueDescriptor* dst) {
    dst->count = src->count;
    dst->family_index = src->family_index;
    for (uint32_t i = 0; i < src->count; ++i) {
        dst->priorities[i] = src->priorities[i];
    }
}

void queue_descriptor_create_1p(QueueDescriptor* descriptor, uint32_t family_index, uint32_t count, float priority) {
    descriptor->family_index = family_index;
    descriptor->count = count;
    for (uint32_t i = 0; i < descriptor->count; ++i) {
        descriptor->priorities[i] = priority;
    }
}

void queue_descriptor_create_np(QueueDescriptor* descriptor, uint32_t family_index, uint32_t count, float* priorities) {
    descriptor->family_index = family_index;
    descriptor->count = count;
    for (uint32_t i = 0; i < descriptor->count; ++i) {
        descriptor->priorities[i] = priorities[i];
    }
}
