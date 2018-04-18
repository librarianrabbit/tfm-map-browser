#include "mapitemdialog.h"
#include "ui_mapitemdialog.h"

#include "mapitem.h"

#include <QListWidgetItem>
#include <QFileDialog>
#include <QFile>

#include <QDebug>

extern QString lastDir;

MapItemDialog::MapItemDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Ui::MapItemDialog)
{
    ui->setupUi(this);

    connect(ui->mapCode, SIGNAL(textEdited(QString)), SLOT(onCodeChanged(QString)));

    connect(ui->mapLoadXml, SIGNAL(clicked()), SLOT(loadXml()));

    connect(ui->mapAddImage, SIGNAL(clicked()), SLOT(addImages()));
    connect(ui->mapRemoveImage, SIGNAL(clicked()), SLOT(removeImages()));
    connect(ui->mapClearImages, SIGNAL(clicked()), ui->mapImages, SLOT(clear()));
}

void MapItemDialog::loadXml()
{
    QString file = QFileDialog::getOpenFileName(this, QString(), lastDir);
    if (file.isEmpty()) return;

    QFile fobj(file);
    lastDir = QFileInfo(fobj).absoluteDir().absolutePath();

    if (fobj.open(QFile::ReadOnly))
    {
        QString data = fobj.readAll();
        ui->mapXml->setPlainText(data);
    }
}

void MapItemDialog::addImages()
{
    QStringList imgs = QFileDialog::getOpenFileNames(this, QString(), lastDir);

    bool ft = false;

    foreach (QString img, imgs)
    {
        if (img.isEmpty()) continue;

        if (!ft)
        {
            lastDir = QFileInfo(img).absoluteDir().absolutePath();
            ft = true;
        }

        bool exists = false;

        for (int row = 0; row < ui->mapImages->count(); row++)
        {
            QListWidgetItem* it = ui->mapImages->item(row);

            if (it->text().toLower() == img.toLower())
            {
                exists = true;
            }
        }

        if (exists) continue;

        ui->mapImages->addItem(img);
    }
}

void MapItemDialog::removeImages()
{
    qDeleteAll(ui->mapImages->selectedItems());
}

void MapItemDialog::loadMapData(QTreeWidgetItem* mapitem)
{
    MapItem* mi = (MapItem*)mapitem;

    setCode(mi->m_code);

    setName(mi->m_name);
    setType(mi->m_type);

    setState(mi->m_state);

    setVotes(mi->m_votes);
    setPercent(mi->m_percent);

    setDescription(mi->m_description);
    setXml(mi->m_xml);
    setExtra(mi->m_extra);

    setImages(mi->m_images);
}

void MapItemDialog::updateMapData(QTreeWidgetItem* mapitem)
{
    MapItem* mi = (MapItem*)mapitem;
    mi->setMapData(code(), name(), type(), state(), votes(), percent(), description(), xml(), extra(), images());
}

void MapItemDialog::setParentItem(QTreeWidgetItem* item)
{
    m_item = item;
}

void MapItemDialog::onCodeChanged(const QString& text)
{
    if (itemExists(text))
    {
        ui->warning->setText("<font color=red>" + tr("Já existe um mapa com este código.") + "</font>");
        ui->mapSave->setEnabled(false);
    }
    else if (text == "@")
    {
        ui->warning->setText("<font color=red>" + tr("Digite o código do mapa.") + "</font>");
        ui->mapSave->setEnabled(false);
    }
    else if (text.length() != 7)
    {
        ui->warning->setText("<font color=red>" + tr("Código do mapa inválido.") + "</font>");
        ui->mapSave->setEnabled(false);
    }
    else
    {
        ui->warning->setText(QString());
        ui->mapSave->setEnabled(true);
    }
}

bool MapItemDialog::itemExists(const QString& name)
{
    for (int x = 0; x < m_item->childCount(); x++)
    {
        QTreeWidgetItem* it = m_item->child(x);
        QString i = it->text(0);

        if (name.toLower() == m_map.toLower())
        {
            return false;
        }
        else if (name.toLower() == i.toLower())
        {
            return true;
        }
    }

    return false;
}

QString MapItemDialog::code()
{
    return ui->mapCode->text();
}

void MapItemDialog::setCode(const QString& acode)
{
    ui->mapCode->setText(acode);
    m_map = acode;
}

QString MapItemDialog::type()
{
    return ui->mapType->text();
}

void MapItemDialog::setType(const QString& atype)
{
    ui->mapType->setText(atype);
}

QString MapItemDialog::name()
{
    return ui->mapName->text();
}

void MapItemDialog::setName(const QString& aname)
{
    ui->mapName->setText(aname);
}

int MapItemDialog::state()
{
    if (ui->mapStatePerm->isChecked()) return MapItem::Permed;
    else if (ui->mapStateErased->isChecked()) return MapItem::Removed;

    return MapItem::Temporary;
}

void MapItemDialog::setState(int astate)
{
    if (astate == MapItem::Permed) ui->mapStatePerm->setChecked(true);
    else if (astate == MapItem::Removed) ui->mapStateErased->setChecked(true);
    else ui->mapStateVoting->setChecked(true);
}

int MapItemDialog::votes()
{
    return ui->mapVotes->value();
}

void MapItemDialog::setVotes(int avotes)
{
    ui->mapVotes->setValue(avotes);
}

int MapItemDialog::percent()
{
    return ui->mapPercent->value();
}

void MapItemDialog::setPercent(int apercent)
{
    ui->mapPercent->setValue(apercent);
}

QString MapItemDialog::xml()
{
    return ui->mapXml->toPlainText();
}

void MapItemDialog::setXml(const QString& axml)
{
    ui->mapXml->setPlainText(axml);
}

QString MapItemDialog::description()
{
    return ui->mapDescription->toPlainText();
}

void MapItemDialog::setDescription(const QString& adescription)
{
    ui->mapDescription->setPlainText(adescription);
}

QString MapItemDialog::extra()
{
    return ui->mapExtras->toPlainText();
}

void MapItemDialog::setExtra(const QString& aextra)
{
    ui->mapExtras->setPlainText(aextra);
}

QStringList MapItemDialog::images()
{
    QStringList list;

    for (int row = 0; row < ui->mapImages->count(); row++)
    {
        QListWidgetItem* item = ui->mapImages->item(row);
        list << item->text();
    }

    return list;
}

void MapItemDialog::setImages(const QStringList& aimages)
{
    foreach (QString img, aimages)
    {
        ui->mapImages->addItem(img);
    }
}

MapItemDialog::~MapItemDialog()
{
    delete ui;
}
