#include "tableresizedialog.h"
#include "ui_tableresizedialog.h"

TableResizeDialog::TableResizeDialog(Table* table, QWidget* parentForPlacment, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableResizeDialog)
{  
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->_table = table;
    if (parentForPlacment != nullptr){
        this->move(parentForPlacment->x() + int(parentForPlacment->geometry().width() / 2)  - int(this->geometry().width() / 2),
                   parentForPlacment->y() + int(parentForPlacment->geometry().height() / 2) - int(this->geometry().height() / 2));
    }
}

TableResizeDialog::~TableResizeDialog()
{
    delete ui;
}

void TableResizeDialog::on_pushButtonCancel_clicked()
{
    this->close();
}

void TableResizeDialog::on_pushButtonOk_clicked()
{
   _table->resize(ui->spinBoxRows->value(), ui->spinBoxColumns->value());
   this->close();
}
