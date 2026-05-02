# Twilight Princess Mod Menu — Reference Rapide

## Boutons GameCube (masques)

| Bouton | Masque | Constante |
|--------|--------|-----------|
| D-Left | 0x0001 | PAD_BUTTON_LEFT |
| D-Right | 0x0002 | PAD_BUTTON_RIGHT |
| D-Down | 0x0004 | PAD_BUTTON_DOWN |
| D-Up | 0x0008 | PAD_BUTTON_UP |
| Z | 0x0010 | PAD_TRIGGER_Z |
| R | 0x0020 | PAD_TRIGGER_R |
| L | 0x0040 | PAD_TRIGGER_L |
| A | 0x0100 | PAD_BUTTON_A |
| B | 0x0200 | PAD_BUTTON_B |
| X | 0x0400 | PAD_BUTTON_X |
| Y | 0x0800 | PAD_BUTTON_Y |
| Start | 0x1000 | PAD_BUTTON_START |

## Items principaux (IDs)

### Consommables
| ID | Nom | Effet |
|----|-----|-------|
| 0x00 | HEART | +4 HP |
| 0x01-0x07 | RUPEES (G/B/Y/R/P/O/S) | +1/5/10/20/50/100/200 |
| 0x08 | S_MAGIC | +4 magie |
| 0x09 | L_MAGIC | +8 magie |
| 0x1F | TRIPLE_HEART | +3 coeurs |
| 0x21 | KAKERA_HEART | +1 demi-coeur max |
| 0x22 | UTUWA_HEART | +5 demi-coeurs max + full heal |

### Armes et outils
| ID | Nom |
|----|-----|
| 0x28 | SWORD (Ordon) |
| 0x29 | MASTER_SWORD |
| 0x2A | WOOD_SHIELD |
| 0x2B | SHIELD (Ordon) |
| 0x2C | HYLIA_SHIELD |
| 0x40 | BOOMERANG |
| 0x41 | SPINNER |
| 0x42 | IRONBALL |
| 0x43 | BOW |
| 0x44 | HOOKSHOT |
| 0x45 | HVY_BOOTS |
| 0x46 | COPY_ROD |
| 0x47 | W_HOOKSHOT |
| 0x48 | KANTERA (lanterne) |
| 0x49 | LIGHT_SWORD |
| 0x53 | LIGHT_ARROW |

### Sacs et upgrades
| ID | Nom |
|----|-----|
| 0x34 | WALLET_LV1 (300) |
| 0x35 | WALLET_LV2 (600) |
| 0x36 | WALLET_LV3 (1000) |
| 0x50 | BOMB_BAG_LV1 |
| 0x54 | ARROW_LV1 (60 max) |
| 0x55 | ARROW_LV2 (60 max) |
| 0x56 | ARROW_LV3 (100 max) |

### Bouteilles
| ID | Nom |
|----|-----|
| 0x60 | EMPTY_BOTTLE |
| 0x61 | RED_BOTTLE (potion rouge) |
| 0x62 | GREEN_BOTTLE (potion verte) |
| 0x63 | BLUE_BOTTLE (potion bleue) |
| 0x64 | MILK_BOTTLE |
| 0x66 | OIL_BOTTLE |
| 0x6C | FAIRY_BOTTLE |

### Vêtements
| ID | Nom |
|----|-----|
| 0x2E | WEAR_CASUAL |
| 0x2F | WEAR_KOKIRI |
| 0x30 | ARMOR |
| 0x31 | WEAR_ZORA |

## Fonctions clés pour le mod menu

### Vie / Coeurs
```cpp
dComIfGs_getLife()                    // u16 — vie actuelle (en demi-coeurs)
dComIfGs_setLife(u16 life)            // set vie
dComIfGs_getMaxLife()                 // u16 — max vie
dComIfGs_setMaxLife(u8 maxLife)       // set max vie (ex: 40 = 20 coeurs)
dComIfGp_setItemLifeCount(f32, 0)    // ajouter de la vie (pour display)
dComIfGp_setItemMaxLifeCount(s16)    // ajouter max vie (pour display)
```

