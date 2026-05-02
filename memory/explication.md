# Guide de Modding Twilight Princess — Pour toi

Ce guide explique tout ce qu'on a mis en place, comment ça marche, et comment modifier/ajouter des choses.

## Lexique

| Terme | C'est quoi |
|-------|------------|
| **DOL** | Le fichier exécutable principal du jeu GameCube. Equivalent d'un `.exe` sur PC. Contient tout le code du moteur. Nom : `main.dol` ou `framework.dol`. |
| **REL** | Relocatable Module. Un plugin chargé dynamiquement par le jeu. Les acteurs (ennemis, objets, NPCs) sont des RELs. On peut créer nos propres RELs = nos propres mods. |
| **GCI** | GameCube Image. Format de fichier sauvegarde/données sur la memory card GameCube. Notre mod REL est emballé dans un GCI pour être lu depuis la memory card virtuelle. |
| **ISO** | Image disque complète du jeu. Contient le DOL + tous les fichiers (textures, modèles, sons, RELs). |
| **CISO** | Compressed ISO. Un ISO compressé où les blocs vides sont retirés. Plus petit mais même contenu. |
| **FST** | File System Table. La table des fichiers dans l'ISO — dit où chaque fichier est stocké sur le disque. |
| **Gecko Code** | Code de triche pour GameCube/Wii. Patchs mémoire appliqués au runtime par Dolphin. Le REL Loader est un Gecko code. |
| **REL Loader** | Un Gecko code spécifique qui, au démarrage du jeu, lit un fichier REL depuis la memory card et le charge en mémoire. C'est le pont entre notre mod et le jeu. |
| **devkitPPC** | Le compilateur C/C++ pour PowerPC (le processeur du GameCube). Installé via devkitPro. Produit du code machine que le GameCube comprend. |
| **libtp_rel** | Bibliothèque C++ créée par la communauté TP (zsrtp). Fournit des headers pour accéder aux données du jeu (vie, items, manette, etc.) et des outils (hook, console). |
| **Hook** | Technique qui remplace le début d'une fonction du jeu par un saut vers ton code. Ton code s'exécute, puis appelle la fonction originale. Le jeu ne sait pas qu'il a été modifié. |
| **Trampoline** | La fonction "retour" créée par le hook. Quand tu appelles `return_fapGm_Execute()`, ça exécute la vraie `fapGm_Execute` originale. |
| **Code cave** | Espace libre (zéros) dans le DOL qu'on peut utiliser pour écrire du code. Adresse : `0x80004D18`, 1228 bytes. |
| **fapGm_Execute** | La fonction du jeu appelée à chaque frame (60 fps). C'est là qu'on se hook pour exécuter notre mod. Adresse : `0x80018A6C`. |
| **PowerPC (PPC)** | Le processeur du GameCube. Architecture big-endian, RISC, 32-bit. Très différent du x86 de ton PC. |
| **Big-endian** | Les octets sont stockés dans l'ordre inverse du PC. `0x1234` est stocké `12 34` sur GameCube mais `34 12` sur PC. |

## Comment ça marche (vue d'ensemble)

```
Toi: écris du C++ dans mod_menu_rel/source/main.cpp
  │
  ▼
devkitPPC: compile ton C++ en PowerPC (make us)
  │
  ▼
REL.us.gci: ton mod emballé pour la memory card
  │
  ▼
Dolphin: au boot, le Gecko code "REL Loader" lit le GCI
  │        depuis la memory card et charge ton code en RAM
  ▼
Ton hook: remplace fapGm_Execute par ta fonction
  │        qui s'exécute 60x par seconde
  ▼
Le jeu: tourne normalement + ton mod par-dessus
```

## Les fichiers importants

| Fichier | Où | Rôle |
|---------|-----|------|
| `mod_menu_rel/source/main.cpp` | Ton code | Le code du mod. C'est LE fichier que tu modifies. |
| `mod_menu_rel/include/main.h` | Header | Déclaration de la classe Mod et ses méthodes. |
| `mod_menu_rel/assets/us.lst` | Symboles | Liste des adresses mémoire du jeu US. Format: `ADRESSE:NOM`. Si tu as besoin d'appeler une fonction du jeu, ajoute son adresse ici. |
| `mod_menu_rel/Makefile` | Build | Le script de compilation. `make us` compile pour GCN US. |
| `mod_menu_rel/REL.us.gci` | Sortie | Le mod compilé, prêt à copier sur la memory card Dolphin. |
| `mod_menu_rel/externals/libtp_rel/` | Lib | La bibliothèque TP. Tu n'as pas besoin de la modifier. |
| `TP_Extracted/` | Jeu | Le jeu extrait. Dolphin le lance depuis `TP_Extracted/sys/main.dol`. |
| `~/Library/Application Support/Dolphin/GameSettings/GZ2E01.ini` | Dolphin config | Contient le Gecko code REL Loader. Ne pas toucher. |
| `~/Library/Application Support/Dolphin/GC/USA/Card A/` | Memory card | Où copier le `REL.us.gci`. |

