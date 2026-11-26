#include "database.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QDateTime>
#include <QVariant>

Database &Database::instance()
{
    static Database inst;   // C++11 局部静态变量，线程安全的懒汉单例
    return inst;
}

Database::Database()
{
}

Database::~Database()
{
    if (m_db.isOpen())
        m_db.close();
}

bool Database::open(const QString &dbPath)
{
    if (QSqlDatabase::contains("hospital_conn"))
        m_db = QSqlDatabase::database("hospital_conn");
    else
        m_db = QSqlDatabase::addDatabase("QSQLITE", "hospital_conn");

    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    if (!createTables())
        return false;
    if (!seedData())
        return false;
    return true;
}

bool Database::isOpen() const
{
    return m_db.isOpen();
}

QSqlDatabase Database::db() const
{
    return m_db;
}

QString Database::lastError() const
{
    return m_lastError;
}

bool Database::createTables()
{
    QSqlQuery q(m_db);
    const QStringList ddl = {
        // 用户表：登录凭据与角色
        "CREATE TABLE IF NOT EXISTS users ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT NOT NULL UNIQUE,"
        "  password TEXT NOT NULL,"
        "  role TEXT NOT NULL DEFAULT '医生')",
        // 患者表：本实验核心业务表
        "CREATE TABLE IF NOT EXISTS patients ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  gender TEXT,"
        "  age INTEGER,"
        "  id_card TEXT UNIQUE,"
        "  phone TEXT,"
        "  department TEXT,"
        "  diagnosis TEXT,"
        "  doctor TEXT,"
        "  admit_date TEXT)",
        // 医生表（选做）
        "CREATE TABLE IF NOT EXISTS doctors ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  department TEXT,"
        "  title TEXT,"
        "  phone TEXT)",
        // 科室表（选做）
        "CREATE TABLE IF NOT EXISTS departments ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE,"
        "  location TEXT,"
        "  phone TEXT)",
        // 操作日志表（选做）
        "CREATE TABLE IF NOT EXISTS operation_logs ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT,"
        "  action TEXT,"
        "  detail TEXT,"
        "  log_time TEXT)"
    };

    for (const QString &sql : ddl) {
        if (!q.exec(sql)) {
            m_lastError = q.lastError().text();
            return false;
        }
    }
    return true;
}

bool Database::seedData()
{
    QSqlQuery q(m_db);

    // 初始化用户（仅在空表时写入）
    q.exec("SELECT COUNT(*) FROM users");
    if (q.next() && q.value(0).toInt() == 0) {
        q.exec("INSERT INTO users(username,password,role) VALUES('admin','123456','管理员')");
        q.exec("INSERT INTO users(username,password,role) VALUES('doctor','123456','医生')");
    }

    // 初始化科室（选做）
    q.exec("SELECT COUNT(*) FROM departments");
    if (q.next() && q.value(0).toInt() == 0) {
        q.exec("INSERT INTO departments(name,location,phone) VALUES('内科','门诊楼2层','0769-11110001')");
        q.exec("INSERT INTO departments(name,location,phone) VALUES('外科','门诊楼3层','0769-11110002')");
        q.exec("INSERT INTO departments(name,location,phone) VALUES('儿科','门诊楼1层','0769-11110003')");
        q.exec("INSERT INTO departments(name,location,phone) VALUES('骨科','住院楼4层','0769-11110004')");
    }

    // 初始化医生（选做）
    q.exec("SELECT COUNT(*) FROM doctors");
    if (q.next() && q.value(0).toInt() == 0) {
        q.exec("INSERT INTO doctors(name,department,title,phone) VALUES('李强','内科','主任医师','13900000001')");
        q.exec("INSERT INTO doctors(name,department,title,phone) VALUES('王芳','外科','副主任医师','13900000002')");
        q.exec("INSERT INTO doctors(name,department,title,phone) VALUES('陈静','儿科','主治医师','13900000003')");
        q.exec("INSERT INTO doctors(name,department,title,phone) VALUES('周涛','骨科','主治医师','13900000004')");
    }

    // 初始化患者示例数据
    q.exec("SELECT COUNT(*) FROM patients");
    if (q.next() && q.value(0).toInt() == 0) {
        const QStringList rows = {
            "INSERT INTO patients(name,gender,age,id_card,phone,department,diagnosis,doctor,admit_date)"
            " VALUES('张三','男',45,'441900197901011234','13800000001','内科','高血压','李强','2026-11-01')",
            "INSERT INTO patients(name,gender,age,id_card,phone,department,diagnosis,doctor,admit_date)"
            " VALUES('李四','女',32,'441900199202022345','13800000002','外科','阑尾炎','王芳','2026-11-02')",
            "INSERT INTO patients(name,gender,age,id_card,phone,department,diagnosis,doctor,admit_date)"
            " VALUES('王五','男',58,'441900196801033456','13800000003','内科','糖尿病','李强','2026-11-03')",
            "INSERT INTO patients(name,gender,age,id_card,phone,department,diagnosis,doctor,admit_date)"
            " VALUES('赵六','女',27,'441900199704044567','13800000004','儿科','支气管炎','陈静','2026-11-04')",
            "INSERT INTO patients(name,gender,age,id_card,phone,department,diagnosis,doctor,admit_date)"
            " VALUES('钱七','男',63,'441900196205055678','13800000005','骨科','骨折','周涛','2026-11-05')"
        };
        for (const QString &sql : rows)
            q.exec(sql);
    }
    return true;
}

bool Database::checkLogin(const QString &username, const QString &password, QString *role)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT role FROM users WHERE username = ? AND password = ?");
    q.addBindValue(username);
    q.addBindValue(password);
    if (q.exec() && q.next()) {
        if (role)
            *role = q.value(0).toString();
        return true;
    }
    return false;
}

void Database::logOperation(const QString &username, const QString &action, const QString &detail)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO operation_logs(username,action,detail,log_time) VALUES(?,?,?,?)");
    q.addBindValue(username);
    q.addBindValue(action);
    q.addBindValue(detail);
    q.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    q.exec();
}
