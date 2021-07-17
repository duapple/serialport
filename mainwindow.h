#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
#include <QListWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define DATA_SEND_COUNT       20

typedef struct global_configuration{
    int data_send_count = DATA_SEND_COUNT;
} GLOBAL_CONF_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    GLOBAL_CONF_t globle_conf;

    QList<QSerialPort::BaudRate> baudRate;
    QList<QSerialPort::DataBits> dataBits;
    QList<QSerialPort::Parity> parity;
    QList<QSerialPort::StopBits> stopBits;
    QList<QSerialPort::FlowControl> flowControl;
    QList<QString> endChar;

private slots:
    void on_radioButton_openPort_toggled(bool checked);

    void on_pushButton_clearDateReceive_clicked();

    void on_pushButton_dataSend_clicked();

    void on_pushButton_clearDateReceive_2_clicked();

    void on_action_triggered();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    QSerialPort *port;
    QList<QSerialPortInfo> infoList;
    QSettings settings;

    void initSerialPortSetting(void);
    void restoreUiSettings(void);
    void connections(void);
    void serialPortDataReceive(void);
    void sendData(void);
    void sendData(QString &s);

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
