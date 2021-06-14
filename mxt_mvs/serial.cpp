#include "serial.h"

QSerialPort *serial;
QSerialPort *reviever;

QByteArray serialData;
QString serialBuffer;

void init_serial(std::string port, int baudrate) {

    serial = new QSerialPort();

    serial->setPortName(port.c_str());
    serial->setBaudRate(baudrate);

    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    serialBuffer = "";
}

int connect_serial() {
    bool connected = serial->open(QIODevice::ReadWrite);
    if (connected) {
        qDebug("Successfully connected.");
        //QSerialPort::connect(serial, SIGNAL(readyRead()), reviever, SLOT(recv_messages()));
        return 0;
    }
    else {
        qDebug("Could not connect, please check connection settings and retry.");
        return -1;
    }
}

void disconnect_serial() {

    if(serial->isOpen()) {
        serial->close();
        qDebug("Connection closed.");
    }
    else {
        qDebug("No connection opened, ignoring command.");
    }

}

void send_message(std::string msg) {
    msg += "\n";
    serial->write(msg.c_str());
}

void recv_messages() {
    serialData = serial->readAll();
    serialBuffer += QString::fromStdString(serialData.toStdString());
    qDebug(serialBuffer.toStdString().c_str());
}
