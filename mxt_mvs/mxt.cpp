/*-------------------- Definition aller notwendigen MXT- Funktionen -------------------*/

#include "includes.h"

// Globale Variablen / Variablen für die UDP Verbindung zum Roboter
#define maxschritt 5 // Maximale Schrittweite [mm] bei 100% Bewegung
static long slen, rlen, sock;
static fd_set rxfds;
static int ready;
static struct sockaddr_in transmit_addr;
static timeval timeout;

MXTCMD MXTsend;
MXTCMD MXTrecv;

// Programmabbruch mit STRG+C
int endcmd = 0;
void endprg(int dummy) {
    dummy += dummy;
    endcmd = 1;
}

// Erstellen und Aufrufen des UDP- Sockets
void mxt_init() {

    // Erstellen eines UDP-Sockets zum Robotercontroller (IP-Adresse: 192.168.0.1)
    transmit_addr.sin_family = AF_INET;
    transmit_addr.sin_port = htons(10000);
    transmit_addr.sin_addr.s_addr = inet_addr("192.168.0.1");

    if ((sock = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Error opening UDP socket: %ld\n", sock);
    }
    else printf("Socket created with ID: %ld\n", sock);

}

// Vorbereiten des MXT- Befehls zum Empfangen der aktuellen Roboterposition (Ohne Bewegung)
void mxt_prep_recv_pos(MXTCMD &cmd) {
    memset(&cmd, 0, sizeof (cmd));

    cmd.Command = MXT_CMD_NULL;
    cmd.SendType = MXT_TYP_NULL;
    cmd.RecvType = MXT_TYP_POSE;
    cmd.SendIOType = MXT_IO_NULL;
    cmd.RecvIOType = MXT_IO_NULL;
    cmd.RecvType1 = MXT_TYP_POSE;
    cmd.RecvType2 = MXT_TYP_JOINT;
    cmd.RecvType3 = MXT_TYP_PULSE;
}

// Vorbereiten des MXT- Befehls zum Senden und Ausführen neuer Positionsdaten (Bewegung)
void mxt_prep_move_pos(MXTCMD &cmd, POSE pos) {
    memset(&cmd, 0, sizeof (cmd));

    cmd.Command = MXT_CMD_MOVE;
    cmd.SendType = MXT_TYP_POSE;
    cmd.RecvType = MXT_TYP_POSE;
    cmd.SendIOType = MXT_IO_NULL;
    cmd.RecvIOType = MXT_IO_NULL;
    cmd.RecvType1 = MXT_TYP_POSE;
    cmd.RecvType2 = MXT_TYP_JOINT;
    cmd.RecvType3 = MXT_TYP_PULSE;

    cmd.dat.pos.w.x = pos.w.x;
    cmd.dat.pos.w.y = pos.w.y;
    cmd.dat.pos.w.z = pos.w.z;
}

// Vorbereiten des MXT- Befehls zum Beenden von MXT
void mxt_prep_end(MXTCMD &cmd) {
    memset(&cmd, 0, sizeof (cmd));

    cmd.Command = MXT_CMD_END;
    cmd.SendType = MXT_TYP_NULL;
    cmd.RecvType = MXT_TYP_NULL;
    cmd.SendIOType = MXT_IO_NULL;
    cmd.RecvIOType = MXT_IO_NULL;
    cmd.RecvType1 = MXT_TYP_NULL;
    cmd.RecvType2 = MXT_TYP_NULL;
    cmd.RecvType3 = MXT_TYP_NULL;
}

// RT-Threads
void *mxt_recv_pos(void* data)
{
    int counter = 0;

    mxt_prep_recv_pos(MXTsend);

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

        counter++;
        auto end = std::chrono::steady_clock::now();

        std::cout << "Elapsed time in milliseconds: " << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()/1000.0 << std::endl;
    }

    return data;
}

