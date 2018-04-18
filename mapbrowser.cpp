#include "mapbrowser.h"
#include "ui_mapbrowser.h"

#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QWebFrame>

#include "maploader.h"

#include "mapfolder.h"
#include "mapitem.h"

#include "maptohtml.h"

#include <QDebug>

QString lastDir;
QString defaultTheme;

MapBrowser::MapBrowser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapBrowser)
{
    defaultTheme = qApp->style()->objectName();

    m_lang = 0;
    m_en = new QTranslator(this);
    m_qtpt = new QTranslator(this);

    if (!m_en->load(":/translations/mapbrowser_en.qm"))
    {
        delete m_en;
        m_en = 0;
    }

    if (!m_qtpt->load(":/translations/qt_pt"))
    {
        delete m_qtpt;
        m_qtpt = 0;
    }

    QString cf = qApp->applicationFilePath();
    cf.replace(QRegExp(".exe$", Qt::CaseInsensitive), ".ini");

    bool cfe = QFile(cf).exists();

    settings = new QSettings(cf, QSettings::IniFormat, this);

    if (settings->value("translation", "pt").toString() == "en")
    {
        if (m_en)
        {
            qApp->installTranslator(m_en);
            m_lang = m_en;
        }
    }
    else if (!cfe)
    {
        if (m_en)
        {
            QSettings tmp("Standalone.ini", QSettings::IniFormat, this);

            if (tmp.value("translation", "pt").toString() == "en")
            {
                qApp->installTranslator(m_en);
                m_lang = m_en;
            }
        }
    }

    lastDir = settings->value("last_directory").toString();

    ui->setupUi(this);

    m_preview = new MapPreview(this);

    if (settings->value("theme", defaultTheme).toString() == "cleanlooks")
    {
        qApp->setStyle("cleanlooks");
        ui->actionUsar_o_tema_CleanLooks->setChecked(true);
    }

    if (m_lang == m_en && m_en)
    {
        ui->actionEnglish->setChecked(true);
        ui->actionPortugu_s->setChecked(false);
    }
    else
    {
        ui->actionEnglish->setChecked(false);
        ui->actionPortugu_s->setChecked(true);

        if (m_qtpt)
        {
            qApp->installTranslator(m_qtpt);
        }
    }

    ui->line->setCursor(Qt::SplitHCursor);
    ui->line->installEventFilter(this);

    ui->mainToolBar->addAction(QIcon(":/images/addfolder.ico"), tr("Adicionar pasta"))->setData(MapAction::AddNewFolder);
    ui->mainToolBar->addAction(QIcon(":/images/addmap.ico"), tr("Adicionar mapa"))->setData(MapAction::AddNewMap);
    ui->mainToolBar->addAction(QIcon(":/images/edit.ico"), tr("Editar"))->setData(MapAction::Edit);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon(":/images/move.ico"), tr("Mover"))->setData(MapAction::Move);
    ui->mainToolBar->addAction(QIcon(":/images/delitem.ico"), tr("Apagar"))->setData(MapAction::Delete);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon(":/images/save.ico"), tr("Exportar"))->setData(MapAction::Export);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon(":/images/exit.ico"), tr("Sair"), this, SLOT(close()));

    ui->menuArquivo->addActions(ui->mainToolBar->actions());

    QDir dd(qApp->applicationDirPath());
    QString ddir = dd.filePath("Maps");

    ddir = settings->value("root", ddir).toString();

    m_loader = new MapLoader(this);
    m_loader->setRoot(ddir);
    m_loader->load(ui->treeWidget);

    ui->treeWidget->expandAll();

    ui->webView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

    connect(ui->mainToolBar, SIGNAL(actionTriggered(QAction*)), SLOT(processToolbarAction(QAction*)));
    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(mapChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(updatePreviewObject()));

    connect(ui->actionAlterar_a_pasta_raiz, SIGNAL(triggered()), SLOT(getNewRoot()));
    connect(ui->actionPortugu_s, SIGNAL(triggered()), SLOT(langPt()));
    connect(ui->actionEnglish, SIGNAL(triggered()), SLOT(langEn()));
    connect(ui->actionUsar_o_tema_CleanLooks, SIGNAL(triggered()), SLOT(toggleTheme()));
    connect(ui->actionSobre, SIGNAL(triggered()), SLOT(showAbout()));
    connect(ui->actionSobre_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MapBrowser::updatePreviewObject()
{
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject(this->m_preview->objectName(), this->m_preview, QScriptEngine::QtOwnership);
}

void MapBrowser::getNewRoot()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString(), lastDir);
    if (dir.isEmpty()) return;

    lastDir = dir;

    ui->webView->stop();
    ui->webView->setHtml(QString());

    m_loader->setRoot(dir);
    m_loader->load(ui->treeWidget);
}

void MapBrowser::showAbout()
{
    QMessageBox::about(this, "MapExplorer v1.0", tr("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">MapExplorer v1.0</span></p><br/><span style=\" font-size:11pt; font-weight:600; vertical-align:super;\">by Rafaelfsilva</span>"));
}

void MapBrowser::toggleTheme()
{
    if (settings->value("theme", defaultTheme).toString() == "cleanlooks")
    {
        qApp->setStyle(defaultTheme);
        ui->actionUsar_o_tema_CleanLooks->setChecked(false);
    }
    else
    {
        qApp->setStyle("cleanlooks");
        ui->actionUsar_o_tema_CleanLooks->setChecked(true);
    }

    settings->setValue("theme", qApp->style()->objectName());
}

