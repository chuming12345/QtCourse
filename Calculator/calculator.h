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
};

#endif // CALCULATOR_H
