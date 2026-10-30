#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;

// 替换对话框
class ReplaceDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReplaceDialog(QWidget *parent = nullptr);

signals:
    void findNext(const QString &text, bool caseSensitive);
    void replace(const QString &find, const QString &replace, bool all);

private slots:
    void onFind();
    void onReplace();
    void onReplaceAll();

private:
    QLineEdit *m_find;
    QLineEdit *m_replace;
    QCheckBox *m_case;
};

#endif // REPLACEDIALOG_H
