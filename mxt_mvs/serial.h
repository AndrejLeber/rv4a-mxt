#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include <QtSerialPort/QSerialPort>

extern QSerialPort *serial;
extern QSerialPort *reviever;

extern QByteArray serialData;
extern QString serialBuffer;

void init_serial(std::string port, int baudrate);

int connect_serial();

void disconnect_serial();

void send_message(std::string msg);

void recv_messages();

#endif // SERIAL_H

