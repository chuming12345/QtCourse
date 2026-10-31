#include <QMessageBox>
#include "qwmainwind.h"
#include "ui_qwmainwind.h"
#include "finddialog.h"
#include "replacedialog.h"

#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QFontDialog>
#include <QColorDialog>
#include <QCoreApplication>
#include <QTextCharFormat>
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>
#include <QTextDocument>
#include <QTimer>
#include <QTextCursor>
#include <QPalette>

void QWMainWind::updateCurFile(QString aFile)
{
    fCurFileName = aFile;
    fLabCurFile->setText("当前文件：" + fCurFileName);
}

void QWMainWind::setTextModified(bool m)
{
    setWindowModified(m);
    QString t = fCurFileName.isEmpty() ? "未命名" : fCurFileName;
    setWindowTitle(t + " - 简单文本编辑器[*]");
}

void QWMainWind::iniUI()
{
    // ===== 状态栏 =====
    fLabCurFile = new QLabel;
    fLabCurFile->setMinimumWidth(150);
    fLabCurFile->setText("当前文件：");
    ui->statusBar->addWidget(fLabCurFile);

    progressBar1 = new QProgressBar;
    progressBar1->setMaximumWidth(200);
    progressBar1->setMinimum(5);
    progressBar1->setMaximum(50);
    progressBar1->setValue(m_edit->font().pointSize());
    ui->statusBar->addWidget(progressBar1);

    // ===== 工具栏：字体大小 / 字体名称 =====
    spinFontSize = new QSpinBox;
    spinFontSize->setMinimum(5);
    spinFontSize->setMaximum(50);
    spinFontSize->setValue(m_edit->font().pointSize());
    spinFontSize->setMinimumWidth(50);
    ui->mainToolBar->addWidget(new QLabel("字体大小 "));
    ui->mainToolBar->addWidget(spinFontSize);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(new QLabel(" 字体 "));
    comboFont = new QFontComboBox;
    comboFont->setMinimumWidth(150);
    ui->mainToolBar->addWidget(comboFont);

    setCentralWidget(m_edit);

    // ===== 新增 Action =====
    actSave        = new QAction("保存", this);
    actSave->setShortcut(QKeySequence::Save);
    actSaveAs      = new QAction("另存为...", this);
    actUndo        = new QAction("撤销", this);
    actUndo->setShortcut(QKeySequence::Undo);
    actRedo        = new QAction("重做", this);
    actRedo->setShortcut(QKeySequence::Redo);
    actSelectAll   = new QAction("全选", this);
    actSelectAll->setShortcut(QKeySequence::SelectAll);
    actFind        = new QAction("查找...", this);
    actFind->setShortcut(QKeySequence::Find);
    actReplace     = new QAction("替换...", this);
    actAutoWrap    = new QAction("自动换行", this);
    actAutoWrap->setCheckable(true);
    actAutoWrap->setChecked(true);
    actLineNumber  = new QAction("显示行号", this);
    actLineNumber->setCheckable(true);
    actLineNumber->setChecked(true);
    actBgColor     = new QAction("背景颜色...", this);
    actViewToolBar = new QAction("工具栏", this);
    actViewToolBar->setCheckable(true);
    actViewToolBar->setChecked(true);
    actViewStatusBar = new QAction("状态栏", this);
    actViewStatusBar->setCheckable(true);
    actViewStatusBar->setChecked(true);

    // 文件 菜单
    ui->menu->addSeparator();
    ui->menu->addAction(actSave);
    ui->menu->addAction(actSaveAs);

    // 编辑 菜单
    ui->menu_2->addSeparator();
    ui->menu_2->addAction(actUndo);
    ui->menu_2->addAction(actRedo);
    ui->menu_2->addAction(actSelectAll);
    ui->menu_2->addSeparator();
    ui->menu_2->addAction(actFind);
    ui->menu_2->addAction(actReplace);

    // 格式 菜单
    ui->menu_3->addSeparator();
    ui->menu_3->addAction(actAutoWrap);
    ui->menu_3->addAction(actLineNumber);
    ui->menu_3->addAction(actBgColor);

    // 视图 菜单
    QMenu *menuView = new QMenu("视图", this);
    menuView->addAction(actViewToolBar);
    menuView->addAction(actViewStatusBar);
    ui->menuBar->addMenu(menuView);

    // 关于 菜单（在代码中创建，规避 UI 文件抽风 / actAbout 缺失）
    QMenu *menuAbout = new QMenu("关于", this);
    QAction *actAbout = new QAction("关于本软件", this);
    connect(actAbout, &QAction::triggered, this, &QWMainWind::on_actAbout_triggered);
    menuAbout->addAction(actAbout);
    ui->menuBar->addMenu(menuAbout);

    // 定时自动保存（选做）：每 60s 若已修改且有文件名则自动保存
    QTimer *autoSave = new QTimer(this);
    autoSave->setInterval(60000);
    connect(autoSave, &QTimer::timeout, this, [this]() {
        if (!fCurFileName.isEmpty() && m_edit->document()->isModified())
            saveToFile(fCurFileName);
    });
    autoSave->start();
}

