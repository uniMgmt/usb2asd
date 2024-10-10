#ifndef SETPRICEDIALOG_H
#define SETPRICEDIALOG_H

#include <QDialog>
#include <QLineEdit>

class SetPriceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetPriceDialog(QWidget *parent = nullptr);
    int getPrice() const;

private slots:
    void onOkClicked();

private:
    QLineEdit *m_priceInput;
    int m_price;
};

#endif // SETPRICEDIALOG_H