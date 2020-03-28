#ifndef TABLE_H
#define TABLE_H

#include <QAbstractTableModel>
#include <vector>
#include <QVariant>
#include <QMetaType>


struct Cell{
    /*
       Структура - ячейка таблицы.
     */

    QVariant data;
    QMetaType::Type type;
};

#define tableData std::vector<std::vector<Cell>>    // Таблица состоит из матрицы типа Cell

class Table : public QAbstractTableModel
{
    /*
     Класс - Модель таблицы.
     Хранит данные, редактирует данные.
    */

    Q_OBJECT
private:
    // Таблица состоит из матрицы типа Cell
    tableData _table;

public:
    Table();

    // Reimplemented methods from QAbstract model
    // Used by TableView
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags (const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    // Возвращает число строк в таблице
    int rowCount() const;
    // Возвращает число столбцов в таблице
    int columnCount() const;

    // Заменяет текущую таблицу на передаваемый параметр
    void setData(const tableData &table);

    // Устанавливает число строк
    void setRowsAmount(const size_t &rows);

    // Устанавливает число столбцов
    void setColumnsAmount(const size_t &columns);

    // Изменяет размерность таблицы
    void resize(const size_t &rows, const size_t &columns);

    // Обновляет отображение таблицы в TableView
    void resetModel();

    // Возвращает таблицу
    tableData data();

    // Сортировка
    void sort(int column, Qt::SortOrder direction);

    // Удаляет все данные
    void clear();

    // Функция для дебага - выводит таблицу в консоль
    void print() const;

public: signals: void resized();

};

#endif // TABLE_H
