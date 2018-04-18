#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>
#include <QFile>
#include <QDir>

#include "maptree.h"
#include "mapitem.h"
#include "mapfolder.h"

class MapLoader : public QObject
{
    Q_OBJECT
public:
    explicit MapLoader(QObject* parent = 0);

    QString root();
    void setRoot(const QString& aroot);

    void load(MapTree* mt);

    void saveDirectory(MapFolder* mf);
    void saveMap(MapItem* map, MapFolder* mf, bool updateDir = false, const QString& lastCode = QString());

private:
    MapFolder* _addDirectory(const QString& name, MapFolder* where);
    void _loadDirectory(const QString& directory, MapFolder* mf);

    void _loadMap(const QString& file, MapFolder* mf);

    void _defaultRoot(MapTree* mt);

private:
    QString m_root;

    friend class MapTree;
};

#endif // MAPLOADER_H
