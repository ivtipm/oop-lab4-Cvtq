#include "file.h"
#include <QFileDialog>
#include <QDataStream>

File::File(Table *table)
{
    connect(table, &Table::dataChanged, this, &File::setHasChangesFlagTrue);
    connect(this, &File::fileOpened, this, &File::setHasChangesFlagFalse);
    connect(this, &File::fileSaved, this, &File::setHasChangesFlagFalse);
}

// Opens file located in the path value and writes the file data into table variale
void File::open(Table *table, const QString &path) {
    this->_file.setFileName(path);
    _file.open(QIODevice::ReadWrite);
    QDataStream outStream(&_file);
    size_t rows, columns;
    tableData data;
    outStream >> rows;
    outStream >> columns;
    data.resize(rows);
    for (size_t i = 0; i < data.size(); i++){
        data[i].resize(columns);
        for (size_t j = 0; j < data[i].size(); j++){
            outStream >> data[i][j].data;
        }
    }
    _file.close();
    table->setData(data);
    emit fileOpened();
}

// Saves table into file located in the path
void File::saveAs(const tableData &table, const QString &path) {
    this->_file.setFileName(path + ".b");
    _file.open(QIODevice::ReadWrite);
    QDataStream outStream(&_file);
    outStream << table.size();
    outStream << table[0].size();
    for (size_t i = 0; i < table.size(); i++){
        for (size_t j = 0; j < table[i].size(); j++){
            outStream << table[i][j].data;
        }
    }
    _file.close();
    emit fileSaved();
}

bool File::hasChanges() {
    return this->_hasChanges;
}

void File::setHasChangesFlag(bool flag) {
    this->_hasChanges = flag;
}

void File::setHasChangesFlagTrue() {
    this->_hasChanges = true;
}

void File::setHasChangesFlagFalse() {
    this->_hasChanges = false;
}
