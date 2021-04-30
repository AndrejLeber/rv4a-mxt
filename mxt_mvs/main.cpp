/*-------------------- MVS-Funktion mit Echtzeitthread-------------------*/

/* Wichtiger Hinweis: Als Vorlage für dieses Programm wurde der Code von Jürgen Palczynski
 * aus seiner Diplomarbeit übernommen und an einigen stellen modifiziert / optimiert
 */

#include <cmath>
#include <iostream>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sched.h>
#include <cerrno>
#include <ctime>
#include <chrono>
#include <vector>
#include <cstring>

#include "strdef.h"
#include "mxt.h"
#include "r3_protocol.h"

#include "Bahnplanung.h"


// Globale Variablen / Variablen für die UDP Verbindung zum Roboter

static long slen, rlen;
int sock;
static fd_set rxfds;
static int ready;
static struct sockaddr_in transmit_addr;
static timeval timeout;

MXTCMD mxt_send;
MXTCMD mxt_recv;

// Sleeping variables
const int NSEC_IN_SEC = 1000000001;
const int SLEEPTIME_MS = 10;
const int INTERVAL = SLEEPTIME_MS * 1000 * 1000;

// Programmabbruch mit STRG+C
static int endcmd;

static void endprg(int dummy) {
    dummy += dummy;
    endcmd = 1;
}

// Funktion zur Konfiguration des MXT Befehls zur Abfrage der Positionsdaten ohne Bewegung
void mxt_prep_data_recv(MXTCMD &MXT_send);

std::vector<float> test_function(int a, int b) {
    std::vector<float> res;

    JOINT test;

    return res;
}

// RT-Threads
void *get_rt_data_thread(void *data) {
    int counter = 0;
    struct timespec timestamp{};
    clock_gettime(CLOCK_REALTIME, &timestamp);

    //data = nullptr;
    mxt_prep_data_recv(mxt_send);

    while (!endcmd) {
        // Messen der Startzeit
        auto start = std::chrono::steady_clock::now();
        // Abfrage der aktuellen Roboterposition
        slen = sendto(sock, static_cast<void *>(&mxt_send), sizeof(mxt_send), 0,
                      reinterpret_cast<struct sockaddr *>(&transmit_addr), sizeof(transmit_addr));
        if (slen != sizeof(mxt_send)) {
            std::cout << "Could not send package, " << slen << " bytes sent." << std::endl;
        }
        else{
            std::cout << slen << " bytes successfully sent" << std::endl;
        }

        FD_ZERO(&rxfds);
        FD_SET(sock, &rxfds);
        timeout.tv_sec = 10; // Einstellen der maximalen Übertragungszeit
        timeout.tv_usec = 0;

        ready = select(sock + 1, &rxfds, nullptr, nullptr, &timeout);
        if (!ready) {
            std::cout << "Connection Timeout" << std::endl;
            return nullptr;
        }
        rlen = recvfrom(sock, static_cast<void *>(&mxt_recv), sizeof(mxt_recv), 0, nullptr, nullptr);
        if (rlen != sizeof(mxt_recv)) {
            std::cout << rlen << " bytes received, not equal to " << sizeof(mxt_recv) << std::endl;
        }
        else{
            // Ausgabe der aktuellen Roboterposition auf der Konsole
            std::cout << "x = " << mxt_recv.dat.pos.w.x;
            std::cout << "y = " << mxt_recv.dat.pos.w.y;
            std::cout << "z = " << mxt_recv.dat.pos.w.z;
            std::cout << "a = " << mxt_recv.dat.pos.w.a;
            std::cout << "b = " << mxt_recv.dat.pos.w.b;
            std::cout << "c = " << mxt_recv.dat.pos.w.c;
            std::cout << "l1 = " << mxt_recv.dat.pos.w.l1;
            std::cout << "l2 = " << mxt_recv.dat.pos.w.l2 << std::endl;
            std::cout << "sflg1 = " << mxt_recv.dat.pos.sflg1;
            std::cout << "sflg2 = " << mxt_recv.dat.pos.sflg2;
            std::cout << "Counter = " << counter;
        }

        timestamp.tv_nsec += INTERVAL;
        if (timestamp.tv_nsec >= NSEC_IN_SEC) {
            timestamp.tv_nsec = 0;
            timestamp.tv_sec++;
        }

        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &timestamp, nullptr);
        //std::cout << "Elapsed time: " << timestamp.tv_nsec/1e6 << "ms." << std::endl;
        counter++;
        auto end = std::chrono::steady_clock::now();

        std::cout << "Elapsed time in milliseconds: "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0 << std::endl;
    }

    return data;
}

