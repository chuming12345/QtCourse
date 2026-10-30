#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;

// 查找对话框
class FindDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindDialog(QWidget *parent = nullptr);

signals:
    void findNext(const QString &text, bool caseSensitive);

private slots:
    void onFindClicked();

private:
    QLineEdit *m_edit;
    QCheckBox *m_case;
};

#endif // FINDDIALOG_H
