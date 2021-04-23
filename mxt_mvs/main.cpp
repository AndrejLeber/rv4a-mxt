/*-------------------- MVS-Funktion mit Echtzeitthread-------------------*/

/* Wichtiger Hinweis: Als Vorlage für dieses Programm wurde der Code von Jürgen Palczynski
 * aus seiner Diplomarbeit übernommen und an einigen stellen modifiziert / optimiert
 */

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

#include "strdef.h"

#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/mman.h>
#include <limits.h>
#include <chrono>

#include <vector>

// Globale Konstanten
#define maxschritt 5 // Maximale Schrittweite [mm] bei 100% Bewegung

// Globale Variablen / Variablen für die UDP Verbindung zum Roboter

static long slen, rlen, sock;
static fd_set rxfds;
static int ready;
static struct sockaddr_in transmit_addr;
static timeval timeout;

MXTCMD MXTsend;
MXTCMD MXTrecv;

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

// Funktion für die eigentliche MVS-Bewegung
int mvs(MXTCMD &MXT_move_recv, MXTCMD &MXT_Move_send, POSE &ziel, float speed);

std::vector<float> test_function(int a, int b) {
    std::vector<float> res;

    JOINT test;

    return res;
}

// RT-Threads
void *get_rt_data_thread(void* data)
{
    int counter = 0;
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);

    //data = nullptr;
    mxt_prep_data_recv(MXTsend);

    while(!endcmd) {
        // Messen der Startzeit
        auto start = std::chrono::steady_clock::now();
        // Abfrage der aktuellen Roboterposition
        slen = sendto(sock, static_cast<void*>(&MXTsend), sizeof (MXTsend), 0, reinterpret_cast<struct sockaddr*>(&transmit_addr), sizeof (transmit_addr));
        if (slen != sizeof (MXTsend)) {
            printf("Could not send package, %ld bytes sent\n", slen);
        }
        else printf("%ld bytes successfully sent\n", slen);

        FD_ZERO(&rxfds);
        FD_SET(sock, &rxfds);
        timeout.tv_sec = 10; // Einstellen der maximalen Übertragungszeit
        timeout.tv_usec = 0;

        ready = select(sock + 1, &rxfds, nullptr, nullptr, &timeout);
        if (!ready) {
            printf("Connection timeout, please try again.\n");
            return nullptr;
        }
        rlen = recvfrom(sock, static_cast<void*>(&MXTrecv), sizeof (MXTrecv), 0, nullptr, nullptr);
        if (rlen != sizeof (MXTrecv)) {
            printf("%ld bytes recieved, but not equal to size of MXTrecv.\n", rlen);
        }
        else {
            // Ausgabe der aktuellen Roboterposition auf der Konsole
            printf("x = %f ; ", static_cast<double>(MXTrecv.dat.pos.w.x));
            printf("y = %f ; ", static_cast<double>(MXTrecv.dat.pos.w.y));
            printf("z = %f \n", static_cast<double>(MXTrecv.dat.pos.w.z));
            printf("a = %f ; ", static_cast<double>(MXTrecv.dat.pos.w.a));
            printf("b = %f ; ", static_cast<double>(MXTrecv.dat.pos.w.b));
            printf("c = %f \n", static_cast<double>(MXTrecv.dat.pos.w.c));
            printf("l1 = %f ; ", static_cast<double>(MXTrecv.dat.pos.w.l1));
            printf("l2 = %f \n", static_cast<double>(MXTrecv.dat.pos.w.l2));
            printf("sflg1 = %d ; ", MXTrecv.dat.pos.sflg1);
            printf("sflg2 = %d \n",MXTrecv.dat.pos.sflg2);
            printf("Counter: %i\n", counter);
        }

        timestamp.tv_nsec += INTERVAL;
        if (timestamp.tv_nsec >= NSEC_IN_SEC) {
            timestamp.tv_nsec = 0;
            timestamp.tv_sec++;
        }

        clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME, &timestamp, nullptr);
        //std::cout << "Elapsed time: " << timestamp.tv_nsec/1e6 << "ms." << std::endl;
        counter++;
        auto end = std::chrono::steady_clock::now();

        std::cout << "Elapsed time in milliseconds: " << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()/1000.0 << std::endl;
    }

    return data;
}

