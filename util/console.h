#ifndef CONSOLE_H
#define CONSOLE_H

#include <QDebug>

/**
 * @brief Output for debugging
 *
 * Use this macro to output debugging information for debug builds.
 * Release builds will discard this macro and not output anything.
 * The class and function which called the macro is automatically included
 * in the output, and the macro can be called without any arguments.
 *
 * @see qDebug()
 */
#define DEBUG qDebug() << "ExtPlane-Plugin:" << Q_FUNC_INFO

/**
 * @brief Output for information
 *
 * Use this macro to output information for both debug and release builds
 * to the console. The macro can be called without any arguments.
 *
 * @see qCritical()
 */
#define INFO qCritical() << "ExtPlane-Plugin:"

#endif // CONSOLE_H
