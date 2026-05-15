#include "emojipicker.h"
#include <QApplication>
#include <QFont>

int main(int argc, char* argv[]) {
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME"))
        qputenv("QT_QPA_PLATFORMTHEME", "kde");

    QApplication app(argc, argv);
    app.setApplicationName("lgl-emoji-picker");
    app.setApplicationVersion("1.0.0");

    QFont appFont = app.font();
    appFont.setPointSize(appFont.pointSize() + 2);
    app.setFont(appFont);

    EmojiPicker picker;
    picker.show();

    return app.exec();
}
