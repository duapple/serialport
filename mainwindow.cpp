#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QTextEdit>
#include <QDateTime>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settings("SeriapPortAssistant", "SplitterWidget")
{
    ui->setupUi(this);

    port = new QSerialPort(this);

    ui->radioButton_openPort->setStyleSheet("QRadioButton::indicator {width:15px;height:15px;border-radius:7px}"
                                   "QRadioButton::indicator:checked {background-color:green;}"
                                   "QRadioButton::indicator:unchecked {background-color:red;}"
                                   );

    ui->pushButton_dataSend->setStyleSheet("color: rgb(215, 215, 215)}");
    ui->pushButton_dataSend->setEnabled(false);
    ui->textEdit_dataSend->installEventFilter(this);

    initSerialPortSetting();
    connections();
    restoreUiSettings();

    // ui->textEdit_dataReceive->setFontWeight(7);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete port;
}

void MainWindow::initSerialPortSetting(void)
{
    infoList = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &info, infoList)
    {
        ui->comboBox_port->addItem(info.portName() + " (" + info.description() + ")");
    }
    if (ui->comboBox_port->count() == 0) {
        ui->comboBox_port->addItem(tr("NULL"));
        ui->radioButton_openPort->setEnabled(false);
    }

    this->baudRate << QSerialPort::Baud115200 << QSerialPort::Baud57600 << QSerialPort::Baud38400
                   << QSerialPort::Baud19200 << QSerialPort::Baud9600 << QSerialPort::Baud4800
                   << QSerialPort::Baud2400 << QSerialPort::Baud1200;
    QStringList baudRate;
    baudRate << tr("115200") << tr("57600") << tr("38400")
             << tr("19200") << tr("9600") << tr("4800")
             << tr("2400") << tr("1200");
    ui->comboBox_baudRate->addItems(baudRate);

    this->dataBits << QSerialPort::Data8 << QSerialPort::Data7
                   << QSerialPort::Data6 << QSerialPort::Data5;
    QStringList dataBits;
    dataBits << tr("8") << tr("7") << tr("6") << tr("5");
    ui->comboBox_dateBit->addItems(dataBits);

    this->parity << QSerialPort::NoParity << QSerialPort::OddParity << QSerialPort::EvenParity
                 << QSerialPort::SpaceParity << QSerialPort::MarkParity;
    QStringList parity;
    parity << tr("无校验") << tr("奇校验") << tr("偶校验")
           << tr("空校验") << tr("标记校验");
    ui->comboBox_parityBit->addItems(parity);

    this->stopBits << QSerialPort::OneStop << QSerialPort::OneAndHalfStop << QSerialPort::TwoStop;
    QStringList stopBits;
    stopBits << tr("1") << tr("1.5") << tr("2");
    ui->comboBox_stopBit->addItems(stopBits);

    this->flowControl << QSerialPort::NoFlowControl << QSerialPort::SoftwareControl << QSerialPort::HardwareControl;
    QStringList flowControl;
    flowControl << tr("无") << tr("软件") << tr("硬件");
    ui->comboBox_flowControl->addItems(flowControl);

    this->endChar << "\r\n" << "\r" << "\n" << "" << "\n\r" << "\0";
    QStringList endChar;
    endChar << tr("\\r\\n") << tr("\\r") << tr("\\n") << tr("NULL") << tr("\\n\\r") << tr("\\0");
    ui->comboBox_endChar->addItems(endChar);

    ui->checkBox_echo->setChecked(true);
    ui->textEdit_dataReceive->setReadOnly(true);
}

void MainWindow::restoreUiSettings()
{
    if (settings.contains("splitterSizes"))
    {
        ui->splitter->restoreState(settings.value("splitterSizes").toByteArray());
        qDebug() << "splitter size load success";
    }

    if (settings.contains("port"))
    {
        ui->comboBox_port->setCurrentText(settings.value("port").toString());
    }

    if (settings.contains("bandRate"))
    {
        ui->comboBox_baudRate->setCurrentText(settings.value("bandRate").toString());
    }

    if (settings.contains("echoEnable"))
    {
        ui->checkBox_echo->setChecked(settings.value("echoEnable").toBool());
    }

    if (settings.contains("timestamp"))
    {
        ui->checkBox_timestamp->setChecked(settings.value("timestamp").toBool());
    }

    if (settings.contains("endChar"))
    {
        ui->comboBox_endChar->setCurrentText(settings.value("endChar").toString());
    }

    if (settings.contains("send_data_records"))
    {
        QStringList send_data_records = settings.value("send_data_records").toStringList();
        int size = send_data_records.length() > globle_conf.data_send_count ? send_data_records.length() : send_data_records.length();
        for (int i = 0; i < size; i++)
        {
            ui->listWidget->insertItem(i, send_data_records[i]);
            QListWidgetItem *item = ui->listWidget->item(i);
            item->setToolTip(send_data_records[i]);
        }
    }
}

