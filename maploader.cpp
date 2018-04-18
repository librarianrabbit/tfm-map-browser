#include "maploader.h"

#include <QtXml/qdom.h>
#include <QTextStream>
#include <QApplication>

#include <QDebug>

MapLoader::MapLoader(QObject* parent) :
    QObject(parent)
{
}

void MapLoader::saveDirectory(MapFolder* mf)
{
    QDomDocument doc;
    QDomElement e = doc.createElement("mapdir");
    doc.appendChild(e);

    QList<MapItem*> maps = mf->maps();

    QDomElement xmap = doc.createElement("maplist");
    xmap.setAttribute("size", maps.count());

    foreach (MapItem* map, maps)
    {
        QDomElement cmap = doc.createElement("map");
        cmap.appendChild(doc.createTextNode(map->m_code + ".map"));

        saveMap(map, mf);

        xmap.appendChild(cmap);
    }

    e.appendChild(xmap);

    QList<MapFolder*> dirs = mf->folders();

    QDomElement xdir = doc.createElement("dirlist");
    xdir.setAttribute("size", dirs.count());

    foreach (MapFolder* dir, dirs)
    {
        QDomElement cdir = doc.createElement("mapdir");
        cdir.appendChild(doc.createTextNode(dir->text(0)));
        xdir.appendChild(cdir);
    }

    e.appendChild(xdir);

    QDir dir = mf->fullDirectory();
    QFile fxml = dir.filePath("mapdir.xml");

    if (fxml.open(QFile::WriteOnly))
    {
        QTextStream ts(&fxml);
        doc.save(ts, 4);
        fxml.close();
    }
}

void MapLoader::saveMap(MapItem* map, MapFolder* mf, bool updateDir, const QString& lastCode)
{
    if (updateDir)
    {
        QDir dir(mf->fullDirectory());

        if (!lastCode.isEmpty())
        {
            QFile way = dir.filePath(lastCode + ".map");
            way.remove();

            way.setFileName(dir.filePath(lastCode + ".xml"));
            way.remove();
        }

        saveDirectory(mf);

        return;
    }

    QDomDocument doc;
    QDomElement e = doc.createElement("map");
    doc.appendChild(e);

    e.appendChild(doc.createElement("name")).appendChild(doc.createTextNode(map->m_name));
    e.appendChild(doc.createElement("type")).appendChild(doc.createTextNode(map->m_type));
    e.appendChild(doc.createElement("state")).appendChild(doc.createTextNode(QString::number(map->m_state)));
    e.appendChild(doc.createElement("votes")).appendChild(doc.createTextNode(QString::number(map->m_votes)));
    e.appendChild(doc.createElement("percent")).appendChild(doc.createTextNode(QString::number(map->m_percent)));
    e.appendChild(doc.createElement("description")).appendChild(doc.createTextNode(map->m_description));
    e.appendChild(doc.createElement("extra")).appendChild(doc.createTextNode(map->m_extra));

    QDomElement images = e.appendChild(doc.createElement("mapimages")).toElement();
    images.setAttribute("size", map->m_images.count());

    foreach (QString image, map->m_images)
    {
        images.appendChild(doc.createElement("mapimage")).appendChild(doc.createTextNode(image));
    }

    QDir dir(mf->fullDirectory());
    QFile fcode(dir.filePath(map->m_code + ".map"));

    if (fcode.open(QFile::WriteOnly))
    {
        QTextStream ts(&fcode);
        doc.save(ts, 4);
        fcode.close();
    }

    QFile fxml(dir.filePath(map->m_code + ".xml"));

    if (fxml.open(QFile::WriteOnly))
    {
        fxml.write(map->m_xml.toAscii());
        fxml.close();
    }
}

void MapLoader::_defaultRoot(MapTree* mt)
{
    QDir dir(qApp->applicationDirPath());
    if (!dir.mkdir("Maps")) return;

    MapFolder* mf = new MapFolder(mt);
    mf->setText("Maps");
    mf->renamed();

    mt->setCurrentItem(mf);
}

