#include "SetPriceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

SetPriceDialog::SetPriceDialog(QWidget *parent)
    : QDialog(parent), m_price(0)
{
    setWindowTitle("Set VMC Price");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Enter price (in cents):", this);
    mainLayout->addWidget(label);

    m_priceInput = new QLineEdit(this);
    mainLayout->addWidget(m_priceInput);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &SetPriceDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

int SetPriceDialog::getPrice() const
{
    return m_price;
}

void SetPriceDialog::onOkClicked()
{
    bool ok;
    int price = m_priceInput->text().toInt(&ok);
    if (ok && price >= 0 && price <= 999) {
        m_price = price;
        accept();
    } else {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid price between 0 and 999 cents.");
    }
}