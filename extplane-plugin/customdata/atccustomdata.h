#ifndef ATCCUSTOMDATA_H
#define ATCCUSTOMDATA_H

#include <QObject>
#include <QString>

#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"

#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>

class ATCCustomData : public QObject
{
    Q_OBJECT
public:
    explicit ATCCustomData(QObject *parent = 0);
    static int DataCallback(void *inRefcon, void *outValue, int inOffset, int inMaxLength);
    void updateData();
    QString getData(){ return _data; };

private:
    QString _data;
    std::ostringstream _gLogPath;
    std::ifstream _gLogFileIFS;
    std::streampos _gPos;

};


#endif // ATCCUSTOMDATA_H
