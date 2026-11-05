#include "calculator.h"

#include <QGridLayout>
#include <QPushButton>
#include <QFont>
#include <QKeyEvent>

Calculator::Calculator(QWidget *parent) : QWidget(parent)
{
    // ⑥ 阶段：样式表美化 + 焦点设置（最终版）
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
    btnBackspace->setObjectName("btnBackspace");
    btnClear->setObjectName("btnClear");
    btnDiv->setObjectName("btnDiv");
    btnMul->setObjectName("btnMul");
    btnSub->setObjectName("btnSub");
    btnAdd->setObjectName("btnAdd");
    btnEqual->setObjectName("btnEqual");
    btnDot->setObjectName("btnDot");
    btn0->setObjectName("btn0");
    btn1->setObjectName("btn1");
    btn2->setObjectName("btn2");
    btn3->setObjectName("btn3");
    btn4->setObjectName("btn4");
    btn5->setObjectName("btn5");
    btn6->setObjectName("btn6");
    btn7->setObjectName("btn7");
    btn8->setObjectName("btn8");
    btn9->setObjectName("btn9");

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
    setStyleSheet(
        "QWidget { font-family: 'Segoe UI', 'Microsoft YaHei'; }"
        "QLineEdit { background: #1e2530; color: #ffffff; border-radius: 6px;"
        "            padding: 6px 10px; selection-background-color: #3a7afe; }"
        "QPushButton { background: #f0f2f5; color: #222;"
        "              border-radius: 8px; min-height: 40px; font-size: 16px; }"
        "QPushButton:hover { background: #d7e3ff; }"
        "QPushButton:pressed { background: #3a7afe; color: #fff; }"
        "QPushButton#btnAdd, QPushButton#btnSub, QPushButton#btnMul, QPushButton#btnDiv {"
        "              background: #ffe0b2; }"
        "QPushButton#btnEqual { background: #3a7afe; color: #fff; }"
        "QPushButton#btnClear { background: #ffd0d0; }"
    );
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
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
    if (!display->text().contains('.'))
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
        if (right == 0.0) { abortOperation(); return false; }
        waitingOperand = left / right;
    }
    return true;
}

void Calculator::keyPressEvent(QKeyEvent *e)
{
    // 小键盘数字键在 Qt 中映射为 Qt::Key_0..Qt::Key_9（并带 Qt::KeypadModifier 修饰符），
    // 因此无需单独枚举 NumPad，同一分支即可同时覆盖主键盘与小键盘。
    switch (e->key()) {
    case Qt::Key_0: digitClicked("0"); break;
    case Qt::Key_1: digitClicked("1"); break;
    case Qt::Key_2: digitClicked("2"); break;
    case Qt::Key_3: digitClicked("3"); break;
    case Qt::Key_4: digitClicked("4"); break;
    case Qt::Key_5: digitClicked("5"); break;
    case Qt::Key_6: digitClicked("6"); break;
    case Qt::Key_7: digitClicked("7"); break;
    case Qt::Key_8: digitClicked("8"); break;
    case Qt::Key_9: digitClicked("9"); break;
    case Qt::Key_Period: decimalPointPressed(); break;
    case Qt::Key_Plus:       additiveOperatorPressed("+"); break;
    case Qt::Key_Minus:      additiveOperatorPressed("-"); break;
    case Qt::Key_Asterisk:   multiplicativeOperatorPressed("*"); break;
    case Qt::Key_Slash:      multiplicativeOperatorPressed("/"); break;
    case Qt::Key_Equal: case Qt::Key_Return: case Qt::Key_Enter: equalPressed(); break;
    case Qt::Key_Backspace:  backspacePressed(); break;
    case Qt::Key_Escape:     clearAll(); break;
    case Qt::Key_Delete:     clearAll(); break;
    default: QWidget::keyPressEvent(e);
    }
}
