#ifndef FILE_H
#define FILE_H

#include <QFile>
#include <QObject>
#include "table.h"

#define tableData std::vector<std::vector<Cell>>

class File: public QObject
{
    Q_OBJECT

private:
    QFile _file;
    bool _hasChanges = false;  
    bool _haveSavePath = false;

public:
    /*
       В конструктор передается модель таблицы.
       При изменении модели этот класс узнает, что файл на диске теперь отличается.
    */
    File(Table *table);

    // Открывает файл на диске и заносит данные в модель таблицы.
    void open(Table *table, const QString &path);

    // Сохраняет данные модели таблицы в файл на диске.
    void saveAs(const tableData &table, const QString &path);

    bool hasChanges();

    void setHasChangesFlag(bool flag);
    void setHasChangesFlagTrue();
    void setHasChangesFlagFalse();

signals:
    void fileOpened();
    void fileSaved();
};

#endif // FILE_H
