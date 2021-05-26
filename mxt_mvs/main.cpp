/*-------------------- MVS-Funktion mit Echtzeitthread-------------------*/

/* Wichtiger Hinweis: Als Vorlage für dieses Programm wurde der Code von Jürgen Palczynski
 * aus seiner Diplomarbeit übernommen und an einigen stellen modifiziert / optimiert
 */

#include <includes.h>

extern int endcmd;

int main()
{
    //Loggen in Datei: > ~/Desktop/log.txt

    // STRG+C abfangen
    signal(SIGINT, endprg);

    mxt_init();

    // Definieren einer Zielposition
    POSE* ziel = new  (POSE);
    ziel->w.x = 200.0;
    ziel->w.y = 200.0;
    ziel->w.z = 145.0;

    void* data = (void*)ziel;
    init_rt_mvs_thread(80, data, endcmd);

}