### Rupees
```cpp
dComIfGs_getRupee()                   // u16
dComIfGs_setRupee(u16 rupees)         // set rupees
dComIfGs_getRupeeMax()                // u16 (300/600/1000)
dComIfGs_setWalletSize(u8 size)       // 0=300, 1=600, 2=1000
```

### Items
```cpp
dComIfGs_setItem(int slotNo, u8 itemNo)   // mettre un item dans un slot
execItemGet(u8 itemNo)                     // exécuter l'acquisition d'item
```

### Flèches
```cpp
dComIfGs_getArrowNum() / dComIfGs_setArrowNum(u8)
dComIfGs_getArrowMax() / dComIfGs_setArrowMax(u8)   // 30/60/100
```

### Bombes
```cpp
dComIfGs_setBombNum(u8 bagIdx, u8 count)
dComIfGs_setBombMax(u8 type, u8 max)
```

### Magie
```cpp
dComIfGs_getMagic() / dComIfGs_setMagic(u8)
dComIfGs_getMaxMagic() / dComIfGs_setMaxMagic(u8)
```

### Épées / Boucliers
```cpp
dComIfGs_setCollectSword(u8 swordNo)   // 0=Ordon, 1=Master, 3=Light
dComIfGs_setCollectShield(u8 shieldNo) // 0=Wood, 1=Ordon, 2=Hylian
```

## Spawn d'ennemis

### Fonction de spawn
```cpp
fpc_ProcID fopAcM_create(
    s16 profName,        // ID du profil ennemi
    u32 parameters,      // Params (0 = défaut)
    const cXyz* pos,     // Position (x, y, z)
    int roomNo,          // Numéro de salle
    const csXyz* angle,  // Rotation
    const cXyz* scale,   // Échelle
    s8 argument          // Argument (-1 = défaut)
);
```

### Ennemis courants (profils)

| Profil | Ennemi |
|--------|--------|
| fpcNm_E_ST_e | Skulltula |
| fpcNm_E_DB_e | Deku Baba |
| fpcNm_E_BS_e | Bulblin |
| fpcNm_E_WB_e | Bokoblin |
| fpcNm_E_YC_e | Yellow Chuchu |
| fpcNm_E_RB_e | Red Bokoblin |
| fpcNm_E_KK_e | Kargarok |
| fpcNm_E_HM_e | Helmasaur |
| fpcNm_E_AI_e | Aeralfos |
| fpcNm_E_FM_e | Freezard |
| fpcNm_E_BA_e | Baba Serpent |
| fpcNm_E_SM_e | Stalfos |
| fpcNm_E_DF_e | Darknut |
| fpcNm_E_MB_e | Moblin |
| fpcNm_E_SH_e | Stalfos Warrior |
| fpcNm_E_TK_e | Tok |

### Boss
| Profil | Boss |
|--------|------|
| fpcNm_B_BH_e | Bugblin King |
| fpcNm_B_OH_e | Stallord |
| fpcNm_B_TN_e | Blizzeta |
| fpcNm_B_GG_e | Morpheel |

### Obtenir position du joueur
```cpp
fopAc_ac_c* player = dComIfGp_getPlayer(0);
cXyz playerPos = fopAcM_GetPosition(player);
csXyz* playerAngle = fopAcM_GetAngle_p(player);
```

## Dessin 2D (overlay)

### Base class
```cpp
class dDlst_base_c {
    virtual void draw() {}
};
```

### Enregistrement overlay
```cpp
dComIfGd_set2DOpaTop(dDlst_base_c* dlst);  // par-dessus tout
```

### Rectangle semi-transparent
```cpp
dDlst_2DQuad_c quad;
GXColor color = {0, 0, 0, 0x80};
quad.init(x, y, w, h, color);
quad.draw();
```

### Texte debug
```cpp
dDbVw_Report(int x, int y, const char* format, ...);
```

### Coordonnées : 640x480, origine (0,0) = top-left

## Pieges et problemes rencontres (modding)

