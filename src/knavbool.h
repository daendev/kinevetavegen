#ifndef KNAVBOOL_H_INCLUDED
#define KNAVBOOL_H_INCLUDED

#include "knavinit.h"
#include "knavkirajz.h"

bool elagaz(Mezo* m, Szin sz);
Mezo* mezore_katt(int x, int y, Mezo** starttomb);
bool megy_a_jatek(Jatekos* jatekoslista);
bool lephet(Mezo* innen, int ennyit);
bool dobhat_harmat(Jatekadatok jatek);
bool dobhat_meg(int* szamok, bool* harmat_dobhat);
bool jatekos_nyert(Jatekadatok* jatek);
bool kurzor_gombon(Gomb g, int x, int y);
Gomb* gombra_katt(Gomb* gomblista, Pont kurzor);
bool bazis_ures(Jatekadatok jatek);

#endif
