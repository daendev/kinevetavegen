#include "knavvezerles.h"

static int* hozzaad(int* tomb, int ezt){
    if(tomb==NULL){ // ha kezdetben üres a tömb
        tomb = malloc(sizeof(int));
        tomb[0] = -1; // tesz bele egy végjelet
    }
    int meret=0;
    while(tomb[meret++]!=-1);  // végjel segítségével megszámolja, hány elemű a tömb
    int* uj = malloc((meret+1)*sizeof(int)); // foglal egy 1-gyel nagyobbat
    int i;
    for(i=0;i<meret-1;i++){
        uj[i] = tomb[i]; // bemásolja az újba az elejétől a -1 előtti elemeket
    }
    uj[i] = ezt; // a végjel elé, de a többi után beteszi az új elemet
    uj[i+1] = -1; // lezárja végjellel a tömböt
    free(tomb);
    return uj;
}

static int* dob(int* tomb){
    return hozzaad(tomb,rand()%6+1);
}

void auto_dob(Jatekadatok* jatek, SDL_Surface* screen){
    bool harmat_dobhat = dobhat_harmat(*jatek);
    jatek->dobottszamok = dob(jatek->dobottszamok);
    while(dobhat_meg(jatek->dobottszamok,&harmat_dobhat)){ // amíg dobhat,
        jatek->dobottszamok = dob(jatek->dobottszamok); // addig dob
    }
}

int dobas(Jatekadatok* jatek, SDL_Surface* screen, SDL_Event* e){
    bool harmat_dobhat = dobhat_harmat(*jatek);

    Pont kurzor;
    Gomb* gombkatt;
    while(SDL_WaitEvent(e) && e->type != SDL_QUIT){
        switch(e->type){
            case SDL_MOUSEMOTION:
                kurzor = (Pont) {e->motion.x,e->motion.y};
                jatekter_kirajz(kurzor,screen,*jatek);
                szoveg_kiir(screen,*jatek, "DOBAS");
                gombok_kirajz(kurzor,screen,jatek->gomblista);
                SDL_Flip(screen);
                break;
            case SDL_MOUSEBUTTONDOWN:
                gombkatt = gombra_katt(jatek->gomblista,kurzor);
                if(gombkatt==NULL) break; // nem kattintottunk gombra
                switch(gombkatt->tipus){
                    case dobokocka:
                        jatek->dobottszamok = dob(jatek->dobottszamok);
                        jatekter_kirajz(kurzor,screen,*jatek);
                        szoveg_kiir(screen,*jatek, "DOBAS");
                        gombok_kirajz(kurzor,screen,jatek->gomblista);
                        SDL_Flip(screen);
                        if(!dobhat_meg(jatek->dobottszamok, &harmat_dobhat)) return 0; // nem enged tovább dobni, ha a szabályok szerint nem dobhatunk már többet
                        break;
                    case vissza:
                        return 2;
                    default: break;
                }
                break;
        }
    }
    return 1;
}

static void bazisra(Babu* b, Mezo** starttomb){
    int j;
    Mezo* erre;
    erre = starttomb[b->szin];
    for(j=0;j<4;j++){ // megkeressük az első üres bázismezőt a listában
        if(erre->rajta == NULL){
                erre->rajta = b; // és ráhelyezzük a bábut
                b->bazison = true;
                break;
        }
        erre = erre->next;
    } // garantáltan lesz üres bázismező, mert 4 egyszínű bábu van és 4 bázismező
}

static void lep(Mezo* innen, int ennyit, Mezo** starttomb){
    int i;
    Mezo* m = innen;
    for(i=0;i<ennyit;i++){
        if(elagaz(m,innen->rajta->szin)){
            m = m->elag; // ha talál a bábuval egyező színű elágazást, akkor arra fog továbbmenni
            innen->rajta->celban = true;
        }
        else m = m->next;
    }
    if(m->rajta != NULL){ // ha a célmezőn már tartózkodik egy bábu, kiüti azt
        bazisra(m->rajta, starttomb);
    }
    m->rajta = innen->rajta;
    innen->rajta = NULL;
}

static bool jatekos_lephet(Jatekadatok* jatek, int ennyit){
    Mezo* ptr = keres(jatek->starttomb[jatek->jatekosok->szin],4); // startmezőtől kezdve
    while(ptr!=NULL){ // a játékos által bejárható pálya végéig
        // ha talál olyan vele azonos színű bábut, amely tud lépni, igaz
        if(ptr->rajta!=NULL && ptr->rajta->szin==jatek->jatekosok->szin && lephet(ptr,ennyit)) return true;
        if(elagaz(ptr,jatek->jatekosok->szin)) ptr = ptr->elag;
        else ptr = ptr->next;
    }
    return false; // ha lefutott a ciklus, nem találtunk ilyen bábut
}

static bool probal_lepni(Jatekadatok* jatek, Pont kurzor, int ennyit){
    bool a = ennyit==6;
    bool b = bazis_ures(*jatek);
    bool c = jatekos_lephet(jatek,ennyit);
    if((!a && !c) || (a && b && !c)) return true; // Boole algebrával a szabályok szerint összeollózott feltétel, amely igaz, ha a játékos a dobott számmal nem tud mit kezdeni

    Mezo* kattintott = mezore_katt(kurzor.x,kurzor.y,jatek->starttomb);
    // ha hatost dobtunk és mezőre kattintottunk és van rajta bábu és az a bázison van és olyan színű mint a játékos és a startmező üres
    if(kattintott!=NULL && ennyit==6 && kattintott->rajta!=NULL && kattintott->rajta->szin==jatek->jatekosok->szin && kattintott->rajta->bazison && keres(jatek->starttomb[jatek->jatekosok->szin],4)->rajta==NULL){
        kattintott->rajta->bazison = false;
        keres(jatek->starttomb[jatek->jatekosok->szin],4)->rajta = kattintott->rajta; // a bábut a startmezőre helyezi
        kattintott->rajta = NULL;
        return true;
    }
    // mezőre kattintottunk, az nem üres és saját bábu van rajta és az nincs a bázison és léphet
    if(kattintott!=NULL && kattintott->rajta!=NULL && kattintott->rajta->szin==jatek->jatekosok->szin && !kattintott->rajta->bazison && lephet(kattintott,ennyit)){
        lep(kattintott,ennyit,jatek->starttomb);
        return true;
    }
    return false; // ha egyik feltétel sem teljesül, nem tudtunk lépni
}