void *mvs_thread(void *data) {
    // Erstellen der notwendigen MXT-Variablen
    MXTCMD MXT_move_send;
    MXTCMD MXT_move_recv;
    mxt_prep_data_recv(MXT_move_send);

    // Abfrage der aktuellen Roboterposition
    slen = sendto(sock, static_cast<void *>(&MXT_move_send), sizeof(MXT_move_send), 0,
                  reinterpret_cast<struct sockaddr *>(&transmit_addr), sizeof(transmit_addr));
    if (slen != sizeof(MXT_move_send)) {
        std::cout << "Could not send package, " << slen << " bytes sent." << std::endl;
    }
    else {
        std::cout << slen << " bytes successfully sent" << std::endl;
    }

    FD_ZERO(&rxfds);
    FD_SET(sock, &rxfds);
    timeout.tv_sec = 10; // Einstellen der maximalen Übertragungszeit
    timeout.tv_usec = 0;

    ready = select(sock + 1, &rxfds, nullptr, nullptr, &timeout);
    if (!ready) {
        std::cout << "Connection timeout, please try again." << std::endl;
        return nullptr;
    }
    rlen = recvfrom(sock, static_cast<void *>(&MXT_move_recv), sizeof(MXT_move_recv), 0, nullptr, nullptr);
    if (rlen != sizeof(MXT_move_recv)) {
        std::cout << rlen << " bytes received, not equal to " << sizeof(mxt_recv) << std::endl;
    }

    memcpy(&MXT_move_send.dat.pos, &MXT_move_recv.dat.pos, sizeof(POSE));

    // Aktuelle Roboterposition
    POSE start;
    start.w.x = MXT_move_recv.dat.pos.w.x;
    start.w.y = MXT_move_recv.dat.pos.w.y;
    start.w.z = MXT_move_recv.dat.pos.w.z;

    // Zielposition der MVS-Funktion
    POSE ziel;
    ziel.w.x = 205.0;//start.w.x;
    ziel.w.y = 200.0;//start.w.y+5;
    ziel.w.z = 150.0;//start.w.z;

    POSE ziel2;
    ziel2.w.x = 205.0;//start.w.x+5;
    ziel2.w.y = 205.0;//ziel.w.y;
    ziel2.w.z = 150.0;//ziel.w.z;

    POSE ziel3;
    ziel3.w.x = ziel2.w.x + 50;
    ziel3.w.y = ziel2.w.y;
    ziel3.w.z = ziel2.w.z;

    POSE ziel4;
    ziel4.w.x = ziel3.w.x;
    ziel4.w.y = ziel3.w.y + 50;
    ziel4.w.z = ziel3.w.z;


    // Bewegung über MVS-Funktion zur Zielposition (v  = 5%)
    int moveit = 0;
    moveit = mvs(start, ziel, 5);

    if (moveit == -1) {
        moveit = mvs(ziel, ziel2, 5);
    }
//    if (moveit == -1) {
//        moveit = mvs(MXT_move_recv, MXT_move_send, ziel3, 5);
//    }
//    if (moveit == -1) {
//        moveit = mvs(MXT_move_recv, MXT_move_send, ziel4, 5);
//    }

    // Bewegung über MVS-Funktion zurück zur Startposition (v = 5%)
//    if (moveit == -1) {
//        moveit = mvs(start, 5);
//    }

    return data;
}

int main() {
    int tcp_sock;
    int ret;


    //Johannes Zeugs Anfang
    POSE start;
    start.w.x = 1;
    start.w.y = 0;
    start.w.z = 5;

    POSE target;
    target.w.x = -1;
    target.w.y = 0;
    target.w.z = -5;

    float vbahn=25;

    STEPS stuetz; //diese Struktur wird über die FUnktion Sinoide ausgefüllt

    stuetz=Sinoide(start, target,vbahn);

    //Johannes Zeugs Ende


    std::cout << "Program start" << std::endl;

    // Establish TCP/IP connection with robot for R3 commands
    tcp_sock = open_connection("127.0.0.1", 10001);
    if(tcp_sock == -1){
        return -1;
    }

    // Execute the start-up routine
    ret = start_robot(tcp_sock);
    if(ret == -1){
        std::cout << "Failed to start the robot." << std::endl;
        return -1;
    }

    // STRG+C abfangen
    signal(SIGINT, endprg);

    // Erstellen eines UDP-Sockets zum Robotercontroller (IP-Adresse: 192.168.0.1)
    transmit_addr.sin_family = AF_INET;
    transmit_addr.sin_port = htons(10000);
    transmit_addr.sin_addr.s_addr = inet_addr("192.168.0.1");

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        std::cout << "Error opening UDP socket: " << sock << std::endl;
    } else
        std::cout << "Socket created with ID: " << sock << std::endl;

    // Create Realtime-Threads
    struct sched_param param{};
    pthread_attr_t attr;
    pthread_t t1;

    /* Lock memory */
    //if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
    {
        std::cout << "mlockall failed" << std::endl;
        //exit(-2);
    }

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
        std::cout << "init pthread attributes failed" << std::endl;
        goto out;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, 16384);
    if (ret) {
        std::cout << "pthread setstacksize failed" << std::endl;
        goto out;
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
        std::cout << "pthread setschedpolicy failed" << std::endl;
        goto out;
    }
    param.sched_priority = 80;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
        std::cout << "pthread setschedparam failed" << std::endl;
        goto out;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        std::cout << "pthread setinheritsched failed" << std::endl;
        goto out;
    }

    /* Create a pthread t1 with specified attributes */
    ret = pthread_create(&t1, &attr, mvs_thread, nullptr);
    if (ret) {
        std::cout << "create pthread failed" << std::endl;
        std::cout << strerror(errno) << std::endl;
        goto out;
    }

    /* Join the thread and wait until it is done */
    ret = pthread_join(t1, nullptr);
    if (ret) {
        std::cout << "joining pthread t1 failed" << std::endl;
    }
    out:
    // munlockall();
    while (!endcmd) {

    }
    return ret;
}

void mxt_prep_data_recv(MXTCMD &MXT_send) {
    memset(&MXT_send, 0, sizeof(MXT_send));
    // Erstellung der Übertragungsdaten
    MXT_send.Command = MXT_CMD_NULL;
    MXT_send.SendType = MXT_TYP_NULL;
    MXT_send.RecvType = MXT_TYP_POSE;
    MXT_send.SendIOType = MXT_IO_NULL;
    MXT_send.RecvIOType = MXT_IO_NULL;
    MXT_send.RecvType1 = MXT_TYP_POSE;
    MXT_send.RecvType2 = MXT_TYP_JOINT;
    MXT_send.RecvType3 = MXT_TYP_PULSE;
}

