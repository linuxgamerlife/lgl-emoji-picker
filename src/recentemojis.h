#pragma once
#include <QStringList>

namespace RecentEmojis {
    QStringList load();
    void save(const QString& emoji);
}
