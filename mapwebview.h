#ifndef MAPWEBVIEW_H
#define MAPWEBVIEW_H

#include <QWebView>

class MapWebView : public QWebView
{
    Q_OBJECT
public:
    explicit MapWebView(QWidget *parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *event);
};

#endif // MAPWEBVIEW_H
