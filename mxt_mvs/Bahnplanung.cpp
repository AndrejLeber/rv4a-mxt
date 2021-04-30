//
// Created by Johannes on 27.04.2021.
//

#include "Bahnplanung.h"
#include "strdef.h"
#include "cmath"
#include "vector"
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;




//Funktion die das Vorzeichen eines Wertes/Vektors zurück gibt -1/0/+1
template <typename T>
int sign (const T &val) { return (val > 0) - (val < 0); }

template <typename T>
std::vector<int> sign (const std::vector<T> &v) {
    std::vector<int> r(v.size());
    std::transform(v.begin(), v.end(), r.begin(), (int(*)(const T&))sign);
    return r;
}



STEPS Sinoide(POSE start, POSE target, float speed){
    STEPS Stuetz; //Struktur mit Stützstellen in X,y,z
    int n_Axis=3;
    vector<float> se(3); // Teilstrecke für x,y,z
    vector<int> bm{200,200,200}; //Maximale Achsbeschleunigung
    vector<float>vm(3); //Geschwindigkeit der Einzelachsen
    vector<float>tb(3); //Zeit für Bremsen oder Beschleunigen
    vector<float>tv(3); //Zeit während konstanter Geschwindigkeit
    vector<float>te(3); //Zeit für gesamte Bahn
    float tipo=0.01; //Echtzeit Takt. Grundlegend für Stützstellen
    vector<int> dir;     //Integer der die Richtung für Sützstellen vorgibt -1/0/+1
    vector<float> time_steps; //Vektor in dem t abgespeichert ist

    //Teilstrecken bestimmen
    se[0]=target.w.x-start.w.x;
    se[1]=target.w.y-start.w.y;
    se[2]=target.w.z-start.w.z;

    //Richtungsvorzeichen für alle Achsen bestimmen
    dir=sign(se);

    //Betrag von allen Teilstrecken
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


    //Bestimmung tb,tv,te, vm, bm
    for (int i=0;i<n_Axis;i++)
    {
        if (vm[i]>sqrt((se[i]*bm[i]/2))) //%Prüfung ob vm höher ist als vm,max
        {
            vm[i]=sqrt((se[i]*bm[i]/2)); //%vm auf vm,max setzen
            cout << "Programmierte Geschwindigkeit wird nicht erreicht" << endl;
        }
        tb[i]=2*vm[i]/bm[i]; //Beschleunigungsdauer
        te[i]=(se[i]/vm[i])+tb[i]; //Gesamtdauer
        tv[i]=te[i]-tb[i]; //Konstantfahrdauer
    }

    //Höchstes te für vollsynchrone Bewegung bestimmen
    float te_m = *max_element(te.begin(), te.end());
    float tv_m = *max_element(tv.begin(), tv.end());
    float tb_m = *max_element(tb.begin(), tb.end());


    for (int i=0;i<n_Axis;i++)
    {
        if (te[i]!=te_m)
        {
            te[i]=te_m;
            tv[i]=tv_m;
            tb[i]=tb_m;
            vm[i]=se[i]/(te[i]-tb[i]);
            bm[i]=2*vm[i]/tb[i];
        }
    }

    int a=te_m/tipo; //Berechnen der notwendig Anzahl an Stützstellen


    for (int i=0; i<a;i++) //t-Vektor füllen mit Zeiten... 0, 0.1, 0.2 ,....
    {
     time_steps.push_back(i*tipo);
    }


    //X-Achse ******************************************
    int j=0;

         for (int i=0; i<time_steps.size() ;i++) //Iteration über alle Stützstellen
         {

             if (time_steps[i]<tb[j]) //Beschleunigen
             {
                 Stuetz.x.push_back(start.w.x+dir[j]*bm[j]*(pow(time_steps[i],2)*0.25+(pow(tb[j],2)/(8*M_PI*M_PI))*(cos(2*M_PI*time_steps[i]/tb[j])-1)));
             }
             if (time_steps[i] >= tb[j] & time_steps[i]<tv[j]) //konstante Geschwindigkeit
             {
                 Stuetz.x.push_back(start.w.x+dir[j]*vm[j]*(time_steps[i]-0.5*tb[j]));
             }
             if (time_steps[i] >= tv[j]) //Bremsphase
             {
                 Stuetz.x.push_back(start.w.x+dir[j]*bm[j]*0.5*(te[j]*(time_steps[i]+tb[j])     -0.5*(pow(time_steps[i],2)+pow(te[j],2)+2*pow(tb[j],2))    +pow(tb[j],2)/(4*pow(M_PI,2))    *(1-cos(2*M_PI*(time_steps[i]-tv[j])/tb[j]))    ));
             }
         }



    //Y-Achse ******************************************
    j=1;

    for (int i=0; i<time_steps.size() ;i++) //Iteration über alle Stützstellen
    {

        if (time_steps[i]<tb[j]) //Beschleunigen
        {
            Stuetz.y.push_back(start.w.y+dir[j]*bm[j]*(pow(time_steps[i],2)*0.25+(pow(tb[j],2)/(8*M_PI*M_PI))*(cos(2*M_PI*time_steps[i]/tb[j])-1)));
        }
        if (time_steps[i] >= tb[j] & time_steps[i]<tv[j]) //konstante Geschwindigkeit
        {
            Stuetz.y.push_back(start.w.y+dir[j]*vm[j]*(time_steps[i]-0.5*tb[j]));
        }
        if (time_steps[i] >= tv[j]) //Bremsphase
        {
            Stuetz.y.push_back(start.w.y+dir[j]*bm[j]*0.5*(te[j]*(time_steps[i]+tb[j])     -0.5*(pow(time_steps[i],2)+pow(te[j],2)+2*pow(tb[j],2))    +pow(tb[j],2)/(4*pow(M_PI,2))    *(1-cos(2*M_PI*(time_steps[i]-tv[j])/tb[j]))    ));
        }
    }

    //Z-Achse******************************************
    j=2;

    for (int i=0; i<time_steps.size() ;i++) //Iteration über alle Stützstellen
    {

        if (time_steps[i]<tb[j]) //Beschleunigen
        {
            Stuetz.z.push_back(start.w.z+dir[j]*bm[j]*(pow(time_steps[i],2)*0.25+(pow(tb[j],2)/(8*M_PI*M_PI))*(cos(2*M_PI*time_steps[i]/tb[j])-1)));
        }
        if (time_steps[i] >= tb[j] & time_steps[i]<tv[j]) //konstante Geschwindigkeit
        {
            Stuetz.z.push_back(start.w.z+dir[j]*vm[j]*(time_steps[i]-0.5*tb[j]));
        }
        if (time_steps[i] >= tv[j]) //Bremsphase
        {
            Stuetz.z.push_back(start.w.z+dir[j]*bm[j]*0.5*(te[j]*(time_steps[i]+tb[j])     -0.5*(pow(time_steps[i],2)+pow(te[j],2)+2*pow(tb[j],2))    +pow(tb[j],2)/(4*pow(M_PI,2))    *(1-cos(2*M_PI*(time_steps[i]-tv[j])/tb[j]))    ));
        }
    }






    return Stuetz;

}