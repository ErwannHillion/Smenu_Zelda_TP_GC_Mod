# Twilight Princess Decomp — Architecture Complète

## Vue d'ensemble

```
┌──────────────────────────────────────────────────────┐
│              Game Code (src/d/)                       │
│  ~800 acteurs, scènes, caméra, events, UI, menus     │
├──────────────────────────────────────────────────────┤
│         Framework (src/f_pc/, f_op/, f_ap/)           │
│  Process manager, actor lifecycle, draw tags          │
├──────────────────────────────────────────────────────┤
│            Z2AudioLib (src/Z2AudioLib/)               │
│  Audio spécifique Zelda TP                            │
├──────────────────────────────────────────────────────┤
│         JSystem (libs/JSystem/) — 618 fichiers        │
│  J3D (3D), J2D (2D), JPA (particles),                │
│  JAudio2, JKernel (heaps, archives), JMath            │
├──────────────────────────────────────────────────────┤
│      Dolphin SDK (libs/dolphin/) — 315 fichiers       │
│      Revolution SDK (libs/revolution/) — 494 fich.    │
│  GX, PAD, DVD, OS, DSP, MTX, VI, ARAM                │
├──────────────────────────────────────────────────────┤
│           PowerPC Hardware (GameCube/Wii)              │
└──────────────────────────────────────────────────────┘
```

## 1. Game Loop Principal

**Fichier** : `src/m_Do/m_Do_main.cpp`

### Boot sequence
1. `main()` — Point d'entrée, init OS, crée le thread principal
2. `main01()` — Thread principal :
   - `mDoMch_Create()` — Heaps, exception manager, RNG
   - `mDoGph_Create()` — Framebuffer, zbuffer, display lists
   - `mDoCPd_c::create()` — Controller input
   - `fapGm_Create()` — Framework du jeu
   - `fopAcM_initManager()` — Actor manager
   - `cDyl_InitAsync()` — Modules REL dynamiques

### Boucle principale (ligne 745-774)
```cpp
do {
    mDoCPd_c::read();      // Lecture des manettes
    fapGm_Execute();       // Logique de jeu (fpcM_Management)
    mDoAud_Execute();      // Audio
    debug();               // Debugger
} while (true);
```

### fapGm_Execute() décomposé
```
fpcM_Management(NULL, fapGm_After)
├── fpcDt_Handler()  — Suppression de processus
├── fpcPi_Handler()  — Gestion des priorités
├── fpcCt_Handler()  — Création de processus
├── fpcEx_Handler()  — Exécution de tous les processus actifs
└── fpcDw_Handler()  — Rendu de tous les processus
```

## 2. Système d'Acteurs (f_pc / f_op)

### Hiérarchie
- **f_pc** (Framework Process) — Système de processus générique (layers, priorities, queues)
- **f_op** (Framework Operation) — Gestion spécifique au jeu (acteurs, scènes, overlaps)

### Cycle de vie d'un acteur
1. **Creation** — `fopAcM_create(profName, params, pos, roomNo, angle, scale, arg)`
2. **Execute** — Appelé chaque frame par `fpcEx_Handler()`, met à jour état/physique
3. **Draw** — Appelé chaque frame par `fpcDw_Handler()`, rendu via J3D
4. **Delete** — `fopAcM_delete(actor)`, cleanup et désallocation

### Groupes d'acteurs
- `fopAc_ACTOR_e` — Objets génériques
- `fopAc_PLAYER_e` — Link
- `fopAc_ENEMY_e` — Ennemis
- `fopAc_ENV_e` — Environnement
- `fopAc_NPC_e` — PNJ

### Fichiers clés
- `include/f_op/f_op_actor_mng.h` — Signatures de spawn/gestion d'acteurs
- `include/f_pc/f_pc_name.h` — IDs de tous les profils (acteurs, ennemis, etc.)
- `src/f_pc/f_pc_profile_lst.cpp` — Registre des profils

## 3. Système de Rendu (GX / J3D / J2D)

### Couches de rendu
```
Game (d_drawlist.cpp) → J3D/J2D (JSystem) → GX (Dolphin SDK) → Hardware GPU
```

### Draw Buffers (d_drawlist.h — dDlst_list_c)
17 buffers de rendu par priorité :
- `DB_OPA_LIST_SKY` / `DB_XLU_LIST_SKY` — Ciel
- `DB_OPA_LIST_BG` / `DB_XLU_LIST_BG` — Background
- `DB_OPA_LIST` / `DB_XLU_LIST` — Objets 3D principaux
- `DB_OPA_LIST_DARK` / `DB_XLU_LIST_DARK` — Ombres
- `DB_OPA_LIST_ITEM3D` / `DB_XLU_LIST_ITEM3D` — Items
- `DB_LIST_2D_SCREEN` — Overlay 2D
- `DB_LIST_Z_XLU` — Translucide Z-sorted

