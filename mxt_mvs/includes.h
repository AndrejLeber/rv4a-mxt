/*-------------------- Include aller notwendigen Libraries-------------------*/


#ifndef INCLUDES_H
#define INCLUDES_H

// Globale Headers
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/mman.h>
#include <limits.h>
#include <chrono>
#include <vector>
#include "unistd.h"

// Eigene Headers
#include "strdef.h"
#include "mxt.h"
#include "rt.h"
#include "bahnplanung.h"
#include "r3_protocol.h"

#endif // INCLUDES_H
