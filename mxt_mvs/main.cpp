/*-------------------- MVS-Funktion mit Echtzeitthread-------------------*/

/* Wichtiger Hinweis: Als Vorlage für dieses Programm wurde der Code von Jürgen Palczynski
 * aus seiner Diplomarbeit übernommen und an einigen stellen modifiziert / optimiert
 */

#include <includes.h>
#include "matplotlibcpp.h"

#define R3

static long sock;
extern int endcmd;
static std::string userinput;
extern STEPS recv_msgs;

// Programmabbruch mit STRG+C
void endprg(int dummy) {
    dummy += dummy;
    endcmd = 1;
    std::cout << "Ending program..." << std::endl;
    stop_robot(sock);
    usleep(1e4);
    close(sock);
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
    std::cout << "Bitte Enter drücken um fortzufahren." << std::endl;
    std::cin >> var;
    std::cout << "Bin hier" << std::endl;

    // Definieren einer Zielposition
    POSE* ziel = new(POSE);
    ziel->w.x = 150.01;
    ziel->w.y = 250.02;
    ziel->w.z = 200.03;
    mxt_init();
    void* data = (void*)ziel;
    init_rt_mvs_thread(80, data, endcmd);

    ziel->w.x = 100.0;
    ziel->w.y = 200.0;
    ziel->w.z = 50.0;
    init_rt_mvs_thread(80, data, endcmd);

    std::cout << "1. Element von x: " << recv_msgs.x.at(0) << std::endl;
    matplotlibcpp::subplot(3,1,1);
    matplotlibcpp::plot(recv_msgs.t, recv_msgs.x);
    matplotlibcpp::title("Gefahrene Wegstrcke in x-Richtung");
    matplotlibcpp::xlabel("Zeit t [s]");
    matplotlibcpp::ylabel("x in [mm]");
    matplotlibcpp::subplot(3,1,2);
    matplotlibcpp::plot(recv_msgs.t, recv_msgs.y);
    matplotlibcpp::title("Gefahrene Wegstrcke in y-Richtung");
    matplotlibcpp::xlabel("Zeit t [s]");
    matplotlibcpp::ylabel("y in [mm]");
    matplotlibcpp::subplot(3,1,3);
    matplotlibcpp::plot(recv_msgs.t, recv_msgs.z);
    matplotlibcpp::title("Gefahrene Wegstrcke in z-Richtung");
    matplotlibcpp::xlabel("Zeit t [s]");
    matplotlibcpp::ylabel("z in [mm]");
    //matplotlibcpp::legend();
    //matplotlibcpp::save("/home/pi/Desktop/results.png");
    matplotlibcpp::show();

    while(!endcmd) {

    };

    return 0;
}

