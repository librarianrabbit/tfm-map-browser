#ifndef MAPITEMDIALOG_H
#define MAPITEMDIALOG_H

#include <QDialog>

#include <QTreeWidgetItem>

namespace Ui {
    class MapItemDialog;
}

class MapItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapItemDialog(QWidget *parent = 0);
    ~MapItemDialog();

    void setParentItem(QTreeWidgetItem* item);

    bool itemExists(const QString& name);

    void updateMapData(QTreeWidgetItem* mapitem);
    void loadMapData(QTreeWidgetItem* mapitem);

private slots:
    void onCodeChanged(const QString& text);

    void loadXml();

    void addImages();
    void removeImages();

public:
    QString code();
    void setCode(const QString& acode);

    QString type();
    void setType(const QString& atype);

    QString name();
    void setName(const QString& aname);

    int state();
    void setState(int astate);

    int votes();
    void setVotes(int avotes);

    int percent();
    void setPercent(int apercent);

    QString xml();
    void setXml(const QString& axml);

    QString description();
    void setDescription(const QString& adescription);

    QString extra();
    void setExtra(const QString& aextra);

    QStringList images();
    void setImages(const QStringList& aimages);

private:
    Ui::MapItemDialog *ui;

    QString m_map;
    QTreeWidgetItem* m_item;
};

#endif // MAPITEMDIALOG_H
