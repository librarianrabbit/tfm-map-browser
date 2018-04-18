#ifndef MAPFOLDERVALIDATOR_H
#define MAPFOLDERVALIDATOR_H

#include <QValidator>

class MapFolderValidator : public QValidator
{
    Q_OBJECT
public:
    explicit MapFolderValidator(QObject *parent = 0);

    State validate(QString& input, int& pos) const;
};

#endif // MAPFOLDERVALIDATOR_H
