#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QtCore/QObject>

namespace Drive
{

class FileSystemHelper : public QObject
{
    Q_OBJECT
public:
    static FileSystemHelper& instance();

    static void setWindowsFolderIcon(const QString& folderPath, int iconNumber);

private:
    Q_DISABLE_COPY(FileSystemHelper)
    explicit FileSystemHelper(QObject *parent = 0);
    
};

}


#endif FILE_UTILS_H