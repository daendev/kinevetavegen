#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <math.h>
#include <time.h>
#include "knavinit.h"
#include "knavkirajz.h"
#include "knavvezerles.h"
#include "knavbool.h"

int fut(SDL_Surface* screen, SDL_Event* e, bool* gepiellenfelek){
    Jatekadatok jatek = jatek_init(); // létrehoz egy játékot
    int i;
    for(i=0;i<4;i++){ // beállítja a gépi ellenfeleket a bemenet szerint
        jatek.jatekosok->gepi = gepiellenfelek[i];
        jatek.jatekosok = jatek.jatekosok->kovetkezo;
    }

    // létrehozzuk a gombokat és a játék gomblistájához adjuk őket
    gomb_listahoz_ad(&jatek.gomblista,gomb_letrehoz((Pont){10,590},"VISSZA",semleges,vissza));
    gomb_listahoz_ad(&jatek.gomblista,gomb_letrehoz((Pont){380,380},"DOBOKOCKA",semleges,dobokocka));

    Pont kurzor = {0,0};
    jatekter_kirajz(kurzor,screen,jatek);
    gombok_kirajz(kurzor,screen,jatek.gomblista);
    SDL_Flip(screen); // játéktér kirajzolása
    while(megy_a_jatek(jatek.jatekosok)){ // amíg a játék tart
        if(jatek.jatekosok->nyert){ // ha egy játékos nyert
            jatek.jatekosok = jatek.jatekosok->kovetkezo; // az kimarad és jön a következő
            continue; // a ciklus következő futásakor
        }
        if(jatek.jatekosok->gepi){ // ha gépi játékoson van a sor
            auto_dob(&jatek,screen); // automatikusan dob
            auto_lep(&jatek,screen); // és lép
        } else { // ha emberi játékos következik, dob és lép
            switch(dobas(&jatek,screen,e)){ // a dobásfüggvény visszatérési értékét vizsgáljuk
                case 0: break;
                case 1: // dobásból kilépéssel térünk vissza
                    mindent_felszabadit(jatek);
                    return 1; // ilyenkor a játékszimuláció is 1-gyel tér vissza, ebből tudni fogja a főmenü, hogy a kilépést választottuk
                case 2: // dobásból "vissza" gombbal térünk vissza
                    mindent_felszabadit(jatek);
                    return 0; // ilyenkor a főmenübe megyünk vissza, persze csak a lefoglalt memóriaterület felszabadítása után
            }
            switch(lepes(&jatek,screen,e)){ // ugyanez, csak a lépés függvénnyel
                case 0: break;
                case 1:
                    mindent_felszabadit(jatek);
                    return 1;
                case 2:
                    mindent_felszabadit(jatek);
                    return 0;
            }
        }
        jatek.jatekosok->nyert = jatekos_nyert(&jatek); // megvizsgáljuk, hogy a soron lévő játékos nyert-e
        free(jatek.dobottszamok);
        jatek.dobottszamok = NULL;
        jatek.jatekosok = jatek.jatekosok->kovetkezo;
    }
    if(megy_a_jatek(jatek.jatekosok)) { // ha a játék még tart, egyértelműen a kilépés gombra nyomtunk, ennek megfelelően
        mindent_felszabadit(jatek);
        return 1; // kilépéskóddal visszatérünk
    }
    // a játéknak vége van
    for(i=0;i<4;i++){ // megkeressük azt, aki nem nyert
        if(!jatek.jatekosok->nyert) jatek.rangsor[3] = jatek.jatekosok->szin; // és beállítjuk a rangsorban a színét az utolsó helyre
        jatek.jatekosok = jatek.jatekosok->kovetkezo;
    }

    Gomb* gombkatt;
    while(SDL_WaitEvent(e) && e->type != SDL_QUIT){
        switch(e->type){
        case SDL_MOUSEMOTION:
            kurzor = (Pont) {e->motion.x,e->motion.y};
            jatekter_kirajz(kurzor,screen,jatek);
            gombok_kirajz(kurzor,screen,jatek.gomblista);
            SDL_Flip(screen); // ezek a kirajzolások továbbra is a pályát rajzolják ki, és a kurzor alatti mezőre/gombra kijelölést tesznek
            break;
        case SDL_MOUSEBUTTONDOWN:
            gombkatt = gombra_katt(jatek.gomblista,kurzor);
            if(gombkatt!=NULL && gombkatt->tipus==vissza){ // elég csak a "vissza" gombot vizsgálni, más gomb nem produkálna semmilyen változást
                mindent_felszabadit(jatek);
                return 0;
            }
            break;
        }
    }
    mindent_felszabadit(jatek);
    return 1; // ha nem "vissza" gombbal léptünk ki, de kiléptünk a ciklusból, egyértelműen a kilépés gombra kattintottunk

}

