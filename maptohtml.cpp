#include "maptohtml.h"

#include <QFile>

#include "mapbrowser.h"

#include <QDebug>

MapToHtml::MapToHtml(QObject *parent) :
    QObject(parent)
{
}

void MapToHtml::buildFolderView(MapFolder* mf)
{
    QFile fht(":/html/folderview.html");
    if (!fht.open(QFile::ReadOnly)) return;

    QString code = fht.readAll();
    fht.close();

    code = code.arg(mf->text(0));

    MapFolder* pmf = (MapFolder*)mf->parent();

    if (pmf)
    {
        code = code.arg("<a href='#' onclick='window.preview.mapClicked(" + QString::number((int)pmf) + ");'>/" + pmf->path() + "</a>");
    }
    else
    {
        code = code.arg("<a href='#' onclick='window.preview.mapClicked(" + QString::number((int)mf) + ");'>/</a>");
    }

    QString maps;

    foreach (MapItem* map, mf->maps())
    {
        maps.append("<li><a href='#' onclick='window.preview.mapClicked(" + QString::number((int)map) + ");'>" + map->m_code + (map->m_name.isEmpty() ? QString() : " - " + map->m_name) + "</a></li><br/>");
    }

    code = code.arg(maps);

    maps.clear();

    foreach (MapFolder* dir, mf->folders())
    {
        maps.append("<li><a href='#' onclick='window.preview.mapClicked(" + QString::number((int)dir) + ");'>" + dir->text(0) + "</a></li><br/>");
    }

    code = code.arg(maps);

    code = code.arg(tr("Mapas:"), tr("Pastas:"));

    m_html = code;
}

void MapToHtml::buildItemExport(MapItem* mi, const QString& where)
{
    QFile fht(":/html/itemexport.html");
    if (!fht.open(QFile::ReadOnly)) return;

    QString code = fht.readAll();
    fht.close();

    code = code.arg(mi->m_code, mi->m_name, mi->m_type);

    QString state;
    if (mi->m_state == MapItem::Permed) state = tr("Permanente");
    else if (mi->m_state == MapItem::Removed) state = tr("Apagado");
    else state = tr("Em votação");

    code = code.arg(state);
    code = code.arg(mi->m_votes).arg(mi->m_percent);
    code = code.arg(mi->m_description.replace("\n", "<br/>"));

    QDir fd(QFileInfo(where).absoluteDir());
    fd.mkdir("images");
    fd.cd("images");

    QFile::copy(":/images/left_" + tr("pt") + ".png", fd.filePath("left_" + tr("pt") + ".png"));
    QFile::copy(":/images/right_" + tr("pt") + ".png", fd.filePath("right_" + tr("pt") + ".png"));
    QFile::copy(":/images/map.png", fd.filePath("map.png"));

    if (mi->m_images.length() > 0)
    {
        fd.cdUp();
        fd.mkdir(mi->m_code);
        fd.cd(mi->m_code);
    }

    QString dimg;

    foreach (QString img, mi->m_images)
    {
        QFile fimg(img);
        QFileInfo info(fimg);
        QString timg = mi->m_code + "/" + info.fileName();

        fimg.copy(fd.filePath(info.fileName()));

        dimg.append("<img class='preview' name='img' style='display: none;' width='100%' src='").append(timg).append("'></img>\n");
    }

    code = code.arg(dimg, mi->m_extra.replace("\n", "<br/>"));

    code = code.arg(tr("Tipo:")).arg(tr("Votos:")).arg(tr("Descrição:"));
    code = code.arg(tr("Imagens:")).arg(tr("Informações adicionais:"));
    code = code.arg("images/left_" + tr("pt") + ".png");

    code = code.arg(mi->m_xml);

    code = code.arg("images/right_" + tr("pt") + ".png");
    code = code.arg("images/map.png");

    code = code.arg("", "none");

    m_html = code;
}

void MapToHtml::_exportMap(MapItem* mi, const QString& where, const QString& root)
{
    QFile fht(":/html/itemexport.html");
    if (!fht.open(QFile::ReadOnly)) return;

    QString code = fht.readAll();
    fht.close();

    code = code.arg(mi->m_code, mi->m_name, mi->m_type);

    QString state;
    if (mi->m_state == MapItem::Permed) state = tr("Permanente");
    else if (mi->m_state == MapItem::Removed) state = tr("Apagado");
    else state = tr("Em votação");

    code = code.arg(state);
    code = code.arg(mi->m_votes).arg(mi->m_percent);
    code = code.arg(mi->m_description.replace("\n", "<br/>"));

    QDir fd(where);

    if (mi->m_images.length() > 0)
    {
        fd.mkdir(mi->m_code);
        fd.cd(mi->m_code);
    }

    QString dimg;

    foreach (QString img, mi->m_images)
    {
        QFile fimg(img);
        QFileInfo info(fimg);
        QString timg = QDir(where).relativeFilePath(fd.filePath(info.fileName()));

        fimg.copy(fd.filePath(info.fileName()));

        dimg.append("<img class='preview' name='img' style='display: none;' width='100%' src='").append(timg).append("'></img>\n");
    }

    code = code.arg(dimg, mi->m_extra.replace("\n", "<br/>"));

    code = code.arg(tr("Tipo:")).arg(tr("Votos:")).arg(tr("Descrição:"));
    code = code.arg(tr("Imagens:")).arg(tr("Informações adicionais:"));

    QDir at(where);

    QDir rt(root);
    rt.cd("images");

    code = code.arg(at.relativeFilePath(rt.filePath("left_" + tr("pt") + ".png")));

    code = code.arg(mi->m_xml);

    code = code.arg(at.relativeFilePath(rt.filePath("right_" + tr("pt") + ".png")));
    code = code.arg(at.relativeFilePath(rt.filePath("map.png")));

    MapFolder* mf = (MapFolder*)mi->parent();
    code = code.arg("<a href='./index.html'>/" + mf->path() + "</a>", "block");

    QFile fl(QDir(where).filePath(mi->m_code + ".html"));
    if (fl.open(QFile::WriteOnly))
    {
        fl.write(code.toAscii());
        fl.close();
    }
}