void MainWindow::on_radioButton_openPort_toggled(bool checked)
{
    if (checked) {
        port->setPort(infoList[ui->comboBox_port->currentIndex()]);
        port->setBaudRate(baudRate[ui->comboBox_baudRate->currentIndex()]);
        port->setDataBits(dataBits[ui->comboBox_dateBit->currentIndex()]);
        port->setParity(parity[ui->comboBox_parityBit->currentIndex()]);
        port->setStopBits(stopBits[ui->comboBox_stopBit->currentIndex()]);
        port->setFlowControl(flowControl[ui->comboBox_flowControl->currentIndex()]);
        if (port->open(QSerialPort::ReadWrite))
        {
            ui->radioButton_openPort->setText(tr("关闭串口"));
            ui->pushButton_dataSend->setStyleSheet("background-color: rgb(85, 170, 255);");
            statusBar()->showMessage("Open port success:  " + port->portName() + " ( " + infoList[ui->comboBox_port->currentIndex()].description() + " )");
            qInfo() << "Open serial port: " << ui->comboBox_port->currentText() << "success";
        } else {
            checked = false;
            ui->radioButton_openPort->setChecked(checked);
            QString message = "Connect to " + ui->comboBox_port->currentText() + " failed : " + port->errorString();
            statusBar()->showMessage(message,5000);
            ui->pushButton_dataSend->setStyleSheet("background-color: rgb(215, 215, 215);");
            qInfo() << "Open serial port: " << ui->comboBox_port->currentText() << "failed";
        }
    } else {
        port->close();
        ui->radioButton_openPort->setText(tr("打开串口"));
        ui->pushButton_dataSend->setStyleSheet("background-color: rgb(215, 215, 215)");
        statusBar()->showMessage("");
        qInfo() << "Close serial port: " << port->portName() << "success";
    }

    ui->comboBox_port->setEnabled(!checked);
    ui->comboBox_baudRate->setEnabled(!checked);
    ui->comboBox_dateBit->setEnabled(!checked);
    ui->comboBox_stopBit->setEnabled(!checked);
    ui->comboBox_parityBit->setEnabled(!checked);
    ui->comboBox_flowControl->setEnabled(!checked);
    ui->pushButton_dataSend->setEnabled(checked);
}

void MainWindow::connections(void)
{
    connect(port, &QSerialPort::readyRead, this, &MainWindow::serialPortDataReceive);
}

void MainWindow::serialPortDataReceive(void)
{
    QByteArray data = port->readAll();
    QString text;

    if (ui->checkBox_timestamp->isChecked())
    {
        QDateTime timestamp = QDateTime::currentDateTime();
        text = timestamp.toString("[yyyy-MM-dd hh:mm:ss] ");
    }

    text += data;
    ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
    ui->textEdit_dataReceive->insertPlainText(text);
    ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
}


void MainWindow::on_pushButton_clearDateReceive_clicked()
{
    ui->textEdit_dataReceive->clear();
}


void MainWindow::on_pushButton_dataSend_clicked()
{
    sendData();
}

