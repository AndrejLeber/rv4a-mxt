/*-------------------- MVS-Funktion mit Echtzeitthread-------------------*/

// Entwickler: Romina Gänsler, Johannes Allert, Patrick Bertsch, Andrej Leber
// Letztes Revisionsdatum: 15.06.21

#include <includes.h>

#define R3
#define PLOTS

#ifdef PLOTS
#include "matplotlibcpp.h"
#endif

extern QSerialPort *serial;

static long sock;
int endcmd = 0;
static std::string userinput;
extern STEPS recv_msgs;

// Programmabbruch mit STRG+C
[[noreturn]] void endprg(int dummy) {
    dummy += dummy;
    endcmd = 1;
    std::cout << "\n\nEnding program...\n\n" << std::endl;
    stop_robot(sock);
    usleep(1e4);
    close(sock);
    disconnect_serial();
    usleep(5e5);
    exit(-1);
}

int main()
{
    // STRG+C abfangen
    signal(SIGINT, endprg);
    std::cout << " ------ Aborting program with STRG+C -------" << std::endl << std::endl;

    // Initialisieren der seriellen Kommunikation mit der Heizplatine
    serial = new QSerialPort();
    init_serial("/dev/ttyACM0", 115200);

    connect_serial();

    // Einlesen und parsen der G-Code Datei
    std::ifstream gcode_file;
    std::string filename;
    std::cout << "Please enter the full path and filename of the desired GCode- File: ";
    //std::cin >> filename;
    filename = "/home/pi/Desktop/GCode/krueppelV3.gcode";
    std::cout << filename << " will be used." << std::endl;
    gcode_file.open(filename);
    if(!gcode_file.is_open()){
        std::cout << "File could not be opened." << std::endl;
        return -1;
    }
    std::vector<GCode> vec_gcode;
    std::string line;

    while(getline(gcode_file, line)){
        GCode gcode{};
        try {
            static_cast<std::istringstream>(line) >> gcode;
        } catch (std::exception e) {
            std::cout << "Parser exception: " << e.what() <<std::endl;
            return -1;
        }

        if (!gcode.command_id.empty()) {
            vec_gcode.push_back(gcode);
        }
    }

    std::cout << "Reading " << vec_gcode.size() << " lines of G-Code completed." << std::endl;

#ifdef R3
    // Aufbau einer Verbindung über R3 und Einschalten der Servors
    sock = open_connection("192.168.0.1", 10002);
    start_robot(sock);

    std::cout << "To start the robot program, please type in a program number and press ENTER." << std::endl;
    std::cin >> userinput;

    std::string load_prg_with_number = CMD_PRG_LOAD + userinput;
    std::cout << "Program Nr." << userinput << " will be loaded and executed ..." << std::endl;
    send_command(sock, CMD_PRG_RESET);
    send_command(sock, load_prg_with_number);
    usleep(1e6);
    send_command(sock, CMD_PRG_RUN);
#endif

    char var;
    std::cout << "Press any key + ENTER to continue starting MXT realtime communication." << std::endl;
    std::cin >> var;

    mxt_init();
    void* data = &vec_gcode;


    init_rt_mvs_thread(80, data);

    matplotlibcpp::subplot(3,1,1);
    matplotlibcpp::plot(recv_msgs.t, recv_msgs.x);
    matplotlibcpp::title("Gefahrene Wegstrecke in x-Richtung");
    matplotlibcpp::ylabel("x in [mm]");

    matplotlibcpp::subplot(3,1,2);
    matplotlibcpp::plot(recv_msgs.t, recv_msgs.y);
    matplotlibcpp::title("Gefahrene Wegstrecke in y-Richtung");
    matplotlibcpp::ylabel("y in [mm]");

    matplotlibcpp::subplot(3,1,3);
    matplotlibcpp::plot(recv_msgs.t, recv_msgs.z, ".");
    matplotlibcpp::title("Gefahrene Wegstrecke in z-Richtung");
    matplotlibcpp::xlabel("Zeit t [s]");
    matplotlibcpp::ylabel("z in [mm]");
    std::cout << "End of program." << std::endl;
    matplotlibcpp::save("/home/pi/Desktop/results.png");
    matplotlibcpp::show();

    while(!endcmd) {

    }

    return 0;
}

