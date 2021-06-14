/*-------------------- MVS-Funktion mit Echtzeitthread-------------------*/

/* Wichtiger Hinweis: Als Vorlage für dieses Programm wurde der Code von Jürgen Palczynski
 * aus seiner Diplomarbeit übernommen und an einigen stellen modifiziert / optimiert
 */

#include <includes.h>

#define R3

#ifdef PLOTS
#include "matplotlibcpp.h"
#endif

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

//    init_serial("/dev/ttyACM0", 115200);
//    bool status = connect_serial();
//    send_message("M106 S255");
//    std::cout << "Bitte Taste drücken, um Lüfter auszuschalten" << std::endl;
//    string input;
//    cin >> input;
//    if (input != "") {
//        send_message("M107");
//    }

    ifstream gcode_file;
    string filename;
    std::cout << "Please enter the filename: ";
    //std::cin >> filename;
    filename = "/home/pi/Desktop/test2.gcode";
    gcode_file.open(filename);
    if(!gcode_file.is_open()){
        std::cout << "File could not be opened." << std::endl;
        return -1;
    }
    vector<GCode> vec_gcode;
    string line;

    while(getline(gcode_file, line)){
        GCode gcode{};
        (std::istringstream) line >> gcode;
        if (gcode.command_id.size() > 0) {
            vec_gcode.push_back(gcode);
        }
    }

    std::cout << "Read " << vec_gcode.size() << " lines of G-Code." << std::endl;
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

    char var;
    std::cout << "Bitte a + Enter drücken um fortzufahren." << std::endl;
    std::cin >> var;

    mxt_init();
    void* data = &vec_gcode;

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

    matplotlibcpp::plot(recv_msgs.t, recv_msgs.z, ".");
    matplotlibcpp::title("Gefahrene Wegstrecke in z-Richtung");
    matplotlibcpp::xlabel("Zeit t [s]");
    matplotlibcpp::ylabel("z in [mm]");
    //matplotlibcpp::legend();
    std::cout << "ENDE" << std::endl;
    matplotlibcpp::save("/home/pi/Desktop/results.png");
    matplotlibcpp::show();

    while(!endcmd) {

    };

    return 0;
}