### 1. Impossible d'ajouter un nouveau fichier .cpp au build
Le build system (configure.py + ninja) est concu pour la decomp : chaque Translation Unit (TU) doit correspondre a un segment du binaire original defini dans `config/GZ2E01/splits.txt`. Un nouveau fichier `.cpp` ajoute via `Object(MatchingFor(...), "d/mon_fichier.cpp")` dans configure.py sera **silencieusement ignore** car il n'existe pas dans splits.txt.

**Solution** : integrer le code du mod directement dans un fichier existant deja compile et linke (ex: `m_Do_main.cpp`). Utiliser des fonctions `static` pour eviter les conflits de symboles.

### 2. NonMatching = False (pas linke)
Dans configure.py, `NonMatching = False` signifie que l'objet n'est **jamais linke**. `Equivalent = config.non_matching` n'est linke que si `--non-matching` est passe. Seuls `Matching = True` et `MatchingFor(versions)` sont linkes par defaut.

### 3. dDbVw_Report n'est pas linkable
`dDbVw_Report()` est dans `d_debug_viewer.cpp` qui est marque `Equivalent` — donc pas linke en build normal. 

**Solution** : utiliser `JUTReport(x, y, format, ...)` de `JSystem/JUtility/JUTReport.h` qui est toujours linke car fait partie de JSystem.

### 4. dItemNo_FAIRY_BOTTLE_e n'existe pas
Le nom correct dans l'enum est `dItemNo_FAIRY_e` (0x6C), pas `dItemNo_FAIRY_BOTTLE_e`. Toujours verifier les noms exacts dans `include/d/d_item_data.h`.

### 5. Les FAILED apres le build sont normaux pour un mod
Le build system verifie que le DOL genere est identique a l'original. Quand on modifie le code (mod), cette verification echoue et affiche `FAILED` pour `framework.dol` et les `.rel`. C'est attendu — le DOL moddé est quand meme genere dans `build/GZ2E01/framework.dol`.

### 6. Ou hooker le mod dans la game loop
Le meilleur endroit est dans `m_Do_main.cpp`, boucle `do {} while(true)` (ligne ~990) :
- **Input** : juste apres `mDoCPd_c::read()` (l'input vient d'etre lu)
- **Draw** : juste apres `debug()` (dernier moment avant vsync, dessine par-dessus tout)

### 7. Reconfigurer apres modification de configure.py
Apres toute modification de `configure.py`, il faut re-executer `python3 configure.py` pour regenerer `build.ninja` AVANT de lancer `ninja`.

### 8. CRITIQUE : Ajouter du code C dans une TU decale les adresses data → crash
Chaque TU dans splits.txt a des plages d'adresses fixes (.text, .rodata, .data). Si on ajoute du code C, la section .text grossit et **decale toutes les sections data** (.rodata, .data, .sdata, etc.). Les RELs (modules dynamiques des acteurs) ont des references codees en dur vers ces adresses → le jeu crash (ecran noir ou vert).

**Solution** : ne PAS modifier le code source C. Utiliser un **patch binaire** (code cave) a la place.

### 9. Le DOL est coince entre le header et la FST dans le CISO
L'espace pour le DOL dans le CISO est exactement `FST_offset - DOL_offset = 4020736 bytes`. Le DOL original fait 4020672 bytes (64 bytes de marge). Toute augmentation de taille ecrase la FST → ecran vert.

Deplacer la FST dans le CISO ne marche PAS (ecran noir — Dolphin ne gere pas bien les FST deplacees dans les CISO).

**Solution** : extraire le CISO en dossier Dolphin-bootable avec `tools/extract_ciso.py`, puis patcher le DOL dans le dossier.

### 10. SOLUTION VALIDEE : Patch binaire via code cave
La methode qui marche :
1. Extraire le CISO : `python3 tools/extract_ciso.py input.ciso TP_Extracted/`
2. Patcher le DOL : `python3 tools/apply_mod.py TP_Extracted/sys/main.dol TP_Extracted/sys/main.dol`
3. Lancer dans Dolphin : ouvrir `TP_Extracted/sys/main.dol`

Le patch ecrit du code PowerPC dans un espace libre du DOL (code cave a 0x80004D18, 1228 bytes) et remplace une instruction dans la boucle principale (BL fapGm_Execute a 0x80006444) par un branchement vers le code cave. Le code cave execute le mod puis appelle fapGm_Execute avant de revenir. Taille du DOL inchangee, pas de decalage d'adresses.