void MapLoader::load(MapTree* mt)
{
    mt->clear();

    QFile root(m_root + "map_root");

    if (!root.exists())
    {
        _defaultRoot(mt);
        return;
    }

    if (!root.open(QFile::ReadOnly))
    {
        _defaultRoot(mt);
        return;
    }

    QString rootdir = m_root + root.readAll();
    root.close();

    _loadDirectory(rootdir, (MapFolder*)mt->invisibleRootItem());
}

void MapLoader::_loadDirectory(const QString& directory, MapFolder* mf)
{
    QDir dir(directory);
    QString xml = dir.absolutePath().append("/mapdir.xml");

    QFile fname(xml);
    if (!fname.exists()) return;

    QDomDocument doc;
    if (!doc.setContent(&fname)) return;

    QDomElement e = doc.documentElement();
    if (e.nodeName() != "mapdir") return;

    mf = _addDirectory(dir.dirName(), mf);
    mf->setDirectory(dir.dirName());

    QDomNode node = e.firstChild();

    if (node.nodeName() == "maplist")
    {
        int size = node.toElement().attribute("size", "0").toInt();

        if (size)
        {
            QDomNode map = node.firstChild();

            for (int x = 0; x < size; x++)
            {
                if (map.nodeName() != "map") continue;

                _loadMap(dir.filePath(map.toElement().text()), mf);
                map = map.nextSibling();
            }
        }

        node = node.nextSibling();
    }

    if (node.nodeName() == "dirlist")
    {
        int size = node.toElement().attribute("size", "0").toInt();

        if (size)
        {
            QDomNode mapdir = node.firstChild();

            for (int x = 0; x < size; x++)
            {
                if (mapdir.nodeName() != "mapdir") continue;

                QDir tmp = dir;

                if (tmp.cd(mapdir.toElement().text()))
                {
                    _loadDirectory(tmp.absolutePath(), mf);
                    mapdir = mapdir.nextSibling();
                }
            }
        }
    }
}

void MapLoader::_loadMap(const QString& file, MapFolder* mf)
{
    QFile fmap(file);
    if (!fmap.exists()) return;
    if (!fmap.open(QFile::ReadOnly)) return;

    QFileInfo finf(fmap);

    QString mapcode = finf.baseName();

    QDomDocument doc;
    doc.setContent(fmap.readAll());
    fmap.close();

    QDomElement e = doc.documentElement();
    if (e.nodeName() != "map") return;

    QDomNode node = e.firstChild();
    QString mapname = node.toElement().text();

    node = node.nextSibling();
    QString maptype = node.toElement().text();

    node = node.nextSibling();
    int mapstate = node.toElement().text().toInt();

    node = node.nextSibling();
    int mapvotes = node.toElement().text().toInt();

    node = node.nextSibling();
    int mappercent = node.toElement().text().toInt();

    node = node.nextSibling();
    QString mapdescription = node.toElement().text();

    node = node.nextSibling();
    QString mapextra = node.toElement().text();

    node = node.nextSibling();
    QStringList mapimages;

    if (node.nodeName() == "mapimages")
    {
        int size = node.toElement().attribute("size", 0).toInt();

        if (size)
        {
            QDomNode img = node.firstChild();

            for (int x = 0; x < size; x++)
            {
                if (img.nodeName() != "mapimage") continue;

                mapimages << img.toElement().text();
                img = img.nextSibling();
            }
        }
    }

    QString mapxml = mapcode + ".xml";

    QFile fxml(finf.absoluteDir().filePath(mapxml));
    mapxml.clear();

    if (fxml.exists())
    {
        if (fxml.open(QFile::ReadOnly))
        {
            mapxml = fxml.readAll();
            fxml.close();
        }
    }

    MapItem* mi = new MapItem(mf);
    mi->setText(mapcode);
    mi->setMapData(mapcode, mapname, maptype, mapstate, mapvotes, mappercent, mapdescription, mapxml, mapextra, mapimages);
}

MapFolder* MapLoader::_addDirectory(const QString& name, MapFolder* where)
{
    MapFolder* mf = new MapFolder(where);
    mf->setText(name);

    return mf;
}

QString MapLoader::root()
{
    return m_root;
}

void MapLoader::setRoot(const QString& aroot)
{
    QDir dir(aroot);
    m_root = dir.toNativeSeparators(dir.absolutePath().append("/"));
}
