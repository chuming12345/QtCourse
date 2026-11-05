#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QString>

class Calculator : public QWidget
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);

private slots:
    void digitClicked(const QString &digit);
    void decimalPointPressed();
    void backspacePressed();
    void clearAll();

private:
    QLineEdit *display = nullptr;
    bool waitingForOperand = true;   // ② 阶段：引入“等待操作数”状态
};

#endif // CALCULATOR_H
