#include "mapwebview.h"

#include <QContextMenuEvent>
#include <QWebFrame>
#include <QWebElement>
#include <QMenu>
#include <QAction>

#include <QDebug>

MapWebView::MapWebView(QWidget *parent) :
    QWebView(parent)
{
}

void MapWebView::contextMenuEvent(QContextMenuEvent *event)
{
    QWebHitTestResult r = this->page()->mainFrame()->hitTestContent(event->pos());

    if (!r.imageUrl().isEmpty())
    {
        QMenu menu(this);
        QAction* act = this->pageAction(QWebPage::CopyImageToClipboard);
        act->setText(tr("Copiar imagem"));
        menu.addAction(act);
        menu.exec(this->mapToGlobal(event->pos()));
    }
    else if (r.isContentSelected())
    {
        QMenu menu(this);
        QAction* act = this->pageAction(QWebPage::Copy);
        act->setText(tr("Copiar"));
        menu.addAction(act);
        menu.exec(this->mapToGlobal(event->pos()));
    }
}
