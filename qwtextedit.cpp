#include "qwtextedit.h"

#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QResizeEvent>

// 行号区控件，仅负责把绘制事件转交给编辑器
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(QWTextEdit *editor) : QWidget(editor), m_editor(editor) {}
    QSize sizeHint() const override { return QSize(m_editor->lineNumberAreaWidth(), 0); }
protected:
    void paintEvent(QPaintEvent *event) override { m_editor->lineNumberAreaPaintEvent(event); }
private:
    QWTextEdit *m_editor;
};

QWTextEdit::QWTextEdit(QWidget *parent)
    : QPlainTextEdit(parent), m_show(true)
{
    m_lineArea = new LineNumberArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &QWTextEdit::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &QWTextEdit::updateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &QWTextEdit::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int QWTextEdit::lineNumberAreaWidth()
{
    if (!m_show) return 0;
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) { max /= 10; ++digits; }
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void QWTextEdit::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void QWTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineArea->scroll(0, dy);
    else
        m_lineArea->update(0, rect.y(), m_lineArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void QWTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    m_lineArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void QWTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extra;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection sel;
        sel.format.setBackground(QColor(Qt::yellow).lighter(160));
        sel.format.setProperty(QTextFormat::FullWidthSelection, true);
        sel.cursor = textCursor();
        sel.cursor.clearSelection();
        extra.append(sel);
    }
    setExtraSelections(extra);
}

void QWTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (!m_show) return;

    QPainter painter(m_lineArea);
    painter.fillRect(event->rect(), QColor(240, 240, 240));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, m_lineArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void QWTextEdit::paintEvent(QPaintEvent *e)
{
    QPlainTextEdit::paintEvent(e);
}

void QWTextEdit::setLineNumberVisible(bool b)
{
    m_show = b;
    m_lineArea->setVisible(b);
    updateLineNumberAreaWidth(0);
}
