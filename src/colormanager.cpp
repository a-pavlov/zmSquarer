#include "colormanager.h"
#include <algorithm>

ColorManager::ColorManager() {
    availableColors
            << "aliceblue"
            << "antiquewhite"
            << "aqua"
            << "aquamarine"
            << "azure"
            << "beige"
            << "bisque"
            << "blanchedalmond"
            << "cornsilk"
            << "cyan"
            << "ghostwhite"
            << "gold"
            << "greenyellow"
            << "honeydew"
            << "ivory"
            << "lavender"
            << "lavenderblush"
            << "lemonchiffon"
            << "lightgoldenrodyellow"
            << "lightpink";
}

QString ColorManager::addMon(int monId) {
    Q_ASSERT(!availableColors.isEmpty());
    assignedColors.push_back(qMakePair(monId, availableColors.takeFirst()));
    return assignedColors.back().second;
}

void ColorManager::removeMon(int monId) {
    auto itr = std::remove_if(assignedColors.begin(), assignedColors.end(), [=](CI clItem) -> bool { return clItem.first == monId; });
    Q_ASSERT(itr != assignedColors.end());
    if (itr != assignedColors.end()) availableColors.append(itr->second);
    assignedColors.erase(itr);
}

QString ColorManager::getMonColor(int monId) const {
    auto it = std::find_if(assignedColors.begin(), assignedColors.end(), [=](CI clItem) -> bool { return clItem.first == monId; });
    return it != assignedColors.end()?it->second:QString();
}

QString ColorManager::nextMonColor(int monId, const QStringList stopColors) {
    auto itr = std::find_if(assignedColors.begin(), assignedColors.end(), [=](CI clItem) -> bool { return clItem.first == monId; });
    Q_ASSERT(itr != assignedColors.end());
    if (itr != assignedColors.end()) {
        int ccount = colorsCount(itr->second);
        Q_ASSERT(ccount <= 2);
        Q_ASSERT(ccount > 0);
        CI data = *itr;
        auto next = assignedColors.erase(itr);

        for(auto candidate = next; candidate != assignedColors.end(); ++candidate) {
            if (stopColors.contains(candidate->second)) continue;
            if (data.second == candidate->second) continue;

        }

        for(auto candidate = assignedColors.begin(); candidate != next; ++candidate) {

        }

    }

    return QString();
}

void ColorManager::load() {

}

void ColorManager::save() const {

}

void ColorManager::replaceIndexes(int oldIndex, int newIndex) {
    auto it = std::find_if(assignedColors.begin(), assignedColors.end(), [=](CI clItem) -> bool { return clItem.first == oldIndex; });
    Q_ASSERT(it != assignedColors.end());
    if (it != assignedColors.end()) it->first = newIndex;
}

int ColorManager::colorsCount(const QString& color) const {
    return static_cast<int>(std::count_if(assignedColors.begin(), assignedColors.end(), [=](const CI& c) -> bool { return color == c.second;}));
}
