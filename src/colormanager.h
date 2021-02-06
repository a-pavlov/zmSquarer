#ifndef COLORMANAGER_H
#define COLORMANAGER_H

#include <QList>
#include <QStringList>
#include <QPair>


class ColorManager {
private:
    typedef QPair<int, QString> CI;
    QStringList availableColors;
    QList<CI> assignedColors;
    int colorsCount(const QString& color) const;
public:
    ColorManager();
    QString addMon(int monId);
    void removeMon(int monId);
    QString getMonColor(int monId) const;
    QString nextMonColor(int monId, const QStringList stopColors);
    void load();
    void save() const;
    void replaceIndexes(int oldIndex, int newIndex);
};

#endif // COLORMANAGER_H
