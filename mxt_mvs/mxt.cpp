/*-------------------- Definition aller notwendigen MXT- Funktionen -------------------*/

#include "includes.h"

// Globale Variablen / Variablen für die UDP Verbindung zum Roboter
#define maxschritt 5 // Maximale Schrittweite [mm] bei 100% Bewegung
static long slen, rlen, sock;
static fd_set rxfds;
static int ready;
static struct sockaddr_in transmit_addr;
static timeval timeout;
int endcmd;

STEPS recv_msgs;

MXTCMD MXTsend;
MXTCMD MXTrecv;

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
MXTCMD mxt_prep_move_pos(POSE pos) {
    MXTCMD res;
    memset(&res, 0, sizeof (res));

    res.Command = MXT_CMD_MOVE;
    res.SendType = MXT_TYP_POSE;
    res.RecvType = MXT_TYP_POSE;
    res.SendIOType = MXT_IO_NULL;
    res.RecvIOType = MXT_IO_NULL;
    res.RecvType1 = MXT_TYP_POSE;
    res.RecvType2 = MXT_TYP_JOINT;
    res.RecvType3 = MXT_TYP_PULSE;

    res.dat.pos = pos;

    return res;
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

void* mxt_mvs_pos(void* data)
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
    start = mxt_recv.dat.pos;

    // Zielposition der MVS-Funktion
    POSE* ziel;
    ziel = (POSE*)data;

    // Bewegung über MVS-Funktion zur Zielposition
    int moveit = 0;
    moveit = move_sinoide(mxt_send, mxt_recv, start, ziel, 20.0f, 400.0f);
    if (moveit) {
        return nullptr;
    }
}

int mvs(MXTCMD &mxt_send, MXTCMD &mxt_recv, POSE start, POSE* ziel, float speed) {


    // Berechnen und Ausgeben der Entfernung von aktueller Roboterposition zur Zielposition
    float xdiff, ydiff, zdiff;
    xdiff = ziel->w.x-start.w.x;
    ydiff = ziel->w.y-start.w.y;
    zdiff = ziel->w.z-start.w.z;

    //TEST
    printf("Startposition: x=%f, y=%f, z=%f\n", static_cast<double>(start.w.x),
           static_cast<double>(start.w.y),
           static_cast<double>(start.w.z));

    printf("Startwinkel: a=%f, b=%f, c=%f\n", static_cast<double>(start.w.a),
           static_cast<double>(start.w.b),
           static_cast<double>(start.w.c));

    printf("Zielposition: x=%f, y=%f, z=%f\n", static_cast<double>(ziel->w.x),
           static_cast<double>(ziel->w.y),
           static_cast<double>(ziel->w.z));

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
    mxt_send = mxt_prep_move_pos(start);

    //printf("%i %i", mxt_send.Command, mxt_send.SendType);

    // Zeitmessung
    struct timespec ts0, ts1, ts2;
    clock_gettime(CLOCK_MONOTONIC,&ts0);

    // Vorbereiten zum Senden
    FD_ZERO(&rxfds);
    FD_SET(sock, &rxfds);
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // Starten der Fahrbewegung
    for (int i = 0; i < dminschritte; i++) {

        printf("Schleifendurchlauf: %i\n", i);

        // Vorbereiten des (nächsten) Schritts
        mxt_send.dat.pos.w.x += dx;
        mxt_send.dat.pos.w.y += dy;
        mxt_send.dat.pos.w.z += dz;

        printf("Gesendete Position: x=%f, y=%f, z=%f\n", static_cast<double>(mxt_send.dat.pos.w.x),
               static_cast<double>(mxt_send.dat.pos.w.y),
               static_cast<double>(mxt_send.dat.pos.w.z));

        //Ausführen des nächsten Schritts
        slen = sendto(sock, static_cast<void*>(&mxt_send), sizeof(mxt_send), 0, reinterpret_cast<struct sockaddr*>(&transmit_addr), sizeof(transmit_addr));
        if(slen!=sizeof(mxt_send)) {
            printf("Could not send package, %ld bytes sent\n", slen);
        }
        else {
            printf("%ld bytes succesfully sent\n",slen);
            clock_gettime(CLOCK_MONOTONIC, &ts1);
        }
        ready = select(sock + 1 , &rxfds, nullptr, nullptr, &timeout);
        if(!ready) {
            printf("Connection Timeout\n");
            return 0;
        }
        rlen = recvfrom(sock, static_cast<void*>(&mxt_recv), sizeof (mxt_recv), 0, nullptr, nullptr);
        if (rlen != sizeof (mxt_recv)) {
            printf("%ld bytes recieved, but not equal to size of MXTrecv.\n", rlen);
        }

        clock_gettime(CLOCK_MONOTONIC, &ts2);

        printf("Zeitstempel (vor send): %fms\n", 1000.0*ts1.tv_sec-1000.0*ts0.tv_sec+1e-6*ts1.tv_nsec-1e-6*ts0.tv_nsec);
        printf("Zeitstempel (nach recv): %fms\n", 1000.0*ts2.tv_sec-1000.0*ts0.tv_sec+1e-6*ts2.tv_nsec-1e-6*ts0.tv_nsec);
        printf("Delta-Zeitstempel: %fms\n", 1000.0*ts2.tv_sec-1000.0*ts1.tv_sec+1e-6*ts2.tv_nsec-1e-6*ts1.tv_nsec);

    }

    return -1;
}

