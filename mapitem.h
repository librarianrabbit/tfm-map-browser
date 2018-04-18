#ifndef MAPITEM_H
#define MAPITEM_H

#include <QMenu>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class MapLoader;
class MapToHtml;
class MapItemDialog;

class MapItem : public QTreeWidgetItem
{
public:
    enum
    {
        Temporary = 1,
        Permed = 2,
        Removed = 3
    };

public:
    MapItem(QTreeWidget* parent = 0);
    MapItem(QTreeWidgetItem* parent);

    void setText(const QString &text);

    QMenu* menu(QWidget* parent = 0);

    void deleteMe();

public:
    void setMapData(const QString& acode, const QString& aname,
                    const QString& atype, int astate, int avotes,
                    int apercent, const QString& adescription,
                    const QString& axml, const QString& aextra,
                    const QStringList& aimages);

public slots:
    void edited(bool updateCode = true, const QString& lastCode = QString());

private:
    void clearMapData();

    QString m_code;

    QString m_name;
    QString m_type;

    int m_state;

    int m_votes;
    int m_percent;

    QString m_description;
    QString m_xml;
    QString m_extra;

    QStringList m_images;

public:
    static const int MapType = QTreeWidgetItem::UserType + 1;

    friend class MapLoader;
    friend class MapToHtml;
    friend class MapItemDialog;
};

#endif // MAPITEM_H