### Système 2D (overlay)
- `mp2DOpaDrawLists[64]` — Opaque 2D (64 slots)
- `mp2DXluDrawLists[32]` — Translucide 2D (32 slots)
- `mp2DOpaTopDrawLists[16]` — Top-most 2D (16 slots, priorité max)

### Enregistrement pour le rendu 2D
```cpp
dComIfGd_set2DOpa(dDlst_base_c* dlst);     // Opaque
dComIfGd_set2DXlu(dDlst_base_c* dlst);     // Translucide
dComIfGd_set2DOpaTop(dDlst_base_c* dlst);  // Par-dessus tout
```

### Quad 2D (rectangles)
Classe `dDlst_2DQuad_c` dans `include/d/d_drawlist.h` :
```cpp
class dDlst_2DQuad_c : public dDlst_base_c {
    void init(s16 posX, s16 posY, s16 width, s16 height, GXColor& color);
    virtual void draw();
};
```

### Texte debug
```cpp
dDbVw_Report(int x, int y, const char* format, ...);  // src/d/d_debug_viewer.cpp
```

### Résolution : 640x480, origine top-left (0,0)

## 4. Input — Manette GameCube

### Constantes boutons (libs/dolphin/include/dolphin/pad.h)
```cpp
#define PAD_BUTTON_LEFT    0x0001
#define PAD_BUTTON_RIGHT   0x0002
#define PAD_BUTTON_DOWN    0x0004
#define PAD_BUTTON_UP      0x0008
#define PAD_TRIGGER_Z      0x0010
#define PAD_TRIGGER_R      0x0020
#define PAD_TRIGGER_L      0x0040
#define PAD_BUTTON_A       0x0100
#define PAD_BUTTON_B       0x0200
#define PAD_BUTTON_X       0x0400
#define PAD_BUTTON_Y       0x0800
#define PAD_BUTTON_START   0x1000
```

### API principale (include/m_Do/m_Do_controller_pad.h)
```cpp
mDoCPd_c::getTrig(PAD_1)      // Boutons pressés CE frame
mDoCPd_c::getHold(PAD_1)      // Boutons maintenus
mDoCPd_c::getTrigA/B/X/Y/Z()  // Raccourcis par bouton
mDoCPd_c::getHoldA/B/X/Y/Z()  // Raccourcis hold
mDoCPd_c::getStickX/Y(PAD_1)  // Stick analogique (-1.0 à 1.0)
```

### Combo detection
```cpp
// Exemple : B maintenu + Z pressé
if ((mDoCPd_c::getHold(PAD_1) & PAD_BUTTON_B) &&
    (mDoCPd_c::getTrig(PAD_1) & PAD_TRIGGER_Z)) {
    // Combo détecté
}
```

## 5. Audio

### Stack audio (4 couches)
```
DSP hardware → AX (mixer) → JAudio2 (81 headers) → Z2Audio (game)
```

- **Z2AudioMgr** — Manager principal, hérite de Z2SeMgr + Z2SeqMgr + Z2SceneMgr
- Heap dédiée : ~10 MB
- Assets : `/Audiores/Z2Sound.baa`, `/Audiores/Seqs/Z2SoundSeqs.arc`

## 6. Système de fichiers

### Stack I/O
```
DVD async → JKRArchive (RARC) → Décompression YAZ0 → JKRHeap
```

### Types d'archives (JKRArchive)
- `JKRDvdArchive` — Lecture depuis DVD
- `JKRAramArchive` — Lecture depuis ARAM (16 MB cache rapide)
- `JKRMemArchive` — Archive déjà en mémoire
- `JKRCompArchive` — Archive compressée

### Modes de montage
```cpp
MOUNT_MEM  = 1   // Déjà en RAM
MOUNT_ARAM = 2   // Depuis ARAM
MOUNT_DVD  = 3   // Depuis DVD
MOUNT_COMP = 4   // Compressé
```

## 7. Scènes et Stages

### Types de scènes
- `dScnPly_c` — Gameplay actif
- `dScnLogo_c` — Logo/ouverture
- `dScnMenu_c` — Menu principal
- `dScnRoom_c` — Chargement de salle

### Structure d'un stage
- **Stage** — Zone du monde (ex: "F_SP104" = Province de Firone)
- **Room** — Salle individuelle (index 0-255)
- **Layer** — Couches de placement d'acteurs
- **Multi** — Transitions multi-salles

### Fonctions clés
```cpp
dComIfGp_roomControl_getStayNo()    // Numéro de salle actuel
dComIfGp_getStartStageName()        // Nom du stage actuel
dComIfGp_setNextStage(name, room)   // Transition vers un autre stage
```

## 8. Inventaire et Sauvegarde

### Structure de sauvegarde (d_save.h)
- `dSv_player_c` — Données joueur complètes (0x1EC bytes)
  - `dSv_player_status_a_c` — Vie, rupees, oil, équipement (0x28 bytes)
  - `dSv_player_item_c` — Items en inventaire, 24 slots (0x30 bytes)
  - `dSv_player_item_record_c` — Flèches, bombes, bouteilles (0x0C bytes)
  - `dSv_player_item_max_c` — Capacités max (0x08 bytes)
  - `dSv_player_collect_c` — Collections (cristaux, miroir, poes) (0x10 bytes)

