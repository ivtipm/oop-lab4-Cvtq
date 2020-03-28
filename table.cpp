#include "table.h"
#include <QDebug>


Table::Table(){
    /*
     * Изначальная таблица может быть создана как минимум с 1 ячейкой для корректного сохранения в файл
     */
    Cell cell = {"" , QMetaType::QString};
    _table = {{cell}};
}


// Reimplemeted method from QAbstractTableModel
int Table::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _table.size();
}

// Reimplemeted method from QAbstractTableModel
int Table::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _table[0].size();
}

int Table::rowCount() const
{
    return _table.size();
}

int Table::columnCount() const
{
    return _table[0].size();
}

// Reimplemeted method from QAbstractTableModel
QVariant Table::data(const QModelIndex &index, int role) const{
    if (!index.isValid())
        return QVariant();

    if (size_t(index.row()) >= _table.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        QVariant str = _table[index.row()][index.column()].data;
        return str;
    }
    return QVariant();
}

// Return a copy of the tableData
tableData Table::data() {
    return _table;
}

// Reimplemeted method from QAbstractTableModel
Qt::ItemFlags Table::flags (const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

// Reimplemeted method from QAbstractTableModel
// Edit data in the cell with indexes index,index
bool Table::setData(const QModelIndex &index, const QVariant &value, int role){
    Q_UNUSED(role);
    try {
        _table[index.row()][index.column()].data = value;
        emit this->dataChanged(index,index);
        return true;
    } catch (...) {
        return false;
    }
}

void Table::setData(const tableData &table) {
    // make check if table.size is zero
    this->setRowsAmount(table.size());
    for (size_t i = 0; i < table.size(); i++){
        _table[i].resize(table[i].size());
        for (size_t j = 0; j < table[i].size(); j++){
            _table[i][j].data = table[i][j].data;
        }
    }
    this->resetModel();
}

// Reimplemeted method from QAbstractTableModel
void Table::setRowsAmount(const size_t &rows){
    if (rows <= 0) return;
    if (_table.size() == rows) return;
    _table.resize(rows);
    this->resetModel();
}

// Changes amount of columns in the table and resets model
void Table::setColumnsAmount(const size_t &columns){
    if (columns <= 0) return;
    //if (_table[0].size() == columns) return;
    for (size_t i = 0; i<_table.size(); i++){
           _table[i].resize(columns);
    }
   this->resetModel();
}

// Changes amount of rows and columns of table
void Table::resize(const size_t &rows, const size_t &columns){
    this->setRowsAmount(rows);
    this->setColumnsAmount(columns);
    this->resetModel();
    emit resized();
}

// Сортировка
void Table::sort(int column, Qt::SortOrder direction){
    QStringList list;
    for (int i=0; i<this->rowCount(); i++){
        list.append(this->_table[i][column].data.toString());
    }
    list.sort();
    if (direction == Qt::AscendingOrder) {
        // по возрастанию
        for (int i=0; i<this->rowCount(); i++){
            this->_table[i][column].data = list[i];
        }
    } else
        if (direction == Qt::DescendingOrder) {
        //по убыванию
        for (int i=0; i<this->rowCount(); i++){
            this->_table[i][column].data = list[this->rowCount() - i - 1];
        }
    }
    this->resetModel();
}

// Resets model for viewers
void Table::resetModel() {
    this->beginResetModel();
    this->endResetModel();
}

// Удаляет все данные
void Table::clear(){
    //this->_table.clear();
    this->resize(1,1);
    this->_table[0][0] = Cell{"", QMetaType::QString};
    this->resetModel();
}

// Debug
void Table::print() const{
    QString line;
    for (size_t i = 0; i<_table.size(); i++){
        for (size_t j = 0; j<_table[i].size(); j++){
                //qDebug() << typeid( _table[i][j]).name();
                line +=_table[i][j].data.toString() + " ";
            }
        line.chop(1);
        qDebug() << line;
        qDebug() << " ";
        line.clear();
    }
}