void QWMainWind::iniSignalSlots()
{
    connect(spinFontSize, SIGNAL(valueChanged(int)),
            this, SLOT(on_spinBoxFontSize_valueChanged(int)));
    connect(comboFont, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(on_comboFont_currentIndexChanged(const QString &)));

    connect(m_edit, &QPlainTextEdit::copyAvailable, this, &QWMainWind::on_txtEdit_copyAvailable);
    connect(m_edit, &QPlainTextEdit::selectionChanged, this, &QWMainWind::on_txtEdit_selectionChanged);
    connect(m_edit->document(), &QTextDocument::modificationChanged, this, &QWMainWind::on_document_modified);

    // 剪切/复制/粘贴：连接到真实编辑器 m_edit（.ui 中占位 txtEdit 已被 m_edit 取代，连接它无效）
    connect(ui->actCut,   &QAction::triggered, m_edit, &QPlainTextEdit::cut);
    connect(ui->actCopy,  &QAction::triggered, m_edit, &QPlainTextEdit::copy);
    connect(ui->actPaste, &QAction::triggered, m_edit, &QPlainTextEdit::paste);

    connect(actSave,        SIGNAL(triggered()),            this, SLOT(slotSave()));
    connect(actSaveAs,      SIGNAL(triggered()),            this, SLOT(slotSaveAs()));
    connect(actUndo,        SIGNAL(triggered()),            this, SLOT(slotUndo()));
    connect(actRedo,        SIGNAL(triggered()),            this, SLOT(slotRedo()));
    connect(actSelectAll,   SIGNAL(triggered()),            this, SLOT(slotSelectAll()));
    connect(actFind,        SIGNAL(triggered()),            this, SLOT(slotFind()));
    connect(actReplace,     SIGNAL(triggered()),            this, SLOT(slotReplace()));
    connect(actAutoWrap,    SIGNAL(toggled(bool)),          this, SLOT(slotAutoWrap(bool)));
    connect(actLineNumber,  SIGNAL(toggled(bool)),          this, SLOT(slotLineNumber(bool)));
    connect(actBgColor,     SIGNAL(triggered()),            this, SLOT(slotBgColor()));
    connect(actViewToolBar, SIGNAL(toggled(bool)),          this, SLOT(slotViewToolBar(bool)));
    connect(actViewStatusBar, SIGNAL(toggled(bool)),        this, SLOT(slotViewStatusBar(bool)));
}

QWMainWind::QWMainWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QWMainWind)
{
    ui->setupUi(this);

    m_edit = new QWTextEdit(this);
    m_edit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    iniUI();
    iniSignalSlots();

    setTextModified(false);
}

QWMainWind::~QWMainWind()
{
    delete ui;
}

// ============================================================
// 文本编辑使能状态
// ============================================================
void QWMainWind::on_txtEdit_copyAvailable(bool b)
{
    ui->actCut->setEnabled(b);
    ui->actCopy->setEnabled(b);
    ui->actPaste->setEnabled(m_edit->canPaste());
}

