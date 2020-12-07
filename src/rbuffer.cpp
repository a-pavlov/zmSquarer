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
}

int RBuffer::findCharacter(char c) const {
    for(size_t i = content_length; i < used; ++i) {
        if (buffer[i] == c) return static_cast<int>(i);
    }

    return -1;
}

