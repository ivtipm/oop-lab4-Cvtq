#ifndef TABLERESIZEDIALOG_H
#define TABLERESIZEDIALOG_H

#include <QDialog>
#include "table.h"

namespace Ui {
class TableResizeDialog;
}

class TableResizeDialog : public QWidget       
{
    /*
     * Форма, предлагающая изменить размеры таблицы.
     */
    Q_OBJECT

public:
    explicit TableResizeDialog(Table* table, QWidget* parentForPlacment, QWidget *parent = nullptr);
    ~TableResizeDialog();

private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonOk_clicked();

private:
    Ui::TableResizeDialog *ui;
    Table* _table;
};

#endif // TABLERESIZEDIALOG_H
