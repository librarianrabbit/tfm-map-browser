#ifndef MAPACTION_H
#define MAPACTION_H

#include <QObject>
#include <QTreeWidgetItem>

class MapAction : public QObject
{
    Q_OBJECT

public:
    enum
    {
        Delete = 1,
        AddNewFolder = 2,
        AddNewMap = 3,
        Edit = 4,
        Export = 5,
        Move = 6
    };

public:
    explicit MapAction(int act, QTreeWidgetItem* it, QObject *parent = 0);

    int action() { return m_action; }
    QTreeWidgetItem* item() { return m_item; }

private:
    int m_action;
    QTreeWidgetItem* m_item;
};

#endif // MAPACTION_H
