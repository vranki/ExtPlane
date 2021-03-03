#include <cstdlib>
#include <QDebug>
#include <XPLMPlugin.h>
#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>
#include "xplaneplugin.h"
#include "console.h"

XPlanePlugin *globalPlugin = nullptr;

PLUGIN_API float MyFlightLoopCallback(
        float inElapsedSinceLastCall,
        float inElapsedTimeSinceLastFlightLoop,
        int inCounter,
        void *inRefcon) {
    if(globalPlugin)
        return globalPlugin->flightLoop(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon);
    return 1;
}

PLUGIN_API int XPluginStart(
        char * outName,
        char * outSig,
        char *outDesc) {
    XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 0.01f, NULL);
    Q_ASSERT(!globalPlugin);
    globalPlugin = new XPlanePlugin();
    return globalPlugin->pluginStart(outName, outSig, outDesc);
}

PLUGIN_API void XPluginStop() {
    DEBUG;
    XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, 0);
    globalPlugin->pluginStop();
    delete globalPlugin;
    globalPlugin = nullptr;
}

PLUGIN_API void XPluginDisable() {
    DEBUG;
    if(globalPlugin)
        globalPlugin->pluginDisable();
}

PLUGIN_API int XPluginEnable() {
    DEBUG;
    if(globalPlugin)
        return globalPlugin->pluginEnable();
    return 0;
}

PLUGIN_API void XPluginReceiveMessage(
        XPLMPluginID inFromWho,
        long inMessage,
        void *inParam) {
    Q_UNUSED(inFromWho)
    Q_UNUSED(inMessage)
    Q_UNUSED(inParam)
}
