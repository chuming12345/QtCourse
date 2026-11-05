#include "calculator.h"

#include <QGridLayout>
#include <QPushButton>
#include <QFont>

Calculator::Calculator(QWidget *parent) : QWidget(parent)
{
    // ④ 阶段：修复连续小数点 / 除零 / 结果后继续输入等边界问题
    // 显示框
    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);
    display->setFocusPolicy(Qt::NoFocus);
    QFont f = display->font();
    f.setPointSize(20);
    display->setFont(f);

    // 按钮
    QPushButton *btnBackspace = new QPushButton("←");
    QPushButton *btnClear = new QPushButton("C");
    QPushButton *btnDiv = new QPushButton("/");
    QPushButton *btnMul = new QPushButton("*");
    QPushButton *btnSub = new QPushButton("-");
    QPushButton *btnAdd = new QPushButton("+");
    QPushButton *btnEqual = new QPushButton("=");
    QPushButton *btnDot = new QPushButton(".");
    QPushButton *btn0 = new QPushButton("0");
    QPushButton *btn1 = new QPushButton("1");
    QPushButton *btn2 = new QPushButton("2");
    QPushButton *btn3 = new QPushButton("3");
    QPushButton *btn4 = new QPushButton("4");
    QPushButton *btn5 = new QPushButton("5");
    QPushButton *btn6 = new QPushButton("6");
    QPushButton *btn7 = new QPushButton("7");
    QPushButton *btn8 = new QPushButton("8");
    QPushButton *btn9 = new QPushButton("9");
    connect(btn0, &QPushButton::clicked, this, [this](){ digitClicked("0"); });
    connect(btn1, &QPushButton::clicked, this, [this](){ digitClicked("1"); });
    connect(btn2, &QPushButton::clicked, this, [this](){ digitClicked("2"); });
    connect(btn3, &QPushButton::clicked, this, [this](){ digitClicked("3"); });
    connect(btn4, &QPushButton::clicked, this, [this](){ digitClicked("4"); });
    connect(btn5, &QPushButton::clicked, this, [this](){ digitClicked("5"); });
    connect(btn6, &QPushButton::clicked, this, [this](){ digitClicked("6"); });
    connect(btn7, &QPushButton::clicked, this, [this](){ digitClicked("7"); });
    connect(btn8, &QPushButton::clicked, this, [this](){ digitClicked("8"); });
    connect(btn9, &QPushButton::clicked, this, [this](){ digitClicked("9"); });
    connect(btnDot,       &QPushButton::clicked, this, &Calculator::decimalPointPressed);
    connect(btnBackspace, &QPushButton::clicked, this, &Calculator::backspacePressed);
    connect(btnClear,     &QPushButton::clicked, this, &Calculator::clearAll);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(display, 0, 0, 1, 4);
    mainLayout->addWidget(btnBackspace, 1, 0);
    mainLayout->addWidget(btnClear,     1, 1);
    mainLayout->addWidget(btnDiv,       1, 3);
    mainLayout->addWidget(btn7, 2, 0); mainLayout->addWidget(btn8, 2, 1);
    mainLayout->addWidget(btn9, 2, 2); mainLayout->addWidget(btnMul, 2, 3);
    mainLayout->addWidget(btn4, 3, 0); mainLayout->addWidget(btn5, 3, 1);
    mainLayout->addWidget(btn6, 3, 2); mainLayout->addWidget(btnSub, 3, 3);
    mainLayout->addWidget(btn1, 4, 0); mainLayout->addWidget(btn2, 4, 1);
    mainLayout->addWidget(btn3, 4, 2); mainLayout->addWidget(btnAdd, 4, 3);
    mainLayout->addWidget(btn0, 5, 0, 1, 2);
    mainLayout->addWidget(btnDot, 5, 2);
    mainLayout->addWidget(btnEqual, 5, 3);
    setLayout(mainLayout);
    setWindowTitle("Calculator");
    connect(btnAdd, &QPushButton::clicked, this, [this](){ additiveOperatorPressed("+"); });
    connect(btnSub, &QPushButton::clicked, this, [this](){ additiveOperatorPressed("-"); });
    connect(btnMul, &QPushButton::clicked, this, [this](){ multiplicativeOperatorPressed("*"); });
    connect(btnDiv, &QPushButton::clicked, this, [this](){ multiplicativeOperatorPressed("/"); });
    connect(btnEqual, &QPushButton::clicked, this, &Calculator::equalPressed);
}

void Calculator::digitClicked(const QString &digit)
{
    if (waitingForOperand) {
        display->setText(digit);
        waitingForOperand = false;
    } else {
        if (display->text() == "0")
            display->setText(digit);
        else
            display->setText(display->text() + digit);
    }
}

void Calculator::decimalPointPressed()
{
    if (waitingForOperand) {
        display->setText("0.");
        waitingForOperand = false;
        return;
    }
    if (!display->text().contains('.'))   // ④ 修复：已含小数点则不再添加
        display->setText(display->text() + ".");
}

void Calculator::additiveOperatorPressed(const QString &op)
{
    double operand = display->text().toDouble();
    if (!pendingOperator.isEmpty()) {
        if (!calculate(operand, pendingOperator))
            return;
        display->setText(QString::number(waitingOperand));
    } else {
        waitingOperand = operand;
    }
    pendingOperator = op;
    waitingForOperand = true;
}

void Calculator::multiplicativeOperatorPressed(const QString &op)
{
    double operand = display->text().toDouble();
    if (!pendingOperator.isEmpty()) {
        if (!calculate(operand, pendingOperator))
            return;
        display->setText(QString::number(waitingOperand));
    } else {
        waitingOperand = operand;
    }
    pendingOperator = op;
    waitingForOperand = true;
}

void Calculator::equalPressed()
{
    double operand = display->text().toDouble();
    if (pendingOperator.isEmpty())
        return;
    if (!calculate(operand, pendingOperator))
        return;
    pendingOperator.clear();
    display->setText(QString::number(waitingOperand));
    waitingForOperand = true;
}

void Calculator::backspacePressed()
{
    if (waitingForOperand)
        return;
    QString text = display->text();
    text.chop(1);
    if (text.isEmpty())
        text = "0";
    display->setText(text);
}

void Calculator::clearAll()
{
    display->setText("0");
    waitingOperand = 0.0;
    pendingOperator.clear();
    waitingForOperand = true;
}

void Calculator::abortOperation()
{
    clearAll();
    display->setText("零除错误");
}

bool Calculator::calculate(double right, const QString &op)
{
    double left = waitingOperand;
    if (op == "+")      waitingOperand = left + right;
    else if (op == "-") waitingOperand = left - right;
    else if (op == "*") waitingOperand = left * right;
    else if (op == "/") {
        if (right == 0.0) { abortOperation(); return false; }  // ④ 修复：除零保护
        waitingOperand = left / right;
    }
    return true;
}
