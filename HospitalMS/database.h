#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>

// 数据库单例（Singleton）：统一管理连接、建表、初始化数据、登录校验与操作日志。
// 采用单例可以保证整个程序只持有一个数据库连接，避免重复打开导致的状态不一致。
class Database
{
public:
    static Database &instance();

    // 打开数据库（若文件不存在则自动创建），并完成建表与种子数据初始化
    bool open(const QString &dbPath);
    bool isOpen() const;
    QSqlDatabase db() const;

    // 登录校验：凭据正确返回 true，并通过 role 输出用户角色
    bool checkLogin(const QString &username, const QString &password, QString *role = nullptr);

    // 记录用户操作日志（选做：用户操作记录保存与查询）
    void logOperation(const QString &username, const QString &action, const QString &detail);

    QString lastError() const;

private:
    Database();
    ~Database();
    Q_DISABLE_COPY(Database)

    bool createTables();
    bool seedData();

    QSqlDatabase m_db;
    QString      m_lastError;
};

#endif // DATABASE_H
