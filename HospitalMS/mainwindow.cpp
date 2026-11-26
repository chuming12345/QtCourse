#include "mainwindow.h"
#include "database.h"
#include "patientdelegate.h"

#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolBar>
#include <QAction>
#include <QTableView>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QStatusBar>
#include <QDate>
#include <QVariant>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("医院诊疗系统 - 用户信息管理");
    resize(1000, 640);

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    buildLoginPage();
    buildManagePage();

    m_stack->setCurrentIndex(0);   // 启动先显示登录页

    // 状态栏常驻显示开发者学号与姓名
    m_statusLabel = new QLabel("开发者：邬婧文    学号：2024414300230", this);
    statusBar()->addPermanentWidget(m_statusLabel);
    statusBar()->showMessage("请先登录");
}

MainWindow::~MainWindow()
{
}

// ---------------------------------------------------------------------------
// 登录页
// ---------------------------------------------------------------------------
void MainWindow::buildLoginPage()
{
    m_loginPage = new QWidget;
    m_loginPage->setObjectName("loginPage");
    m_loginPage->setStyleSheet("QWidget#loginPage{background:#eef3fb;}");

    QLabel *title = new QLabel("医院诊疗系统");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:28px;font-weight:bold;color:#2b5fd9;");
    QLabel *sub = new QLabel("用户信息管理 · 登录");
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color:#6b7280;font-size:14px;");

    m_userEdit = new QLineEdit;
    m_userEdit->setPlaceholderText("请输入用户名（admin）");
    m_pwdEdit = new QLineEdit;
    m_pwdEdit->setPlaceholderText("请输入密码（123456）");
    m_pwdEdit->setEchoMode(QLineEdit::Password);

    QPushButton *loginBtn = new QPushButton("登  录");
    loginBtn->setStyleSheet(
        "QPushButton{background:#2b5fd9;color:#fff;border-radius:6px;padding:9px;font-size:15px;}"
        "QPushButton:hover{background:#1e4bb8;}");
    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLogin);
    connect(m_pwdEdit, &QLineEdit::returnPressed, this, &MainWindow::onLogin);

    QLabel *dev = new QLabel("开发者：邬婧文    学号：2024414300230");
    dev->setAlignment(Qt::AlignCenter);
    dev->setStyleSheet("color:#9aa4b2;font-size:12px;");

    QWidget *card = new QWidget;
    card->setMaximumWidth(360);
    card->setStyleSheet("QWidget{background:#ffffff;border-radius:10px;}");
    QVBoxLayout *cardLay = new QVBoxLayout(card);
    cardLay->setContentsMargins(28, 28, 28, 28);
    cardLay->setSpacing(12);
    cardLay->addWidget(title);
    cardLay->addWidget(sub);
    cardLay->addSpacing(6);
    cardLay->addWidget(new QLabel("用户名"));
    cardLay->addWidget(m_userEdit);
    cardLay->addWidget(new QLabel("密码"));
    cardLay->addWidget(m_pwdEdit);
    cardLay->addSpacing(4);
    cardLay->addWidget(loginBtn);
    cardLay->addWidget(dev);

    QVBoxLayout *outer = new QVBoxLayout(m_loginPage);
    outer->addStretch();
    QHBoxLayout *row = new QHBoxLayout;
    row->addStretch();
    row->addWidget(card);
    row->addStretch();
    outer->addLayout(row);
    outer->addStretch();

    m_stack->addWidget(m_loginPage);
}

// ---------------------------------------------------------------------------
// 管理页（患者信息）
// ---------------------------------------------------------------------------
void MainWindow::buildManagePage()
{
    m_managePage = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(m_managePage);
    lay->setContentsMargins(8, 8, 8, 8);
    lay->setSpacing(8);

    // 工具栏：用代码方式添加按钮与查询框
    m_toolBar = new QToolBar;
    m_toolBar->setMovable(false);
    m_toolBar->setStyleSheet(
        "QToolBar{background:#f7f9fc;border:1px solid #dde3ea;border-radius:6px;"
        "spacing:6px;padding:4px;}");

    QAction *actAdd     = m_toolBar->addAction("新增");
    QAction *actDel     = m_toolBar->addAction("删除");
    QAction *actSave    = m_toolBar->addAction("保存修改");
    QAction *actRefresh = m_toolBar->addAction("刷新");
    m_toolBar->addSeparator();
    m_toolBar->addWidget(new QLabel("  查询："));
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("按姓名 / 身份证 / 科室 查询");
    m_searchEdit->setMaximumWidth(240);
    m_toolBar->addWidget(m_searchEdit);
    QAction *actSearch = m_toolBar->addAction("查询");
    QAction *actAll    = m_toolBar->addAction("显示全部");

    connect(actAdd,     &QAction::triggered, this, &MainWindow::onAddPatient);
    connect(actDel,     &QAction::triggered, this, &MainWindow::onDeletePatient);
    connect(actSave,    &QAction::triggered, this, &MainWindow::onSaveChanges);
    connect(actRefresh, &QAction::triggered, this, &MainWindow::onRefresh);
    connect(actSearch,  &QAction::triggered, this, &MainWindow::onSearch);
    connect(actAll,     &QAction::triggered, this, &MainWindow::onShowAll);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &MainWindow::onSearch);

    // 视图与模型（MVC：Model=QSqlTableModel，View=QTableView）
    m_view = new QTableView;
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setAlternatingRowColors(true);
    m_view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    m_model = new QSqlTableModel(this, Database::instance().db());
    m_model->setTable("patients");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    m_queryModel = new QSqlQueryModel(this);

    setupPatientModel();
    applyPatientModel();

    lay->addWidget(m_toolBar);
    lay->addWidget(m_view);

    m_stack->addWidget(m_managePage);
}

