#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QStackedWidget;
class QLineEdit;
class QTableView;
class QSqlTableModel;
class QSqlQueryModel;
class QLabel;
class QToolBar;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLogin();            // 登录控制
    void onAddPatient();       // 新增患者
    void onDeletePatient();    // 删除患者（带确认）
    void onSaveChanges();      // 提交表格中的修改
    void onRefresh();          // 刷新
    void onSearch();           // 通用查询（QSqlQueryModel）
    void onShowAll();          // 显示全部患者

private:
    void buildLoginPage();
    void buildManagePage();
    void setupPatientModel();
    void applyPatientModel();  // 应用患者表模型（隐藏ID列、设置代理、列宽）
    void refreshPatientView(); // 增删改后刷新视图

    QStackedWidget *m_stack = nullptr;

    // 登录页
    QWidget   *m_loginPage = nullptr;
    QLineEdit *m_userEdit  = nullptr;
    QLineEdit *m_pwdEdit   = nullptr;

    // 管理页
    QWidget        *m_managePage = nullptr;
    QToolBar       *m_toolBar    = nullptr;
    QLineEdit      *m_searchEdit = nullptr;
    QTableView     *m_view       = nullptr;
    QSqlTableModel *m_model      = nullptr;
    QSqlQueryModel *m_queryModel = nullptr;
    QLabel         *m_statusLabel = nullptr;

    QString m_currentUser;
};

#endif // MAINWINDOW_H
