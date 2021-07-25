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
    ui->listWidget_data_send_list->setGridSize(QSize()); //设置默认大小

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

void MainWindow::flush_device(void)
{
    ui->comboBox_port->clear();

    infoList = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &info, infoList)
    {
        ui->comboBox_port->addItem(info.portName() + " (" + info.description() + ")");
    }
    if (ui->comboBox_port->count() == 0) {
        ui->comboBox_port->addItem(tr("NULL"));
        ui->radioButton_openPort->setEnabled(false);
    }
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

    if (settings.contains("splitterSizes2"))
    {
        ui->splitter_2->restoreState(settings.value("splitterSizes2").toByteArray());
        qDebug() << "splitter2 size load success";
    }

    if (settings.contains("mainwindow"))
    {
        restoreState(settings.value("mainwindow").toByteArray());
    }

    if (settings.contains("geometry"))
    {
        restoreGeometry(settings.value("geometry").toByteArray());
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

    if (settings.contains("data_send_list_window"))
    {
        ui->dockWidget->setHidden(settings.value("data_send_list_window").toBool());
        ui->dockWidget->isHidden() ? ui->action->setText("显示预定义输入窗口") : ui->action->setText("关闭预定义输入窗口");
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

    if (settings.contains("send_data_list"))
    {
        QStringList send_data_list = settings.value("send_data_list").toStringList();
        int size = send_data_list.length();
        for (int i = 0; i < size; i++)
        {
            ui->listWidget_data_send_list->insertItem(i, send_data_list[i]);
            QListWidgetItem *item = ui->listWidget_data_send_list->item(i);
            item->setToolTip(send_data_list[i]);
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
            statusBar()->setStyleSheet("color:green");
            qInfo() << "Open serial port: " << ui->comboBox_port->currentText() << "success";
        } else {
            checked = false;
            ui->radioButton_openPort->setChecked(checked);
            QString message = "Connect to " + ui->comboBox_port->currentText() + " failed : " + port->errorString();
            statusBar()->showMessage(message,5000);
            statusBar()->setStyleSheet("color:red");
            ui->pushButton_dataSend->setStyleSheet("background-color: rgb(215, 215, 215);");
            qInfo() << "Open serial port: " << ui->comboBox_port->currentText() << "failed";
        }
    } else {
        port->close();
        ui->radioButton_openPort->setText(tr("打开串口"));
        ui->pushButton_dataSend->setStyleSheet("background-color: rgb(215, 215, 215)");
        statusBar()->showMessage("Port " + port->portName() + " ( " + infoList[ui->comboBox_port->currentIndex()].description() + " ) is closed");
        statusBar()->setStyleSheet("color:red");
        qInfo() << "Close serial port: " << port->portName() << "success";
    }

    ui->comboBox_port->setEnabled(!checked);
    ui->pushButton_dev_flush->setEnabled(!checked);
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

    if (is_close_receive) return ;

    if (ui->checkBox_timestamp->isChecked())
    {
        QDateTime timestamp = QDateTime::currentDateTime();
        text = timestamp.toString("[yyyy-MM-dd hh:mm:ss]>> ");
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
        statusBar()->setStyleSheet("color:red");
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

    is_sent = true;

    if (ui->textEdit_dataSend->toPlainText() != "")
    {
        ui->listWidget->setItemSelected(ui->listWidget->currentItem(), false);
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

    QString dataSend = ui->textEdit_dataSend->toPlainText();

    /* Add end charactor */
    if (ui->comboBox_endChar->currentText() != "NULL")
    {
        dataSend += endChar[ui->comboBox_endChar->currentIndex()];
    }

    // Show the data sent.
    if (ui->checkBox_echo->isChecked())
    {
        QString echo = dataSend;

        /* Add timestamp */
        if (ui->checkBox_timestamp->isChecked())
        {
            QDateTime timestamp = QDateTime::currentDateTime();
            echo = timestamp.toString("[yyyy-MM-dd hh:mm:ss]<< ") + echo;
        }
        ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
        ui->textEdit_dataReceive->insertPlainText(echo);
        ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
    }

    /* Send data to serial port. */
    if (port->write(dataSend.toStdString().c_str()) == -1)
    {
        statusBar()->showMessage("Send data failed: " + port->errorString(), 5000);
        statusBar()->setStyleSheet("color:red");
        return ;
    }

    ui->textEdit_dataSend->clear();
}

void MainWindow::sendData(QString &s)
{
    if (ui->radioButton_openPort->isChecked() == false)
    {
        statusBar()->showMessage("Send data failed: Port is closed", 5000);
        statusBar()->setStyleSheet("color:red");
        qDebug() << "Can't send data, beacuse port is closed.";
        return ;
    }

    is_sent = true;

    QString dataSend = s;

    /* Add end charactor */
    if (ui->comboBox_endChar->currentText() != "NULL")
    {
        dataSend += endChar[ui->comboBox_endChar->currentIndex()];
    }

    // Show the data sent.
    if (ui->checkBox_echo->isChecked())
    {
        QString echo = dataSend;

        /* Add timestamp */
        if (ui->checkBox_timestamp->isChecked())
        {
            QDateTime timestamp = QDateTime::currentDateTime();
            echo = timestamp.toString("[yyyy-MM-dd hh:mm:ss]<< ") + echo;
        }
        ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
        ui->textEdit_dataReceive->insertPlainText(echo);
        ui->textEdit_dataReceive->moveCursor(QTextCursor::End);
    }


    /* Send data to serial port. */
    if (port->write(dataSend.toStdString().c_str()) == -1)
    {
        statusBar()->showMessage("Send data failed: " + port->errorString(), 5000);
        statusBar()->setStyleSheet("color:red");
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
        if (event->type() == QEvent::KeyPress)      // 回车发送
        {
            if (key->key() == Qt::Key_Return)
            {
                sendData();
                return true;
            }
            else if (key->key() == Qt::Key_Up)        // 选择旧的发送记录，上翻
            {
                if (ui->listWidget->currentRow() == 0 && is_sent == true)
                {
                    ui->textEdit_dataSend->setText(ui->listWidget->currentItem()->text());
                    is_sent = false;
                    return true;
                }
                else
                {
                    if (ui->listWidget->currentRow() != ui->listWidget->count() - 1)
                    {
                        ui->listWidget->setCurrentRow(ui->listWidget->currentRow() + 1);
                        ui->textEdit_dataSend->setText(ui->listWidget->currentItem()->text());
                        return true;
                    }
                }
            }
            else if (key->key() == Qt::Key_Down)    // 选择旧的发送记录，下翻
            {
                if (ui->listWidget->currentRow() - 1 >= 0)
                {
                    ui->listWidget->setCurrentRow(ui->listWidget->currentRow() - 1);
                    ui->textEdit_dataSend->setText(ui->listWidget->currentItem()->text());
                    return true;
                }
                else {
                    ui->textEdit_dataSend->clear();
                    is_sent = true;
                }
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
    else if (ui->dockWidget == target)
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if (event->type() == QEvent::KeyPress)      // 回车发送
        {
            if (key->key() == Qt::Key_Return)
            {
                printf("test ok\n");
                return true;
            }
        }
    }

    return QWidget::eventFilter(target, event);
}

void MainWindow::on_pushButton_clearDateReceive_2_clicked()
{
    ui->dockWidget->setHidden(!ui->dockWidget->isHidden());
}

/* 保存窗口配置，以便下次启动载入配置 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("splitterSizes", ui->splitter->saveState());
    settings.setValue("splitterSizes2", ui->splitter_2->saveState());
    settings.setValue("mainwindow", saveState());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("port", ui->comboBox_port->currentText());
    settings.setValue("bandRate", ui->comboBox_baudRate->currentText());
    settings.setValue("echoEnable", ui->checkBox_echo->isChecked());
    settings.setValue("timestamp", ui->checkBox_timestamp->isChecked());
    settings.setValue("endChar", ui->comboBox_endChar->currentText());
    settings.setValue("data_send_list_window", ui->dockWidget->isHidden());

    QStringList send_data_records;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        send_data_records << ui->listWidget->item(i)->text();
    }
    settings.setValue("send_data_records", send_data_records);

    QStringList send_data_list;
    for (int i = 0; i < ui->listWidget_data_send_list->count(); i++)
    {
        send_data_list << ui->listWidget_data_send_list->item(i)->text();
    }
    settings.setValue("send_data_list", send_data_list);

    QWidget::closeEvent(event);
}

void MainWindow::on_action_triggered()
{
    ui->dockWidget->setHidden(!(ui->dockWidget->isHidden()));
    ui->dockWidget->isHidden() ? ui->action->setText("显示预定义输入窗口") : ui->action->setText("关闭预定义输入窗口");
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->textEdit_dataSend->setText(item->text());
}


void MainWindow::on_pushButton_dev_flush_clicked()
{
    flush_device();
}


void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item = ui->listWidget->itemAt(pos);
    if (item == NULL) return;

    QMenu *popMenu = new QMenu(this);
    QAction *deleteSeed = new QAction(tr("Delete"), this);
    popMenu->addAction(deleteSeed);
    connect(deleteSeed, SIGNAL(triggered()), this, SLOT(deleteSeedSlot()));
    popMenu->exec(QCursor::pos());
    delete popMenu;
    delete deleteSeed;
}

void MainWindow::deleteSeedSlot()
{
    QList<QListWidgetItem*> list = ui->listWidget->selectedItems();
    if(list.size() == 0) return;
    foreach (QListWidgetItem* var, list)
    {
         int r = ui->listWidget->row(var);
         ui->listWidget->takeItem(r);
         delete var;
    }
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString dataSend = item->text();
    sendData(dataSend);
    ui->textEdit_dataSend->clear();
}

void MainWindow::on_pushButton_add_send_item_clicked()
{
    ui->listWidget_data_send_list->setItemSelected(ui->listWidget_data_send_list->currentItem(), false);
    ui->listWidget_data_send_list->addItem("请输入");
    ui->listWidget_data_send_list->setCurrentRow(ui->listWidget_data_send_list->count() - 1);
    ui->listWidget_data_send_list->currentItem()->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
}


void MainWindow::on_listWidget_data_send_list_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item = ui->listWidget_data_send_list->itemAt(pos);
    if (item == NULL)
    {
        QMenu *popMenu = new QMenu(this);
        QAction *addItem = new QAction(tr("Add Item"), this);
        popMenu->addAction(addItem);
        connect(addItem, SIGNAL(triggered()), this, SLOT(add_data_send_item_Slot()));
        popMenu->exec(QCursor::pos());
        delete popMenu;
        delete addItem;
        return ;
    }

    QMenu *popMenu = new QMenu(this);
    QAction *deleteSeed = new QAction(tr("Delete"), this);
    QAction *editItem = new QAction(tr("Edit"), this);
    QAction *addItem = new QAction(tr("Add Item"), this);
    popMenu->addAction(deleteSeed);
    popMenu->addAction(editItem);
    popMenu->addAction(addItem);
    connect(deleteSeed, SIGNAL(triggered()), this, SLOT(delete_data_send_item_Slot()));
    connect(editItem, SIGNAL(triggered()), this, SLOT(edit_data_send_item_Slot()));
    connect(addItem, SIGNAL(triggered()), this, SLOT(add_data_send_item_Slot()));
    popMenu->exec(QCursor::pos());
    delete popMenu;
    delete deleteSeed;
    delete editItem;
    delete addItem;
}

void MainWindow::delete_data_send_item_Slot()
{
    QList<QListWidgetItem*> list = ui->listWidget_data_send_list->selectedItems();
    if(list.size() == 0) return;
    foreach (QListWidgetItem* var, list)
    {
         int r = ui->listWidget_data_send_list->row(var);
         ui->listWidget_data_send_list->takeItem(r);
         delete var;
    }
}

void MainWindow::edit_data_send_item_Slot()
{
    ui->listWidget_data_send_list->currentItem()->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
}

void MainWindow::add_data_send_item_Slot()
{
    ui->listWidget_data_send_list->setItemSelected(ui->listWidget_data_send_list->currentItem(), false);
    ui->listWidget_data_send_list->addItem("请输入");
    ui->listWidget_data_send_list->setCurrentRow(ui->listWidget_data_send_list->count() - 1);
    ui->listWidget_data_send_list->currentItem()->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
}

void MainWindow::on_listWidget_data_send_list_itemClicked(QListWidgetItem *item)
{
    if (item->text() != "" && !(ui->listWidget_data_send_list->currentItem()->flags() & Qt::ItemIsEditable) )
    {
        QString data = item->text();
        sendData(data);
    }
}

void MainWindow::on_listWidget_data_send_list_currentTextChanged(const QString &currentText)
{
    qDebug() << currentText;
}


void MainWindow::on_action_2_triggered()
{
    ui->textEdit_dataReceive->clear();
}


void MainWindow::on_action_4_triggered()
{
    is_display_stop = !is_display_stop;
    !is_display_stop ? ui->action_4->setText("暂停显示") : ui->action_4->setText("开始显示");
}


void MainWindow::on_action_3_triggered()
{
    is_close_receive = !is_close_receive;
    is_close_receive ? ui->action_3->setText("打开接收") : ui->action_3->setText("关闭接收");
}


void MainWindow::on_action_5_triggered()
{
    save_data_send_list();
}

void MainWindow::save_data_send_list(void)
{
    for (int i = 0; i < ui->listWidget_data_send_list->count(); i++)
    {
        ui->listWidget_data_send_list->item(i)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
}