### Fonctions principales
```cpp
// Vie
dComIfGs_getLife() / dComIfGs_setLife(u16)
dComIfGs_getMaxLife() / dComIfGs_setMaxLife(u8)

// Rupees
dComIfGs_getRupee() / dComIfGs_setRupee(u16)
dComIfGs_setWalletSize(u8)  // 0=300, 1=600, 2=1000

// Items
dComIfGs_setItem(slot, itemId)
execItemGet(u8 itemNo)

// Bombes/Flèches
dComIfGs_setArrowNum(u8) / dComIfGs_setArrowMax(u8)
dComIfGs_setBombNum(bag, count) / dComIfGs_setBombMax(type, max)
```

## 9. Libs — Inventaire

| Lib | Fichiers | Rôle |
|-----|----------|------|
| JSystem | 618 | Middleware Nintendo (J3D, J2D, JPA, JAudio2, JKernel) |
| Revolution SDK | 494 | SDK Wii (GX, WPAD, NAND, etc.) |
| Dolphin SDK | 315 | SDK GameCube (GX, PAD, DVD, OS, MTX) |
| PowerPC_EABI | 152 | Runtime compilateur + MSL (libc) |
| TRK_MINNOW | 32 | Debugger (MetroTRK) |
| **Total** | **1 611** | |

## 10. Structure des sources

```
src/
├── m_Do/           # Engine principal (memory, graphics, controller, audio)
├── f_ap/           # Application framework (game execution)
├── f_op/           # Operation framework (actors, scenes, overlaps, cameras)
├── f_pc/           # Process framework (layers, priorities, execution)
├── d/              # Game data et logique
│   ├── actor/      # ~800 acteurs (d_a_*.cpp)
│   ├── d_stage.cpp # Chargement de stages
│   ├── d_com_inf_game.cpp # Info globale du jeu
│   ├── d_camera.cpp # Système de caméra
│   ├── d_drawlist.cpp # Système de rendu
│   ├── d_item.cpp  # Système d'items
│   ├── d_save.cpp  # Sauvegarde
│   └── d_meter2.cpp # HUD
├── SSystem/        # Utilitaires système
├── Z2AudioLib/     # Audio
└── c/              # Utilitaires de base
```

## 11. Modding — Architecture du système REL custom

### Pipeline de modding validé
```
Code C++ (mod_menu_rel/source/)
        │
        ▼ compile avec devkitPPC (make us)
REL.us.rel (module relogeable PowerPC)
        │
        ▼ pack dans un GCI (gcipack.py)
REL.us.gci (fichier memory card GameCube)
        │
        ▼ copié dans Dolphin memory card
~/Library/Application Support/Dolphin/GC/USA/Card A/
        │
        ▼ chargé au runtime par le REL Loader (Gecko code)
Jeu avec mod actif
```

### Fichiers du mod
```
mod_menu_rel/
├── source/main.cpp           # Code du mod
├── include/main.h            # Header du mod
├── assets/us.lst             # Symboles du jeu (adresse:nom)
├── externals/libtp_rel/      # Bibliothèque d'accès au jeu
│   ├── include/tp/           # Headers du jeu (d_save.h, d_item.h, etc.)
│   ├── include/display/      # Console d'affichage
│   └── include/patch.h       # Hook de fonctions
├── Makefile                  # Build (make us)
├── REL.us.rel                # Module compilé
└── REL.us.gci                # Fichier memory card prêt
```

### Comment le REL est chargé
1. Le Gecko code REL Loader se hook dans `main01()` à `0x800063F8`
2. Il écrit "Custom REL File" à `0x80004D18` (nom du fichier à chercher)
3. Au boot, il cherche un GCI avec ce nom sur la memory card slot A
4. Il lit le REL depuis le GCI et le charge en RAM
5. Il appelle `_prolog` du REL qui exécute `mod::main()`
6. `mod::main()` hook `fapGm_Execute` pour s'exécuter chaque frame

### API clé de libtp_rel
```cpp
// Hook une fonction pour exécuter du code custom
libtp::patch::hookFunction(originalFunc, newFunc);

// Accès aux données du jeu
libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a

// Manette
libtp::tp::m_do_controller_pad::cpadInfo[PAD_1].mButtonFlags      // boutons tenus
libtp::tp::m_do_controller_pad::cpadInfo[PAD_1].mPressedButtonFlags // boutons pressés ce frame

// Console (affichage texte)
libtp::display::setConsole(true/false, 25);  // montrer/cacher
libtp::display::print(line, "texte");        // écrire ligne
libtp::display::clearConsole(0, 0);          // tout effacer

// Items
libtp::tp::d_item::execItemGet(itemId);      // donner un item
```
