#ifndef MAPFOLDER_H
#define MAPFOLDER_H

#include <QMenu>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class MapItem;

class MapFolder : public QTreeWidgetItem
{    
public:
    MapFolder(QTreeWidget* parent = 0);
    MapFolder(QTreeWidgetItem* parent);

    void setText(const QString &text);

    QMenu* menu(QWidget* parent = 0);

    QString directory();
    void setDirectory(const QString& dir);

    QString fullDirectory();

    QList<MapFolder*> folders();
    QList<MapItem*> maps();

    QString path();

    void deleteMe();

    void deleteChild(QTreeWidgetItem* item);

    QTreeWidgetItem* find(const QString& name);
    bool exists(const QString& name);

public slots:
    void renamed();

private:
    QString m_directory;

public:
    static const int FolderType = QTreeWidgetItem::UserType;

public:
    static void copyDirectory(const QString& a, const QString& b);
    static void removeDirectory(const QString& directory);
};

#endif // MAPFOLDER_H
