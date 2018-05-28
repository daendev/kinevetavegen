#ifndef KNAVVEZERLES_H_INCLUDED
#define KNAVVEZERLES_H_INCLUDED

#include "knavinit.h"
#include "knavkirajz.h"
#include "knavbool.h"

int dobas(Jatekadatok* jatek, SDL_Surface* screen, SDL_Event* e);
int lepes(Jatekadatok* jatek, SDL_Surface* screen, SDL_Event* e);
void auto_dob(Jatekadatok* jatek, SDL_Surface* screen);
void auto_lep(Jatekadatok* jatek, SDL_Surface* screen);

#endif
