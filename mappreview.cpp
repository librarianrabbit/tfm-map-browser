#include "mappreview.h"

#include <QApplication>
#include <QClipboard>

#include <QtXml/qdom.h>
#include <QDebug>

#include "mapitem.h"

MapPreview::MapPreview(QObject *parent) :
    QObject(parent)
{
    this->setObjectName("preview");
}

void MapPreview::copyToClipboard(const QString& data)
{
    QApplication::clipboard()->setText(data);
}

void MapPreview::mapClicked(int addr)
{
    MapItem* mi = (MapItem*)addr;
    if (!mi) return;

    mi->treeWidget()->setCurrentItem(mi);
}
