#ifndef MAPTOHTML_H
#define MAPTOHTML_H

#include <QObject>

#include "mapitem.h"
#include "mapfolder.h"

class MapToHtml : public QObject
{
    Q_OBJECT
public:
    explicit MapToHtml(QObject *parent = 0);

    void buildItemView(MapItem* mi);
    void buildFolderView(MapFolder* mf);

    void buildItemExport(MapItem* mi, const QString& where);
    void buildFolderExport(MapFolder *mf, const QString& where);

    QString html();

private:
    void _exportMap(MapItem* mi, const QString& where, const QString& root);
    void _exportFolder(MapFolder* mf, const QString& where, const QString& root);
    void _exportIndex(MapFolder* mf, const QString& where, const QString& root);

private:
    QString m_html;

    bool m_inside;
};

#endif // MAPTOHTML_H
