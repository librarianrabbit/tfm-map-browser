#ifndef MAPPREVIEW_H
#define MAPPREVIEW_H

#include <QObject>

class MapPreview : public QObject
{
    Q_OBJECT
public:
    explicit MapPreview(QObject *parent = 0);

public slots:
    void copyToClipboard(const QString& data);
    void mapClicked(int addr);
};

#endif // MAPPREVIEW_H
