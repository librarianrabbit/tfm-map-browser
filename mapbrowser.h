#ifndef MAPBROWSER_H
#define MAPBROWSER_H

#include <QMainWindow>

#include <QTreeWidgetItem>
#include <QSettings>
#include <QTranslator>

#include "maploader.h"
#include "mappreview.h"

namespace Ui {
    class MapBrowser;
}

class MapBrowser : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapBrowser(QWidget *parent = 0);
    ~MapBrowser();

    MapLoader* loader();

public slots:
    void mapChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void processToolbarAction(QAction* act);

    void getNewRoot();

    void langPt();
    void langEn();

    void retranslate();

    void toggleTheme();

    void showAbout();

    void updatePreviewObject();

    MapPreview* preview();

protected:
    bool eventFilter(QObject* sender, QEvent* event);

private:
    void redoLayout(const QSize& twsize, const QSize& wvsize);

private:
    Ui::MapBrowser *ui;

    QSettings* settings;

    QTranslator* m_lang;
    QTranslator* m_en;
    QTranslator* m_qtpt;

    MapLoader* m_loader;

    MapPreview* m_preview;
};

#endif // MAPBROWSER_H
