#include "settings.h"
#include "ui_settings.h"
#include <QDesktopWidget>
#include <QFileDialog>

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    // 设置关闭销毁
    this->setAttribute(Qt::WA_DeleteOnClose);

    // 设置应用模态对话框，用于子窗口的焦点锁定
    this->setWindowModality(Qt::ApplicationModal);

    // 窗口屏幕显示居中
    // this->move((QApplication::desktop()->width() - this->width())/2,(QApplication::desktop()->height() - this->height())/2);

    restore_settings();
}

Settings::Settings(QWidget *parent, GlobalSettings *pGlobalSettings) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    globalSettings = pGlobalSettings;

    // 设置关闭销毁
    this->setAttribute(Qt::WA_DeleteOnClose);

    // 设置应用模态对话框，用于子窗口的焦点锁定
    this->setWindowModality(Qt::ApplicationModal);

    // 窗口屏幕显示居中
    // this->move((QApplication::desktop()->width() - this->width())/2,(QApplication::desktop()->height() - this->height())/2);

    restore_settings();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::closeEvent(QCloseEvent *event)
{
    settings.setValue("geometry", saveGeometry());

    QWidget::closeEvent(event);
}

void Settings::restore_settings()
{
    if (settings.contains("geometry"))
    {
        restoreGeometry(settings.value("geometry").toByteArray());
    }

    ui->checkBox_log_to_file->setChecked(globalSettings->config->global_settings.logToFile);
    ui->lineEdit_log_path->setText(globalSettings->config->global_settings.logPath);
    ui->checkBox_enable_timeshift->setChecked(globalSettings->config->global_settings.enableTimeshift);
}

void Settings::getSettings(void)
{
    globalSettings->config->global_settings.logToFile = ui->checkBox_log_to_file->isChecked();
    globalSettings->config->global_settings.enableTimeshift = ui->checkBox_enable_timeshift->isChecked();
    globalSettings->config->global_settings.logPath = ui->lineEdit_log_path->displayText();
}

void Settings::on_pushButton_save_settings_clicked()
{
    getSettings();
    emit enable_log_to_file(globalSettings->config->global_settings.logToFile);
}


void Settings::on_pushButton_quit_settings_clicked()
{
    this->close();
}


void Settings::on_pushButton_restore_default_clicked()
{
    ui->checkBox_log_to_file->setChecked(false);
    ui->lineEdit_log_path->clear();
    ui->checkBox_enable_timeshift->setChecked(false);
    getSettings();
}


void Settings::on_pushButton_choose_log_path_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this, "选择日志路径", ui->lineEdit_log_path->displayText());
    if (filename == "") {
        return ;
    }
    qDebug("file path: %s", qPrintable(filename));

    ui->lineEdit_log_path->setText(filename);
}


void Settings::on_checkBox_log_to_file_clicked(bool checked)
{

}

