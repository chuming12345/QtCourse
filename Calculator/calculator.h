#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QString>
#include <QResizeEvent>

class Calculator : public QWidget
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *e) override;    // ⑤ 阶段：键盘事件
    void resizeEvent(QResizeEvent *event) override;   // 界面缩放时字体等比放大

private slots:
    void digitClicked(const QString &digit);
    void decimalPointPressed();
    void additiveOperatorPressed(const QString &op);
    void multiplicativeOperatorPressed(const QString &op);
    void equalPressed();
    void backspacePressed();
    void clearAll();

private:
    void abortOperation();
    bool calculate(double right, const QString &op);

    QLineEdit *display = nullptr;
    double waitingOperand = 0.0;
    QString pendingOperator;
    bool waitingForOperand = true;
};

#endif // CALCULATOR_H
