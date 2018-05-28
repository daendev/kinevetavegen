#ifndef KNAVKIRAJZ_H_INCLUDED
#define KNAVKIRAJZ_H_INCLUDED
#include <math.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include "knavinit.h"
#include "knavbool.h"

double tavolsag(Pont a, Pont b);
Pont poziciot_pontba(Pozicio p);
int milyen_szin(Szin sz);
void jatekter_kirajz(Pont kurzor, SDL_Surface* screen, Jatekadatok j);
void szoveg_kiir(SDL_Surface* screen, Jatekadatok j, char* dobasvlepes);
char* szin2string(Szin sz);
void gombok_kirajz(Pont kurzor, SDL_Surface* screen, Gomb* gomblista);
void fomenu_kirajz(Pont kurzor,SDL_Surface* screen,bool* gepijatekosok);

#endif
