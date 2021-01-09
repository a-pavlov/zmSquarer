#include "rbuffer.h"
#include <cassert>
#include <cstring>
#include <QtGlobal>
#include <QDebug>

RBuffer::rbuffer_size RBuffer::RBUF_MEM_STEP = 4096;
const RBuffer::rbuffer_size RBuffer::NETCAM_RBUFFSIZE = 4096;

RBuffer::RBuffer(): used(0)
  , content_length(0)
  , frameIndex(0) {
}

char* RBuffer::checkBufferSize(rbuffer_size numBytes) {
    assert(RBUF_MEM_STEP > 0);
    long min_size_to_alloc;
    long real_alloc;
    long new_size;

    if ((buffer.size() - used) >= numBytes) {
        return &buffer[used];
    }

    min_size_to_alloc = numBytes - (buffer.size() - used);
    real_alloc = ((min_size_to_alloc / RBUF_MEM_STEP) *RBUF_MEM_STEP);


    if ((min_size_to_alloc - real_alloc) > 0) {
        real_alloc += RBUF_MEM_STEP;
    }

    new_size = buffer.size() + real_alloc;
    buffer.resize(new_size);
    return &buffer[used];
}

RBuffer::rbuffer_size RBuffer::append(const char* src, rbuffer_size srcSize) {
    if (srcSize == 0) {
        return 0;
    } else {
        checkBufferSize(srcSize);
        Q_ASSERT(getFree() > srcSize);
        Q_ASSERT(src != nullptr);

        if (src) {
            memcpy(&buffer[used], src, srcSize);
        }

        used += srcSize;
        return srcSize;
    }
}

void RBuffer::clear() {
    used = 0;
    content_length = 0;
    soi_position = 0;
}

void RBuffer::reset() {
    clear();
    frameIndex = 0;
}

int RBuffer::findCharacter(char c) const {
    for(size_t i = content_length; i < used; ++i) {
        if (buffer[i] == c) return static_cast<int>(i);
    }

    return -1;
}

#ifdef Q_OS_WIN
void *memmem(const void *haystack, size_t haystack_len,
    const void * const needle, const size_t needle_len)
{
    if (haystack == NULL) return NULL; // or assert(haystack != NULL);
    if (haystack_len == 0) return NULL;
    if (needle == NULL) return NULL; // or assert(needle != NULL);
    if (needle_len == 0) return NULL;

    for (const char *h = (const char*)haystack;
            haystack_len >= needle_len;
            ++h, --haystack_len) {
        if (!memcmp((const void*)h, needle, needle_len)) {
            return (void*)h;
        }
    }
    return NULL;
}
#endif

void RBuffer::calculateSoiPosition() {
    Q_ASSERT(content_length > 0);
    soi_position = 0;
    char *ptr_buffer = (char*)memmem(&buffer[0], content_length, "\xff\xd8", 2);

    if (ptr_buffer != nullptr) {
        soi_position = ptr_buffer - &buffer[0];
    }
}

