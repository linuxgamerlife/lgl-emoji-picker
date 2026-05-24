#include "emojipicker.h"
#include "emojidata.h"
#include "recentemojis.h"
#include <QApplication>
#include <QClipboard>
#include <QEvent>
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
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <utility>

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

QString lookupEmojiName(const QString& emoji) {
    for (const auto& p : EMOJIS)
        if (p.first == emoji) return p.second;
    return emoji;
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
    m_scrollArea = new QScrollArea(this);
    auto* scrollArea = m_scrollArea;
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

    m_resizeTimer = new QTimer(this);
    m_resizeTimer->setSingleShot(true);
    connect(m_resizeTimer, &QTimer::timeout, this, &EmojiPicker::onResized);

    m_search->installEventFilter(this);

    m_recentEmojis = RecentEmojis::load();
    populateRecent();
    populate(EMOJIS);
    m_search->setFocus();
}

QPushButton* EmojiPicker::makeEmojiBtn(const QString& emoji, const QString& name, int size) {
    auto* btn = new QPushButton(emoji, this);
    btn->setFixedSize(size, size);
    btn->setToolTip(name);
    btn->setFocusPolicy(Qt::StrongFocus);
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  border: 1px solid palette(mid);"
        "  border-radius: 6px;"
        "  padding: 0;"
        "}"
        "QPushButton:focus {"
        "  border: 3px solid palette(highlight);"
        "  background: palette(highlight);"
        "  color: palette(highlighted-text);"
        "}"
    ));
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

void EmojiPicker::clearGridItems(QLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0))
        delete item;
}

void EmojiPicker::populateRecent() {
    clearLayout(m_recentGrid);
    m_recentBtns.clear();
    if (m_recentEmojis.isEmpty()) {
        m_recentBox->hide();
        return;
    }
    m_recentBox->show();
    for (int i = 0; i < m_recentEmojis.size(); ++i) {
        auto* btn = makeEmojiBtn(m_recentEmojis[i], lookupEmojiName(m_recentEmojis[i]), 48);
        btn->installEventFilter(this);
        m_recentGrid->addWidget(btn, i / m_cols, i % m_cols);
        m_recentBtns << btn;
    }
}

void EmojiPicker::populate(const QVector<QPair<QString, QString>>& emojis) {
    clearGridItems(m_emojiGrid);
    for (auto* btn : std::as_const(m_emojiBtns))
        btn->hide();

    m_emojiBtns.clear();
    m_filtered = emojis;
    for (int i = 0; i < emojis.size(); ++i) {
        const QString& emoji = emojis[i].first;
        auto* btn = m_emojiButtonCache.value(emoji, nullptr);
        if (!btn) {
            btn = makeEmojiBtn(emoji, emojis[i].second);
            btn->installEventFilter(this);
            m_emojiButtonCache.insert(emoji, btn);
        }
        btn->show();
        m_emojiGrid->addWidget(btn, i / m_cols, i % m_cols);
        m_emojiBtns << btn;
    }
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

void EmojiPicker::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
    m_resizeTimer->start(50);
}

void EmojiPicker::onResized() {
    int newCols = computeCols();
    if (newCols == m_cols)
        return;
    m_cols = newCols;
    populate(m_filtered);
    populateRecent();
}

int EmojiPicker::computeCols() const {
    if (!m_scrollArea)
        return m_cols;
    // margins: 12 left + 12 right = 24; step: 56px button + 4px spacing = 60
    int avail = m_scrollArea->viewport()->width() - 24;
    return qMax(1, avail / 60);
}

bool EmojiPicker::moveCursor(const QVector<QPushButton*>& buttons, int idx, int dr, int dc) {
    int newIdx = idx + dr * m_cols + dc;
    if (newIdx < 0 || newIdx >= buttons.size())
        return false;
    // prevent left/right wrapping across rows
    if (dc != 0 && (idx / m_cols) != (newIdx / m_cols))
        return false;
    buttons[newIdx]->setFocus();
    if (m_scrollArea)
        m_scrollArea->ensureWidgetVisible(buttons[newIdx]);
    return true;
}

bool EmojiPicker::handleEmojiButtonKey(QPushButton* btn, QKeyEvent* event) {
    const int recentIdx = m_recentBtns.indexOf(btn);
    const bool inRecent = recentIdx >= 0;
    const bool recentVisible = m_recentBox->isVisible() && !m_recentBtns.isEmpty();
    const QVector<QPushButton*>& buttons = inRecent ? m_recentBtns : m_emojiBtns;
    const int idx = inRecent ? recentIdx : m_emojiBtns.indexOf(btn);

    if (idx < 0)
        return false;

    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        selectEmoji(btn->text());
        return true;
    case Qt::Key_Left:
        moveCursor(buttons, idx, 0, -1);
        return true;
    case Qt::Key_Right:
        moveCursor(buttons, idx, 0, +1);
        return true;
    case Qt::Key_Down:
        if (inRecent && idx + m_cols >= m_recentBtns.size() && !m_emojiBtns.isEmpty()) {
            m_emojiBtns[qMin(idx % m_cols, m_emojiBtns.size() - 1)]->setFocus();
            if (m_scrollArea)
                m_scrollArea->ensureWidgetVisible(m_emojiBtns[qMin(idx % m_cols, m_emojiBtns.size() - 1)]);
        } else {
            moveCursor(buttons, idx, +1, 0);
        }
        return true;
    case Qt::Key_Up:
        if (!inRecent && idx < m_cols && recentVisible) {
            m_recentBtns[qMin(idx % m_cols, m_recentBtns.size() - 1)]->setFocus();
        } else if (idx < m_cols) {
            m_search->setFocus();
            m_search->selectAll();
        } else {
            moveCursor(buttons, idx, -1, 0);
        }
        return true;
    case Qt::Key_Escape:
        close();
        return true;
    default:
        if (!event->text().isEmpty() && event->text().at(0).isPrint()) {
            m_search->setFocus();
            QApplication::sendEvent(m_search, event);
            return true;
        }
        break;
    }
    return false;
}

bool EmojiPicker::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() != QEvent::KeyPress)
        return QDialog::eventFilter(obj, event);

    auto* ke = static_cast<QKeyEvent*>(event);

    if (obj == m_search) {
        const bool recentVisible = m_recentBox->isVisible() && !m_recentBtns.isEmpty();
        if (ke->key() == Qt::Key_Down && (recentVisible || !m_emojiBtns.isEmpty())) {
            if (recentVisible)
                m_recentBtns[0]->setFocus();
            else
                m_emojiBtns[0]->setFocus();
            return true;
        }
        return QDialog::eventFilter(obj, event);
    }

    auto* btn = qobject_cast<QPushButton*>(obj);
    if (!btn)
        return QDialog::eventFilter(obj, event);

    if (handleEmojiButtonKey(btn, ke))
        return true;

    return QDialog::eventFilter(obj, event);
}
