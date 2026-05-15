#include "recentemojis.h"
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QFileInfo>
#include <QTextStream>

static const QString RECENT_FILE = QDir::homePath() + "/.cache/lgl-emoji-picker/recent.txt";
static constexpr int MAX_RECENT = 16;

QStringList RecentEmojis::load() {
    QFile f(RECENT_FILE);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    QStringList result;
    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);
    while (!in.atEnd() && result.size() < MAX_RECENT) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty() && line.size() <= 32)
            result << line;
    }
    return result;
}

void RecentEmojis::save(const QString& emoji) {
    QStringList recent = load();
    recent.removeAll(emoji);
    recent.prepend(emoji);
    if (recent.size() > MAX_RECENT)
        recent = recent.mid(0, MAX_RECENT);

    QDir().mkpath(QFileInfo(RECENT_FILE).absolutePath());
    QSaveFile f(RECENT_FILE);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    for (const QString& e : recent)
        out << e << "\n";
    f.commit();
}
