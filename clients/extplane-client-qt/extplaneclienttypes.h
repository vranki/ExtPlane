#ifndef EXTPLANECLIENTTYPES_H
#define EXTPLANECLIENTTYPES_H

class QQmlContext;

class ExtPlaneClientTypes
{
public:
    ExtPlaneClientTypes() = delete;
    static void registerQmlTypes();
    static void publishClient(QQmlContext *context, bool simulated = false);
};

#endif // EXTPLANECLIENTTYPES_H
