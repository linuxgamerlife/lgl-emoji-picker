#include "emojipicker.h"
#include "emojidata.h"
#include "recentemojis.h"
#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QKeyEvent>
#include <QIcon>
#include <QLabel>
#include <QMimeData>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace {
bool copyWithProcess(const QString& program, const QStringList& arguments, const QString& text) {
    if (!QFileInfo::exists(program))
        return false;

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start(QIODevice::WriteOnly);
    if (!process.waitForStarted(1000))
        return false;

    process.write(text.toUtf8());
    process.closeWriteChannel();
    if (!process.waitForFinished(2000)) {
        process.kill();
        process.waitForFinished();
        return false;
    }

    return process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
}

void setQtClipboard(const QString& text) {
    auto* mimeData = new QMimeData;
    mimeData->setText(text);
    QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);

    if (QApplication::clipboard()->supportsSelection()) {
        auto* selectionData = new QMimeData;
        selectionData->setText(text);
        QApplication::clipboard()->setMimeData(selectionData, QClipboard::Selection);
    }
}

void copyToClipboard(const QString& text) {
    setQtClipboard(text);

    if (qEnvironmentVariableIsSet("WAYLAND_DISPLAY")) {
        if (copyWithProcess(QStringLiteral("/usr/bin/wl-copy"),
                            {QStringLiteral("--type"), QStringLiteral("text/plain;charset=utf-8")},
                            text))
            return;

        copyWithProcess(QStringLiteral("/usr/bin/wl-copy"), {}, text);
        return;
    }

    if (qEnvironmentVariableIsSet("DISPLAY")) {
        if (copyWithProcess(QStringLiteral("/usr/bin/xclip"),
                            {QStringLiteral("-selection"), QStringLiteral("clipboard")},
                            text))
            return;

        copyWithProcess(QStringLiteral("/usr/bin/xsel"),
                        {QStringLiteral("--clipboard"), QStringLiteral("--input")},
                        text);
    }
}
}

EmojiPicker::EmojiPicker(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("LGL Emoji Picker");
    setWindowIcon(QIcon(QStringLiteral(":/icons/packaging/icons/256x256/lgl-emoji-picker.png")));
    setMinimumSize(720, 560);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(12);

    // Search bar
    auto* searchBox = new QHBoxLayout;
    searchBox->setContentsMargins(0, 0, 0, 0);
    m_search = new QLineEdit(this);
    m_search->setPlaceholderText("Search emoji...");
    connect(m_search, &QLineEdit::textChanged, this, &EmojiPicker::onSearchChanged);
    connect(m_search, &QLineEdit::returnPressed, this, [this] {
        if (!m_filtered.isEmpty())
            selectEmoji(m_filtered[0].first);
    });
    searchBox->addWidget(m_search);
    root->addLayout(searchBox);

    // Recent section
    m_recentBox = new QWidget(this);
    auto* recentLayout = new QVBoxLayout(m_recentBox);
    recentLayout->setContentsMargins(0, 0, 0, 0);
    recentLayout->setSpacing(6);

    auto* recentLabel = new QLabel("Recent", m_recentBox);
    recentLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    recentLayout->addWidget(recentLabel);

    auto* recentInner = new QWidget(m_recentBox);
    m_recentGrid = new QGridLayout(recentInner);
    m_recentGrid->setSpacing(4);
    m_recentGrid->setContentsMargins(0, 0, 0, 0);
    recentLayout->addWidget(recentInner, 0, Qt::AlignHCenter);
    root->addWidget(m_recentBox);

    // Emoji grid in scroll area
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_emojiWidget = new QWidget(scrollArea);
    m_emojiGrid = new QGridLayout(m_emojiWidget);
    m_emojiGrid->setSpacing(4);
    m_emojiGrid->setContentsMargins(12, 4, 12, 12);
    m_emojiGrid->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    scrollArea->setWidget(m_emojiWidget);

    root->addWidget(scrollArea);

    auto* creditRow = new QHBoxLayout;
    creditRow->setContentsMargins(0, 0, 0, 0);
    creditRow->setSpacing(6);
    creditRow->addStretch();

    auto* creditIcon = new QLabel(this);
    QPixmap creditPixmap(QStringLiteral(":/credits/references/Black-Don.png"));
    creditIcon->setPixmap(creditPixmap.scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    creditIcon->setFixedSize(22, 22);
    creditRow->addWidget(creditIcon);

    auto* creditLabel = new QLabel("Inspired by TheBlackDon", this);
    creditLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    creditRow->addWidget(creditLabel);
    root->addLayout(creditRow);

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect geom = screen->availableGeometry();
    int w = qMin(int(geom.width() * 0.5), 700);
    int h = qMin(int(geom.height() * 0.55), 600);
    resize(w, h);
    move(geom.center() - QPoint(w / 2, h / 2));

    m_recentEmojis = RecentEmojis::load();
    populateRecent();
    populate(EMOJIS);
    m_search->setFocus();
}

QPushButton* EmojiPicker::makeEmojiBtn(const QString& emoji, int size) {
    auto* btn = new QPushButton(emoji, this);
    btn->setFixedSize(size, size);
    btn->setToolTip(emoji);
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QFont emojiFont = btn->font();
    emojiFont.setPointSize(size >= 56 ? 22 : 18);
    btn->setFont(emojiFont);
    connect(btn, &QPushButton::clicked, this, [this, emoji] { selectEmoji(emoji); });
    return btn;
}

void EmojiPicker::clearLayout(QLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
}

void EmojiPicker::populateRecent() {
    clearLayout(m_recentGrid);
    if (m_recentEmojis.isEmpty()) {
        m_recentBox->hide();
        return;
    }
    m_recentBox->show();
    for (int i = 0; i < m_recentEmojis.size(); ++i)
        m_recentGrid->addWidget(makeEmojiBtn(m_recentEmojis[i], 48), i / GRID_COLS, i % GRID_COLS);
}

void EmojiPicker::populate(const QVector<QPair<QString, QString>>& emojis) {
    clearLayout(m_emojiGrid);
    m_filtered = emojis;
    for (int i = 0; i < emojis.size(); ++i)
        m_emojiGrid->addWidget(makeEmojiBtn(emojis[i].first), i / GRID_COLS, i % GRID_COLS);
}

void EmojiPicker::onSearchChanged(const QString& text) {
    if (text.isEmpty()) {
        m_recentBox->show();
        populate(EMOJIS);
        return;
    }
    m_recentBox->hide();
    QString q = text.toLower();
    QVector<QPair<QString, QString>> filtered;
    for (const auto& pair : EMOJIS) {
        if (pair.second.contains(q, Qt::CaseInsensitive) || pair.first.contains(q))
            filtered << pair;
    }
    populate(filtered);
}

void EmojiPicker::selectEmoji(const QString& emoji) {
    RecentEmojis::save(emoji);
    copyToClipboard(emoji);
    m_recentEmojis = RecentEmojis::load();
    populateRecent();
    m_search->setFocus();
}

void EmojiPicker::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape)
        close();
    else
        QDialog::keyPressEvent(event);
}
