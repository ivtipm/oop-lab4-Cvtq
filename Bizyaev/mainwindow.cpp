#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tableresizedialog.h"
#include <QFileDialog>
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QTableView>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableView->setModel(this->table);   // Связываем с моделью

    this->addUndoAction();

    // Добавление информации в статус-бар
    ui->statusbar->addPermanentWidget(tableSelection);
    ui->statusbar->addPermanentWidget(tableSize);

    // Undo:
    // При измении данных в таблице
    connect(this->table, &Table::dataChanged, this, &MainWindow::addUndoAction);
    // При изменении размеров таблицы
    connect(this->table, &Table::resized, this, &MainWindow::addUndoAction);

    // Как только в Table произойдут изменения, File узнает об этом
    connect(this->table, &Table::modelReset, this->f, &File::setHasChangesFlagTrue);

    // Нужно для обновления информации в статус-баре  
    connect(this->table, &Table::modelReset, this, &MainWindow::updateInformation);

    // Обновляет информацию о выделенной ячейке
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::updateSelection);

    // Связываем главное меню с методами
    connect(ui->actionResize,  &QAction::triggered, this, &MainWindow::showResizeDialog);
    connect(ui->actionDebug,   &QAction::triggered, this, &MainWindow::printToConsole);
    connect(ui->actionOpen,    &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionSave,    &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSaveAs,  &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->actionExit,    &QAction::triggered, this, &MainWindow::exit);
    connect(ui->actionClear,   &QAction::triggered, this, &MainWindow::clear);
    connect(ui->actionUndo,    &QAction::triggered, this, &MainWindow::undo);
    connect(ui->actionAbout,   &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionHelp,    &QAction::triggered, this, &MainWindow::howToUse);
    connect(ui->actionNew,     &QAction::triggered, this,  &MainWindow::newFile);


    connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::sort);

    // Таймер на автосохранение
    timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &MainWindow::autoSave);
    this->timer->start(60000); // таймер на сохранение срабатывает раз в минуту
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Вызываем метод сортировки у таблицы
void MainWindow::sort(int column){
    this->addUndoAction();
    Qt::SortOrder direction = ui->tableView->horizontalHeader()->sortIndicatorOrder();
    this->table->sort(column, direction);

}
// Обновляет информацию в статус-баре
void MainWindow::updateInformation(){
    this->tableSize->setText(QString("Rows: ") + QString::number(this->table->rowCount()) + QString("   Columns: ") + QString::number(this->table->columnCount()));
}

// Сохраняет дейсвтия пользователя
void MainWindow::addUndoAction(){
    this->actions.push_back(this->table->data());
    if (this->actions.size()>30){
        (this->actions.erase(this->actions.begin()));
    }
}

// Отменяет последене действие в таблице
void MainWindow::undo(){
    this->actions.pop_back();
    if (this->actions.size() != 0) {
        this->table->setData(this->actions.back());
    }
}

// Обновляет данные о выделении в статус-баре
void MainWindow::updateSelection(){
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    if (!indexes.empty()){
        QModelIndex indexF = indexes.first();
        QModelIndex indexL = indexes.last();
        if (indexF == indexL){
            this->tableSelection->setText(QString("Selection: ") + QString::number(indexF.row() + 1) + QString(":") + QString::number(indexF.column() + 1));
        }
        else{
            this->tableSelection->setText(QString("Selection: ") + QString::number(indexF.row() + 1) + QString("-") + QString::number(indexF.column() + 1) + QString(":") +
                                          QString::number(indexL.row() + 1) + QString("-") + QString::number(indexL.column() + 1));
        }
    }
    else {
            this->tableSelection->setText(QString("Selection: - "));
        }
}

// Очищает все ячеки, находящиеся в выделении
void MainWindow::clear() {
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    QModelIndex index;
    for (int i = 0; i < indexes.count(); ++i)
    {
        index = indexes.at(i);
        this->table->setData(index, "");
        // To get the row/column numbers use index.row() / index.column()
    }
}
// Shows TableResizeDialog form
void MainWindow::showResizeDialog() {
    TableResizeDialog* dialog = new TableResizeDialog(table, this);
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
}


// Сохранение копии
void MainWindow::autoSave() {
    if (this->currentFileHaveSavePath){
        this->fileSavePath.chop(2);
        this->f->saveAs(this->table->data(), this->fileSavePath + QString(" - backup"));
        qDebug() << "Autosaved!";
    }
}

// Сохранение в файл. Если файл еще не был сохранен - вызывается saveAs()
void MainWindow::save() {
    if (this->currentFileHaveSavePath){
        this->f->saveAs(this->table->data(), this->fileSavePath);
    } else {
        this->saveAs();
    }
}

void MainWindow::savePathUpdate(QString path){
    this->currentFileHaveSavePath= true;
    this->fileSavePath = path;
}

// Evokes SaveAs dialog and then calls saveAs method of Table
void MainWindow::saveAs() {
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Database (*.b)"));
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec()) {
        this->f->saveAs(this->table->data(), dialog.selectedFiles()[0]);
        savePathUpdate(dialog.selectedFiles()[0]);
    }
}

// Evokes Open dialog and then calls open method of Table
void MainWindow::open() {
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Database (*.b);; All files (*.*)"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
        this->f->open(table, dialog.selectedFiles()[0]);
        this->f->setHasChangesFlagFalse();                               // небольшой костыль - при table.setData() вызвается сигнал, по которому мы считаем, что в таблице произошли изменения. решение - определить свои сигналы
    }
    savePathUpdate(dialog.selectedFiles()[0]);
}

// Cброс программы
void MainWindow::restart(){
   this->currentFileHaveSavePath = false;
   this->fileSavePath = "";
   this->table->clear();
   this->actions.clear();
   this->f->setHasChangesFlagFalse();
}

void MainWindow::newFile(){
    int userReply = showMaybeSaveWindow();
    switch (userReply){
        case QMessageBox::Save:
            // Сохраняем файл и создаем новый документ
            this->save();
            restart();
            break;
        case QMessageBox::Cancel:
            // Отмена
            break;
        case QMessageBox::Discard:
            // Создаем новый документ
            restart();
            break;
     }
}

int MainWindow::showMaybeSaveWindow(){
    QMessageBox message(this);
    message.setWindowTitle(this->windowTitle());
    message.setText("The database has been modified.");
    message.setInformativeText("Do you want to save your changes?");
    message.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    message.setDefaultButton(QMessageBox::Save);
    return message.exec();
}
void MainWindow::closeEvent(QCloseEvent *event){
    // If changes were made
    if (this->f->hasChanges()){
        int userReply = showMaybeSaveWindow();
        // Switch for different user-choices (buttons)
        switch (userReply){
            case QMessageBox::Save:
                // Make a save and accept close event
                this->save();
                event->accept();
                break;
            case QMessageBox::Cancel:
                // Ignore close event
                event->ignore();
                break;
            case QMessageBox::Discard:
                // Just exit application
                event->accept();
                break;
            default:  event->ignore(); break;
        }

    }
    else {
        event->accept();
    }
}

// Closes window
void MainWindow::exit() {
    this->close();
}

// Debug Function
void MainWindow::printToConsole(){
    table->print();
}

void MainWindow::about() {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(this->windowIcon());
    msgBox.setText("Database editor v1.0");
    msgBox.exec();
}

void MainWindow::howToUse() {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(this->windowIcon());
    msgBox.setText("Почти как в Excel. Для сортировки надо нажимать на столбцы.");
    msgBox.exec();
}
