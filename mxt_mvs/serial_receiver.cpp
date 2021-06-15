#include "includes.h"
#include "serial_receiver.h"

extern QSerialPort *serial;

QByteArray serialData;
QString serialBuffer;

Serial_receiver::Serial_receiver()
{
    serialBuffer = "";
}

void Serial_receiver::connect_readyread() {
    connect(serial, SIGNAL(readyRead()), this, SLOT(serial_recv()));
    std::cout << "Signal readyRead() is now connected." << std::endl;
}

void Serial_receiver::disconnect_readyread() {
    disconnect(serial, SIGNAL(readyRead()), this, SLOT(serial_recv()));
    std::cout << "Signal readyRead() is now disconnected." << std::endl;
}

void Serial_receiver::serial_recv() {
    serialBuffer = "";
    serialData = serial->readAll();
    serialBuffer += QString::fromStdString(serialData.toStdString());
    std::cout << serialBuffer.toStdString() << std::endl;
}
