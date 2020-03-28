#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include "table.h"
#include "file.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateInformation();
    void updateSelection();
    void clear();

public slots:
    void showResizeDialog();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

    Table *table = new Table();
    File* f = new File(table);
    QTimer *timer;

    bool currentFileHaveSavePath = false;
    QString fileSavePath = "";
    std::vector<tableData> actions;

    QLabel* tableSize = new QLabel("Rows: 1   Columns: 1");
    QLabel* tableSelection = new QLabel("Selected: 1:1");

    int showMaybeSaveWindow();
    void addUndoAction();
    void savePathUpdate(QString path);
    void sort(int column);

    void autoSave();

    void newFile();
    void restart();

    // Menu
    void about();
    void howToUse();
    void open();
    void save();
    void saveAs();
    void undo();
    void exit();
    void printToConsole();
};
#endif // MAINWINDOW_H
