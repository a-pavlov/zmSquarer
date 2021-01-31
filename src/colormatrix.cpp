#include "colormatrix.h"
#include <algorithm>
#include <numeric>
#include <QDebug>

const std::array<const char*, CC_MAX> ColorMatrix::colors= {
   "aliceblue", "antiquewhite", "aqua", "aquamarine", "azure",
   "beige", "bisque", "blanchedalmond", "cornsilk", "cyan",
   "ghostwhite", "gold", "greenyellow", "honeydew", "ivory",
   "lavender", "lavenderblush", "lemonchiffon", "lightgoldenrodyellow", "lightpink" };

ColorMatrix::ColorMatrix()
    : camsCount(0)
    , lastColor(0) {
    for(size_t i = 0; i < CC_MAX; ++i)
        for(size_t j = 0; j < CC_MAX; ++j)
            matrix[i][j] = 0;
}

size_t ColorMatrix::findCamColorIndex(size_t camIndex) const {
    Q_ASSERT(camIndex < camsCount);
    for(auto itrColor = matrix.begin(); itrColor != matrix.end(); ++itrColor) {
        if ((*itrColor)[camIndex] == 1) {
            return (itrColor - matrix.begin());
        }
    }

    return CC_MAX;
}

const char* ColorMatrix::getColor(size_t colorIndex) const {
    Q_ASSERT(colorIndex < CC_MAX);
    return colors[colorIndex];
}

size_t ColorMatrix::nextColorIndex(size_t camIndex, bool unique) {
    Q_ASSERT(camIndex < camsCount);
    size_t ci = findCamColorIndex(camIndex);
    size_t result = ci;
    if (ci == CC_MAX) ci = 0; else ++ci;

    size_t counter = 0;
    while(counter < camsCount) {
        size_t candidate = ci % camsCount;
        size_t cams = std::accumulate(matrix[candidate].begin(), matrix[candidate].end(), 0u);
        if ((cams == 0) || (!unique && cams == 1)) {
            if (result != CC_MAX) matrix[result][camIndex] = 0;
            matrix[candidate][camIndex] = 1;
            result = candidate;
            break;
        }

        ++ci;
        ++counter;
    }

    return result;
}

const char* ColorMatrix::nextColor(size_t camIndex) {
    if (camIndex == CC_MAX) return nullptr;
    Q_ASSERT(camIndex < colors.size());
    return colors[camIndex];
}

size_t ColorMatrix::addCam(size_t camIndex) {
    Q_ASSERT(camsCount < CC_MAX);
    Q_ASSERT(camIndex < CC_MAX);
    matrix[lastColor++][camIndex] = 1;
    return ++camsCount;
}

