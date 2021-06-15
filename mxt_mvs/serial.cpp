#include "serial.h"
#include "includes.h"

extern QSerialPort *serial;
extern QString serialBuffer;

void init_serial(std::string port, int baudrate) {

    serial->setPortName(port.c_str());
    serial->setBaudRate(baudrate);

    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

}

int connect_serial() {
    bool connected = serial->open(QIODevice::ReadWrite);
    if (connected) {
        std::cout << "Successfully connected." << std::endl;
        return 1;
    }
    else {
        std::cout << "Could not connect, please check connection settings and retry." << std::endl;
        return -1;
    }
}

void disconnect_serial() {

    if(serial->isOpen()) {
        serial->close();
        std::cout << "Connection closed." << std::endl;
    }
    else {
        std::cout << "No connection opened, nothing to close." << std::endl;
    }

}

void serial_send(std::string msg) {
    msg += "\n";
    serial->write(msg.c_str());
    serial->flush();
}

void serial_heating_hotend (int temp) {
    std::string msg = "M109 S";
    msg += to_string(temp);
    serial_send(msg);
    std::cout << "Start heating hotend to " << to_string(temp) << " degrees ..." << std::endl;
    std::string recv_msg = serialBuffer.toStdString();
    while (!serialBuffer.contains("ok")) {
        std::cout << recv_msg << std::endl;
        unsigned int temp_pos_start = recv_msg.find("T:");
        unsigned int temp_pos_end = recv_msg.find("/");
        std::cout << "Start: " << temp_pos_start << ", End: " << temp_pos_end << std::endl;

        std::string curr_temp = recv_msg.substr(temp_pos_start,temp_pos_end);
        std::cout << "Actual Hotend Temperature: " << curr_temp << "° Please wait... " << std::endl;
        QThread::msleep(2000);
    }
    std::cout << "Finished heating hotend." << std::endl;
}