void QWMainWind::on_txtEdit_selectionChanged()
{
    QTextCharFormat fmt = m_edit->currentCharFormat();
    ui->actFontItalic->setChecked(fmt.fontItalic());
    ui->actFontBold->setChecked(fmt.font().bold());
    ui->actFontUnder->setChecked(fmt.fontUnderline());
}

void QWMainWind::on_document_modified(bool m)
{
    setTextModified(m);
}

// ============================================================
// 新建 / 打开 / 保存 / 另存为
// ============================================================
void QWMainWind::on_actNew_triggered()
{
    m_edit->clear();
    m_edit->document()->setModified(false);
    updateCurFile("");
    setTextModified(false);
}

void QWMainWind::on_actOpen_triggered()
{
    QString curPath = m_lastDir.isEmpty() ? QCoreApplication::applicationDirPath() : m_lastDir;
    QString selFilter = tr("文本文件(*.txt)");
    QString aFileName = QFileDialog::getOpenFileName(this, tr("打开一个文件"), curPath,
                     "文本文件(*.txt);;C++程序文件(*.cpp);;H头文件(*.h);;所有文件(*.*)",
                     &selFilter);
    if (aFileName.isEmpty())
        return;

    QFile aFile(aFileName);
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "文件无法打开：\n" + aFileName);
        return;
    }
    QTextStream aStream(&aFile);
    aStream.setAutoDetectUnicode(true);
    m_edit->setPlainText(aStream.readAll());
    aFile.close();

    m_lastDir = QFileInfo(aFileName).absolutePath();
    updateCurFile(aFileName);
    m_edit->document()->setModified(false);
    setTextModified(false);
}

void QWMainWind::saveToFile(QString aFile)
{
    QFile file(aFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "文件无法保存：\n" + aFile);
        return;
    }
    QTextStream aStream(&file);
    aStream.setCodec("UTF-8");
    aStream << m_edit->toPlainText();
    file.close();

    updateCurFile(aFile);
    m_edit->document()->setModified(false);
    setTextModified(false);
}

void QWMainWind::slotSave()
{
    if (fCurFileName.isEmpty())
        slotSaveAs();
    else
        saveToFile(fCurFileName);
}

void QWMainWind::slotSaveAs()
{
    QString curPath = m_lastDir.isEmpty() ? QCoreApplication::applicationDirPath() : m_lastDir;
    QString aFileName = QFileDialog::getSaveFileName(this, tr("另存为"), curPath,
                     "文本文件(*.txt);;C++程序文件(*.cpp);;H头文件(*.h);;所有文件(*.*)");
    if (!aFileName.isEmpty()) {
        m_lastDir = QFileInfo(aFileName).absolutePath();
        saveToFile(aFileName);
    }
}

// ============================================================
// 字体 / 格式
// ============================================================
void QWMainWind::on_actFont_triggered()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok)
        m_edit->setFont(font);
}

void QWMainWind::on_spinBoxFontSize_valueChanged(int aFontSize)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(aFontSize);
    m_edit->mergeCurrentCharFormat(fmt);
    progressBar1->setValue(aFontSize);
}

void QWMainWind::on_comboFont_currentIndexChanged(const QString &arg1)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(arg1);
    m_edit->mergeCurrentCharFormat(fmt);
}

void QWMainWind::on_actFontBold_triggered(bool checked)
{
    QTextCharFormat fmt = m_edit->currentCharFormat();
    fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    m_edit->mergeCurrentCharFormat(fmt);
}

void QWMainWind::on_actFontItalic_triggered(bool checked)
{
    QTextCharFormat fmt = m_edit->currentCharFormat();
    fmt.setFontItalic(checked);
    m_edit->mergeCurrentCharFormat(fmt);
}

void QWMainWind::on_actFontUnder_triggered(bool checked)
{
    QTextCharFormat fmt = m_edit->currentCharFormat();
    fmt.setFontUnderline(checked);
    m_edit->mergeCurrentCharFormat(fmt);
}

