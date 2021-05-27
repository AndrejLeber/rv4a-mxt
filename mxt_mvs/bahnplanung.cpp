//
// Created by Johannes on 27.04.2021.
//

#include "bahnplanung.h"
#include "strdef.h"
#include "cmath"
#include "vector"
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

// Funktion die das Vorzeichen eines Wertes/Vektors zurück gibt -1/0/+1
template <typename T>
int sign (const T &val) { return (val > 0) - (val < 0); }

template <typename T>
std::vector<int> sign (const std::vector<T> &v) {
    std::vector<int> r(v.size());
    std::transform(v.begin(), v.end(), r.begin(), (int(*)(const T&))sign);
    return r;
}

STEPS Sinoide(POSE start, POSE target, float speed){
    STEPS Stuetz; // Struktur mit Stützstellen in x,y,z
    int n_Axis=3;
    vector<float> se(3); // Teilstrecke für x,y,z
    vector<int> bm{200,200,200}; // Maximale Achsbeschleunigung
    vector<float>vm(3); // Geschwindigkeit der Einzelachsen
    vector<float>tb(3); // Zeit für Bremsen oder Beschleunigen
    vector<float>tv(3); // Zeit während konstanter Geschwindigkeit
    vector<float>te(3); // Zeit für gesamte Bahn
    float tipo=7.1e-3; // Echtzeit Takt. Grundlegend für Stützstellen
    vector<int> dir;     // Integer der die Richtung für Sützstellen vorgibt -1/0/+1
    vector<float> time_steps; //Vektor in dem t abgespeichert ist

    // Teilstrecken bestimmen
    se[0]=target.w.x-start.w.x;
    se[1]=target.w.y-start.w.y;
    se[2]=target.w.z-start.w.z;

    // Richtungsvorzeichen für alle Achsen bestimmen
    dir=sign(se);

    // Betrag von allen Teilstrecken
    se[0]=abs(se[0]);
    se[1]=abs(se[1]);
    se[2]=abs(se[2]);

    float Gesamtstrecke=sqrt(pow(se[0],2)+pow(se[1],2)+pow(se[2],2));

    float r_x=se[0]/Gesamtstrecke;
    float r_y=se[1]/Gesamtstrecke;
    float r_z=se[2]/Gesamtstrecke;

    vm[0]=speed*r_x;
    vm[1]=speed*r_y;
    vm[2]=speed*r_z;

    // Bestimmung tb,tv,te, vm, bm
    for (int i=0;i<n_Axis;i++)
    {
        // vm[i] = min(vm[i], sqrt((se[i]*bm[i]/2)));
        if (vm[i]>sqrt((se[i]*bm[i]/2))) // Prüfung ob vm höher ist als vm,max
        {
            vm[i]=sqrt((se[i]*bm[i]/2)); // vm auf vm,max setzen
            cout << "Programmierte Geschwindigkeit wird nicht erreicht" << endl;
        }

        if (bm[i] != 0) {
            tb[i]=2*vm[i]/bm[i]; // Beschleunigungsdauer
        }
        else {
            tb[i] = 0;
        }

        if (vm[i] != 0) {
            te[i]=(se[i]/vm[i])+tb[i]; // Gesamtdauer
        }
        else {
            te[i] = 0;
        }

        tv[i]=te[i]-tb[i]; // Konstantfahrdauer
    }

    // Höchstes te für vollsynchrone Bewegung bestimmen
    float te_m = *max_element(te.begin(), te.end());
    float tv_m = *max_element(tv.begin(), tv.end());
    float tb_m = *max_element(tb.begin(), tb.end());

    for (int i=0;i<n_Axis;i++)
    {
        vm[i]=se[i]/(te_m-tb_m);
        bm[i]=2*vm[i]/tb_m;
    }

    int a=te_m/tipo; // Berechnen der notwendig Anzahl an Stützstellen

    for (int i=0; i<a;i++) // t-Vektor füllen mit Zeiten... 0, 0.1, 0.2 ,....
    {
        time_steps.push_back(i*tipo);
    }

    // Alle Achsen ******************************************

    for (float time_step : time_steps) //Iteration über alle Stützstellen
    {
        float temp;
        if (time_step<tb_m) //Beschleunigen
        {
            temp = pow(time_step,2)*0.25+(pow(tb_m,2)/(8*M_PI*M_PI))*(cos(2*M_PI*time_step/tb_m)-1);
            Stuetz.x.push_back(start.w.x+dir[0]*bm[0]*temp);
            Stuetz.y.push_back(start.w.y+dir[1]*bm[1]*temp);
            Stuetz.z.push_back(start.w.z+dir[2]*bm[2]*temp);
        }
        if (time_step >= tb_m & time_step<tv_m) //konstante Geschwindigkeit
        {
            temp = time_step-0.5*tb_m;
            Stuetz.x.push_back(start.w.x+dir[0]*vm[0]*temp);
            Stuetz.y.push_back(start.w.y+dir[1]*vm[1]*temp);
            Stuetz.z.push_back(start.w.z+dir[2]*vm[2]*temp);
        }
        if (time_step >= tv_m) //Bremsphase
        {
            temp = 0.5*(te_m*(time_step+tb_m)-0.5*(pow(time_step,2)+pow(te_m,2)+2*pow(tb_m,2))+pow(tb_m,2)/(4*pow(M_PI,2))*(1-cos(2*M_PI*(time_step-tv_m)/tb_m)));
            Stuetz.x.push_back(start.w.x+dir[0]*bm[0]*temp);
            Stuetz.y.push_back(start.w.y+dir[1]*bm[1]*temp);
            Stuetz.z.push_back(start.w.z+dir[2]*bm[2]*temp);
        }
    }

    return Stuetz;
}
