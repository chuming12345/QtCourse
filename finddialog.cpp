#include "finddialog.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("查找");
    setModal(false);

    m_edit = new QLineEdit(this);
    m_case = new QCheckBox("区分大小写", this);

    QPushButton *btnFind   = new QPushButton("查找下一个", this);
    QPushButton *btnClose  = new QPushButton("关闭", this);

    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(new QLabel("查找内容："));
    h1->addWidget(m_edit);

    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(m_case);
    h2->addStretch();
    h2->addWidget(btnFind);
    h2->addWidget(btnClose);

    QVBoxLayout *v = new QVBoxLayout(this);
    v->addLayout(h1);
    v->addLayout(h2);

    connect(btnFind,  &QPushButton::clicked, this, &FindDialog::onFindClicked);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::close);
}

void FindDialog::onFindClicked()
{
    emit findNext(m_edit->text(), m_case->isChecked());
}
