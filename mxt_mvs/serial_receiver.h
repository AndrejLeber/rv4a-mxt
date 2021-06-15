#ifndef SERIAL_RECEIVER_H
#define SERIAL_RECEIVER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class Serial_receiver : public QObject
{
    Q_OBJECT
public:
    Serial_receiver();
    void connect_readyread();
    void disconnect_readyread();

public slots:
    void serial_recv();
};

#endif // SERIAL_RECEIVER_H
