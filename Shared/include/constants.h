#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QDebug>
#include <QTime>
#include <QLoggingCategory>

/******************************************************************************\
|* Identity
\******************************************************************************/
#define ORG_NAME				"Moebius-Tech"
#define ORG_DOMAIN				"moebius-tech.net"
#define APP_NAME				"SETIscan"
#define APP_VERSION				"0.1"

#define GUI_NAME				"setishow"
#define GUI_VERSION				"0.1"

/******************************************************************************\
|* Plugins, user is relative to $HOME
\******************************************************************************/
#define SYSTEM_PLUGINS_DIR		"/etc/seti/plugins"
#define USER_PLUGINS_DIR		".seti/plugins"

/******************************************************************************\
|* Logging
\******************************************************************************/
Q_DECLARE_LOGGING_CATEGORY(log_plugin)
Q_DECLARE_LOGGING_CATEGORY(log_dsp)
Q_DECLARE_LOGGING_CATEGORY(log_net)
Q_DECLARE_LOGGING_CATEGORY(log_gui)
Q_DECLARE_LOGGING_CATEGORY(log_db)
Q_DECLARE_LOGGING_CATEGORY(log_data)

#endif // CONSTANTS_H
