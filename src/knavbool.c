#include "knavbool.h"

bool elagaz(Mezo* m, Szin sz){
    return m->elag != NULL && m->elag->szin == sz;
}

bool jatekos_nyert(Jatekadatok* jatek){
    int i;
    Jatekos* j = jatek->jatekosok;
    for(i=0;i<4;i++){
        if(!(j->babuk[i]->celban)) return false; // megnézzük, hogy van-e olyan bábuja, amelyik nem ért célba még
    } // ha ez a ciklus lefutott és nem tért vissza a függvény, a játékos nyert
    i=0;
    while(jatek->rangsor[i++]!=semleges); // a rangsorban a következő ő lesz
    jatek->rangsor[i-1] = j->szin;
    return true;
}

bool megy_a_jatek(Jatekos* jatekoslista){
    int i;
    int nyertek=0;
    Jatekos* j = jatekoslista;
    for(i=0;i<4;i++){
        if(j->nyert) nyertek++;
        j = j->kovetkezo;
    }
    return nyertek<3; // a játék akkor ér véget, ha legalább 3 játékos célba ért mind a 4 bábuval
}

bool lephet(Mezo* innen, int ennyit){
    Mezo* ide = innen;
    int i;
    for(i=0;i<ennyit;i++){
        if(elagaz(ide,innen->rajta->szin)) ide = ide->elag; // ha talál a bábuval egyező színű elágazást, akkor arra fog továbbmenni
        else if(ide->next != NULL) ide = ide->next; // vizsgálja, vége van-e a listának, mielőtt továbblép
        else return false; // ha vége van a pályának, nem tud lépni
    }
    if(ide->rajta != NULL && ide->rajta->szin == innen->rajta->szin) return false; // vizsgálja, hogy a célmezőn nincs-e ugyanolyan színű bábu
    return true;
}

bool dobhat_meg(int* szamok, bool* harmat_dobhat){
    int i=0;
    while(szamok[i++]!=-1); // megszámolja a tömb méretét
    if(szamok[i-2]==6){
        *harmat_dobhat = false; // ha a 3 dobás során elsőre 6-ost dobott, többet már nem dobhat hacsak 6-os nem jön neki mégegyszer
        return true; // ha hatost dobott legutoljára, dobhat még
    }
    if(*harmat_dobhat && i<4) return true; // ha hármat dobhat és nem dobott még hármat, dobhat még
    return false; // egyébként nem
}

bool dobhat_harmat(Jatekadatok jatek){
    Jatekos jatekos = *(jatek.jatekosok); // a jelenlegi játékos
    Mezo* ptr = keres(jatek.starttomb[jatekos.szin],4); // a startmezőjétől kezdve
    int i;
    while(ptr != NULL){ // végignézzük az adott játékos által bejárható területet
        if(ptr->rajta!=NULL && ptr->rajta->szin == jatekos.szin){ // ha van rajta bábu és az olyan színű, mint a játékos
            for(i=1;i<6;i++){
                if(lephet(ptr,i)) return false; // talált olyan bábut ami tud egy adott számút lépni
            }
        }
        if(elagaz(ptr,jatekos.szin)) ptr = ptr->elag; // bemegy azonos színű elágazásba
        else ptr = ptr->next; // egyébként következő mező
    }
    return true; // ha egy bábu se tud lépni semennyit
}

Mezo* mezore_katt(int x, int y, Mezo** starttomb){
    int i;
    Pont katt = {x,y};
    Mezo* ez;
    for(i=0;i<4;i++){ // mind a 4 pályarészletet vizsgáljuk
        for(ez = starttomb[i]; ez->next != NULL; ez->elag == NULL ? ez = ez->next : (ez = ez->elag)){ // a ciklus végigmegy az összes mezőn egy pályarészletben, és a kattintás távolságát vizsgálja mindegyiktől. Ha elágazást észlel a listában, akkor bemegy az elágazásba és azt vizsgálja végig
            if(tavolsag(katt,poziciot_pontba(ez->poz)) < 15) return ez;
        }
    }
    return NULL; // a kattintás helyén nem talált mezőt
}

bool kurzor_gombon(Gomb g, int x, int y){
    return x>g.hely.x && y>g.hely.y && x<g.hely.x+g.szelesseg && y<g.hely.y+g.magassag;
}

Gomb* gombra_katt(Gomb* gomblista, Pont kurzor){
    while(gomblista!=NULL){
        if(kurzor_gombon(*gomblista,kurzor.x,kurzor.y)) return gomblista;
        gomblista = gomblista->kov;
    }
    return NULL;
}

bool bazis_ures(Jatekadatok jatek){
    Mezo* ptr = jatek.starttomb[jatek.jatekosok->szin];
    int i;
    for(i=0;i<4;i++){ // a 4 bázismezőt nézzük
        if(ptr->rajta!=NULL) return false;
        ptr = ptr->next;
    }
    return true;
}