void *mvs_thread(void* data)
{
    // Erstellen der notwendigen MXT-Variablen
    MXTCMD MXT_move_send;
    MXTCMD MXT_move_recv;
    mxt_prep_data_recv(MXT_move_send);

    // Abfrage der aktuellen Roboterposition
    slen = sendto(sock, static_cast<void*>(&MXT_move_send), sizeof (MXT_move_send), 0, reinterpret_cast<struct sockaddr*>(&transmit_addr), sizeof (transmit_addr));
    if (slen != sizeof (MXT_move_send)) {
        printf("Could not send package, %ld bytes sent\n", slen);
    }
    else printf("%ld bytes successfully sent\n", slen);

    FD_ZERO(&rxfds);
    FD_SET(sock, &rxfds);
    timeout.tv_sec = 10; // Einstellen der maximalen Übertragungszeit
    timeout.tv_usec = 0;

    ready = select(sock + 1, &rxfds, nullptr, nullptr, &timeout);
    if (!ready) {
        printf("Connection timeout, please try again.\n");
        return nullptr;
    }
    rlen = recvfrom(sock, static_cast<void*>(&MXT_move_recv), sizeof (MXT_move_recv), 0, nullptr, nullptr);
    if (rlen != sizeof (MXT_move_recv)) {
        printf("%ld bytes recieved, but not equal to size of MXTrecv.\n", rlen);
    }

    memcpy(&MXT_move_send.dat.pos, &MXT_move_recv.dat.pos, sizeof (POSE));

    // Aktuelle Roboterposition
    POSE start;
    start.w.x = MXT_move_recv.dat.pos.w.x;
    start.w.y = MXT_move_recv.dat.pos.w.y;
    start.w.z = MXT_move_recv.dat.pos.w.z;

    // Zielposition der MVS-Funktion
    POSE ziel;
    ziel.w.x=205.0;//start.w.x;
    ziel.w.y=200.0;//start.w.y+5;
    ziel.w.z=150.0;//start.w.z;

    POSE ziel2;
    ziel2.w.x=205.0;//start.w.x+5;
    ziel2.w.y=205.0;//ziel.w.y;
    ziel2.w.z=150.0;//ziel.w.z;

    POSE ziel3;
    ziel3.w.x=ziel2.w.x+50;
    ziel3.w.y=ziel2.w.y;
    ziel3.w.z=ziel2.w.z;

    POSE ziel4;
    ziel4.w.x=ziel3.w.x;
    ziel4.w.y=ziel3.w.y+50;
    ziel4.w.z=ziel3.w.z;


    // Bewegung über MVS-Funktion zur Zielposition (v  = 5%)
    int moveit = 0;
    moveit = mvs(MXT_move_recv, MXT_move_send, ziel, 5);

    if (moveit == -1) {
        moveit = mvs(MXT_move_recv, MXT_move_send, ziel2, 5);
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

int main()
{
    printf("Programmanfang:\n");

    // STRG+C abfangen
    signal(SIGINT, endprg);

    // Erstellen eines UDP-Sockets zum Robotercontroller (IP-Adresse: 192.168.0.1)
    transmit_addr.sin_family = AF_INET;
    transmit_addr.sin_port = htons(10000);
    transmit_addr.sin_addr.s_addr = inet_addr("192.168.0.1");

    if ((sock = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Error opening UDP socket: %ld\n", sock);
    }
    else printf("Socket created with ID: %ld\n", sock);

    // Erstellen der Echtzeit- Threads
    struct sched_param param;
    pthread_attr_t attr;
    pthread_t t1;
    int ret;

    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        printf("mlockall failed: %m\n");
        exit(-2);
    }

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
        printf("init pthread attributes failed\n");
        goto out;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, 16384);
    if (ret) {
        printf("pthread setstacksize failed\n");
        goto out;
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
        printf("pthread setschedpolicy failed\n");
        goto out;
    }
    param.sched_priority = 80;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
        printf("pthread setschedparam failed\n");
        goto out;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        printf("pthread setinheritsched failed\n");
        goto out;
    }

    /* Create a pthread t1 with specified attributes */
    ret = pthread_create(&t1, &attr, mvs_thread, nullptr);
    if (ret) {
        printf("create pthread failed\n");
        std::cout << strerror(errno) << std::endl;
        goto out;
    }

    /* Join the thread and wait until it is done */
    ret = pthread_join(t1, nullptr);
    if (ret) {
        printf("joining pthread t1 failed: %m\n");
    }
out:
    munlockall();
    while(!endcmd) {

    }
    return ret;
}

