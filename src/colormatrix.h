#ifndef COLOR_MATRIX
#define COLOR_MATRIX

#include <array>

#define MAX_COLORS 20u
const size_t CC_MAX = 20;

class ColorMatrix {
private:
    static const std::array<const char*, CC_MAX> colors;
    std::array<std::array<int, CC_MAX>, CC_MAX> matrix;
    size_t camsCount;
    int noCamsMarker;
public:
    ColorMatrix(size_t);
    size_t findCamColorIndex(size_t camIndex) const;
    size_t nextColorIndex(size_t camIndex, bool unique);
    const char* nextColor(size_t camIndex);
};

#endif
