#include "mapfolderdialog.h"
#include "ui_mapfolderdialog.h"

#include "mapfoldervalidator.h"

#include <QDebug>

MapFolderDialog::MapFolderDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Ui::MapFolderDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setValidator(new MapFolderValidator(ui->lineEdit));

    connect(ui->lineEdit, SIGNAL(textEdited(QString)), SLOT(onTextChanged(QString)));
}

void MapFolderDialog::onTextChanged(const QString& text)
{
    if (itemExists(text))
    {
        ui->label_2->setText("<font color=red>" + tr("A pasta já existe.") + "</font>");
        ui->pushButton->setEnabled(false);
    }
    else if (text.isEmpty())
    {
        ui->label_2->setText("<font color=red>" + tr("Digite um nome para a pasta.") + "</font>");
        ui->pushButton->setEnabled(false);
    }
    else
    {
        ui->label_2->setText(QString());
        ui->pushButton->setEnabled(true);
    }
}

bool MapFolderDialog::itemExists(const QString& name)
{
    for (int x = 0; x < m_item->childCount(); x++)
    {
        QTreeWidgetItem* it = m_item->child(x);
        QString i = it->text(0);

        if (name.toLower() == m_folder.toLower())
        {
            return false;
        }
        else if (name.toLower() == i.toLower())
        {
            return true;
        }
    }

    return false;
}

void MapFolderDialog::setFolderName(const QString& name)
{
    ui->lineEdit->setText(name);
    m_folder = name;
}

QString MapFolderDialog::folderName()
{
    return ui->lineEdit->text();
}

void MapFolderDialog::setParentItem(QTreeWidgetItem* item)
{
    m_item = item;
}

MapFolderDialog::~MapFolderDialog()
{
    delete ui;
}
