//
// Created by Patrick on 23.04.2021.
//

#include <chrono>
#include <climits>
#include <sys/mman.h>
#include <cassert>
#include <ctime>
#include <cerrno>
#include <sched.h>
#include <pthread.h>
#include "strdef.h"
#include <cstring>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdio>
#include <cmath>
#include "mxt.h"
#include <iostream>


int udp_send(int sock, struct sockaddr_in transmit_addr, MXTCMD mxt) {
    ssize_t slen;

    slen = sendto(sock, static_cast<void *>(&mxt), sizeof(mxt), 0,
                  reinterpret_cast<struct sockaddr *>(&transmit_addr), sizeof(transmit_addr));
    if (slen != sizeof(mxt)) {
        std::cout << "Could not send package, " << slen << " bytes sent" << std::endl;
        return -1;
    }
    std::cout << slen << " bytes successfully sent" << std::endl;
    return 0;
}

int udp_recv(int sock, fd_set rxfds, timeval timeout, MXTCMD &mxt) {
    int ready;
    ssize_t rlen;

    ready = select(sock + 1, &rxfds, nullptr, nullptr, &timeout);
    if (!ready) {
        std::cout << "Connection Timeout" << std::endl;
        return -1;
    }
    rlen = recvfrom(sock, static_cast<void *>(&mxt), sizeof(mxt), 0, nullptr, nullptr);
    if (rlen != sizeof(mxt)) {
        std::cout << rlen << " bytes received, not equal to " << sizeof(mxt) << std::endl;
        return -1;
    }
    return 0;
}

int mxt_move(int sock, fd_set rxfds, timeval timeout, struct sockaddr_in transmit_addr, POSE pose_cmd, POSE &pose_recv){
    MXTCMD mxt;
    int ret;

    // Fill MXT command structure with data
    mxt.Command = MXT_CMD_MOVE;
    mxt.SendType = MXT_TYP_POSE;
    mxt.dat.pos = pose_cmd;

    // Send data
    ret = udp_send(sock, transmit_addr, mxt);
    if(ret == -1){
        return ret;
    }

    // Receive response
    ret = udp_recv(sock, rxfds, timeout, mxt);
    if(ret == -1){
        return ret;
    }

    // Unpack
    pose_recv = mxt.dat.pos;
    return 0;
}

int mxt_end(int sock, struct sockaddr_in transmit_addr) {
    MXTCMD mxt;
    int ret;

    // Fill MXT command structure with data
    mxt.Command = MXT_CMD_END;

    // Send data
    ret = udp_send(sock, transmit_addr, mxt);
    if(ret == -1){
        return ret;
    }

    return 0;
}

int mxt_no_movement(int sock, struct sockaddr_in transmit_addr){
    MXTCMD  mxt;
    int ret;

    // Fill MXT command structure with data
    mxt.Command = MXT_CMD_NULL;

    // Send data
    ret = udp_send(sock, transmit_addr, mxt);
    if(ret == -1){
        return ret;
    }

    return 0;
}


int mvs(POSE start, POSE target, float speed) {
    POSE pose_recv;
    float xdiff, ydiff, zdiff;
    float dx, dy, dz;
    int ret;
    int sock;
    fd_set rxfds;
    timeval timeout{.tv_sec = 1, .tv_usec=0};
    struct sockaddr_in transmit_addr{};

    // Calculate distance to move in each direction
    xdiff = target.w.x - start.w.x;
    ydiff = target.w.y - start.w.y;
    zdiff = target.w.z - start.w.z;

    std::cout << "Distance in x-direction" << xdiff << " mm" << std::endl;
    std::cout << "Distance in y-direction" << ydiff << " mm" << std::endl;
    std::cout << "Distance in z-direction" << zdiff << " mm" << std::endl;

    // Berechnen und Ausgeben der Anzahl notwendiger Schritte unter Berücksichtung der gewünschten Verfahrgeschwindkeit
    int x_steps = static_cast<int>(1 + std::fabs(xdiff) / ((speed / 100) * MAX_STEP_SIZE));
    int y_steps = static_cast<int>(1 + std::fabs(ydiff) / ((speed / 100) * MAX_STEP_SIZE));
    int z_steps = static_cast<int>(1 + std::fabs(zdiff) / ((speed / 100) * MAX_STEP_SIZE));

    std::cout << "Calculated steps in x-direction: " << x_steps << std::endl;
    std::cout << "Calculated steps in y-direction: " << y_steps << std::endl;
    std::cout << "Calculated steps in z-direction: " << z_steps << std::endl;

    // Berechnen und Ausgeben der kürzesten Wegstrecke ("Luftlinie") zwischen aktueller Roboterposition und Zielposition
    float dmin = std::sqrt(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
    std::cout << "Shortest way: " << dmin << " mm" << std::endl;

    // Calculation of the necessary steps for the direct movement
    int dmin_steps = static_cast<int>(1 + std::fabs(dmin) / ((speed / 100) * MAX_STEP_SIZE));
    std::cout << "Steps for shortest way: " << dmin_steps << std::endl;

    // Calculation of the individual step sizes
    dx = xdiff / dmin_steps;
    dy = ydiff / dmin_steps;
    dz = zdiff / dmin_steps;

    std::cout << "Calculated step size in x-direction: " << dx << " mm" << std::endl;
    std::cout << "Calculated step size in y-direction: " << dy << " mm" << std::endl;
    std::cout << "Calculated step size in z-direction: " << dz << " mm" << std::endl;

    // Starting the movement
    for (int i = 0; i < dmin_steps; i++) {
        FD_ZERO(&rxfds);
        FD_SET(sock, &rxfds);

        // Vorbereiten des (nächsten) Schritts
        start.w.x += dx;
        start.w.y += dy;
        start.w.z += dz;

        ret = mxt_move(sock, rxfds, timeout, transmit_addr, start, pose_recv);
        if(ret == -1){
            return 0;
        }

        // Anzeige der aktuellen Position (hängt immer einen Schritt hinterher!)
        std::cout << "Current robot position: ";
        std::cout << "x = " << pose_recv.w.x << " mm,";
        std::cout << "y = " << pose_recv.w.y << " mm,";
        std::cout << "z = " << pose_recv.w.z << " mm" << std::endl;
    }

    return -1;
}