#include "knavinit.h"

/* PÁLYAFELÉPÍTÉST SEGÍTŐ FÜGGVÉNYEK */

Mezo* keres(Mezo* innen, int hanyadikat){
    int i;
    Mezo* ptr = innen;
    for(i=0;i<hanyadikat;i++){ // nem kezeli a pálya végi esetet, a játék jelenlegi verziójában ez felesleges
        ptr = ptr->next;
    }
    return ptr;
}

static Pozicio iranyba_lep(Irany d, Pozicio p, int ennyit){
    switch(d){
    case fel: p.sor-=ennyit; break;
    case le: p.sor+=ennyit; break;
    case jobbra: p.oszlop+=ennyit; break;
    case balra: p.oszlop-=ennyit; break;
    }
    return p;
}

static void forgat(Irany* d, int meret){
    int i;
    for(i=0;i<meret;i++){
        if(d[i]==balra) d[i] = fel;
        else d[i]++;
    }
}

/* BÁBUKAT INICIALIZÁLÓ FÜGVÉNY */

static void babu_init(Mezo** starttomb, Jatekos* jatekosok){
    Mezo* ptr;
    int i,j;
    for(i=0;i<4;i++){ // mind a 4 színen végigmegy
        ptr = starttomb[i]; // az adott játékos pályarészletének az elejétől kezdve
        for(j=0;j<4;j++){ // mind a 4 bázismezőre helyez egy-egy bábut
            Babu* b = (Babu*) malloc(sizeof(Babu));
            b->szin = i;
            b->bazison = true;
            b->celban = false;
            ptr->rajta = b;
            ptr = ptr->next;
            jatekosok->babuk[j] = b; // a bábu(ka)t a játékos(ok)hoz is hozzáadja
        }
        jatekosok = jatekosok->kovetkezo;
    }
}

/* PÁLYÁT FELÉPÍTŐ FÜGGVÉNYEK */

static Mezo* mezo_init(Pozicio p, Szin sz){
    Mezo* m = malloc(sizeof(Mezo));
    m->elag = NULL;
    m->next = NULL;
    m->poz = p;
    m->szin = sz;
    m->rajta = NULL;
    return m;
}

static Mezo* mezo_letrehoz(Mezo* ettol, Irany erre, int ennyivel, Szin sz){
    Mezo* m = (Mezo*) malloc(sizeof(Mezo));
    m->poz = iranyba_lep(erre,ettol->poz,ennyivel);
    m->szin = sz;
    m->rajta = NULL;
    m->elag = NULL;
    m->next = NULL;
    ettol->next = m;
    return m;
}

static Mezo* mezok_egymas_utan(Mezo* ettol, Irany erre, int ennyit, Szin sz){
    int i;
    for(i=0;i<ennyit;i++){
        mezo_letrehoz(ettol,erre,1,sz);
        ettol = ettol->next;
    }
    return ettol;
}

static void palyareszlet(Mezo* kezd, Irany* d, Szin c){
    kezd = mezok_egymas_utan(kezd,d[0],3,c);
    kezd = mezo_letrehoz(kezd,d[1],2,c);
    kezd = mezok_egymas_utan(kezd,d[2],6,semleges);
    kezd = mezok_egymas_utan(kezd,d[3],6,semleges);
    kezd = mezo_letrehoz(kezd,d[4],1,semleges);
    kezd->elag = mezo_letrehoz(kezd,d[5],1,c==zold?kek:c+1); // kihasználjuk, hogy a felsorolt típus integerként van tárolva
    mezok_egymas_utan(kezd->elag,d[5],3,c==zold?kek:c+1);
}

static void osszekot(Mezo* ezt, Mezo* ezzel){
    ezt->next = ezzel; // erre talán nem is kellett volna függvény de most már mindegy
}

static void palya_init(Mezo** kezdet){
    Irany iranytomb[] = {le,jobbra,fel,balra,fel,jobbra};
    int i;
    for(i=0;i<4;i++){ // létrehozza a 4 pályarészletet
        palyareszlet(kezdet[i],iranytomb,i);
        forgat(iranytomb,6);
    }
    for(i=0;i<3;i++){ // összeköti a pályarészleteket, hogy azok egy ciklikus listát alkossanak
        osszekot(keres(kezdet[i], 17),keres(kezdet[i+1],5));
    }
    osszekot(keres(kezdet[3],17), keres(kezdet[0],5));
}

