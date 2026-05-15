#include "emojipicker.h"
#include <QApplication>
#include <QFont>
#include <QIcon>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("lgl-emoji-picker");
    app.setDesktopFileName("lgl-emoji-picker");
    app.setApplicationVersion("1.0.0");
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/packaging/icons/256x256/lgl-emoji-picker.png")));

    QFont appFont = app.font();
    appFont.setPointSize(appFont.pointSize() + 2);
    app.setFont(appFont);

    EmojiPicker picker;
    picker.show();

    return app.exec();
}
