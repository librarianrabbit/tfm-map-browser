#include "mapfolder.h"

#include "maptree.h"
#include "mapitem.h"
#include "mapbrowser.h"
#include "maploader.h"

#include <QDirIterator>

#include <QDebug>

MapFolder::MapFolder(QTreeWidget* parent) :
    QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
    this->setIcon(0, QIcon(":/images/folder.ico"));
}

MapFolder::MapFolder(QTreeWidgetItem* parent) :
    QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
    this->setIcon(0, QIcon(":/images/folder.ico"));
}

QList<MapFolder*> MapFolder::folders()
{
    QList<MapFolder*> f;

    for (int x = 0; x < this->childCount(); x++)
    {
        MapFolder* ch = (MapFolder*)this->child(x);

        if (ch->type() == MapFolder::FolderType)
        {
            f.append(ch);
        }
    }

    return f;
}

QList<MapItem*> MapFolder::maps()
{
    QList<MapItem*> i;

    for (int x = 0; x < this->childCount(); x++)
    {
        MapItem* ch = (MapItem*)this->child(x);

        if (ch->type() == MapItem::MapType)
        {
            i.append(ch);
        }
    }

    return i;
}

void MapFolder::copyDirectory(const QString& a, const QString& b)
{
    QDir source(a);
    QDir target(b);

    if (QDir(target.filePath(source.dirName())).exists())
    {
        removeDirectory(target.filePath(source.dirName()));
    }

    if (!target.mkdir(source.dirName())) return;

    for (QDirIterator it(source) ;; it.next())
    {
        if (it.fileName() == "." || it.fileName() == ".." || it.fileName().isEmpty()) continue;

        QFileInfo file = it.fileInfo();

        if (file.isDir())
        {
            copyDirectory(file.filePath(), target.filePath(source.dirName()));
        }
        else if (file.isFile())
        {
            QDir where(target.filePath(source.dirName()));
            QFile::copy(file.filePath(), where.filePath(file.fileName()));
        }

        if (!it.hasNext()) break;
    }
}

void MapFolder::removeDirectory(const QString& directory)
{
    QDir dir(directory);

    for (QDirIterator it(dir) ;; it.next())
    {
        if (it.fileName() == "." || it.fileName() == ".." || it.fileName().isEmpty()) continue;

        QFileInfo file = it.fileInfo();

        if (file.isDir())
        {
            removeDirectory(it.filePath());
        }
        else if (file.isFile())
        {
            dir.remove(file.fileName());
        }

        if (!it.hasNext()) break;
    }

    QString name = dir.dirName();
    if (dir.cdUp()) dir.rmdir(name);
}

void MapFolder::deleteMe()
{
    MapFolder* parent = (MapFolder*)this->parent();
    if (!parent) return;

    removeDirectory(fullDirectory());

    parent->removeChild(this);
}

void MapFolder::deleteChild(QTreeWidgetItem* item)
{
    if (item->type() == MapFolder::FolderType)
    {
        MapFolder* mf = (MapFolder*)item;
        mf->deleteMe();
    }
    else if (item->type() == MapItem::MapType)
    {
        MapItem* mi = (MapItem*)item;
        mi->deleteMe();
    }

    MapBrowser* mb = (MapBrowser*)this->treeWidget()->parentWidget()->parentWidget();
    mb->loader()->saveDirectory(this);
}