void MainWindow::sendData(void)
{
    if (ui->radioButton_openPort->isChecked() == false)
    {
        statusBar()->showMessage("Send data failed: Port is closed", 5000);
        qDebug() << "Can't send data, beacuse port is closed.";
        return ;
    }

    /* 设发送记录列表刷新 */
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        if (ui->textEdit_dataSend->toPlainText() == ui->listWidget->item(i)->text())
        {
            QListWidgetItem *item = ui->listWidget->takeItem(i);
            ui->listWidget->removeItemWidget(item);
            delete(item);
            break;
        }
    }

    if (ui->textEdit_dataSend->toPlainText() != "")
    {
        ui->listWidget->insertItem(0, ui->textEdit_dataSend->toPlainText());
        ui->listWidget->setCurrentRow(0);
        QListWidgetItem *item = ui->listWidget->item(0);
        item->setToolTip(ui->textEdit_dataSend->toPlainText());
    }

    if (ui->listWidget->count() > globle_conf.data_send_count)
    {
        QListWidgetItem *item = ui->listWidget->takeItem(globle_conf.data_send_count - 1);
        ui->listWidget->removeItemWidget(item);
        delete(item);
    }

    // Show the data sent.
    if (ui->checkBox_echo->isChecked())
    {
        QString echo = ui->textEdit_dataSend->toPlainText();

        /* Add timestamp */
        if (ui->checkBox_timestamp->isChecked())
        {
            QDateTime timestamp = QDateTime::currentDateTime();
            echo = timestamp.toString("[yyyy-MM-dd hh:mm:ss] ") + echo;
        }
        ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
        ui->textEdit_dataReceive->insertPlainText(echo);
        ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
    }

    QString dataSend = ui->textEdit_dataSend->toPlainText();

    /* Add end charactor */
    if (ui->comboBox_endChar->currentText() != "NULL")
    {
        dataSend += endChar[ui->comboBox_endChar->currentIndex()];
    }

    /* Send data to serial port. */
    if (port->write(dataSend.toStdString().c_str()) == -1)
    {
        statusBar()->showMessage("Send data failed: " + port->errorString(), 5000);
        return ;
    }

    ui->textEdit_dataSend->clear();
}

void MainWindow::sendData(QString &s)
{
    if (ui->radioButton_openPort->isChecked() == false)
    {
        statusBar()->showMessage("Send data failed: Port is closed", 5000);
        qDebug() << "Can't send data, beacuse port is closed.";
        return ;
    }

    // Show the data sent.
    if (ui->checkBox_echo->isChecked())
    {
        QString echo = ui->textEdit_dataSend->toPlainText();

        /* Add timestamp */
        if (ui->checkBox_timestamp->isChecked())
        {
            QDateTime timestamp = QDateTime::currentDateTime();
            echo = timestamp.toString("[yyyy-MM-dd hh:mm:ss] ") + echo;
        }
        ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
        ui->textEdit_dataReceive->insertPlainText(echo);
        ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
    }

    QString dataSend = s;

    /* Add end charactor */
    if (ui->comboBox_endChar->currentText() != "NULL")
    {
        dataSend += endChar[ui->comboBox_endChar->currentIndex()];
    }

    /* Send data to serial port. */
    if (port->write(dataSend.toStdString().c_str()) == -1)
    {
        statusBar()->showMessage("Send data failed: " + port->errorString(), 5000);
        return ;
    }

    ui->textEdit_dataSend->clear();
}

/* Text Edit Control clear display text when press enter. */
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->textEdit_dataSend)
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if (event->type() == QEvent::KeyPress)
        {
            if (key->key() == Qt::Key_Return)
            {
                sendData();
                return true;
            }
        }
        else if (key->matches(QKeySequence::Copy)) {
            qDebug() << "Enter Ctrl+C.";
            ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
            char etx[] = {0x03};
            QString etxStr = QString::fromStdString(etx);
            sendData(etxStr);
            return true;
        }
    }
    return QWidget::eventFilter(target, event);
}

void MainWindow::on_pushButton_clearDateReceive_2_clicked()
{
    ui->textEdit_dataSend->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("splitterSizes", ui->splitter->saveState());
    settings.setValue("port", ui->comboBox_port->currentText());
    settings.setValue("bandRate", ui->comboBox_baudRate->currentText());
    settings.setValue("echoEnable", ui->checkBox_echo->isChecked());
    settings.setValue("timestamp", ui->checkBox_timestamp->isChecked());
    settings.setValue("endChar", ui->comboBox_endChar->currentText());
    QStringList send_data_records;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        send_data_records << ui->listWidget->item(i)->text();
    }
    settings.setValue("send_data_records", send_data_records);
    QWidget::closeEvent(event);
}

void MainWindow::on_action_triggered()
{
    //addDockWidget(Qt::RightDockWidgetArea, ui->dockWidget);

//    QDockWidget *dw3 = new QDockWidget("停靠窗口3",this);//构建停靠窗口，指定父类

//    dw3->setFeatures(QDockWidget::DockWidgetMovable);//设置停靠窗口特性，具有全部停靠窗口的特性

//    QTextEdit *dte3 = new QTextEdit("DockWindow Third");
//    dw3->setWidget(dte3);
//    addDockWidget(Qt::RightDockWidgetArea,dw3);
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->textEdit_dataSend->setText(item->text());
}

