#include "atccustomdata.h"

#include <QList>
#include <QTime>
#include <qmath.h>
#include <QDebug>
#include <QThread>

#include <console.h>

#define LOG_FILENAME "Log.txt"
#define UPDATE_INTERVAL_SECONDS 0.01
#define LOG_PREFIX_MONITOR_124THATC "124thATC v2"
#define LOG_PREFIX_MONITOR_COMM " : Communication: "


ATCCustomData *_atcCustomData = NULL;


ATCCustomData::ATCCustomData(QObject *parent) :
    QObject(parent)
{
    INFO << "Creating new ATCCustomData...";

    // Get the logfile path
    char sysPath[1024];
    XPLMGetSystemPath(sysPath);
    _gLogPath << sysPath << LOG_FILENAME;

    // Open file
    _gLogFileIFS.open(const_cast<char*>(_gLogPath.str().c_str()), std::ios::ate);
    if(!_gLogFileIFS.is_open()) {
        INFO << "Warning: Could not open" << LOG_FILENAME;
    }

    // Remember file position
    _gPos = _gLogFileIFS.tellg();

}

void ATCCustomData::updateData() {
    // Init
    std::string line;
    bool done = false;
    if(!_gLogFileIFS.is_open()) return;

    // Try to read all lines searching for our ATC log line
    while(!done) {
        if(!std::getline(_gLogFileIFS, line) || _gLogFileIFS.eof())
        {
            // If we fail, clear stream, return to beginning of line
            _gLogFileIFS.clear();
            _gLogFileIFS.seekg(_gPos);

            // Wait to try again
            done = true;
            continue;
        }

        // Debug output line
        DEBUG << QString::fromStdString(line);

        // Remember the position of the next line in case the next read fails
        _gPos = _gLogFileIFS.tellg();

        // Parse line...
        // First we search for the plugin prefix in the log
        //TODO: Include timestamp in case a message is repeated?
        std::string prefix(LOG_PREFIX_MONITOR_124THATC);
        if (!line.compare(0, prefix.size(), prefix)) {
            // Is it a communication?
            int index = line.find(LOG_PREFIX_MONITOR_COMM);
            if(index > -1) {
                // Communication!
                // Trim and save to data
                std::string comm = line.substr(index+strlen(LOG_PREFIX_MONITOR_COMM));
                DEBUG << "COMM: " << QString::fromStdString(comm);
                _data = QString("%1").arg(comm.c_str());
            }
        }
    }
}



int ATCCustomData::DataCallback(void *inRefcon, void *outValue, int inOffset, int inMaxLength) {
    Q_UNUSED(inRefcon);
    Q_UNUSED(inOffset);
    Q_UNUSED(inMaxLength);

    // Make sure the NavCustomData is initialized. This will happen once when the plugin is reloaded
    // upon the first call to the dataref
    if(_atcCustomData == NULL) {
        _atcCustomData = new ATCCustomData();
    }

    // Init
    _atcCustomData->updateData();
    QString retData = _atcCustomData->getData();
    int packetSize = retData.length();

    // If outvalue is NULL, then we only want to know the length
    if(outValue != NULL) {
        // Copy to outdata
        for(int i = 0; i < packetSize; i++) {
            if(i >= retData.length()) {
                ((char*)outValue)[i] = 0;
            } else {
                ((char*)outValue)[i] = retData[i].toLatin1();
            }
        }
    }

    return packetSize;
}