void MapBrowser::retranslate()
{
    ui->mainToolBar->actions()[0]->setText(tr("Adicionar pasta"));
    ui->mainToolBar->actions()[1]->setText(tr("Adicionar mapa"));
    ui->mainToolBar->actions()[2]->setText(tr("Editar"));
    ui->mainToolBar->actions()[4]->setText(tr("Mover"));
    ui->mainToolBar->actions()[5]->setText(tr("Apagar"));
    ui->mainToolBar->actions()[7]->setText(tr("Exportar"));

    ui->retranslateUi(this);

    this->mapChanged(ui->treeWidget->currentItem(), 0);
}

void MapBrowser::langPt()
{
    if (m_lang == m_en && m_en)
    {
        qApp->removeTranslator(m_en);
        m_lang = 0;

        this->retranslate();
    }

    if (m_lang == m_en && m_en)
    {
        ui->actionEnglish->setChecked(true);
        ui->actionPortugu_s->setChecked(false);

        settings->setValue("translation", "en");

        if (m_qtpt)
        {
            qApp->removeTranslator(m_qtpt);
        }
    }
    else
    {
        ui->actionEnglish->setChecked(false);
        ui->actionPortugu_s->setChecked(true);

        settings->setValue("translation", "pt");

        if (m_qtpt)
        {
            qApp->installTranslator(m_qtpt);
        }
    }
}

void MapBrowser::langEn()
{
    if (!m_lang && m_en)
    {
        qApp->installTranslator(m_en);
        m_lang = m_en;

        this->retranslate();
    }

    if (m_lang == m_en && m_en)
    {
        ui->actionEnglish->setChecked(true);
        ui->actionPortugu_s->setChecked(false);

        settings->setValue("translation", "en");

        if (m_qtpt)
        {
            qApp->removeTranslator(m_qtpt);
        }
    }
    else
    {
        ui->actionEnglish->setChecked(false);
        ui->actionPortugu_s->setChecked(true);

        settings->setValue("translation", "pt");

        if (m_qtpt)
        {
            qApp->installTranslator(m_qtpt);
        }
    }
}

void MapBrowser::processToolbarAction(QAction* act)
{
    QTreeWidgetItem* wd = ui->treeWidget->currentItem();
    if (!wd) return;

    if (act == ui->mainToolBar->actions()[0] || act == ui->mainToolBar->actions()[1])
    {
        if (wd->type() == MapItem::MapType)
        {
            wd = wd->parent();
            if (!wd) return;
        }
    }

    ui->treeWidget->processMapAction(new MapAction(act->data().toInt(), wd, ui->treeWidget));
}

MapLoader* MapBrowser::loader()
{
    return m_loader;
}

MapPreview* MapBrowser::preview()
{
    return m_preview;
}

void MapBrowser::mapChanged(QTreeWidgetItem* current, QTreeWidgetItem*)
{
    if (!current) return;

    ui->webView->stop();
    ui->webView->setHtml(QString());

    ui->mainToolBar->actions()[4]->setEnabled(current->parent() != 0);
    ui->mainToolBar->actions()[5]->setEnabled(current->parent() != 0);

    if (current->type() == MapItem::MapType)
    {
        MapItem* mi = (MapItem*)current;

        MapToHtml mth(this);
        mth.buildItemView(mi);

        ui->webView->setHtml(mth.html());
    }
    else if (current->type() == MapFolder::FolderType)
    {
        MapFolder* mf = (MapFolder*)current;

        MapToHtml mth(this);
        mth.buildFolderView(mf);

        ui->webView->setHtml(mth.html());
    }
}

void MapBrowser::redoLayout(const QSize& twsize, const QSize& wvsize)
{
    static int padleft = ui->treeWidget->x();
    static int padmid = ui->line->x() - ui->treeWidget->x() - ui->treeWidget->width();

    if (this->size() == this->minimumSize()) return;

    if (twsize.width() < ui->treeWidget->minimumSize().width()) return;
    if (wvsize.width() < ui->webView->minimumSize().width()) return;

    ui->treeWidget->resize(twsize);
    ui->webView->resize(wvsize);

    QPoint twmove(padleft, ui->treeWidget->y());
    QPoint lnmove(ui->treeWidget->x() + ui->treeWidget->width() + padmid, ui->line->y());
    QPoint wvmove(ui->line->x() + ui->line->width() + padmid, ui->webView->y());

    ui->treeWidget->move(twmove);
    ui->line->move(lnmove);
    ui->webView->move(wvmove);
}

bool MapBrowser::eventFilter(QObject* sender, QEvent* event)
{
    if (sender == ui->line)
    {
        static bool isResizing = false;
        static QPoint last_pos = QPoint();

        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* e = (QMouseEvent*)event;

            isResizing = true;
            last_pos = e->globalPos();
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            isResizing = false;
            last_pos = QPoint();

            redoLayout(ui->treeWidget->size(), ui->webView->size());
        }
        else if (event->type() == QEvent::MouseMove)
        {
            if (isResizing)
            {
                QMouseEvent* e = (QMouseEvent*)event;
                QPoint tpos = e->globalPos();

                int lx = tpos.x() - last_pos.x();
                last_pos = tpos;

                QSize twsize(ui->treeWidget->width() + lx, ui->treeWidget->height());
                QSize wvsize(ui->webView->width() - lx, ui->webView->height());

                redoLayout(twsize, wvsize);
            }
        }
    }

    return false;
}

MapBrowser::~MapBrowser()
{
    if (!lastDir.isEmpty())
    {
        settings->setValue("last_directory", lastDir);
    }

    delete ui;
}
