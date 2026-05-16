#pragma once
#include <QDialog>
#include <QGridLayout>
#include <QHash>
#include <QLineEdit>
#include <QScrollArea>
#include <QStringList>
#include <QVector>
#include <QPair>

class EmojiPicker : public QDialog {
    Q_OBJECT
public:
    explicit EmojiPicker(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    class QPushButton* makeEmojiBtn(const QString& emoji, int size = 56);
    void populate(const QVector<QPair<QString, QString>>& emojis);
    void populateRecent();
    void clearLayout(QLayout* layout);
    void clearGridItems(QLayout* layout);
    void selectEmoji(const QString& emoji);
    int  computeCols() const;
    bool moveCursor(const QVector<class QPushButton*>& buttons, int idx, int dr, int dc);
    bool handleEmojiButtonKey(class QPushButton* btn, QKeyEvent* event);

    QLineEdit*    m_search;
    QWidget*      m_recentBox;
    QGridLayout*  m_recentGrid;
    QWidget*      m_emojiWidget;
    QGridLayout*  m_emojiGrid;
    QScrollArea*  m_scrollArea  = nullptr;
    class QTimer* m_resizeTimer = nullptr;
    QStringList   m_recentEmojis;
    QVector<QPair<QString, QString>> m_filtered;
    QHash<QString, class QPushButton*> m_emojiButtonCache;
    QVector<class QPushButton*>      m_recentBtns;
    QVector<class QPushButton*>      m_emojiBtns;
    int m_cols = 8;

private slots:
    void onSearchChanged(const QString& text);
    void onResized();
};