int move_sinoide(MXTCMD send_sinoide, MXTCMD recv_sinoide, POSE start, POSE* ziel, float v, float a) {
    STEPS path = Sinoide(start, *ziel, v, a);

    for (unsigned int i = 0; i < path.x.size(); i++) {
        std::cout << "x" << i << " = " << path.x.at(i) << "mm,  y" << i << " = " << path.y.at(i)
                  << "mm,  z" << i << " = " << path.z.at(i) << "mm" << std::endl;
    }

    // Konfigurieren der MXt-Bewegung
    send_sinoide = mxt_prep_move_pos(start);

    // Ausführen der geplanten Bahn
    for (unsigned int i = 0; i < path.x.size(); i++) {

        // Vorbereiten zum Senden
        FD_ZERO(&rxfds);
        FD_SET(sock, &rxfds);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // Vorbereiten des (nächsten) Schritts
        send_sinoide.dat.pos.w.x = path.x.at(i);
        send_sinoide.dat.pos.w.y = path.y.at(i);
        send_sinoide.dat.pos.w.z = path.z.at(i);

        recv_msgs.t = path.t;

        printf("Gesendete Position: x=%f, y=%f, z=%f\n", static_cast<double>(send_sinoide.dat.pos.w.x),
               static_cast<double>(send_sinoide.dat.pos.w.y),
               static_cast<double>(send_sinoide.dat.pos.w.z));

        //Ausführen des nächsten Schritts
        slen = sendto(sock, static_cast<void*>(&send_sinoide), sizeof(send_sinoide), 0, reinterpret_cast<struct sockaddr*>(&transmit_addr), sizeof(transmit_addr));
        if(slen!=sizeof(send_sinoide)) {
            printf("Could not send package, %ld bytes sent\n", slen);
        }
        else {
            printf("%ld bytes succesfully sent\n",slen);
        }
        ready = select(sock + 1 , &rxfds, nullptr, nullptr, &timeout);
        if(!ready) {
            printf("Connection Timeout\n");
            return 0;
        }
        rlen = recvfrom(sock, static_cast<void*>(&recv_sinoide), sizeof (recv_sinoide), 0, nullptr, nullptr);
        if (rlen != sizeof (recv_sinoide)) {
            printf("%ld bytes recieved, but not equal to size of MXTrecv.\n", rlen);
        }

        printf("Empfangene Position: x=%f, y=%f, z=%f\n", static_cast<double>(recv_sinoide.dat.pos.w.x),
               static_cast<double>(recv_sinoide.dat.pos.w.y),
               static_cast<double>(recv_sinoide.dat.pos.w.z));

        recv_msgs.x.push_back(recv_sinoide.dat.pos.w.x);
        recv_msgs.y.push_back(recv_sinoide.dat.pos.w.y);
        recv_msgs.z.push_back(recv_sinoide.dat.pos.w.z);
    }

    return 0;
}