### 11. Adresses memoire cles (GZ2E01 — GCN USA)
```
g_dComIfG_gameInfo  = 0x804061C0
  maxLife (u16)     = +0x00
  life (u16)        = +0x02
  rupee (u16)       = +0x04
  walletSize (u8)   = +0x19
  arrowNum (u8)     = +0xEC
  arrowMax (u8)     = +0xF8
  bombNum[3] (u8)   = +0xED, +0xEE, +0xEF
m_cpadInfo[0]       = 0x803DD2E8
  hold (u32)        = +0x30
  trig (u32)        = +0x34
JUTReport           = 0x802E0530
execItemGet         = 0x80097E8C
fapGm_Execute       = 0x80018A6C
fopAcM_create       = 0x80019D98
Code cave           = 0x80004D18 (1228 bytes libres)
Hook point          = 0x80006444 (BL fapGm_Execute dans main loop)
```

### 12. Le ldscript.lcf n'est PAS regenere par configure.py
Le fichier `build/GZ2E01/ldscript.lcf` est statique — `configure.py` ne le regenere pas. Si on le modifie manuellement (ex: ADDR() au lieu de ALIGN()), le changement persiste et peut casser le build de maniere subtile. Toujours verifier qu'il est intact.

### 13. Les RELs du build NE MATCHENT PAS les originaux — on ne peut pas les remplacer
Le DOL rebuild est 100% matching, mais 0/623 RELs matchent l'original. La decomp des RELs n'est pas terminee. Remplacer DOL+RELs = ecran noir.

Consequence : on NE PEUT PAS modifier le code source C et rebuild le jeu.

### 14. Ajouter une section text2 au DOL ne marche pas
L'apploader du jeu ne charge pas correctement les sections supplementaires. Tester avec text2 a 0x81600000 = ecran noir.

### 15. SOLUTION FINALE VALIDEE : REL custom via libtp_rel + REL Loader Gecko code
C'est la methode qui marche pour un vrai mod menu avec interface :
1. Ecrire le mod en C++ dans `mod_menu_rel/source/main.cpp` avec la lib `libtp_rel`
2. Compiler avec devkitPPC : `make us` (produit `REL.us.gci`)
3. Copier le GCI sur la memory card Dolphin : `~/Library/Application Support/Dolphin/GC/USA/Card A/`
4. Activer le Gecko code REL Loader dans `~/Library/Application Support/Dolphin/GameSettings/GZ2E01.ini`
5. Lancer le jeu depuis `TP_Extracted/sys/main.dol` (DOL original non modifie)

Le REL Loader (Gecko code) lit le fichier "Custom REL File" depuis la memory card et le charge en memoire. Le REL hook `fapGm_Execute` pour executer le mod chaque frame.

### 16. Le constructeur Console de libtp_rel active la console et affiche du texte au boot
Le constructeur `Console(uint8_t)` appelle `setConsole(true, 25)` et affiche nom du projet + version. Si cela se fait trop tot (avant l'ecran titre), ca peut crasher ou polluer l'ecran.

**Solution** : ne PAS utiliser le constructeur Console. Appeler `libtp::display::setConsole(false, 25)` dans `init()` pour cacher la console au demarrage. Utiliser `libtp::display::print(line, text)` et `setConsole(true/false, 25)` directement.

### 17. Le GCI sans REL Loader Gecko code ne fait rien
Le GCI seul sur la memory card ne fait rien. Il FAUT le Gecko code REL Loader actif dans Dolphin pour que le REL soit lu et charge. Les deux sont necessaires ensemble.

### 18. Ordre de test pour debugger un REL qui crash
1. Jeu extrait propre sans rien → doit marcher
2. Ajouter Gecko code REL Loader SANS GCI → doit marcher (loader ne trouve pas le fichier, passe)
3. Ajouter GCI avec hook vide (procNewFrame ne fait que appeler return_fapGm_Execute) → doit marcher
4. Ajouter la logique progressivement (cheats sans affichage, puis affichage)
