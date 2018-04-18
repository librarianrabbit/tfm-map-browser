#include "maptree.h"

#include <QTreeWidgetItem>

#include "mapfolder.h"
#include "mapitem.h"

#include "mapfolderdialog.h"
#include "mapitemdialog.h"

#include "maptohtml.h"
#include "mapbrowser.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QComboBox>
#include <QMessageBox>

#include <QDebug>

extern QString lastDir;

MapTree::MapTree(QWidget *parent) :
    QTreeWidget(parent)
{
}

void MapTree::processMapAction(MapAction* action)
{
    QTreeWidgetItem* where = action->item();

    if (action->action() == MapAction::Delete)
    {
        deleteItem(where);
    }
    else if (action->action() == MapAction::AddNewFolder)
    {
        addFolder(where);
    }
    else if (action->action() == MapAction::AddNewMap)
    {
        addItem(where);
    }
    else if (action->action() == MapAction::Edit)
    {
        if (where->type() == MapFolder::FolderType)
        {
            renameFolder(where);
        }
        else if (where->type() == MapItem::MapType)
        {
            editItem(where);
        }
    }
    else if (action->action() == MapAction::Export)
    {
        if (where->type() == MapFolder::FolderType)
        {
            exportFolder(where);
        }
        else if (where->type() == MapItem::MapType)
        {
            exportItem(where);
        }
    }
    else if (action->action() == MapAction::Move)
    {
        moveItem(where);
    }

    delete action;
}

void MapTree::processAction(QAction* item)
{
    int addr = item->data().toInt();
    if (!addr) return;

    MapAction* action = (MapAction*)addr;
    processMapAction(action);
}

void MapTree::_getFolders(MapFolder* current, QList<MapFolder*>& list)
{
    QList<MapFolder*> f = current->folders();

    foreach (MapFolder* mf, f)
    {
        list.append(mf);
        _getFolders(mf, list);
    }
}

QList<MapFolder*> MapTree::folders()
{
    QList<MapFolder*> f;

    if (this->invisibleRootItem()->childCount() == 1)
    {
        MapFolder* mf = (MapFolder*)this->invisibleRootItem()->child(0);
        f.append(mf);
        _getFolders(mf, f);
    }

    return f;
}

void MapTree::moveItem(QTreeWidgetItem* item)
{
    QList<MapFolder*> mfs = this->folders();
    QStringList list;
    QList<MapFolder*> rlist;

    foreach (MapFolder* mf, mfs)
    {
        if (item->type() == MapFolder::FolderType)
        {
            if (mf == item)
            {
                continue;
            }
            else
            {
                MapFolder* p = (MapFolder*)mf->parent();

                bool pf = false;

                while (p)
                {
                    if (p == item)
                    {
                        pf = true;
                        break;
                    }

                    p = (MapFolder*)p->parent();
                }

                if (pf) continue;
            }
        }

        if (mf == item->parent()) continue;

        list << mf->path();
        rlist << mf;
    }

    QInputDialog id(this, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint);
    id.setComboBoxItems(list);
    id.setLabelText(tr("Escolha uma pasta"));

    if (id.exec() != QDialog::Accepted) return;

    QObjectList idlist = id.children();
    int sel = -1;

    foreach (QObject* obj, idlist)
    {
        if (obj->isWidgetType())
        {
            QComboBox* cb = qobject_cast<QComboBox*>(obj);

            if (cb)
            {
                sel = cb->currentIndex();
            }
        }
    }

    if (sel == -1) return;

    MapFolder* target = rlist.at(sel);

    if (item->type() == MapFolder::FolderType)
    {
        QTreeWidgetItem* ex = target->find(item->text(0));

        if (ex)
        {
            if (ex->type() == MapFolder::FolderType)
            {
                deleteItem(ex);
            }
        }

        MapFolder* old = (MapFolder*)item;

        QDir source(old->fullDirectory());
        QDir where(target->fullDirectory());

        MapFolder::copyDirectory(source.absolutePath(), where.absolutePath());
        MapFolder::removeDirectory(source.absolutePath());

        old->setDirectory(source.dirName());

        MapFolder* mfp = (MapFolder*)old->parent();

        target->addChild(old->parent()->takeChild(old->parent()->indexOfChild(old)));
        old->renamed();

        MapBrowser* mb = (MapBrowser*)this->parentWidget()->parentWidget();
        mb->loader()->saveDirectory(mfp);
    }
    else if (item->type() == MapItem::MapType)
    {
        QTreeWidgetItem* ex = target->find(item->text(0));

        if (ex)
        {
            if (ex->type() == MapItem::MapType)
            {
                deleteItem(ex);
            }
        }

        MapItem* mi = new MapItem(target);
        *mi = *((MapItem*)item);
        mi->setText(item->text(0));
        deleteItem(item);
        mi->edited(true);
        this->expandItem(target);
        this->setCurrentItem(mi);
    }
}