void mxt_prep_data_recv(MXTCMD &MXT_send) {
    memset(&MXT_send, 0, sizeof (MXT_send));
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

int mvs(MXTCMD &MXT_move_recv, MXTCMD &MXT_move_send, POSE &ziel, float speed) {

    // Berechnen und Ausgeben der Entfernung von aktueller Roboterposition zur Zielposition
    float xdiff, ydiff, zdiff;
    xdiff = ziel.w.x-MXT_move_recv.dat.pos.w.x;
    ydiff = ziel.w.y-MXT_move_recv.dat.pos.w.y;
    zdiff = ziel.w.z-MXT_move_recv.dat.pos.w.z;

    printf("Entfernung in x-Richtung = %f mm\n", static_cast<double>(xdiff));
    printf("Entfernung in y-Richtung = %f mm\n", static_cast<double>(ydiff));
    printf("Entfernung in z-Richtung = %f mm\n", static_cast<double>(zdiff));

    // Berechnen und Ausgeben der Anzahl notwendiger Schritte unter Berücksichtung der gewünschten Verfahrgeschwindkeit
    int xschritte = static_cast<int>(1 + fabs(xdiff) / ((speed/100)*maxschritt));
    int yschritte = static_cast<int>(1 + fabs(ydiff) / ((speed/100)*maxschritt));
    int zschritte = static_cast<int>(1 + fabs(zdiff) / ((speed/100)*maxschritt));

    printf("Berechnete Schritte in x-Richtung = %f \n", static_cast<double>(xschritte));
    printf("Berechnete Schritte in y-Richtung = %f \n", static_cast<double>(yschritte));
    printf("Berechnete Schritte in z-Richtung = %f \n", static_cast<double>(zschritte));

    // Berechnen und Ausgeben der kürzesten Wegstrecke ("Luftlinie") zwischen aktueller Roboterposition und Zielposition
    float dmin = sqrt(xdiff*xdiff+ydiff*ydiff+zdiff*zdiff);
    printf("Kürzester Weg berechnet zu: %f mm\n", static_cast<double>(dmin));

    // Berechnen und Ausgeben der notwendigen Schritte für die "3D-Bewegung"
    int dminschritte = static_cast<int>(1 + fabs(dmin) / ((speed/100)*maxschritt));
    printf("Berechnete Schritte des kürzesten Wegs = %f \n", static_cast<double>(dminschritte));

    // Berechnen und Ausgeben der einzelnen Schrittweiten in x, y und z
    float dx, dy, dz;
    dx = xdiff / dminschritte;
    dy = ydiff / dminschritte;
    dz = zdiff / dminschritte;
    printf("Berechnete Schrittweite in x-Richtung = %f mm\n", static_cast<double>(dx));
    printf("Berechnete Schrittweite in y-Richtung = %f mm\n", static_cast<double>(dy));
    printf("Berechnete Schrittweite in z-Richtung = %f mm\n", static_cast<double>(dz));

    // Konfigurieren der MXt-Bewegung
    MXT_move_send.Command = MXT_CMD_MOVE;
    MXT_move_send.SendType = MXT_TYP_POSE;

    // Starten der Bewegung
    for (int i = 0; i < dminschritte; i++) {

        struct timespec timestamp;
        clock_gettime(CLOCK_REALTIME, &timestamp);

        FD_ZERO(&rxfds);
        FD_SET(sock, &rxfds);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // Vorbereiten des (nächsten) Schritts
        MXT_move_send.dat.pos.w.x += dx;
        MXT_move_send.dat.pos.w.y += dy;
        MXT_move_send.dat.pos.w.z += dz;

        //double mvsstatus = static_cast<double>((i/dminschritte)*100.0f); // Aktueller Bewegungsstatus [%]

        //printf("Ausführen der Bewegung zu %f Prozent abgeschlossen.\n", mvsstatus);

        // Ausführen des nächsten Schritts
        slen = sendto(sock, static_cast<void*>(&MXT_move_send), sizeof(MXT_move_send), 0, reinterpret_cast<struct sockaddr*>(&transmit_addr), sizeof(transmit_addr));
        if(slen!=sizeof(MXT_move_send)) {
            printf("Could not send package, %ld bytes sent\n", slen);
        }
        else {
            printf("%ld bytes succesfully sent\n",slen);
        }
        ready = select(sock + 1, &rxfds, nullptr, nullptr, &timeout);
        if(!ready) {
            printf("Connection Timeout\n"); return 0;
        }
        rlen = recvfrom(sock, static_cast<void*>(&MXT_move_recv), sizeof(MXT_move_recv), 0, nullptr, nullptr);
        if(rlen != sizeof(MXT_move_recv)) {
            printf("%ld bytes received, not equal to sizeof(MXTrecv)\n",rlen);
            return 0;
        }

        // Anzeige der aktuellen Position (hängt immer einen Schritt hinterher!)
        printf("Aktuelle Roboterposition: x = %fmm, y = %fmm, z = %fmm\n", static_cast<double>(MXT_move_recv.dat.pos.w.x),
               static_cast<double>(MXT_move_recv.dat.pos.w.y), static_cast<double>(MXT_move_recv.dat.pos.w.z));

        timestamp.tv_nsec += INTERVAL;
        if (timestamp.tv_nsec >= NSEC_IN_SEC) {
            timestamp.tv_nsec = 0;
            timestamp.tv_sec++;
        }

        clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME, &timestamp, nullptr);
    }

    return -1;
}
