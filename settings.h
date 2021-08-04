#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QSettings>
#include "inc/global_settings.h"

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    Settings(QWidget *parent = nullptr, GlobalSettings *pGlobalSettings = nullptr);
    ~Settings();

private slots:

    void on_pushButton_save_settings_clicked();

    void on_pushButton_quit_settings_clicked();

    void on_pushButton_restore_default_clicked();

    void on_pushButton_choose_log_path_clicked();

    void on_checkBox_log_to_file_clicked(bool checked);

signals:
    void enable_log_to_file(bool checked);

private:
    Ui::Settings *ui;
    QSettings settings;
    GlobalSettings *globalSettings;

    void restore_settings(void);
    void getSettings(void);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SETTINGS_H
