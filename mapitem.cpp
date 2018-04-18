#include "mapitem.h"

#include "maptree.h"
#include "mapbrowser.h"

#include <QDebug>

MapItem::MapItem(QTreeWidget* parent) :
    QTreeWidgetItem(parent, QTreeWidgetItem::UserType + 1)
{
    this->setIcon(0, QIcon(":/images/map.ico"));
    clearMapData();
}

MapItem::MapItem(QTreeWidgetItem* parent) :
    QTreeWidgetItem(parent, QTreeWidgetItem::UserType + 1)
{
    this->setIcon(0, QIcon(":/images/map.ico"));
    clearMapData();
}

void MapItem::clearMapData()
{
    m_code = m_name = m_type = m_description = m_xml = m_extra = QString();
    m_state = m_votes = m_percent = 0;
    m_images.clear();
}

void MapItem::deleteMe()
{
    MapFolder* parent = (MapFolder*)this->parent();
    QDir dir(parent->fullDirectory());

    QFile way = dir.filePath(m_code + ".map");
    way.remove();

    way.setFileName(dir.filePath(m_code + ".xml"));
    way.remove();

    parent->removeChild(this);
}

void MapItem::setMapData(const QString& acode, const QString& aname,
                         const QString& atype, int astate, int avotes,
                         int apercent, const QString& adescription,
                         const QString& axml, const QString& aextra,
                         const QStringList& aimages)
{
    m_code = acode;

    m_name = aname;
    m_type = atype;

    m_state = astate;

    m_votes = avotes;
    m_percent = apercent;

    m_description = adescription;
    m_xml = axml;
    m_extra = aextra;

    m_images = aimages;
}

void MapItem::edited(bool updateCode, const QString& lastCode)
{
    MapBrowser* mb = (MapBrowser*)this->treeWidget()->parentWidget()->parentWidget();
    mb->mapChanged(this, 0);

    mb->loader()->saveMap(this, (MapFolder*)this->parent(), updateCode, lastCode);
}

void MapItem::setText(const QString &text)
{
    QTreeWidgetItem::setText(0, text);
    m_code = text;
}

QMenu* MapItem::menu(QWidget* parent)
{
    MapTree* tree = (MapTree*)parent;

    QMenu* fmenu = new QMenu(parent);

    fmenu->addAction(QIcon(":/images/save.ico"), QObject::tr("Exportar"))->setData((int)new MapAction(MapAction::Export, this, tree));
    fmenu->addSeparator();
    fmenu->addAction(QIcon(":/images/move.ico"), QObject::tr("Mover"))->setData((int)new MapAction(MapAction::Move, this, tree));
    fmenu->addAction(QIcon(":/images/edit.ico"), QObject::tr("Editar"))->setData((int)new MapAction(MapAction::Edit, this, tree));
    fmenu->addSeparator();
    fmenu->addAction(QIcon(":/images/delitem.ico"), QObject::tr("Apagar"))->setData((int)new MapAction(MapAction::Delete, this, tree));

    fmenu->connect(fmenu, SIGNAL(triggered(QAction*)), tree, SLOT(processAction(QAction*)));

    return fmenu;
}
