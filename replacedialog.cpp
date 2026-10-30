#include "replacedialog.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("替换");
    setModal(false);

    m_find    = new QLineEdit(this);
    m_replace = new QLineEdit(this);
    m_case    = new QCheckBox("区分大小写", this);

    QPushButton *bFind  = new QPushButton("查找下一个", this);
    QPushButton *bRep   = new QPushButton("替换", this);
    QPushButton *bAll   = new QPushButton("替换全部", this);
    QPushButton *bClose = new QPushButton("关闭", this);

    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(new QLabel("查找内容："));
    h1->addWidget(m_find);

    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(new QLabel("替换为：  "));
    h2->addWidget(m_replace);

    QHBoxLayout *h3 = new QHBoxLayout;
    h3->addWidget(m_case);
    h3->addStretch();
    h3->addWidget(bFind);
    h3->addWidget(bRep);
    h3->addWidget(bAll);
    h3->addWidget(bClose);

    QVBoxLayout *v = new QVBoxLayout(this);
    v->addLayout(h1);
    v->addLayout(h2);
    v->addLayout(h3);

    connect(bFind,  &QPushButton::clicked, this, &ReplaceDialog::onFind);
    connect(bRep,   &QPushButton::clicked, this, &ReplaceDialog::onReplace);
    connect(bAll,   &QPushButton::clicked, this, &ReplaceDialog::onReplaceAll);
    connect(bClose, &QPushButton::clicked, this, &QDialog::close);
}

void ReplaceDialog::onFind()    { emit findNext(m_find->text(), m_case->isChecked()); }
void ReplaceDialog::onReplace() { emit replace(m_find->text(), m_replace->text(), false); }
void ReplaceDialog::onReplaceAll() { emit replace(m_find->text(), m_replace->text(), true); }