int szabalyok(SDL_Surface* screen, SDL_Event* e){
    FILE* fp;
    fp = fopen("szabalyok.txt", "rt");
    if (fp == NULL) {
        perror("szabalyok.txt megnyitása sikertelen");
        return -1;
    }

    Gomb* gombok = NULL;
    gomb_listahoz_ad(&gombok,gomb_letrehoz((Pont){10,590},"VISSZA",semleges,vissza));

    boxColor(screen,0,0,619,619,0xFFFFFFFF);
    int x = 40;
    int y = 100;
    int i;
    for(i=0;i<26;i++){ // 26 sornyi szabályzatot olvasunk be
        char szabalysor[100];
        fgets(szabalysor,100,fp);  // beleteszi az újsor karaktert is
        szabalysor[strlen(szabalysor)-1] = '\0'; // de nekünk az nem kell szóval átírjuk stringlezáró karakterre
        stringColor(screen,x,y,szabalysor,0x000000BF);
        y+=15;
    }
    fclose(fp);

    Pont kurzor = {0,0};
    Gomb* gombkatt = NULL;
    while(SDL_WaitEvent(e) && e->type != SDL_QUIT){
        switch(e->type){
            case SDL_MOUSEMOTION:
                kurzor = (Pont) {e->motion.x,e->motion.y};
                gombok_kirajz(kurzor,screen,gombok);
                SDL_Flip(screen);
                break;
            case SDL_MOUSEBUTTONDOWN:
                gombkatt = gombra_katt(gombok,kurzor);
                if(gombkatt!=NULL && gombkatt->tipus==vissza) return 0; // csak egy gomb van amit érzékelni kell
                break;
        }
    }
    gombok_felszabadit(gombok);
    return 1;
}

void menu(SDL_Surface* screen, SDL_Event* e){
    Gomb* gomblista = NULL;
    int x = 278;
    int y = 230;
    int i;
    gomb_listahoz_ad(&gomblista,gomb_letrehoz((Pont){x,y},"START",semleges,start));
    y+=30;
    for(i=0;i<4;i++){
        gomb_listahoz_ad(&gomblista,gomb_letrehoz((Pont){x,y},szin2string(i),i,ai));
        y+=30;
    }
    gomb_listahoz_ad(&gomblista,gomb_letrehoz((Pont){x,y},"SZABALYOK",semleges,szabaly));

    bool gepijatekosok[] = {false, true, true, true}; // alaphelyzet: a kék játékos ember, a többi gép

    Pont kurzor = {0,0};
    Gomb* gombkatt;
    while(SDL_WaitEvent(e) && e->type != SDL_QUIT){
        switch(e->type){
            case SDL_MOUSEMOTION:
                kurzor = (Pont) {e->motion.x,e->motion.y};
                fomenu_kirajz(kurzor,screen,gepijatekosok);
                gombok_kirajz(kurzor,screen,gomblista);
                SDL_Flip(screen);
                break;
            case SDL_MOUSEBUTTONDOWN:
                gombkatt = gombra_katt(gomblista,kurzor);
                if(gombkatt==NULL) break; // nem kattintottunk gombra
                switch(gombkatt->tipus){ // típus szerint vizsgáljuk a kattintott gombot
                    case start:
                        switch(fut(screen,e,gepijatekosok)){ // start gomb: elindítja a játékot
                            case 0: break;
                            case 1: return;
                        }
                        break;
                    case ai: // mesterséges "intelligencia" kiválasztása
                        gepijatekosok[gombkatt->szin] = !gepijatekosok[gombkatt->szin]; // negálja a booltömb megfelelő tagját
                        fomenu_kirajz(kurzor,screen,gepijatekosok);
                        gombok_kirajz(kurzor,screen,gomblista);
                        SDL_Flip(screen);
                        break;
                    case szabaly:
                        if(szabalyok(screen,e)!=0) return; // szabályok megjelenítése
                        break;
                    default: break;
                }
                break;
        }
    }
    gombok_felszabadit(gomblista);
}

int main(int argc, char *argv[]) {
    SDL_Event ev;
    SDL_Surface *screen;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen = SDL_SetVideoMode(620, 620, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Ki Nevet A Vegen", "Ki Nevet A Vegen");

    srand(time(NULL));

    menu(screen,&ev);

    SDL_Quit();

    return 0;
}