void QWMainWind::on_actToolbarLab_triggered(bool checked)
{
    if (checked)
        ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    else
        ui->mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

// ============================================================
// 撤销 / 重做 / 全选
// ============================================================
void QWMainWind::slotUndo()       { m_edit->undo(); }
void QWMainWind::slotRedo()       { m_edit->redo(); }
void QWMainWind::slotSelectAll()   { m_edit->selectAll(); }

// ============================================================
// 查找 / 替换
// ============================================================
void QWMainWind::slotFind()
{
    if (!m_findDlg) {
        m_findDlg = new FindDialog(this);
        connect(m_findDlg, &FindDialog::findNext,
                this, &QWMainWind::doFind);
    }
    m_findDlg->show();
    m_findDlg->raise();
    m_findDlg->activateWindow();
}

void QWMainWind::slotReplace()
{
    if (!m_replaceDlg) {
        m_replaceDlg = new ReplaceDialog(this);
        connect(m_replaceDlg, &ReplaceDialog::findNext,
                this, &QWMainWind::doFind);
        connect(m_replaceDlg, &ReplaceDialog::replace,
                this, &QWMainWind::doReplace);
    }
    m_replaceDlg->show();
    m_replaceDlg->raise();
    m_replaceDlg->activateWindow();
}

void QWMainWind::doFind(const QString &text, bool caseSensitive)
{
    if (text.isEmpty()) return;
    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor cur = m_edit->document()->find(text, m_edit->textCursor(), flags);
    if (cur.isNull()) {                      // 到末尾后从头再找
        QTextCursor start = m_edit->textCursor();
        start.movePosition(QTextCursor::Start);
        cur = m_edit->document()->find(text, start, flags);
        if (cur.isNull()) {
            QMessageBox::information(this, "查找", "未找到：" + text);
            return;
        }
    }
    m_edit->setTextCursor(cur);
}

void QWMainWind::doReplace(const QString &find, const QString &replace, bool all)
{
    if (find.isEmpty()) return;

    if (all) {
        QTextCursor cur = m_edit->document()->find(find);
        int count = 0;
        while (!cur.isNull() && count < 10000) {
            cur.insertText(replace);
            cur = m_edit->document()->find(find);
            ++count;
        }
        QMessageBox::information(this, "替换", QString("共替换 %1 处").arg(count));
    } else {
        QTextCursor cur = m_edit->textCursor();
        if (cur.selectedText() == find)
            cur.insertText(replace);
        QTextCursor next = m_edit->document()->find(find, m_edit->textCursor());
        if (!next.isNull())
            m_edit->setTextCursor(next);
    }
}

// ============================================================
// 自动换行 / 行号 / 背景色 / 视图
// ============================================================
void QWMainWind::slotAutoWrap(bool checked)
{
    m_edit->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void QWMainWind::slotLineNumber(bool checked)
{
    m_edit->setLineNumberVisible(checked);
}

void QWMainWind::slotBgColor()
{
    QColor c = QColorDialog::getColor(m_edit->palette().color(QPalette::Base), this, "选择背景颜色");
    if (c.isValid()) {
        QPalette p = m_edit->palette();
        p.setColor(QPalette::Base, c);
        m_edit->setPalette(p);
    }
}

void QWMainWind::slotViewStatusBar(bool checked)
{
    ui->statusBar->setVisible(checked);
}

void QWMainWind::slotViewToolBar(bool checked)
{
    ui->mainToolBar->setVisible(checked);
}

// ============================================================
// 关于
// ============================================================
void QWMainWind::on_actAbout_triggered()
{
    QMessageBox::about(this, "关于",
                       "简易记事本程序\n姓名：邬婧文\n学号：2024414300230");
}

// ============================================================
// 关闭前未保存提示
// ============================================================
void QWMainWind::closeEvent(QCloseEvent *event)
{
    if (m_edit->document()->isModified()) {
        QMessageBox::StandardButton r = QMessageBox::warning(this, "未保存",
            "文件已修改但尚未保存，是否保存？",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (r == QMessageBox::Save) {
            slotSave();
            if (m_edit->document()->isModified()) { event->ignore(); return; }
        } else if (r == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }
    event->accept();
}
