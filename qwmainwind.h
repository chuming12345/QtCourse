#ifndef QWMAINWIND_H
#define QWMAINWIND_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QSpinBox>
#include <QFontComboBox>
#include "qwtextedit.h"

class FindDialog;
class ReplaceDialog;

namespace Ui {
class QWMainWind;
}

class QWMainWind : public QMainWindow
{
    Q_OBJECT

private:
    QString      fCurFileName;       // 当前文件名
    QLabel       *fLabCurFile;       // 状态栏：当前文件标签
    QProgressBar *progressBar1;       // 状态栏：字体大小进度条
    QSpinBox     *spinFontSize;       // 工具栏：字体大小
    QFontComboBox*comboFont;          // 工具栏：字体名称

    // 新增的 Action
    QAction *actSave;
    QAction *actSaveAs;
    QAction *actUndo;
    QAction *actRedo;
    QAction *actSelectAll;
    QAction *actFind;
    QAction *actReplace;
    QAction *actAutoWrap;
    QAction *actLineNumber;
    QAction *actBgColor;
    QAction *actViewToolBar;
    QAction *actViewStatusBar;

    FindDialog   *m_findDlg    = nullptr;   // 查找对话框
    ReplaceDialog *m_replaceDlg = nullptr;   // 替换对话框
    QString m_lastDir;                       // 最近使用的文件目录（打开/保存共用）

    void updateCurFile(QString aFile);// 更新当前文件名 + 状态栏
    void iniUI();                     // UI 初始化（含新增菜单/动作）
    void iniSignalSlots();            // 信号与槽关联
    void saveToFile(QString aFile);   // 实际写文件
    void setTextModified(bool m);     // 更新窗口标题 [*]

public:
    explicit QWMainWind(QWidget *parent = 0);
    ~QWMainWind();

    QWTextEdit *m_edit;               // 中央编辑控件（带行号）

private slots:
    // UI 设计器生成的槽
    void on_actFontBold_triggered(bool checked);
    void on_actFontItalic_triggered(bool checked);
    void on_actFontUnder_triggered(bool checked);
    void on_actNew_triggered();
    void on_actOpen_triggered();
    void on_actFont_triggered();
    void on_actToolbarLab_triggered(bool checked);
    void on_spinBoxFontSize_valueChanged(int aFontSize);
    void on_comboFont_currentIndexChanged(const QString &arg1);
    void on_actAbout_triggered();

    // 文本编辑相关
    void on_txtEdit_selectionChanged();
    void on_txtEdit_copyAvailable(bool b);
    void on_document_modified(bool m);

    // 新增功能槽
    void slotSave();
    void slotSaveAs();
    void slotUndo();
    void slotRedo();
    void slotSelectAll();
    void slotFind();
    void slotReplace();
    void slotAutoWrap(bool checked);
    void slotLineNumber(bool checked);
    void slotBgColor();
    void slotViewStatusBar(bool checked);
    void slotViewToolBar(bool checked);

    // 查找/替换回调
    void doFind(const QString &text, bool caseSensitive);
    void doReplace(const QString &find, const QString &replace, bool all);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::QWMainWind *ui;
};

#endif // QWMAINWIND_H
