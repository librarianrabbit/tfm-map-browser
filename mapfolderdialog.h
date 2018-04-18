#ifndef MAPFOLDERDIALOG_H
#define MAPFOLDERDIALOG_H

#include <QDialog>

#include <QTreeWidgetItem>

namespace Ui {
    class MapFolderDialog;
}

class MapFolderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapFolderDialog(QWidget *parent = 0);
    ~MapFolderDialog();

    QString folderName();
    void setFolderName(const QString& name);

    void setParentItem(QTreeWidgetItem* item);

    bool itemExists(const QString& name);

private slots:
    void onTextChanged(const QString& text);

private:    
    Ui::MapFolderDialog *ui;

    QString m_folder;
    QTreeWidgetItem* m_item;
};

#endif // MAPFOLDERDIALOG_H
