#ifndef DATAREF_H
#define DATAREF_H

#include <QString>
#include <QObject>
#include <QDebug>
#include "XPLMDataAccess.h"

class DataRef : public QObject {
    Q_OBJECT
public:
    DataRef(QObject *parent, QString name, XPLMDataRef ref);
    QString &name();
    XPLMDataRef ref();
    double value();
    int subscribers();
    void setSubscribers(int subs);
public slots:
    void setValue(double newValue);
signals:
    void changed(DataRef *ref);
private:
    QString _name;
    XPLMDataRef _ref;
    double _value;
    int _subscribers;
};

#endif // DATAREF_H
