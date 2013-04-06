#include <QDebug>
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "xplaneplugin.h"

XPlanePlugin *globalPlugin=0;

PLUGIN_API __attribute__((visibility("default"))) float	MyFlightLoopCallback(
        float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,
        int inCounter, void *inRefcon) {
    if(globalPlugin)
        return globalPlugin->flightLoop(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon);
    return 1;
}

PLUGIN_API __attribute__((visibility("default"))) int XPluginStart(
        char * outName, char * outSig, char *outDesc) {
    XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 0.01, NULL);
    Q_ASSERT(!globalPlugin);
    globalPlugin = new XPlanePlugin();
    if(globalPlugin) {
        return globalPlugin->pluginStart(outName,outSig,outDesc);
    } else {
        qDebug() <<  Q_FUNC_INFO << "Unable to create plugin";
        return 0;
    }
}

PLUGIN_API __attribute__((visibility("default"))) void	XPluginStop(void) {
    qDebug() <<  Q_FUNC_INFO;
    XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, 0);
    globalPlugin->pluginStop();
    delete globalPlugin;
    globalPlugin = 0;
}

PLUGIN_API __attribute__((visibility("default"))) void XPluginDisable(void) {
    qDebug() <<  Q_FUNC_INFO;
}

PLUGIN_API __attribute__((visibility("default"))) int XPluginEnable(void) {
    qDebug() <<  Q_FUNC_INFO;
    return 1;
}

PLUGIN_API __attribute__((visibility("default"))) void XPluginReceiveMessage(
        XPLMPluginID inFromWho, long inMessage, void *inParam) {
    if(globalPlugin)
        globalPlugin->receiveMessage(inFromWho, inMessage, inParam);
}

