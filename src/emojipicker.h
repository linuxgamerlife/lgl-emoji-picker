#pragma once
#include <QDialog>
#include <QGridLayout>
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

private:
    class QPushButton* makeEmojiBtn(const QString& emoji, int size = 56);
    void populate(const QVector<QPair<QString, QString>>& emojis);
    void populateRecent();
    void clearLayout(QLayout* layout);
    void selectEmoji(const QString& emoji);

    QLineEdit*   m_search;
    QWidget*     m_recentBox;
    QGridLayout* m_recentGrid;
    QWidget*     m_emojiWidget;
    QGridLayout* m_emojiGrid;
    QStringList  m_recentEmojis;
    QVector<QPair<QString, QString>> m_filtered;

    static constexpr int GRID_COLS = 8;

private slots:
    void onSearchChanged(const QString& text);
};