void MainWindow::setupPatientModel()
{
    m_model->select();
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "姓名");
    m_model->setHeaderData(2, Qt::Horizontal, "性别");
    m_model->setHeaderData(3, Qt::Horizontal, "年龄");
    m_model->setHeaderData(4, Qt::Horizontal, "身份证号");
    m_model->setHeaderData(5, Qt::Horizontal, "联系电话");
    m_model->setHeaderData(6, Qt::Horizontal, "科室");
    m_model->setHeaderData(7, Qt::Horizontal, "诊断");
    m_model->setHeaderData(8, Qt::Horizontal, "主治医生");
    m_model->setHeaderData(9, Qt::Horizontal, "入院日期");
}

void MainWindow::applyPatientModel()
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(new PatientDelegate(2, this));   // 性别列使用下拉代理
    m_view->setColumnHidden(0, true);                        // 隐藏主键列
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::refreshPatientView()
{
    if (m_view->model() == m_queryModel)
        onSearch();      // 当前处于查询结果，重新执行查询以保持一致
    else
        m_model->select();
}

// ---------------------------------------------------------------------------
// 登录
// ---------------------------------------------------------------------------
void MainWindow::onLogin()
{
    const QString user = m_userEdit->text().trimmed();
    const QString pwd  = m_pwdEdit->text();

    if (user.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空！");
        return;
    }

    QString role;
    if (Database::instance().checkLogin(user, pwd, &role)) {
        m_currentUser = user;
        Database::instance().logOperation(user, "登录", "登录成功");
        m_statusLabel->setText(QString("当前用户：%1（%2）    开发者：邬婧文  学号：2024414300230")
                                   .arg(user, role));
        statusBar()->showMessage("登录成功，欢迎使用医院诊疗系统");
        m_model->select();
        applyPatientModel();
        m_stack->setCurrentIndex(1);   // 切换到管理页
        m_userEdit->clear();
        m_pwdEdit->clear();
    } else {
        Database::instance().logOperation(user, "登录", "登录失败：用户名或密码错误");
        QMessageBox::critical(this, "登录失败", "用户名或密码错误，请重新输入！");
        m_pwdEdit->selectAll();
        m_pwdEdit->setFocus();
    }
}

// ---------------------------------------------------------------------------
// 新增患者（含必要字段校验）
// ---------------------------------------------------------------------------
void MainWindow::onAddPatient()
{
    QDialog dlg(this);
    dlg.setWindowTitle("新增患者");
    dlg.setMinimumWidth(360);
    QFormLayout *form = new QFormLayout(&dlg);

    QLineEdit *nameEdit = new QLineEdit;
    QComboBox *genderBox = new QComboBox;
    genderBox->addItem("男");
    genderBox->addItem("女");
    QSpinBox *ageBox = new QSpinBox;
    ageBox->setRange(0, 120);
    ageBox->setValue(30);
    QLineEdit *idCardEdit = new QLineEdit;
    QLineEdit *phoneEdit = new QLineEdit;
    QLineEdit *deptEdit = new QLineEdit;
    QLineEdit *diagnosisEdit = new QLineEdit;
    QLineEdit *doctorEdit = new QLineEdit;
    QLineEdit *dateEdit = new QLineEdit(QDate::currentDate().toString("yyyy-MM-dd"));

    form->addRow("姓名 *：", nameEdit);
    form->addRow("性别：", genderBox);
    form->addRow("年龄：", ageBox);
    form->addRow("身份证号 *：", idCardEdit);
    form->addRow("联系电话：", phoneEdit);
    form->addRow("科室：", deptEdit);
    form->addRow("诊断：", diagnosisEdit);
    form->addRow("主治医生：", doctorEdit);
    form->addRow("入院日期：", dateEdit);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted)
        return;

    // 必要字段校验
    if (nameEdit->text().trimmed().isEmpty() || idCardEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "校验失败", "姓名和身份证号为必填项，请补全后重试！");
        return;
    }

    QSqlQuery q(Database::instance().db());
    q.prepare("INSERT INTO patients(name,gender,age,id_card,phone,department,diagnosis,doctor,admit_date)"
              " VALUES(?,?,?,?,?,?,?,?,?)");
    q.addBindValue(nameEdit->text().trimmed());
    q.addBindValue(genderBox->currentText());
    q.addBindValue(ageBox->value());
    q.addBindValue(idCardEdit->text().trimmed());
    q.addBindValue(phoneEdit->text().trimmed());
    q.addBindValue(deptEdit->text().trimmed());
    q.addBindValue(diagnosisEdit->text().trimmed());
    q.addBindValue(doctorEdit->text().trimmed());
    q.addBindValue(dateEdit->text().trimmed());

    if (!q.exec()) {
        QMessageBox::critical(this, "新增失败",
                              "插入数据失败（可能是身份证号重复）：\n" + q.lastError().text());
        return;
    }
    Database::instance().logOperation(m_currentUser, "新增患者", nameEdit->text().trimmed());
    QMessageBox::information(this, "成功", "患者记录已新增！");
    refreshPatientView();
}

