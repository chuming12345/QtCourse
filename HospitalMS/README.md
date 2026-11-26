# 医院诊疗系统 · 用户信息管理（实验3）

Qt 5.15.2 + SQLite + MVC（Model/View）实现的医院患者信息管理桌面应用。
开发者：邬婧文　学号：2024414300230

## 一、运行方式

1. 用 Qt Creator 打开 `HospitalMS.pro`，选择 MinGW 套件构建运行；
2. 首次运行会在可执行文件同目录自动生成 `hospital.db`（SQLite），并自动建表与写入示例数据；
3. 默认账号：`admin / 123456`（管理员）、`doctor / 123456`（医生）。

## 二、功能

- MVC 分层：`QSqlTableModel` / `QSqlQueryModel` 作为 Model，`QTableView` 作为 View，`MainWindow` 负责控制逻辑；
- 登录检测：用户名 + 密码校验，成功进入管理页，失败明确提示；
- 患者信息：列表显示、按姓名/身份证/科室查询、新增、修改（表格内编辑）、删除（二次确认）；
- 自定义代理：性别列使用 `QComboBox` 下拉编辑；
- 工具栏代码化：用 `QToolBar` + `addWidget/addAction` 动态添加查询框与按钮；
- 界面切换：`QStackedWidget` 在登录页与管理页之间切换；
- 开发者学号姓名：登录页与状态栏常驻显示；
- 选做：医生表、科室表、操作日志表（登录/增删改均记录）。

## 三、数据库设计（SQLite：hospital.db）

| 表 | 字段 | 说明 |
|---|---|---|
| users | id, username, password, role | 登录用户与角色 |
| patients | id, name, gender, age, id_card, phone, department, diagnosis, doctor, admit_date | 患者信息（id_card 唯一） |
| doctors | id, name, department, title, phone | 医生信息（选做） |
| departments | id, name, location, phone | 科室信息（选做） |
| operation_logs | id, username, action, detail, log_time | 操作日志（选做） |

建表语句（节选）：

```sql
CREATE TABLE IF NOT EXISTS patients (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  gender TEXT,
  age INTEGER,
  id_card TEXT UNIQUE,
  phone TEXT,
  department TEXT,
  diagnosis TEXT,
  doctor TEXT,
  admit_date TEXT
);
```

## 四、异常与边界测试场景

1. 登录错误凭据：输入 admin/000000 → 提示“用户名或密码错误”，不进入系统；
2. 新增缺失必填项：姓名为空 → 弹窗“姓名和身份证号为必填项”；
3. 身份证号重复：新增已存在的 id_card → 唯一约束报错并给出提示；
4. 查询无结果：查询不存在的关键字 → 提示“未找到相关记录”，列表为空；
5. 删除确认：删除前弹出确认框，选“否”则取消；
6. 修改后刷新：表格内修改性别/年龄 → 保存修改后 `select()` 刷新，界面与库一致；
7. 数据库连接失败：数据库路径不可写/驱动缺失时，启动弹出“数据库连接失败”并显示路径与错误信息。

## 五、AI 辅助开发过程（摘要）

- 问题：如何在 Qt 中用 Model/View 把患者表绑定到 `QTableView` 并支持编辑后写回数据库？
- 提示词：给出 `QSqlTableModel` 绑定 SQLite 表、`OnManualSubmit` 提交、自定义代理下拉编辑的完整示例，并说明刷新时机。
- AI 方案：`QSqlTableModel` + `setTable("patients")` + `OnManualSubmit`，保存时 `submitAll()`，修改后 `select()`；性别列用 `QStyledItemDelegate` 的 `QComboBox`。
- 运行验证：新增重复身份证号触发唯一约束错误；保存修改后刷新，库中数据与表格一致。
- 发现的问题：`QSqlQuery::addBindValue` 传入 `QString` 编译报错（`QVariant` 不完整类型）→ 补 `#include <QVariant>`。
- 自己的修改：补充必填校验、删除确认、查询无结果提示、连接失败提示，保证数据一致性。