void MapFolder::renamed()
{
    if (!m_directory.isEmpty())
    {
        QDir up(fullDirectory());

        QString oldname = up.dirName();
        if (!up.cdUp()) return;
        if (!up.rename(oldname, this->text(0))) return;

        m_directory = this->text(0);

        if (!this->parent())
        {
            QFile maproot(up.filePath("map_root"));

            if (maproot.open(QFile::WriteOnly))
            {
                maproot.write(this->text(0).toAscii());
                maproot.close();

                MapBrowser* mb = (MapBrowser*)this->treeWidget()->parentWidget()->parentWidget();
                mb->mapChanged(this, 0);
            }
            else
            {
                up.rename(this->text(0), oldname);
            }
        }
        else
        {
            MapBrowser* mb = (MapBrowser*)this->treeWidget()->parentWidget()->parentWidget();
            mb->mapChanged(this, 0);

            mb->loader()->saveDirectory((MapFolder*)this->parent());
        }
    }
    else
    {
        MapFolder* mf = (MapFolder*)this->parent();
        MapBrowser* mb = (MapBrowser*)this->treeWidget()->parentWidget()->parentWidget();

        if (!mf)
        {
            QDir root(mb->loader()->root());

            if (root.mkdir(this->text(0)))
            {
                QFile froot(root.filePath("map_root"));

                if (froot.open(QFile::WriteOnly))
                {
                    froot.write(this->text(0).toAscii());
                    froot.close();

                    m_directory = this->text(0);

                    mb->mapChanged(this, 0);
                    mb->loader()->saveDirectory(this);
                }
            }
        }
        else
        {
            QDir dir(mf->fullDirectory());

            if (dir.mkdir(this->text(0)))
            {
                m_directory = this->text(0);
                mb->mapChanged(this, 0);
                mb->loader()->saveDirectory(this);
                mb->loader()->saveDirectory(mf);
            }
        }
    }
}

QString MapFolder::directory()
{
    return m_directory;
}

void MapFolder::setDirectory(const QString& dir)
{
    m_directory = dir;
}

QString MapFolder::fullDirectory()
{
    MapBrowser* mb = (MapBrowser*)this->treeWidget()->parentWidget()->parentWidget();
    QString root = mb->loader()->root();

    QDir dir(root);
    if (dir.cd(this->path())) return dir.absolutePath();

    return QString();
}

void MapFolder::setText(const QString &text)
{
    QTreeWidgetItem::setText(0, text);
}

QString MapFolder::path()
{
    QString pt;
    QStringList tree;

    MapFolder* mf = this;

    while (mf != 0)
    {
        QString tmp = mf->directory();
        if (mf != this) tmp.append("/");
        tree << tmp;
        mf = (MapFolder*)mf->parent();
    }

    for (int x = tree.count() - 1; x >= 0; x--)
    {
        pt.append(tree.at(x));
    }

    return pt;
}

QTreeWidgetItem* MapFolder::find(const QString& name)
{
    for (int x = 0; x < this->childCount(); x++)
    {
        if (name.toLower() == this->child(x)->text(0).toLower())
        {
            return this->child(x);
        }
    }

    return 0;
}

bool MapFolder::exists(const QString& name)
{
    return (find(name) != 0);
}

QMenu* MapFolder::menu(QWidget* parent)
{
    MapTree* tree = (MapTree*)parent;

    QMenu* fmenu = new QMenu(parent);

    fmenu->addAction(QIcon(":/images/addfolder.ico"), QObject::tr("Adicionar pasta"))->setData((int)new MapAction(MapAction::AddNewFolder, this, tree));
    fmenu->addAction(QIcon(":/images/addmap.ico"), QObject::tr("Adicionar mapa"))->setData((int)new MapAction(MapAction::AddNewMap, this, tree));
    fmenu->addSeparator();
    fmenu->addAction(QIcon(":/images/save.ico"), QObject::tr("Exportar"))->setData((int)new MapAction(MapAction::Export, this, tree));
    fmenu->addSeparator();

    if (this->parent() != 0)
    {
        fmenu->addAction(QIcon(":/images/move.ico"), QObject::tr("Mover"))->setData((int)new MapAction(MapAction::Move, this, tree));
    }

    fmenu->addAction(QIcon(":/images/edit.ico"), QObject::tr("Renomear"))->setData((int)new MapAction(MapAction::Edit, this, tree));

    if (this->parent() != 0)
    {
        fmenu->addSeparator();
        fmenu->addAction(QIcon(":/images/delitem.ico"), QObject::tr("Apagar"))->setData((int)new MapAction(MapAction::Delete, this, tree));
    }

    fmenu->connect(fmenu, SIGNAL(triggered(QAction*)), tree, SLOT(processAction(QAction*)));

    return fmenu;
}