## Comment modifier le mod

### 1. Edite le code
```
Ouvre: mod_menu_rel/source/main.cpp
```
Le point d'entrée est `Mod::procNewFrame()` — appelé chaque frame.

### 2. Compile
```bash
cd mod_menu_rel
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=$DEVKITPRO/devkitPPC
export PATH=$DEVKITPPC/bin:$PATH
make clean && make us
```

### 3. Déploie
```bash
cp REL.us.gci ~/Library/Application\ Support/Dolphin/GC/USA/Card\ A/
```

### 4. Teste
Relance `TP_Extracted/sys/main.dol` dans Dolphin.

### Raccourci tout-en-un
```bash
cd mod_menu_rel && export DEVKITPRO=/opt/devkitpro && export DEVKITPPC=$DEVKITPRO/devkitPPC && export PATH=$DEVKITPPC/bin:$PATH && make clean && make us && cp REL.us.gci ~/Library/Application\ Support/Dolphin/GC/USA/Card\ A/ && echo "DONE"
```

## Comment ajouter un nouveau cheat

Exemple: ajouter "Z + L = vitesse x2"

### Étape 1 : Trouver l'adresse de la donnée
Cherche dans le projet decomp (`config/GZ2E01/symbols.txt`) ou dans `libtp_rel/include/tp/`:
```bash
grep "speed\|velocity" config/GZ2E01/symbols.txt
grep "speed\|velocity" libtp_rel/include/tp/*.h
```

### Étape 2 : Ajouter le code dans procNewFrame()
```cpp
if ((hold & Button_Z) && (trig & Button_L))
{
    // Ton cheat ici
}
```

### Étape 3 : Si tu as besoin d'une fonction du jeu pas dans us.lst
Trouve son adresse dans `config/GZ2E01/symbols.txt`, puis ajoute-la dans `mod_menu_rel/assets/us.lst`:
```
80XXXXXX:maFonction
```

## Comment ajouter une catégorie au menu

1. Ajoute le nom dans `catNames[]`
2. Crée un tableau d'options (ex: `static const char* mesOpts[] = {...}`)
3. Ajoute le count dans `optCounts[]`
4. Ajoute le tableau dans `optNames[]`
5. Incrémente `NUM_CATS`
6. Ajoute un `case` dans `executeAction()`

## Les données du joueur accessibles

```cpp
auto& s = dComIfG_gameInfo.save.save_file.player.player_status_a;
s.maxHealth        // u16 — max vie (en demi-coeurs, 40 = 20 coeurs)
s.currentHealth    // u16 — vie actuelle
s.currentRupees    // u16 — rupees
s.currentWallet    // enum — WALLET(0), BIG_WALLET(1), GIANT_WALLET(2)
s.maxLanternOil    // u16
s.currentLanternOil // u16

auto& rec = dComIfG_gameInfo.save.save_file.player.player_item_record;
rec.bow_ammo        // u8 — flèches
rec.bomb_bag_1_ammo // u8 — bombes sac 1
rec.bomb_bag_2_ammo // u8 — bombes sac 2
rec.bomb_bag_3_ammo // u8 — bombes sac 3

auto& mx = dComIfG_gameInfo.save.save_file.player.player_item_max;
mx.max_arrow_capacity  // u8
mx.max_bomb_capacity   // u8
```

## Les boutons de la manette

```cpp
// Tenus ce frame
uint32_t hold = cpadInfo[PAD_1].mButtonFlags;
// Pressés CE frame (pas le précédent)
uint32_t trig = cpadInfo[PAD_1].mPressedButtonFlags;

// Masques
Button_A          = 0x0100
Button_B          = 0x0200
Button_X          = 0x0400
Button_Y          = 0x0800
Button_Z          = 0x0010
Button_L          = 0x0040
Button_R          = 0x0020
Button_Start      = 0x1000
Button_DPad_Up    = 0x0008
Button_DPad_Down  = 0x0004
Button_DPad_Left  = 0x0001
Button_DPad_Right = 0x0002
```

