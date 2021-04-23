#ifndef STRDEF_H
#define STRDEF_H

//************************************************************************************
//**********
// Beispielprogramm zur externen Echtzeit-Steuerung
// Header-Datei zur Definition der Datenstruktur fÜr das Datenpaket
//************************************************************************************
//**********
// strdef.h
// Führen Sie ab Software-Version H7 die Zeile #define VER_H7 aus.
// Kennzeichnen Sie die Zeile bei der Version H6 oder Älteren Versionen als Kommentar
// (keine Verarbeitung)

#define VER_H7
/*************************************************************************************
****/
/* Gelenk-Koordinatensystem (nicht verwendete Achsen auf 0 setzen) */
/* Eine detaillierte Beschreibung jedes Roboters finden */
/* Sie im Technischen Handbuch des Roboters. */
/*************************************************************************************
****/
typedef struct{
    float j1; // Winkel J1-Achse (Radiant)
    float j2; // Winkel J2-Achse (Radiant)
    float j3; // Winkel J3-Achse (Radiant)
    float j4; // Winkel J4-Achse (Radiant)
    float j5; // Winkel J5-Achse (Radiant)
    float j6; // Winkel J6-Achse (Radiant)
    float j7; // Zusatzachse 1 (Winkel J7-Achse) (Radiant)
    float j8; // Zusatzachse 2 (Winkel J8-Achse) (Radiant)
} JOINT;
/*************************************************************************************
****/
/* XYZ-Koordinatensystem (nicht verwendete Achsen auf 0 setzen)*/
/* Eine detaillierte Beschreibung jedes Roboters finden*/
/* Sie im Technischen Handbuch des Roboters.*/
/*************************************************************************************
****/
typedef struct{
    float x; // X-Achsen-Koordinate (mm)
    float y; // Y-Achsen-Koordinate (mm)
    float z; // Z-Achsen-Koordinate (mm)
    float a; // A-Achsen-Koordinate (Radiant)
    float b; // B-Achsen-Koordinate (Radiant)
    float c; // C-Achsen-Koordinate (Radiant)
    float l1; // Zusatzachse 1 (mm oder Radiant)
    float l2; // Zusatzachse 2 (mm oder Radiant)
} WORLD;
typedef struct{
    WORLD w;
    unsigned int sflg1; // Stellungsmerker 1
    unsigned int sflg2; // Stellungsmerker 2
} POSE;
/*************************************************************************************
****/
/* Impulskoordinatensystem (nicht verwendete Achsen auf 0 setzen)*/
/* Die Koordinaten jedes Gelenks sind Über Motorimpulse festgelegt.*/
/*************************************************************************************
****/
typedef struct{
    long p1; // Achse Motor 1
    long p2; // Achse Motor 2
    long p3; // Achse Motor 3
    long p4; // Achse Motor 4
    long p5; // Achse Motor 5
    long p6; // Achse Motor 6
    long p7; // Zusatzachse 1 (Achse Motor 7)
    long p8; // Zusatzachse 2 (Achse Motor 8)
} PULSE;
typedef union { // Befehlsdaten
    POSE pos; // XYZ-Daten [mm/rad]
    JOINT jnt; // Gelenkdaten [rad]
    PULSE pls; // Impulsdaten [Impulse]
    long lng1[8]; // Ganze Zahlen [% / dimensionslos]
} Data_type;
enum point_type{
    CORNER_POINT_POS,
    CORNER_POINT_JOINT,
    CORNER_POINT_BOTH,
    POINT_POS,
    POINT_JOINT,
    POINT_BOTH
};
typedef struct {
    const char *name;
    point_type point_t;
    POSE pos; // XYZ-Daten [mm/rad]
    JOINT jnt; // Gelenkdaten [rad]
    int frame_nmbr;
}Store_type;
/*******************************************/
/* Datenpaket zur Echtzeit-Steuerung */
/*******************************************/
typedef struct enet_rtcmd_str {
    unsigned short Command; // Befehl
#define MXT_CMD_NULL 0 // Externer Echtzeit-Befehl gesperrt
#define MXT_CMD_MOVE 1 // Externer Echtzeit-Befehl freigegeben
#define MXT_CMD_END 255 // Externer Echtzeit-Befehl Ende
    unsigned short SendType; // Festlegung Befehlsdatentyp
    unsigned short RecvType; // Festlegung Überwachungsdatentyp
    //////////// Befehl- oder Überwachungsdatentyp ///
#define MXT_TYP_NULL 0 // Keine Daten
    // FÜr Befehl und Ãœberwachung
    ////////////////////
#define MXT_TYP_POSE 1 // XYZ-Daten
#define MXT_TYP_JOINT 2 // Gelenkdaten
#define MXT_TYP_PULSE 3 // Impulsdaten
    ///////////// Für auf die Position bezogene Größen ///
#define MXT_TYP_FPOSE 4 // XYZ-Daten (nach Filterung)
#define MXT_TYP_FJOINT 5 // Gelenkdaten (nach Filterung)
#define MXT_TYP_FPULSE 6 // Impulsdaten (nach Filterung)
#define MXT_TYP_FB_POSE 7 // XYZ-Daten (Encoderimpulse) <H7A>
#define MXT_TYP_FB_JOINT 8 // Gelenkdaten (Encoderimpulse) <H7A>
#define MXT_TYP_FB_PULSE 9 // Impulsdaten (Encoderimpulse) <H7A>
    // FÜr auf Strom bezogene Größen//////// <H7A>
#define MXT_TYP_CMDCUR 10 // Strombefehl <H7A>
#define MXT_TYP_FBKCUR 11 // StromrückfÜhrung <H7A>
    unsigned short reserve; // Reserviert
    union rtdata { // Befehlsdaten
        POSE pos; // XYZ-Daten [mm/rad]
        JOINT jnt; // Gelenkdaten [rad]
        PULSE pls; // Impulsdaten [Impulse]
        long lng1[8]; // Ganze Zahlen [% / dimensionslos]
    } dat;
    unsigned short SendIOType; // Datentypfestlegung der E/A-Signaldaten beim Senden
    unsigned short RecvIOType; // Datentypfestlegung der E/A-Signaldaten beim Empfang
#define MXT_IO_NULL 0 // Keine Daten
#define MXT_IO_OUT 1 // Ausgangssignal
#define MXT_IO_IN 2 // Eingangssignal
    unsigned short BitTop; // Kopfbitnummer
    unsigned short BitMask; // Festlegung Bitmaske zur Übertragung (0x000â€“0xffff)
    unsigned short IoData; // Ein- und Ausgangssignaldaten (0x0000-0xffff)
    unsigned short TCount; // Wert des Zählers zur Zeitüberwachung
    unsigned long CCount; // Wert des Zählers fÜr Kommunikationsdaten
#ifdef VER_H7
    unsigned short RecvType1; // Festlegung des Datentyps zum Empfang 1
    unsigned short reserve1; // Reserviert 1
    union rtdata1 { // Überwachungsdaten 1
        POSE pos1; // XYZ-Daten [mm/rad]
        JOINT jnt1; // Gelenkdaten [mm/rad]
        PULSE pls1; // Impulsdaten [mm/rad]
        long lng1[8]; // Ganze Zahl [%/dimensionslos]
    } dat1;
    unsigned short RecvType2; // Festlegung des Datentyps zum Empfang 2
    unsigned short reserve2; // Reserviert 2
    union rtdata2 { // Überwachungsdaten 2
        POSE pos2; // XYZ-Daten [mm/rad]
        JOINT jnt2; // Gelenkdaten [mm/rad]
        PULSE pls2; // Impulsdaten [mm/rad] oder ganze Zahl [%/dimensionslos]
        long lng2[8]; // Ganze Zahl [%/dimensionslos]
    } dat2;
    unsigned short RecvType3; // Festlegung des Datentyps zum Empfang 3
    unsigned short reserve3; // Reserviert 3
    union rtdata3 { // Ãœberwachungsdaten 3
        POSE pos3; // XYZ-Daten [mm/rad] .
        JOINT jnt3; // Gelenkdaten [mm/rad] .
        PULSE pls3; // Impulsdaten [mm/rad] oder ganze Zahl [%/dimensionslos]
        long lng3[8]; // Ganze Zahl [%/dimensionslos]
    } dat3;
#endif
} MXTCMD;

extern MXTCMD MXTsend;
extern MXTCMD MXTrecv;

#endif // STRDEF_H
