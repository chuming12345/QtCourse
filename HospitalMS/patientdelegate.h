#ifndef PATIENTDELEGATE_H
#define PATIENTDELEGATE_H

#include <QStyledItemDelegate>

// 自定义代理：为“性别”列提供下拉框编辑，提升数据录入的规范性。
class PatientDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PatientDelegate(int genderColumn, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

private:
    int m_genderColumn;
};

#endif // PATIENTDELEGATE_H
