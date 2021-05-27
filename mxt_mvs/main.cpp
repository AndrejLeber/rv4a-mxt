/*-------------------- MVS-Funktion mit Echtzeitthread-------------------*/

/* Wichtiger Hinweis: Als Vorlage für dieses Programm wurde der Code von Jürgen Palczynski
 * aus seiner Diplomarbeit übernommen und an einigen stellen modifiziert / optimiert
 */

#include <includes.h>

// #define R3

static long sock;
extern int endcmd;
static std::string userinput;

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

    // Definieren einer Startposition
    POSE start, ziel;
    start.w.x = 200.0f;
    start.w.y = 200.0f;
    start.w.z = 150.0f;

    // Definieren einer Zielposition
    ziel.w.x = 200.0f;
    ziel.w.y = 200.0f;
    ziel.w.z = 100.0f;

    STEPS res;
    res = Sinoide(start, ziel, 1);

//    std::cout << res.y.size() << std::endl;

//    std::vector<int> test = {1,2,3,4,5};
//    std::cout << test.size() << std::endl;
//    std::cout << res.x[0] << std::endl;

    for (int i = 0; i < res.x.size(); i++) {
        std::cout << "x" << i << " = " << res.x[i] << "mm,  y" << i << " = " << res.y[i] << "mm,  z" << i << " = " << res.z[i] << "mm" << std::endl;
    }

    // // Definieren einer Zielposition
    //    POSE* ziel = new(POSE);
    //    ziel->w.x = 200.0;
    //    ziel->w.y = 200.0;
    //    ziel->w.z = 150.0;
    //    mxt_init();
    //    void* data = (void*)ziel;
    //    init_rt_mvs_thread(80, data, endcmd);


    //std::string ip = "192.168.0.1";
    //uint16_t port = 10001;

    while(!endcmd) {

    };

    return 0;
}

