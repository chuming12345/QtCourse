#include "patientdelegate.h"

#include <QComboBox>

PatientDelegate::PatientDelegate(int genderColumn, QObject *parent)
    : QStyledItemDelegate(parent), m_genderColumn(genderColumn)
{
}

QWidget *PatientDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    if (index.column() == m_genderColumn) {
        QComboBox *box = new QComboBox(parent);
        box->addItem("男");
        box->addItem("女");
        return box;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void PatientDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == m_genderColumn) {
        if (QComboBox *box = qobject_cast<QComboBox *>(editor)) {
            box->setCurrentText(index.data(Qt::EditRole).toString());
            return;
        }
    }
    QStyledItemDelegate::setEditorData(editor, index);
}

void PatientDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    if (index.column() == m_genderColumn) {
        if (QComboBox *box = qobject_cast<QComboBox *>(editor)) {
            model->setData(index, box->currentText(), Qt::EditRole);
            return;
        }
    }
    QStyledItemDelegate::setModelData(editor, model, index);
}
