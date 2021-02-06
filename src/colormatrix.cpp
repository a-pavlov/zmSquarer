#include "colormatrix.h"
#include <algorithm>
#include <numeric>
#include <QDebug>

#include "preferences.h"

const std::array<const char*, CC_MAX> ColorMatrix::colors= {
   "aliceblue", "antiquewhite", "aqua", "aquamarine", "azure",
   "beige", "bisque", "blanchedalmond", "cornsilk", "cyan",
   "ghostwhite", "gold", "greenyellow", "honeydew", "ivory",
   "lavender", "lavenderblush", "lemonchiffon", "lightgoldenrodyellow", "lightpink" };

ColorMatrix::ColorMatrix()
    : camsCount(0)
    , lastColor(0) {
   reset();
}

size_t ColorMatrix::findCamColorIndex(size_t camIndex) const {
    Q_ASSERT(camIndex < camsCount);
    for(auto itrColor = matrix.begin(); itrColor != matrix.end(); ++itrColor) {
        if ((*itrColor)[camIndex] == 1) {
            return static_cast<size_t>((itrColor - matrix.begin()));
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

void ColorMatrix::removeCam(size_t camIndex) {
    Q_ASSERT(camIndex < camsCount);
    for(size_t i = 0; i < CC_MAX; ++i) {
        matrix[i][camIndex] = 0;
    }
}

void ColorMatrix::reset() {
    for(size_t i = 0; i < CC_MAX; ++i)
        for(size_t j = 0; j < CC_MAX; ++j)
            matrix[i][j] = 0;
    lastColor = 0;
    camsCount = 0;
}

void ColorMatrix::save() const {
    Preferences pref;
    pref.setValue("Common/ColorMatrix/CamsCount", camsCount);
    pref.setValue("Common/ColorMatrix/LastColor", lastColor);
    pref.beginWriteArray("Common/ColorMatrix");
    for(size_t index = 0; index < matrix.size(); ++index) {
        QStringList line;
        std::transform(matrix[index].begin()
                       , matrix[index].end()
                       , std::back_inserter(line)
                       , [](int i) -> QString { return QString::number(i); });
        pref.setArrayIndex(static_cast<int>(index));
        pref.setValue("Line", line.join(","));
    }

    pref.endArray();
    pref.sync();
}

void ColorMatrix::load() {
    Preferences pref;
    camsCount = static_cast<size_t>(pref.value("Common/ColorMatrix/CamsCount", 0).toInt());
    lastColor = static_cast<size_t>(pref.value("Common/ColorMatrix/LastColor", 0).toInt());
    size_t matrixHeight = static_cast<size_t>(pref.beginReadArray("Common/ColorMatrix"));
    for(size_t index = 0; index < matrixHeight; ++index) {
        pref.setArrayIndex(static_cast<int>(index));
        QStringList line = pref.value("Line", "").toString().split(",");
        std::transform(line.begin()
                       , line.end()
                       , matrix[index].begin()
                       , [](QString elem) -> size_t { return elem.toUInt(); });
    }

    pref.endArray();
}

