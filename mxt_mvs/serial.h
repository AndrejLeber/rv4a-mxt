#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include <QtSerialPort/QSerialPort>

extern QSerialPort *serial;

void init_serial(std::string port, int baudrate);

int connect_serial();

void disconnect_serial();

void serial_send(std::string msg);

void serial_heating_hotend (int temp);

#endif // SERIAL_H

