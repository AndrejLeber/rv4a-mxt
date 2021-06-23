#include "serial.h"
#include "includes.h"

QSerialPort *serial;
bool Finished_serial_cmd;

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
        std::cout << "Successfully connected to USB." << std::endl;
        serial->flush();

        serial_send("M105", 1);
        serial_receive();
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

void serial_send(const std::string &msg, bool print_msg) {
    std::string temp = msg + "\n";
    if (print_msg == 1) {
        std::cout << msg.c_str() << std::endl;
    }
    try {
        serial->write(temp.c_str());
        serial->flush();
    } catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
}

bool serial_receive() {
    serialBuffer = "";
    while (!serial->waitForBytesWritten(1)) {
        serialData = serial->readAll();
        serialBuffer += QString::fromStdString(serialData.toStdString());
        if (serialBuffer.toStdString().find("\n") != std::string::npos)
        {
           std::cout << serialBuffer.toStdString() << std::endl;
           if (serialBuffer.toStdString().find("ok") != std::string::npos) {
               return true;
           }
           serialBuffer = "";
        }
    }
    return false;
}

void serial_heating_hotend (int temp) {
    std::string msg = "M109 S";
    msg += std::to_string(temp);
    serial_send(msg, 1);
    std::cout << "Start heating hotend to " << std::to_string(temp) << " degrees ..." << std::endl;
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
