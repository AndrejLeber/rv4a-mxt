/*-------------------- Deklaration aller notwendigen Funktionen (Definition in functions.cpp)-------------------*/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "strdef.h"
#include "bahnplanung.h"

// Globale Variablen
extern int endcmd;
extern STEPS recv_msgs;

// Berechnen und Ausführen einer einzelnen MVS-Bewegung
int mvs(MXTCMD &mxt_send, MXTCMD &mxt_recv, POSE start, POSE* ziel, float speed);

// Berechnen des Sinodienprofils
STEPS calc_sinoide(POSE start, POSE* ziel, float v, float a);

// Ausführen ddees Sinoidenprofils
int move_sinoide(MXTCMD send_sinoide, MXTCMD recv_sinoide, POSE start, STEPS path);

// Empfangen und Ausgeben der aktuellen Roboterposition
void *mxt_recv_pos(void* data);

// Berechnen und und Senden von Fahrbefehlen zur Implementierung der MVS- Funktion
void *mxt_mvs_pos(void* data);

// Vorbereiten des MXT- Befehls zum Empfangen der aktuellen Roboterposition (Ohne Bewegung)
void mxt_prep_recv_pos(MXTCMD &cmd);

// Vorbereiten des MXT- Befehls zum Senden und Ausführen neuer Positionsdaten (Bewegung)
void mxt_prep_move_pos(MXTCMD &cmd, POSE pos);

// Vorbereiten des MXT- Befehls zum Beenden von MXT
void mxt_prep_end(MXTCMD &cmd);

// Initiieren der MXT- Parameter
void mxt_init();

#endif // FUNCTIONS_H