/* EGYÉB INICIALIZÁLÁS */

static Jatekos* jatekosok_init(){
    int i;
    Jatekos* ptr = NULL;
    for(i=3;i>=0;i--){
        Jatekos* uj = (Jatekos*) malloc(sizeof(Jatekos));
        uj->szin = i;
        uj->gepi = false; // legyen először mindenki ember
        uj->nyert = false;
        uj->kovetkezo = ptr;
        ptr = uj;
    }
    ptr->kovetkezo->kovetkezo->kovetkezo->kovetkezo = ptr; // ciklikus lista zárása
    return ptr; // visszatér kék játékossal
}

Gomb gomb_letrehoz(Pont p,char* szov,Szin sz,Gombtipus t){
    Gomb g;
    g.hely = p;
    g.szoveg = szov;
    g.tipus = t;
    g.szelesseg = 7*strlen(szov) + 28; // a stringhez méretezi a gombot magát
    g.magassag = 21;
    g.szin = sz;
    g.kov = NULL;
    return g;
}

void gomb_listahoz_ad(Gomb** lista, Gomb g){
    Gomb* uj = (Gomb*) malloc(sizeof(Gomb));
    uj->hely = g.hely;
    uj->magassag = g.magassag;
    uj->szelesseg = g.szelesseg;
    uj->szoveg = g.szoveg;
    uj->tipus = g.tipus;
    uj->szin = g.szin;
    Gomb* temp = *lista;
    *lista = uj;
    uj->kov = temp;
}

Jatekadatok jatek_init(){
    Jatekadatok jatek;
    jatek.starttomb[0] = mezo_init((Pozicio){4,11},0);
    jatek.starttomb[1] = mezo_init((Pozicio){3,4},1);
    jatek.starttomb[2] = mezo_init((Pozicio){10,3},2);
    jatek.starttomb[3] = mezo_init((Pozicio){11,10},3); // az egyes játékosok pályarészleteinek a kezdetei
    palya_init(jatek.starttomb); // felépíti a pályát
    jatek.jatekosok = jatekosok_init(); // csinál 4 játékost
    babu_init(jatek.starttomb,jatek.jatekosok); // létrehozza és elhelyezi a bábukat
    int i;
    for(i=0;i<4;i++){
        jatek.rangsor[i] = semleges; // a rangsorban minden helyezést semlegesre állít
    }
    jatek.dobottszamok = NULL;
    jatek.gomblista = NULL;

    return jatek;
}

/* FELSZABADÍTÓ FÜGGVÉNYEK */

static void palyareszlet_felszabadit(Mezo* innen){
    while(innen != NULL){
        if(innen->rajta != NULL) free(innen->rajta); // ha van rajta bábu akkor felszabadítja azt is
        Mezo* temp;
        if(innen->elag!=NULL) temp = innen->elag; // bemegy az elágazásba, ha van
        else temp = innen->next; // egyébként következő mező
        free(innen);
        innen = temp;
    }
}

static void palya_felszabadit(Mezo** starttomb){
    int i;
    for(i=0;i<4;i++){
        palyareszlet_felszabadit(starttomb[i]);
    }
}

void gombok_felszabadit(Gomb* gomblista){
    while(gomblista!=NULL){
        Gomb* temp = gomblista->kov;
        free(gomblista);
        gomblista = temp;
    }
}

static void jatekosok_felszabadit(Jatekos* jatekoslista){
    int i;
    for(i=0;i<4;i++){ // tudjuk, hogy 4 játékos van
        Jatekos* temp = jatekoslista->kovetkezo;
        free(jatekoslista);
        jatekoslista = temp;
    }
}

void mindent_felszabadit(Jatekadatok jatek){
    palya_felszabadit(jatek.starttomb);
    gombok_felszabadit(jatek.gomblista);
    jatekosok_felszabadit(jatek.jatekosok);
}
