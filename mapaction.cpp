#include "mapaction.h"

MapAction::MapAction(int act, QTreeWidgetItem* it, QObject *parent) :
    QObject(parent), m_action(act), m_item(it)
{
}
