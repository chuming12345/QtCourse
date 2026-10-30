#ifndef QWTEXTEDIT_H
#define QWTEXTEDIT_H

#include <QPlainTextEdit>

// 从 QPlainTextEdit 派生，支持在左侧显示行号（实验要求选做项）
class QWTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit QWTextEdit(QWidget *parent = nullptr);

    int  lineNumberAreaWidth();
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void setLineNumberVisible(bool b);
    bool lineNumberVisible() const { return m_show; }

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    QWidget *m_lineArea;
    bool     m_show;
};

#endif // QWTEXTEDIT_H