// ---------------------------------------------------------------------------
// 删除患者（带确认）
// ---------------------------------------------------------------------------
void MainWindow::onDeletePatient()
{
    QModelIndex idx = m_view->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::information(this, "提示", "请先在列表中选择要删除的记录！");
        return;
    }
    if (m_view->model() != m_model) {
        QMessageBox::information(this, "提示", "查询结果仅供查看，请先点击“显示全部”后再删除。");
        return;
    }

    const int row = idx.row();
    const QString name = m_model->record(row).value("name").toString();

    if (QMessageBox::question(this, "确认删除",
            QString("确定要删除患者【%1】的记录吗？此操作不可恢复。").arg(name),
            QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    m_model->removeRow(row);
    if (!m_model->submitAll()) {
        QMessageBox::critical(this, "删除失败", m_model->lastError().text());
        m_model->revertAll();
        return;
    }
    Database::instance().logOperation(m_currentUser, "删除患者", name);
    QMessageBox::information(this, "成功", "记录已删除。");
    refreshPatientView();
}

// ---------------------------------------------------------------------------
// 保存表格内的修改
// ---------------------------------------------------------------------------
void MainWindow::onSaveChanges()
{
    if (m_view->model() != m_model) {
        QMessageBox::information(this, "提示", "查询结果不可直接修改，请先点击“显示全部”。");
        return;
    }
    if (!m_model->submitAll()) {
        QMessageBox::critical(this, "保存失败",
                              "修改未能保存（可能违反唯一约束）：\n" + m_model->lastError().text());
        m_model->revertAll();
        return;
    }
    Database::instance().logOperation(m_currentUser, "修改患者", "提交表格修改");
    QMessageBox::information(this, "成功", "修改已保存到数据库。");
    m_model->select();   // 修改后刷新，保证界面与数据库一致
}

// ---------------------------------------------------------------------------
// 刷新
// ---------------------------------------------------------------------------
void MainWindow::onRefresh()
{
    refreshPatientView();
    statusBar()->showMessage("数据已刷新", 2000);
}

// ---------------------------------------------------------------------------
// 通用查询（QSqlQueryModel + 参数化 QSqlQuery）
// ---------------------------------------------------------------------------
void MainWindow::onSearch()
{
    const QString key = m_searchEdit->text().trimmed();

    QSqlQuery query(Database::instance().db());
    if (key.isEmpty()) {
        query.exec("SELECT id,name,gender,age,id_card,phone,department,diagnosis,doctor,admit_date"
                   " FROM patients");
    } else {
        query.prepare("SELECT id,name,gender,age,id_card,phone,department,diagnosis,doctor,admit_date"
                      " FROM patients WHERE name LIKE ? OR id_card LIKE ? OR department LIKE ?");
        const QString like = "%" + key + "%";
        query.addBindValue(like);
        query.addBindValue(like);
        query.addBindValue(like);
        query.exec();
    }

    m_queryModel->setQuery(query);
    const QStringList heads = {"ID", "姓名", "性别", "年龄", "身份证号",
                               "联系电话", "科室", "诊断", "主治医生", "入院日期"};
    for (int i = 0; i < heads.size(); ++i)
        m_queryModel->setHeaderData(i, Qt::Horizontal, heads.at(i));

    m_view->setModel(m_queryModel);
    m_view->setColumnHidden(0, true);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if (m_queryModel->rowCount() == 0) {
        QMessageBox::information(this, "查询无结果",
            key.isEmpty() ? QString("暂无患者记录。")
                          : QString("未找到与“%1”相关的患者记录。").arg(key));
    }
}

void MainWindow::onShowAll()
{
    m_searchEdit->clear();
    applyPatientModel();
    m_model->select();
}
