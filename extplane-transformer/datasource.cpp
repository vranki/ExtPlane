#include "datasource.h"

DataSource::DataSource(QObject *parent) : QObject(parent)
{}

QString DataSource::helpText() const
{
    return m_helpText;
}

void DataSource::setHelpText(QString newText)
{
    m_helpText = newText;
    emit helpTextChanged(m_helpText);
}
