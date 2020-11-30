#ifndef RBUFFER_H
#define RBUFFER_H

#include <vector>
#include <cassert>

class RBuffer {
private:
    typedef std::vector<char>::size_type rbuffer_size;
    std::vector<char> buffer;
    rbuffer_size used;
    rbuffer_size content_length;
public:
    static const rbuffer_size NETCAM_RBUFFSIZE;
    static rbuffer_size RBUF_MEM_STEP;

    RBuffer();
    void checkBufferSize(rbuffer_size numBytes);
    rbuffer_size append(const char* src, rbuffer_size srcSize);
    void clear();

    rbuffer_size getUsed() const {
        return used;
    }

    rbuffer_size getSize() const {
        return buffer.size();
    }

    rbuffer_size getFree() const {
        return buffer.size() - used;
    }

    rbuffer_size getContentLength() const {
        return content_length;
    }

    void expandContentLength(rbuffer_size cl) {
        assert(content_length + cl <= used);
        content_length += cl;
    }

    void setContentLength(rbuffer_size cl) {
        assert(cl <= used);
        assert(cl >= content_length);
        content_length = cl;
    }

    const char* getPtr() const {
        return &buffer[0];
    }

    const char* getUnmarkedPtr() const {
        return &buffer[content_length];
    }

    size_t getUnmarkedSize() const {
       return used - content_length;
    }

    int findCharacter(char c) const;

    char getChar(size_t pos) const {
        assert(pos < buffer.size());
        return buffer[pos];
    }
};

#endif // BUFFER_H
