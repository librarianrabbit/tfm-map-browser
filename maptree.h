#ifndef MAPTREE_H
#define MAPTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>

#include "mapaction.h"

class MapFolder;
class QFileDialog;

class MapTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit MapTree(QWidget *parent = 0);
    ~MapTree();

    QList<MapFolder*> folders();

public slots:
    void processAction(QAction* item);
    void processMapAction(MapAction* action);

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private:
    void _copyFolder(MapFolder* current, MapFolder* target);

private:
    QMenu* defaultMenu();

    void addFolder(QTreeWidgetItem* item);
    void renameFolder(QTreeWidgetItem* item);
    void exportFolder(QTreeWidgetItem *item);

    void _getFolders(MapFolder* current, QList<MapFolder*>& list);

    void addItem(QTreeWidgetItem* item);
    void editItem(QTreeWidgetItem* item);
    void deleteItem(QTreeWidgetItem* item);
    void moveItem(QTreeWidgetItem* item);
    void exportItem(QTreeWidgetItem* item);

private:
    QTreeWidgetItem* m_tempItem;
    QFileDialog* qfd;
};

#endif // MAPTREE_H
