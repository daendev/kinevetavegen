# Ki nevet a végén
[Ki Nevet A Végén társasjáték](https://hu.wikipedia.org/wiki/Ki_nevet_a_v%C3%A9g%C3%A9n%3F_(t%C3%A1rsasj%C3%A1t%C3%A9k)) C nyelven.

Nagy házi feladat BME-VIK A programozás alapjai 1 tárgyhoz.

### Fordítás
A fordításhoz SDL1.2 library szükséges.
```
sudo apt-get install libsdl1.2-dev libsdl-gfx1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev libsdl-mixer1.2-dev
```

Fordítás (gyökérkönyvtárból):
```
gcc src/*.c -o main `sdl-config --cflags --libs` -lSDL_gfx -lSDL_ttf -lSDL_image -lSDL_mixer -lm
```

### Doksik

 * [Specifikáció](https://drive.google.com/file/d/1BPEMa8lFf7MDAmwCSr6ZSrloymGyCCua/view?usp=sharing)
 * [Tesztelési dokumentáció](https://drive.google.com/file/d/13TvX5Bp6_FOi-RN4ORit-a-I5Pul6Bah/view?usp=sharing)
 * [Felhalsználói dokumentáció](https://drive.google.com/file/d/1Kvbf1GCfa8I1iT-qsLuNQd_iv0XBI7IE/view?usp=sharing)
 
 
