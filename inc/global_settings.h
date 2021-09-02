#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMessageBox>

#define MAX_USER_CONFIG   4

typedef struct global_settings{
    bool logToFile;
    bool enableTimeshift;
    QString logPath;
}Global_Settings_t;

typedef struct user_config{
    QString portName;
    QString portDiscription;
    bool open;
    uint32_t boundRate;
    uint32_t dateBit;
    uint32_t parityBit;
    uint32_t stopBit;
    uint32_t flowControl;
    bool enableReceive;
    bool echo;
    bool timeshift;
    uint8_t endChar;
    QStringList sendRecord;
    bool valid;
} User_Config_t;

typedef struct global_config{
    Global_Settings_t global_settings;
    QList<User_Config_t> user_config;
} Global_Config_t;

class GlobalSettings
{
public:
    GlobalSettings();
    ~GlobalSettings();

    class GlobalSettings *GSettings;

    Global_Config_t globalConfig;
    Global_Config_t *config = &globalConfig;
    QJsonObject* globalSettingsToJosn();
    QString globalSettingsToString();
    int StringToGlobalSettings(QString *str);
};

#endif // GLOBAL_SETTINGS_H