## Donner des items

```cpp
#include <tp/d_item.h>
#include <data/items.h>
using namespace libtp::tp::d_item;
using namespace libtp::data::items;

execItemGet(Boomerang);      // 0x40
execItemGet(Heros_Bow);      // 0x43
execItemGet(Clawshot);       // 0x44
execItemGet(Spinner);        // 0x41
execItemGet(Lantern);        // 0x48
execItemGet(Iron_Boots);     // 0x45
execItemGet(Ball_and_Chain); // 0x42
execItemGet(Empty_Bottle);   // 0x60
// Voir libtp_rel/include/data/items.h pour la liste complète
```

## Console (affichage texte)

```cpp
#include <display/console.h>

// Montrer/cacher la console (25 lignes max)
libtp::display::setConsole(true, 25);   // afficher
libtp::display::setConsole(false, 25);  // cacher

// Ecrire du texte sur une ligne (0-24)
libtp::display::print(0, "Ligne 0");
libtp::display::print(1, "Ligne 1");

// Effacer toutes les lignes
libtp::display::clearConsole(0, 0);

// Couleur de la console (RGBA)
libtp::display::setConsoleColor(255, 255, 255, 200);
```

## Spawn d'ennemis

```cpp
#include <tp/f_op_actor_mng.h>

float pos[3] = {x, y, z};      // position
int16_t rot[3] = {0, 0, 0};    // rotation
float scale[3] = {1, 1, 1};    // échelle

libtp::tp::f_op_actor_mng::fopAcM_create(
    enemyProfileId,  // ex: 0x00EF (Bokoblin)
    0,               // params (0 = défaut)
    pos,             // position
    0,               // room (-1 = courant)
    rot,             // rotation
    scale,           // échelle
    -1               // argument
);
```

### IDs d'ennemis courants
| ID | Ennemi |
|----|--------|
| 0x00EF | Bokoblin |
| 0x01B7 | Bulblin |
| 0x01C5 | Deku Baba |
| 0x01BF | Skulltula |
| 0x00F5 | Chuchu |
| 0x01CF | Helmasaur |
| 0x01E0 | Kargarok |
| 0x01BD | Stalfos |

## Si ça crash

Suis cet ordre pour trouver le problème :

1. **Jeu propre sans rien** → doit marcher. Sinon, re-extraire le CISO (`python3 tools/extract_ciso.py`)
2. **Gecko code REL Loader sans GCI** → doit marcher. Sinon, le Gecko code est mauvais.
3. **GCI avec hook vide** (procNewFrame ne fait que appeler return_fapGm_Execute) → doit marcher. Sinon, problème de build.
4. **Ajouter les features une par une** → trouver laquelle crash.

## Commandes utiles

```bash
# Extraire le CISO (une seule fois)
python3 tools/extract_ciso.py "orig/GZ2E01/Legend of Zelda, The - Twilight Princess (USA).ciso" TP_Extracted

# Compiler le mod
cd mod_menu_rel && export DEVKITPRO=/opt/devkitpro && export DEVKITPPC=$DEVKITPRO/devkitPPC && export PATH=$DEVKITPPC/bin:$PATH && make clean && make us

# Déployer
cp mod_menu_rel/REL.us.gci ~/Library/Application\ Support/Dolphin/GC/USA/Card\ A/

# Chercher une adresse dans les symboles du jeu
grep "motClef" config/GZ2E01/symbols.txt

# Chercher un header dans libtp_rel
grep -r "motClef" libtp_rel/include/

# Voir les items disponibles
cat libtp_rel/include/data/items.h
```

## Ressources

- [zsrtp/REL Template](https://github.com/zsrtp/REL) — Le template qu'on utilise
- [zsrtp/libtp_rel](https://github.com/zsrtp/libtp_rel) — La lib d'accès au jeu
- [zsrtp/Randomizer](https://github.com/zsrtp/Randomizer) — Exemple complet de mod (le randomizer TP)
- [REL Loader Wiki](https://wiki.tprandomizer.com/index.php?title=REL_Loader) — Doc du REL Loader
- [TP Modding Discord](https://discord.gg/aZx8ZFcSPy) — Communauté de modding TP
- [zeldaret/tp](https://github.com/zeldaret/tp) — Le projet de décompilation
