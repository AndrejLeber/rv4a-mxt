#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include <QtSerialPort/QSerialPort>

extern QSerialPort *serial;
extern QString serialBuffer;
extern QByteArray serialData;
extern bool Finished_serial_cmd;

void init_serial(std::string port, int baudrate);

int connect_serial();

void disconnect_serial();

void serial_send(const std::string &msg);

bool serial_receive();

void serial_heating_hotend (int temp);

#endif // SERIAL_H

