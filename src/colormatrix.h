#ifndef COLOR_MATRIX
#define COLOR_MATRIX

#include <array>

#define MAX_COLORS 20u
const size_t CC_MAX = 20;

class ColorMatrix {
private:
    typedef uint index_type;
    static const std::array<const char*, CC_MAX> colors;
    std::array<std::array<int, CC_MAX>, CC_MAX> matrix;
    index_type camsCount;
    index_type lastColor;
public:
    ColorMatrix();
    size_t addCam(size_t camIndex);
    void removeCam(size_t camIndex);
    uint findCamColorIndex(size_t camIndex) const;
    const char* getColor(size_t colorIndex) const;

    size_t nextColorIndex(size_t camIndex, bool unique);
    const char* nextColor(size_t camIndex);
    size_t size() const {
        return camsCount;
    }

    static size_t camsLimit() {
        return CC_MAX;
    }

    void reset();

    void load();
    void save() const;
};

#endif