void MapTree::exportItem(QTreeWidgetItem* item)
{
    QFileDialog qfd(this, QString(), lastDir, tr("Arquivo HTML (*.html);;Todos os arquivos"));
    qfd.setAcceptMode(QFileDialog::AcceptSave);
    qfd.selectFile(item->text(0) + ".html");
    qfd.setOption(QFileDialog::DontUseNativeDialog, true);

    if (qfd.exec() != QFileDialog::AcceptSave) return;

    QString fn = qfd.selectedFiles()[0];

    if (fn.isEmpty()) return;

    QFile fl(fn);
    lastDir = QFileInfo(fl).absoluteDir().absolutePath();

    if (fl.open(QFile::WriteOnly))
    {
        MapItem* mi = (MapItem*)this->m_tempItem;
        this->m_tempItem = 0;

        MapToHtml mth(this);
        mth.buildItemExport(mi, fl.fileName());

        fl.write(mth.html().toAscii());

        fl.close();
    }
}

void MapTree::exportFolder(QTreeWidgetItem *item)
{
    QString fn = QFileDialog::getExistingDirectory(this, QString(), lastDir);
    if (fn.isEmpty()) return;

    lastDir = QDir(fn).absolutePath();

    MapFolder* mf = (MapFolder*)item;

    MapToHtml mth(this);
    mth.buildFolderExport(mf, fn);
}

void MapTree::addFolder(QTreeWidgetItem* item)
{
    MapFolderDialog mfd(this);
    mfd.setParentItem(item);
    if (mfd.exec() == QDialog::Rejected) return;

    QString folder_name = mfd.folderName();

    if (mfd.itemExists(folder_name) || folder_name.isEmpty()) return;

    MapFolder* mf = new MapFolder(item);
    mf->setText(folder_name);

    mf->renamed();

    this->expandItem(item);
    this->setCurrentItem(mf);
}

void MapTree::renameFolder(QTreeWidgetItem* item)
{
    MapFolderDialog mfd(this);

    QTreeWidgetItem* parent = item->parent();
    if (!parent) parent = this->invisibleRootItem();

    mfd.setParentItem(parent);
    mfd.setFolderName(item->text(0));

    if (mfd.exec() == QDialog::Rejected) return;

    QString folder_name = mfd.folderName();

    if (mfd.itemExists(folder_name) || folder_name.isEmpty()) return;

    MapFolder* mf = (MapFolder*)item;
    mf->setText(folder_name);

    mf->renamed();
}

void MapTree::addItem(QTreeWidgetItem* item)
{
    MapItemDialog mid(this);
    mid.setParentItem(item);
    if (mid.exec() == QDialog::Rejected) return;

    QString item_name = mid.code();

    if (mid.itemExists(item_name) || item_name.length() != 7) return;

    MapItem* mi = new MapItem(item);
    mi->setText(item_name);

    mid.updateMapData(mi);

    mi->edited(true);

    this->expandItem(item);
    this->setCurrentItem(mi);
}

void MapTree::editItem(QTreeWidgetItem* item)
{
    MapItemDialog mid(this);

    QTreeWidgetItem* parent = item->parent();
    if (!parent) parent = this->invisibleRootItem();

    mid.setParentItem(parent);
    mid.setCode(item->text(0));

    mid.loadMapData(item);

    if (mid.exec() == QDialog::Rejected) return;

    QString item_name = mid.code();

    if (mid.itemExists(item_name) || item_name.length() != 7) return;

    QString lastCode = item->text(0);
    bool updateCode = (lastCode != item_name);

    MapItem* mi = (MapItem*)item;
    mi->setText(item_name);

    mid.updateMapData(mi);

    mi->edited(updateCode, lastCode);
}

void MapTree::deleteItem(QTreeWidgetItem* item)
{
    MapFolder* parent = (MapFolder*)item->parent();
    if (!parent) return;

    QString title = (item->type() == MapItem::MapType ? tr("Apagar mapa") :
                                                        tr("Apagar pasta"));
    QString message = (item->type() == MapItem::MapType ? tr("Deseja mesmo apagar este mapa ?") :
                                                          tr("Deseja mesmo apagar esta pasta ?"));

    QMessageBox box(QMessageBox::Question, title, message, QMessageBox::Yes | QMessageBox::No, this);
    if (box.exec() != QMessageBox::Yes) return;

    parent->deleteChild(item);
}

void MapTree::contextMenuEvent(QContextMenuEvent* event)
{
    QTreeWidgetItem* item = this->itemAt(event->pos());

    if (!item) return;

    QMenu* menu = 0;

    if (item->type() == MapFolder::FolderType)
    {
        MapFolder* mf = (MapFolder*)item;
        menu = mf->menu(this);
    }
    else if (item->type() == MapItem::MapType)
    {
        MapItem* mi = (MapItem*)item;
        menu = mi->menu(this);
    }

    if (!menu) return;

    menu->exec(event->globalPos());
}

QMenu* MapTree::defaultMenu()
{
    QMenu* menu = new QMenu(this);

    menu->addAction("Criar pasta");

    return menu;
}

MapTree::~MapTree()
{
}