void *mxt_mvs_pos(void* data)
{
    // Erstellen der notwendigen MXT-Variablen
    MXTCMD mxt_send;
    MXTCMD mxt_recv;
    mxt_prep_recv_pos(mxt_send);

    // Abfrage der aktuellen Roboterposition
    slen = sendto(sock, static_cast<void*>(&mxt_send), sizeof (mxt_send), 0, reinterpret_cast<struct sockaddr*>(&transmit_addr), sizeof (transmit_addr));
    if (slen != sizeof (mxt_send)) {
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
    rlen = recvfrom(sock, static_cast<void*>(&mxt_recv), sizeof (mxt_recv), 0, nullptr, nullptr);
    if (rlen != sizeof (mxt_recv)) {
        printf("%ld bytes recieved, but not equal to size of MXTrecv.\n", rlen);
    }

    memcpy(&mxt_send.dat.pos, &mxt_recv.dat.pos, sizeof (POSE));

    // Aktuelle Roboterposition
    POSE start;
    start.w.x = mxt_recv.dat.pos.w.x;
    start.w.y = mxt_recv.dat.pos.w.y;
    start.w.z = mxt_recv.dat.pos.w.z;

    // Zielposition der MVS-Funktion
    POSE *ziel = (POSE*)(data);

    // Bewegung über MVS-Funktion zur Zielposition (v  = 5%)
    int moveit = 0;
    moveit = mvs(mxt_send, mxt_recv, start, ziel, 1);
    if (moveit) {}


    return nullptr;
}

int mvs(MXTCMD &mxt_send, MXTCMD &mxt_recv, POSE start, POSE* ziel, float speed) {

    // Berechnen und Ausgeben der Entfernung von aktueller Roboterposition zur Zielposition
    float xdiff, ydiff, zdiff;
    xdiff = ziel->w.x-start.w.x;
    ydiff = ziel->w.y-start.w.y;
    zdiff = ziel->w.z-start.w.z;

    printf("Entfernung in x-Richtung = %f mm\n", static_cast<double>(xdiff));
    printf("Entfernung in y-Richtung = %f mm\n", static_cast<double>(ydiff));
    printf("Entfernung in z-Richtung = %f mm\n", static_cast<double>(zdiff));

    // Berechnen und Ausgeben der Anzahl notwendiger Schritte unter Berücksichtung der gewünschten Verfahrgeschwindkeit
    int xschritte = static_cast<int>(fabs(xdiff) / ((speed/100)*maxschritt));
    int yschritte = static_cast<int>(fabs(ydiff) / ((speed/100)*maxschritt));
    int zschritte = static_cast<int>(fabs(zdiff) / ((speed/100)*maxschritt));

    printf("Berechnete Schritte in x-Richtung = %f \n", static_cast<double>(xschritte));
    printf("Berechnete Schritte in y-Richtung = %f \n", static_cast<double>(yschritte));
    printf("Berechnete Schritte in z-Richtung = %f \n", static_cast<double>(zschritte));

    // Berechnen und Ausgeben der kürzesten Wegstrecke ("Luftlinie") zwischen aktueller Roboterposition und Zielposition
    float dmin = sqrt(xdiff*xdiff+ydiff*ydiff+zdiff*zdiff);
    printf("Kürzester Weg berechnet zu: %f mm\n", static_cast<double>(dmin));

    // Berechnen und Ausgeben der notwendigen Schritte für die "3D-Bewegung"
    int dminschritte = static_cast<int>(fabs(dmin) / ((speed/100)*maxschritt));
    if (dminschritte == 0) {
        dminschritte = 1;
    }
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
    mxt_prep_move_pos(mxt_send, start);

    // Zeitmessung
    struct timespec ts0, ts1, ts2;
    clock_gettime(CLOCK_MONOTONIC,&ts0);

    // Starten der Fahrbewegung
    for (int i = 0; i < dminschritte; i++) {

        FD_ZERO(&rxfds);
        FD_SET(sock, &rxfds);
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        // Vorbereiten des (nächsten) Schritts
        mxt_send.dat.pos.w.x += dx;
        mxt_send.dat.pos.w.y += dy;
        mxt_send.dat.pos.w.z += dz;

        //Ausführen des nächsten Schritts
        slen = sendto(sock, static_cast<void*>(&mxt_send), sizeof(mxt_send), 0, reinterpret_cast<struct sockaddr*>(&transmit_addr), sizeof(transmit_addr));
        if(slen!=sizeof(mxt_send)) {
            printf("Could not send package, %ld bytes sent\n", slen);
        }
        else {
            printf("%ld bytes succesfully sent\n",slen);
            clock_gettime(CLOCK_MONOTONIC, &ts1);
        }
        ready = select(sock + 1, &rxfds, nullptr, nullptr, &timeout);
        if(!ready) {
            printf("Connection Timeout\n"); return 0;
        }
        rlen = recvfrom(sock, static_cast<void*>(&mxt_recv), sizeof (mxt_recv), 0, nullptr, nullptr);
        if (rlen != sizeof (mxt_recv)) {
            printf("%ld bytes recieved, but not equal to size of MXTrecv.\n", rlen);
        }

        clock_gettime(CLOCK_MONOTONIC, &ts2);

        printf("Zeitstempel (vor send): %fms\n", 1000.0*ts1.tv_sec-1000.0*ts0.tv_sec+1e-6*ts1.tv_nsec-1e-6*ts0.tv_nsec);
        printf("Zeitstempel (nach recv): %fms\n", 1000.0*ts2.tv_sec-1000.0*ts0.tv_sec+1e-6*ts2.tv_nsec-1e-6*ts0.tv_nsec);
        printf("Delta-Zeitstempel: %fms\n", 1000.0*ts2.tv_sec-1000.0*ts1.tv_sec+1e-6*ts2.tv_nsec-1e-6*ts1.tv_nsec);
        printf("Gesendete Position: x=%f, y=%f, z=%f\n", static_cast<double>(mxt_send.dat.pos.w.x),
                                                         static_cast<double>(mxt_send.dat.pos.w.y),
                                                         static_cast<double>(mxt_send.dat.pos.w.z));
    }

    return -1;
}