void MapToHtml::_exportIndex(MapFolder* mf, const QString& where, const QString& root)
{
    QFile fht(":/html/folderexport.html");
    if (!fht.open(QFile::ReadOnly)) return;

    QString code = fht.readAll();
    fht.close();

    code = code.arg(mf->text(0));

    MapFolder* pmf = (MapFolder*)mf->parent();

    if (pmf)
    {
        code = code.arg("<a href='../index.html'>/" + pmf->path() + "</a>");
    }
    else
    {
        code = code.arg("<a href='#'>/</a>");
    }

    QString maps;

    foreach (MapItem* map, mf->maps())
    {
        maps.append("<li><a href='" + map->m_code + ".html'>" + map->m_code + (map->m_name.isEmpty() ? QString() : " - " + map->m_name) + "</a></li><br/>");
    }

    code = code.arg(maps);

    maps.clear();

    foreach (MapFolder* dir, mf->folders())
    {
        maps.append("<li><a href='" + dir->text(0) + "/index.html'>" + dir->text(0) + "</a></li><br/>");
    }

    code = code.arg(maps);

    code = code.arg(tr("Mapas:"), tr("Pastas:"));

    QDir at(where);

    QDir rt(root);
    rt.cd("images");
    code = code.arg(at.relativeFilePath(rt.filePath("folder.png")));

    QFile fl(QDir(where).filePath("index.html"));
    if (fl.open(QFile::WriteOnly))
    {
        fl.write(code.toAscii());
        fl.close();
    }
}

void MapToHtml::_exportFolder(MapFolder* mf, const QString& where, const QString& root)
{
    QDir dir(where);
    QString dname = dir.dirName();
    dir.cdUp();
    dir.mkdir(dname);
    dir.cd(dname);

    _exportIndex(mf, dir.absolutePath(), root);

    foreach (MapItem* mi, mf->maps())
    {
        _exportMap(mi, dir.absolutePath(), root);
    }

    foreach (MapFolder* tf, mf->folders())
    {
        _exportFolder(tf, dir.filePath(tf->text(0)), root);
    }
}

void MapToHtml::buildFolderExport(MapFolder *mf, const QString& where)
{
    QDir fd(where);
    fd.mkdir("images");
    fd.cd("images");

    QFile::copy(":/images/left_" + tr("pt") + ".png", fd.filePath("left_" + tr("pt") + ".png"));
    QFile::copy(":/images/right_" + tr("pt") + ".png", fd.filePath("right_" + tr("pt") + ".png"));
    QFile::copy(":/images/map.png", fd.filePath("map.png"));
    QFile::copy(":/images/folder.png", fd.filePath("folder.png"));

    QDir dir(where);
    dir.mkdir(mf->text(0));
    dir.cd(mf->text(0));

    _exportFolder(mf, dir.absolutePath(), where);
}

void MapToHtml::buildItemView(MapItem* mi)
{
    QFile fht(":/html/itemview.html");
    if (!fht.open(QFile::ReadOnly)) return;

    QString code = fht.readAll();
    fht.close();

    code = code.arg(mi->m_code, mi->m_name, mi->m_type);

    QString state;
    if (mi->m_state == MapItem::Permed) state = tr("Permanente");
    else if (mi->m_state == MapItem::Removed) state = tr("Apagado");
    else state = tr("Em votação");

    code = code.arg(state);
    code = code.arg(mi->m_votes).arg(mi->m_percent);
    code = code.arg(mi->m_description.replace("\n", "<br/>"));

    MapBrowser* mb = (MapBrowser*)this->parent();
    mb->preview()->setProperty("xml", mi->m_xml);

    QString dimg;
    foreach (QString img, mi->m_images)
    {
        dimg.append("<img class='preview' name='img' style='display: none;' width='100%' src='").append(img).append("'></img>\n");
    }

    code = code.arg(dimg, mi->m_extra.replace("\n", "<br/>"));

    code = code.arg(tr("Tipo:")).arg(tr("Votos:")).arg(tr("Descrição:"));
    code = code.arg(tr("Imagens:")).arg(tr("Informações adicionais:"));

    code = code.arg(tr("Copiar para a área de transferência")).arg(tr("pt"));

    MapFolder* mf = (MapFolder*)mi->parent();

    code = code.arg("<a href='#' onclick='window.preview.mapClicked(" + QString::number((int)mf) + ");'>/" + mf->path() + "</a>");

    m_html = code;
}

QString MapToHtml::html()
{
    return m_html;
}
