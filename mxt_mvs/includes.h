/*-------------------- Include aller notwendigen Libraries-------------------*/


#ifndef INCLUDES_H
#define INCLUDES_H

// Globale Headers
#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <cstring>
#include <cmath>
#include <pthread.h>
#include <sched.h>
#include <cerrno>
#include <ctime>
#include <cassert>
#include <sys/mman.h>
#include <climits>
#include <chrono>
#include <vector>
#include "unistd.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QThread>

// Eigene Headers
#include "strdef.h"
#include "mxt.h"
#include "rt.h"
#include "bahnplanung.h"
#include "r3_protocol.h"
#include "matplotlibcpp.h"
#include "GCode.h"
#include "serial.h"
#include <serial_receiver.h>

#endif // INCLUDES_H
