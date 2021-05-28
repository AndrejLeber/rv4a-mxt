#ifndef RT_H
#define RT_H

#include <string>
#include "bahnplanung.h"

extern STEPS recv_msgs;

// Parametrieren und Ausführen des Echtzeitthreads
int init_rt_mvs_thread(int prio, void* data, int endcmd);

#endif // RT_H
