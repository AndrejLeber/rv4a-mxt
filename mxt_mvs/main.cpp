/*-------------------- MVS-Funktion mit Echtzeitthread-------------------*/

/* Wichtiger Hinweis: Als Vorlage für dieses Programm wurde der Code von Jürgen Palczynski
 * aus seiner Diplomarbeit übernommen und an einigen stellen modifiziert / optimiert
 */

#include <includes.h>
#include "matplotlibcpp.h"

#define R3

static long sock;
int endcmd = 0;
static std::string userinput;
extern STEPS recv_msgs;

// Programmabbruch mit STRG+C
void endprg(int dummy) {
    dummy += dummy;
    endcmd = 1;
    std::cout << "\n\nEnding program...\n\n" << std::endl;
    stop_robot(sock);
    usleep(1e4);
    close(sock);
    usleep(1e6);
    exit(-1);
}

int main()
{
    // STRG+C abfangen
    signal(SIGINT, endprg);

    std::cout << " ------ Programmabbruch mit STRG+C -------" << std::endl << std::endl;

#ifdef R3
    // Aufbau einer Verbindung über R3 und Einschalten der Servors
    sock = open_connection("192.168.0.1", 10002);
    start_robot(sock);

    std::cout << "Zum Start eines Programms bitte Programmnr. eingeben und mit ENTER bestätigen." << std::endl;
    std::cin >> userinput;

    std::string load_prg_with_number = CMD_PRG_LOAD + userinput;
    std::cout << "Programm " << userinput << " wird geladen und ausgeführt..." << std::endl;
    std::cout << "Befehl: " << load_prg_with_number << std::endl;
    send_command(sock, CMD_PRG_RESET);
    send_command(sock, load_prg_with_number);
    usleep(1e6);
    send_command(sock, CMD_PRG_RUN);
#endif

#ifdef PLOTS
    // Definieren einer Startposition
    POSE start;
    start.w.x = 200.0f;
    start.w.y = 200.0f;
    start.w.z = 146.0f;

    // Definieren einer Zielposition
    POSE ziel;
    ziel.w.x = 200.0f;
    ziel.w.y = 200.0f;
    ziel.w.z = 150.0f;

    auto t0 = std::chrono::steady_clock::now();
    STEPS x = Sinoide(start, ziel, 10.0f, 200.0f);
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - t0);

    for (unsigned int i = 0; i < x.x.size(); i++) {
        std::cout << "x" << i << " = " << x.x.at(i) << "mm,  y" << i << " = " << x.y.at(i) << "mm,  z" << i << " = " << x.z.at(i) << "mm" << std::endl;
    }

    std::cout << "Laufzeit Bahnplaner [micro_s]: " << elapsed.count() << std::endl;

    // Ergebnisse der Bahnplanung
    namespace plt = matplotlibcpp;
    plt::plot(x.t,x.z);
    plt::show();
    plt::save("/home/pi/Desktop/bahn_x.png");
    plt::clf();

    // Geschwindkeitsverlauf und Beschleunigungsverlauf berechnen und plottten
    std::vector<float> v, a;
    v.push_back(0.0f);
    a.push_back(0.0f);
    for (unsigned int i = 0; i < x.z.size()-1; i++) {
        v.push_back(x.z.at(i+1)-x.z.at(i));
    }
    for (unsigned int i = 0; i < x.z.size()-1; i++) {
        a.push_back(v.at(i+1)-v.at(i));
    }
    plt::plot(x.t,v);
    plt::show();
    plt::save("/home/pi/Desktop/bahn_v.png");
    plt::clf();

    plt::plot(x.t,a);
    plt::save("/home/pi/Desktop/bahn_a.png");
    plt::clf();
#endif

    char var;
    std::cout << "Bitte a + Enter drücken um fortzufahren." << std::endl;
    std::cin >> var;

    // Definieren eines Vektors bestehend aus den einzelnen Zielpositionen
    std::vector<POSE> ziel;

    POSE ziel1;
    ziel1.w.x = 100.0f;
    ziel1.w.y = 200.0f;
    ziel1.w.z = 60.0f;
    ziel.push_back(ziel1);

    POSE ziel2;
    ziel2.w.x = 100.0f;
    ziel2.w.y = 200.0f;
    ziel2.w.z = 70.0f;
    ziel.push_back(ziel2);

    POSE ziel3;
    ziel3.w.x = 100.0f;
    ziel3.w.y = 200.0f;
    ziel3.w.z = 80.0f;
    ziel.push_back(ziel3);

    POSE ziel4;
    ziel4.w.x = 100.0f;
    ziel4.w.y = 200.0f;
    ziel4.w.z = 90.0f;
    ziel.push_back(ziel4);

    POSE ziel5;
    ziel5.w.x = 100.0f;
    ziel5.w.y = 200.0f;
    ziel5.w.z = 100.0f;
    ziel.push_back(ziel5);

    POSE ziel6;
    ziel6.w.x = 100.0f;
    ziel6.w.y = 200.0f;
    ziel6.w.z = 110.0f;
    ziel.push_back(ziel6);

    POSE ziel7;
    ziel7.w.x = 100.0f;
    ziel7.w.y = 200.0f;
    ziel7.w.z = 120.0f;
    ziel.push_back(ziel7);

    POSE ziel8;
    ziel8.w.x = 100.0f;
    ziel8.w.y = 200.0f;
    ziel8.w.z = 130.0f;
    ziel.push_back(ziel8);

    POSE ziel9;
    ziel9.w.x = 100.0f;
    ziel9.w.y = 200.0f;
    ziel9.w.z = 140.0f;
    ziel.push_back(ziel9);

    POSE ziel10;
    ziel10.w.x = 100.0f;
    ziel10.w.y = 200.0f;
    ziel10.w.z = 150.0f;
    ziel.push_back(ziel10);

    POSE ziel11;
    ziel11.w.x = 101.0f;
    ziel11.w.y = 201.0f;
    ziel11.w.z = 160.0f;
    ziel.push_back(ziel11);

    mxt_init();
    void* data = &ziel;

    int status = init_rt_mvs_thread(80, data);


//    matplotlibcpp::subplot(3,1,1);
//    matplotlibcpp::plot(recv_msgs.t, recv_msgs.x);
//    matplotlibcpp::title("Gefahrene Wegstrecke in x-Richtung");
//    //matplotlibcpp::xlabel("Zeit t [s]");
//    matplotlibcpp::ylabel("x in [mm]");
//    matplotlibcpp::subplot(3,1,2);
//    matplotlibcpp::plot(recv_msgs.t, recv_msgs.y);
//    matplotlibcpp::title("Gefahrene Wegstrecke in y-Richtung");
//    //matplotlibcpp::xlabel("Zeit t [s]");
//    matplotlibcpp::ylabel("y in [mm]");
//    matplotlibcpp::subplot(3,1,3);
    matplotlibcpp::plot(recv_msgs.t, recv_msgs.z);
    matplotlibcpp::title("Gefahrene Wegstrecke in z-Richtung");
    matplotlibcpp::xlabel("Zeit t [s]");
    matplotlibcpp::ylabel("z in [mm]");
    //matplotlibcpp::legend();
    std::cout << "ENDE" << std::endl;
    matplotlibcpp::save("/home/pi/Desktop/results.png");
    matplotlibcpp::show();

    while(!endcmd) {

    };

    return status;
}

