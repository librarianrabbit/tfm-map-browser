#include "mapfoldervalidator.h"

MapFolderValidator::MapFolderValidator(QObject *parent) :
    QValidator(parent)
{
}

QValidator::State MapFolderValidator::validate(QString& input, int&) const
{
    if (input.size() > 0)
    {
        input = input.remove(QRegExp("[<>:/\"\\*\\?\\|\\\\]"));
    }

    return Acceptable;
}
