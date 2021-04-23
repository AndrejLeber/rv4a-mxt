//
// Created by Patrick on 23.04.2021.
//

#ifndef RV4A_MXT_MXT_H
#define RV4A_MXT_MXT_H

#include "strdef.h"

#define MAX_STEP_SIZE   5       // Maximale Schrittweite [mm] bei 100% Bewegung
#define NSEC_PER_SEC    1e6

int mvs(POSE start, POSE target, float speed);
int udp_send(int sock, struct sockaddr_in transmit_addr, MXTCMD mxt);
int udp_recv(int sock, fd_set rxfds, timeval timeout, MXTCMD &mxt);
int mxt_move(int sock, fd_set rxfds, timeval timeout, struct sockaddr_in transmit_addr, POSE pose_cmd, POSE &pose_recv);
int mxt_end(int sock, struct sockaddr_in transmit_addr);
int mxt_no_movement(int sock, struct sockaddr_in transmit_addr);

#endif //RV4A_MXT_MXT_H
