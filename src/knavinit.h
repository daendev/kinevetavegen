#ifndef KNAVINIT_H_INCLUDED
#define KNAVINIT_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    kek, sarga, piros, zold, semleges
} Szin;

typedef enum {
    fel, jobbra, le, balra
} Irany;

typedef enum {
    start, ai, vissza, dobokocka, szabaly
} Gombtipus;

typedef struct {
    int x;
    int y;
} Pont;

typedef struct {
    int oszlop;
    int sor;
} Pozicio;

typedef struct {
    Szin szin;
    bool bazison;
    bool celban;
} Babu;

typedef struct Mezo {
    Szin szin;
    Pozicio poz; // az inicializ�l�st megk�nny�ti a n�gyzetr�cs szerinti t�rol�s. A poz�ci� k�nnyen koordin�t�v� alak�that�.
    Babu* rajta;
    struct Mezo* next;
    struct Mezo* elag; // el�gaz�s
} Mezo;

typedef struct Jatekos {
    Szin szin;
    bool gepi;
    bool nyert;
    Babu* babuk[4];
    struct Jatekos* kovetkezo;
} Jatekos;

typedef struct Gomb {
    Pont hely;
    int szelesseg;
    int magassag;
    char* szoveg;
    Szin szin; // men�h�z k�nnyebb vez�rl�s �rdek�ben, m�s szempontb�l nem l�nyeges
    Gombtipus tipus;
    struct Gomb* kov;
} Gomb;

typedef struct {
    Mezo* starttomb[4];
    Jatekos* jatekosok;
    Gomb* gomblista;
    int* dobottszamok;
    Szin rangsor[4];
} Jatekadatok;


Mezo* keres(Mezo* innen, int hanyadikat);
Gomb gomb_letrehoz(Pont p,char* szov,Szin sz,Gombtipus t);
void gomb_listahoz_ad(Gomb** lista, Gomb g);
Jatekadatok jatek_init();
void gombok_felszabadit(Gomb* gomblista);
void mindent_felszabadit(Jatekadatok jatek);

#endif