void auto_lep(Jatekadatok* jatek, SDL_Surface* screen){
    int meret=0;
    while(jatek->dobottszamok[meret++]!=-1);
    int i=0;
    if(meret>2){ // ha több mint 1 szám van a tömbben
        while(jatek->dobottszamok[i++]!=6){ // megkeresi az első hatost
            if(i==meret) break;
        }
        i--; // i index legyen ezen a hatoson
    }
    if(jatek->dobottszamok[i]==-1) return; // 3 dobásból nem dobott 6-ost
    // az index jelenleg a tömb első elemére mutat
    while(i<meret-1){ // addig lép, amíg van szám a tömbben
        Mezo* lephetnek[5] = {NULL}; // 5 elemű tömb, hogy ha mind a 4 bábu benne van, legyen egy NULL végjel
        int j=0;
        Mezo* ptr;
        if(jatek->dobottszamok[i]==6 && keres(jatek->starttomb[jatek->jatekosok->szin],4)->rajta==NULL){ // ha 6-ost dobott és a startmező üres
            ptr = jatek->starttomb[jatek->jatekosok->szin];
            while(ptr!=keres(jatek->starttomb[jatek->jatekosok->szin],4)){ // a startmezőig
                if(ptr->rajta!=NULL){ // ha van rajta bábu
                    lephetnek[j++] = ptr; // bele a tömbbe
                }
                ptr = ptr->next;
            }
        }
        ptr = keres(jatek->starttomb[jatek->jatekosok->szin],4); // a startmezőtől
        while(ptr!=NULL){ // a bejárható pálya végéig
            // ha van rajta bábu és az ugyanolyan színű mint a játékos és az léphet
            if(ptr->rajta!=NULL && ptr->rajta->szin==jatek->jatekosok->szin && lephet(ptr,jatek->dobottszamok[i])) lephetnek[j++] = ptr; // bele a tömbbe
            if(elagaz(ptr,jatek->jatekosok->szin)) ptr = ptr->elag; // ha van azonos színű elágazás, bemegy
            else ptr = ptr->next; // ha nincs, következő mező
        }
        // a tömbben most mező(k)re mutató pointerek vannak
        if(lephetnek[0]==NULL) break;// ha a tömb első eleme NULL, az azt jelenti, hogy nincs olyan bábu, amivel lépni tud
        int k=0;
        while(lephetnek[k]!=NULL && k++<4); // megszámolja, hány mezőről tud lépni
        int ez = rand()%k; // random mező kiválasztása a tömbből
        if(lephetnek[ez]->rajta->bazison){ // ha a kiválasztott bábu a bázison van
            lephetnek[ez]->rajta->bazison = false;
            keres(jatek->starttomb[jatek->jatekosok->szin],4)->rajta = lephetnek[ez]->rajta; // a bábut a startmezőre helyezi
            lephetnek[ez]->rajta = NULL;
        }
        else lep(lephetnek[ez],jatek->dobottszamok[i],jatek->starttomb);
        i++; // sikeres lépés után dobott szám index növelés
    }
}

int lepes(Jatekadatok* jatek, SDL_Surface* screen, SDL_Event* e){
    int meret=0;
    while(jatek->dobottszamok[meret++]!=-1);
    int i=0;
    if(meret>2){ // ha több mint 1 szám van a tömbben
        while(jatek->dobottszamok[i++]!=6){ // megkeresi az első hatost
            if(i==meret) break;
        }
        i--; // i index legyen ezen a hatoson
    }
    if(jatek->dobottszamok[i]==-1) return 0; // 3 dobásból nem dobott 6-ost
    bool lepett = false;

    Pont kurzor;
    Gomb* gombkatt;
    while(SDL_WaitEvent(e) && i<meret-1 && e->type != SDL_QUIT){ // addig várunk eseményre, amíg el nem fogynak a dobott számok, vagy ki nem lépünk
        switch(e->type){
            case SDL_MOUSEMOTION:
                kurzor = (Pont) {e->motion.x,e->motion.y};
                jatekter_kirajz(kurzor,screen,*jatek);
                szoveg_kiir(screen,*jatek, "LEPES");
                gombok_kirajz(kurzor,screen,jatek->gomblista);
                SDL_Flip(screen);
                break;
            case SDL_MOUSEBUTTONDOWN:
                gombkatt = gombra_katt(jatek->gomblista,kurzor);
                if(gombkatt==NULL){ // nem gombra kattintottunk, ezért talán mezőre
                    lepett = probal_lepni(jatek,kurzor,jatek->dobottszamok[i]); // megkísérelünk egy lépést a kattintás helye alapján
                } else switch(gombkatt->tipus){
                    case vissza:
                        return 2;
                    default: break;
                }
                if(lepett){
                    i++;
                    jatekter_kirajz(kurzor,screen,*jatek);
                    szoveg_kiir(screen,*jatek, "LEPES");
                    gombok_kirajz(kurzor,screen,jatek->gomblista);
                    SDL_Flip(screen);
                }
                break;
            case SDL_QUIT:
                return 1;
        }
    }
    return 0;
}
