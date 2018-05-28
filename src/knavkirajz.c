#include "knavkirajz.h"

double tavolsag(Pont a, Pont b){
    int x = a.x-b.x;
    int y = a.y-b.y;
    return sqrt(x*x+y*y);
}

char* szin2string(Szin sz){
    switch(sz){
        case 0: return "KEK";
        case 1: return "SARGA";
        case 2: return "PIROS";
        case 3: return "ZOLD";
        default: return NULL;
    }
}

int milyen_szin(Szin sz){
    switch(sz){
    case kek: return 0x0000FFFF;
    case sarga: return 0xFFFF00FF;
    case piros: return 0xFF0000FF;
    case zold: return 0x00FF00FF;
    default: return -1;
    }
}

Pont poziciot_pontba(Pozicio p){
    return (Pont) {(p.oszlop+1)*40-10,(p.sor+1)*40-10}; // kirajzolásnál és kattintás érzékelésénél lesz majd fontos
}

static bool kurzor_mezon(Pont kurzor, Mezo m){
    return tavolsag(kurzor,poziciot_pontba(m.poz))<15;
}

static void mezo_kirajz(SDL_Surface* screen, Mezo m, bool hover){
    Pont kp = poziciot_pontba(m.poz); // a mező középpontja
    if(m.rajta == NULL){ // üres mezõ
        if(m.szin == semleges)
            circleColor(screen,kp.x,kp.y,15,0x000000C0); // semleges
        else filledCircleColor(screen,kp.x,kp.y,15,milyen_szin(m.szin)-0x9F); // színes - a 0x9F kivonás a színkódból halványítja a színt
    } else {
        filledCircleColor(screen,kp.x,kp.y,15,
                          milyen_szin(m.rajta->szin)); // ha bábu is van rajta
    }
    if(hover){ // ha a kurzor a mezőn van
        filledCircleColor(screen,kp.x,kp.y,15,0x0000002F); // áttetsző szürke átfedést helyez rá
    }
}

static void palyareszlet_kirajz(SDL_Surface* screen, Mezo* kezd, Pont kurzor){
    while(kezd != NULL){
        Mezo m = *kezd;
        mezo_kirajz(screen,m,kurzor_mezon(kurzor,m));
        if(kezd->elag != NULL) kezd = kezd->elag;
        else kezd = kezd->next;
    }
}

static void palya_kirajz(SDL_Surface* screen, Mezo** starttomb,Pont kurzor){
    int i;
    for(i=0;i<4;i++){
        palyareszlet_kirajz(screen,starttomb[i],kurzor);
    }
}

static void gomb_kirajz(SDL_Surface* screen, Gomb g, bool kurzor){
    int szin = kurzor ? 0x0000007F : 0x000000FF; // a gomb szürke vagy fekete attól függően, hogy rajta van-e a kurzor
    rectangleColor(screen,g.hely.x,g.hely.y,g.hely.x+g.szelesseg,g.hely.y+g.magassag,szin);
    stringColor(screen,g.hely.x+12,g.hely.y+8,g.szoveg,szin);
}

void gombok_kirajz(Pont kurzor, SDL_Surface* screen, Gomb* gomblista){
    while(gomblista!=NULL){
        gomb_kirajz(screen,*gomblista,kurzor_gombon(*gomblista,kurzor.x,kurzor.y));
        gomblista = gomblista->kov;
    }
}

static void szamok_kiir(SDL_Surface* screen, int* tomb){
    if(tomb==NULL) return;
    int x = 478;
    int y = 387; // ebbe a porízióba, a dobókocka gomb mellé
    int i = 0;
    while(tomb[i]!=-1){
        char szam[2];
        sprintf(szam,"%d",tomb[i]);
        stringColor(screen,x,y,szam,0x000000FF);
        x+=10; // a következő számot 10 pixellel jobbra
        i++;
    }
}

void szoveg_kiir(SDL_Surface* screen, Jatekadatok j, char* dobasvlepes){
    int x = 220;
    int y = 306; // kb a pálya közepébe, jól látható helyre
    char* jatekos_szine = szin2string(j.jatekosok->szin);
    char szoveg[20];
    sprintf(szoveg,"%s %s",jatekos_szine,dobasvlepes);
    stringColor(screen,x,y,szoveg,0x000000FF);
}

static void rangsor_kirajz(SDL_Surface* screen, Szin* rangsor){
    int x = 10;
    int y = 10;
    int i;
    for(i=0;i<4;i++){
        if(rangsor[i]!=semleges){ // a célba nem ért játékosokat még nem kell kiírni
            char szoveg[10];
            sprintf(szoveg,"%d. %s",i+1,szin2string(rangsor[i]));
            stringColor(screen,x,y,szoveg,0x000000FF);
            y+=10;
        }
    }
}

void jatekter_kirajz(Pont kurzor, SDL_Surface* screen, Jatekadatok j){
    boxColor(screen,0,0,619,619,0xFFFFFFFF);
    palya_kirajz(screen,j.starttomb,kurzor);
    szamok_kiir(screen,j.dobottszamok);
    rangsor_kirajz(screen,j.rangsor);
}

void fomenu_kirajz(Pont kurzor,SDL_Surface* screen,bool* gepijatekosok){
    int x = 255;
    int y = 268; // a hozzájuk tartozó gombok elé
    int i;
    boxColor(screen,0,0,619,619,0xFFFFFFFF);
    for(i=0;i<4;i++){
        if(gepijatekosok[i]) stringColor(screen,x,y,"AI",0x000000FF); // ha az adott játékos gépi ellenfél, azt a gomb elé írja
        y+=30;
    }
}
