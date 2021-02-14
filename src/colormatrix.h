#ifndef COLOR_MATRIX
#define COLOR_MATRIX

#include <array>

#define MAX_COLORS 20u
const unsigned int CC_MAX = 20;

class ColorMatrix {
private:
    typedef unsigned int index_type;
    static const std::array<const char*, CC_MAX> colors;
    std::array<std::array<int, CC_MAX>, CC_MAX> matrix;
    index_type camsCount;
    index_type lastColor;
public:
    ColorMatrix();
    index_type addCam(index_type camIndex);
    void removeCam(index_type camIndex);
    index_type findCamColorIndex(index_type camIndex) const;
    const char* getColor(index_type colorIndex) const;

    index_type nextColorIndex(index_type camIndex, bool unique);
    const char* nextColor(index_type camIndex);
    index_type size() const {
        return camsCount;
    }

    static index_type camsLimit() {
        return CC_MAX;
    }

    void reset();

    void load();
    void save() const;
};

#endif
