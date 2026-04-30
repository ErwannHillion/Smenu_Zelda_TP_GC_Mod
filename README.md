# SMenu — Mod Menu pour The Legend of Zelda: Twilight Princess (GameCube)

> **Note :** Ce projet a été réalisé avec l'aide d'une intelligence artificielle (Claude) à hauteur d'environ 30% du travail total. L'IA a principalement assisté sur la recherche technique, le débogage et la rédaction de documentation. L'architecture du mod, les choix de conception, les tests et la direction créative sont entièrement le fruit du travail de **SHDXW** (Erwann Hillion).

---

## 🇫🇷 Version Française

### Qu'est-ce que SMenu ?

SMenu est un **mod menu complet** pour The Legend of Zelda: Twilight Princess sur GameCube (version NTSC-U, GZ2E01). Il se présente sous la forme d'un module REL (Relocatable Module) chargé dynamiquement au démarrage du jeu via un Gecko Code appelé **REL Loader**.

Le menu s'affiche par-dessus le jeu avec une interface texte navigable à la manette. Quand il est ouvert, les inputs du joueur sont bloqués pour éviter que Link ne bouge ou interagisse avec le monde pendant la navigation du menu.

### Fonctionnalités

#### 🗡️ Catégorie ARMES (11 items)
- Donner ou retirer individuellement chaque arme du jeu
- Boomerang, Arc, Grappin, Spinner, Lanterne, Bottes de Fer, Boulet, Sceptre, Double Grappin, Oeil de Faucon, Sac de Bombes
- Indicateur visuel `[+]` / `[-]` montrant si l'objet est possédé ou non
- **A** pour donner, **X** pour retirer

#### 🧪 Catégorie BOUTEILLES (6 types)
- Bouteille vide, Potion rouge, Potion bleue, Huile pour lanterne, Lait, Eau
- Même système de give/remove que les armes

#### ❤️ Catégorie VIE (7 options)
- `+1 Coeur` — Ajoute un conteneur de coeur
- `+5 Coeurs` — Ajoute 5 conteneurs d'un coup
- `Full Heal` — Remplit la vie au maximum
- `20 Max` — Met les coeurs au maximum (20 coeurs)
- `-1 Coeur` — Retire un conteneur
- `-5 Coeurs` — Retire 5 conteneurs
- `1 Coeur only` — Remet à un seul coeur (challenge mode)

#### 💰 Catégorie RUPEES (9 options)
- `+10`, `+20`, `+50`, `+100` — Ajouter des rubis
- `MAX` — Remplir la bourse au maximum
- `Bourse Max` — Donne la bourse géante (1000) + la remplit
- `-10`, `-50` — Retirer des rubis
- `0 Rupees` — Vider la bourse complètement

#### 👹 Catégorie SPAWN (8 ennemis)
- Faire apparaître des ennemis : Bokoblin, Bulblin, Deku Baba, Skulltula, Chuchu, Helmasaur, Kargarok, Stalfos

#### 🌀 Catégorie WARP (19 destinations)
- Téléportation instantanée vers n'importe quel lieu du jeu
- Zones overworld : Ordon, Firone, Cocorico, Zora, Lac Hylia, Citadelle, Plaine d'Hyrule, Pics Blancs, Désert Gerudo, Bois Sacré...
- Donjons : Temple Sylvestre, Mines Goron, Temple Abyssal, Temple du Temps, Célestia, Palais du Crépuscule, Château d'Hyrule

#### 📊 Barre de stats en temps réel
- Affichage permanent en haut du menu : HP, Rupees, Flèches, Bombes

### Contrôles

| Combinaison | Action |
|-------------|--------|
| **B + Z** | Ouvrir / fermer le menu |
| **D-Pad Haut/Bas** | Naviguer dans les options |
| **D-Pad Gauche/Droite** | Changer de catégorie |
| **A** | Exécuter l'action sélectionnée |
| **X** | Retirer un item (catégories Armes/Bouteilles) |
| **Start** | Fermer le menu |

> Quand le menu est ouvert, **Link est figé** — aucun input n'est transmis au jeu.

### Le parcours de développement — Les galères

Ce projet a été un véritable parcours du combattant. Voici les obstacles majeurs rencontrés et comment ils ont été surmontés :

#### 1. Tentative de modification du code source C (échec)
La première approche a été de modifier directement le code source décompilé du jeu (projet [zeldaret/tp](https://github.com/zeldaret/tp)). Le problème : **ajouter du code C dans une Translation Unit décale les adresses de toutes les sections data** (.rodata, .data, .sdata). Les RELs du jeu (modules dynamiques pour les acteurs, ennemis, NPCs) ont des références codées en dur vers ces adresses. Résultat : écran noir systématique.

#### 2. Le DOL est coincé dans le CISO (échec)
L'espace pour le DOL dans le fichier CISO est limité à exactement 4 020 736 bytes (64 bytes de marge). Le moindre ajout de code fait déborder le DOL sur la FST (File System Table), ce qui corrompt la table des fichiers du jeu. Tenter de déplacer la FST dans le CISO provoque aussi un écran noir — Dolphin ne gère pas bien les FST relocalisées dans les CISO.

#### 3. Ajout d'une section text2 au DOL (échec)
Le format DOL supporte jusqu'à 7 sections text. Nous avons tenté d'ajouter une section text2 à l'adresse 0x81600000. L'apploader du jeu refuse de charger les sections supplémentaires. Écran noir.

#### 4. Reconstruction DOL + RELs (échec)
Tentative de modifier le code source et de reconstruire le DOL ET les 623 RELs du jeu. Problème : le DOL rebuild est 100% matching, mais **aucun des 623 RELs ne matche l'original**. La décompilation des RELs n'est pas terminée. Remplacer les RELs = écran noir.

#### 5. Patch binaire via code cave (succès partiel)
On a trouvé 1 228 bytes d'espace libre dans la section text0 du DOL (à l'adresse 0x80004D18). On y a écrit du code machine PowerPC à la main pour implémenter des cheats basiques (Z+B = heal, etc.). Ça a marché ! Mais 1 228 bytes c'est trop petit pour un vrai menu avec interface.

#### 6. JUTReport ne rend rien en retail (échec)
La fonction `JUTReport` (rendu de texte debug) ne fonctionne pas dans les builds retail car le système de police de caractères n'est pas initialisé. Pas de texte à l'écran = pas de menu visuel.

#### 7. Solution finale : REL custom via libtp_rel + REL Loader (succès !)
La communauté de modding TP ([zsrtp](https://github.com/zsrtp)) a créé une bibliothèque (`libtp_rel`) et un système de chargement de modules REL custom. Le workflow :
1. Écrire le mod en C++ avec libtp_rel
2. Compiler avec devkitPPC → produit un fichier `.rel`
3. Empaqueter dans un `.gci` (format memory card GameCube)
4. Le REL Loader (Gecko Code) charge le module au démarrage

Mais même là, ça n'a pas été simple :
- Le constructeur de la classe Console active la console et affiche du texte trop tôt → écran noir après le Dolby
- Il a fallu cacher la console au boot et ne l'activer que quand le menu est ouvert
- Chaque symbole manquant dans le fichier `us.lst` (carte des adresses mémoire) devait être retrouvé manuellement dans les 48 107 symboles du projet de décompilation

### Installation

#### Prérequis
- [Dolphin Emulator](https://dolphin-emu.org/)
- Une ROM de Twilight Princess GameCube version US (NTSC-U, ID: GZ2E01)
- Python 3 (pour l'extraction de la ROM si elle est en format CISO)

#### Méthode rapide (utilisateur)

1. **Extraire votre ROM** (si elle est en format .ciso) :
```bash
python3 tools/extract_ciso.py votre_rom.ciso TP_Extracted
```
Si votre ROM est déjà en format .iso, vous pouvez l'ouvrir directement dans Dolphin.

2. **Copier le mod sur la memory card Dolphin** :
```bash
# macOS
cp release/REL.us.gci ~/Library/Application\ Support/Dolphin/GC/USA/Card\ A/

# Windows
copy release\REL.us.gci "%APPDATA%\Dolphin Emulator\GC\USA\Card A\"

# Linux
cp release/REL.us.gci ~/.local/share/dolphin-emu/GC/USA/Card\ A/
```

3. **Copier la configuration Gecko Code** :
```bash
# macOS
cp release/GZ2E01.ini ~/Library/Application\ Support/Dolphin/GameSettings/

# Windows
copy release\GZ2E01.ini "%APPDATA%\Dolphin Emulator\GameSettings\"

# Linux
cp release/GZ2E01.ini ~/.local/share/dolphin-emu/GameSettings/
```

4. **Activer les cheats dans Dolphin** :
   - Ouvrir Dolphin
   - Aller dans `Config` → `General` → cocher `Enable Cheats`

5. **Lancer le jeu** :
   - Si vous avez extrait la ROM : ouvrir `TP_Extracted/sys/main.dol`
   - Sinon : ouvrir votre ROM normalement

6. **En jeu** : appuyer sur **B + Z** pour ouvrir le mod menu

#### Méthode développeur (compilation depuis les sources)

##### Prérequis de compilation
- [devkitPro](https://devkitpro.org/) avec devkitPPC installé
- [pyelf2rel](https://pypi.org/project/pyelf2rel/) : `pip install pyelf2rel`
- Python 3
- Git

##### Compilation
```bash
# Cloner le repo avec les submodules
git clone --recursive https://github.com/ErwannHillion/Smenu_Zelda_TP_GC_Mod.git
cd Smenu_Zelda_TP_GC_Mod/mod

# Configurer l'environnement
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=$DEVKITPRO/devkitPPC
export PATH=$DEVKITPPC/bin:$PATH

# Compiler pour GCN US
make us

# Le fichier REL.us.gci est créé dans le dossier mod/
```

##### Déploiement rapide (macOS)
```bash
make clean && make us && cp REL.us.gci ~/Library/Application\ Support/Dolphin/GC/USA/Card\ A/
```

### Structure du projet

```
Smenu_Zelda_TP_GC_Mod/
├── mod/
│   ├── source/main.cpp              # Code source du mod
│   ├── include/main.h               # Header
│   ├── assets/us.lst                # Table des symboles (adresse:nom)
│   ├── externals/libtp_rel/         # Bibliothèque d'accès au jeu (submodule)
│   ├── bin/gcipack.py               # Script d'empaquetage GCI
│   └── Makefile                     # Script de compilation
├── release/
│   ├── REL.us.gci                   # Le mod compilé, prêt à l'emploi
│   └── GZ2E01.ini                   # Configuration Gecko Code pour Dolphin
├── tools/
│   └── extract_ciso.py              # Outil d'extraction CISO → dossier Dolphin
├── .gitmodules                      # Configuration du submodule libtp_rel
├── .gitignore
└── README.md
```

### Comment ça marche techniquement

```
Au démarrage du jeu :
   Dolphin active le Gecko Code "REL Loader"
          ↓
   Le REL Loader cherche "Custom REL File" sur la memory card
          ↓
   Il trouve notre GCI et charge le REL en mémoire RAM
          ↓
   Le REL hook la fonction fapGm_Execute (appelée 60x/sec)
          ↓
   À chaque frame, notre code vérifie les boutons
   et affiche le menu quand B+Z est pressé
          ↓
   Quand le menu est ouvert, les inputs sont bloqués
   pour que Link ne bouge pas
```

---

## 🇬🇧 English Version

### What is SMenu?

SMenu is a **complete mod menu** for The Legend of Zelda: Twilight Princess on GameCube (NTSC-U version, GZ2E01). It takes the form of a custom REL (Relocatable Module) loaded dynamically at game startup via a Gecko Code called **REL Loader**.

The menu is displayed as a text overlay on top of the game, navigable with the controller. When open, player inputs are blocked so Link doesn't move or interact with the world while browsing the menu.

### Features

#### Weapons (11 items)
- Give or remove each weapon individually
- Boomerang, Hero's Bow, Clawshot, Spinner, Lantern, Iron Boots, Ball and Chain, Dominion Rod, Double Clawshots, Hawkeye, Bomb Bag
- Visual indicator `[+]` / `[-]` showing ownership status
- **A** to give, **X** to remove

#### Bottles (6 types)
- Empty Bottle, Red Potion, Blue Potion, Lantern Oil, Milk, Water

#### Health (7 options)
- `+1 Heart` / `+5 Hearts` — Add heart containers
- `Full Heal` — Restore health to max
- `20 Max` — Set to maximum 20 hearts
- `-1 Heart` / `-5 Hearts` — Remove heart containers
- `1 Heart only` — Challenge mode

#### Rupees (9 options)
- `+10`, `+20`, `+50`, `+100` — Add rupees
- `MAX` — Fill wallet to capacity
- `Giant Wallet` — Unlock 1000 capacity + fill
- `-10`, `-50` — Remove rupees
- `0 Rupees` — Empty wallet

#### Spawn Enemies (8 types)
- Bokoblin, Bulblin, Deku Baba, Skulltula, Chuchu, Helmasaur, Kargarok, Stalfos

#### Warp / Teleport (19 destinations)
- Instant teleportation to any location in the game
- Overworld: Ordon, Faron, Kakariko, Zora's Domain, Lake Hylia, Castle Town, Hyrule Field, Snowpeak, Gerudo Desert, Sacred Grove...
- Dungeons: Forest Temple, Goron Mines, Lakebed Temple, Temple of Time, City in the Sky, Palace of Twilight, Hyrule Castle

#### Live Stats Bar
- Real-time display of HP, Rupees, Arrows, and Bombs at the top of the menu

### Controls

| Input | Action |
|-------|--------|
| **B + Z** | Toggle menu open/close |
| **D-Pad Up/Down** | Navigate options |
| **D-Pad Left/Right** | Switch category |
| **A** | Execute selected action |
| **X** | Remove item (Weapons/Bottles categories) |
| **Start** | Close menu |

> When the menu is open, **Link is frozen** — no inputs reach the game.

### Development Journey — The Struggles

This project was a real obstacle course. Here are the major challenges encountered:

1. **Modifying decompiled C source code** — Adding code shifts data section addresses, breaking all 623 REL modules. Black screen.

2. **DOL size constraint in CISO** — Only 64 bytes of margin. Any code addition overwrites the File System Table.

3. **Adding a text2 section to the DOL** — The apploader refuses to load additional sections. Black screen.

4. **Rebuilding DOL + all RELs** — DOL matches 100%, but 0/623 RELs match their originals. The REL decompilation is incomplete.

5. **Binary patching via code cave** — Found 1,228 bytes of free space. Wrote raw PowerPC machine code by hand. Worked for basic cheats but too small for a full menu.

6. **JUTReport doesn't render in retail builds** — Debug font system not initialized. No visible text output.

7. **Final solution: Custom REL via libtp_rel + REL Loader** — Using the TP modding community's tools to create a dynamically loaded module. Even then, the Console constructor caused crashes that had to be debugged step by step.

### Installation

#### Quick Install (User)

1. **Extract your ROM** (if CISO format):
```bash
python3 tools/extract_ciso.py your_rom.ciso TP_Extracted
```

2. **Copy mod GCI to Dolphin memory card**:
```bash
# macOS
cp release/REL.us.gci ~/Library/Application\ Support/Dolphin/GC/USA/Card\ A/

# Windows
copy release\REL.us.gci "%APPDATA%\Dolphin Emulator\GC\USA\Card A\"

# Linux
cp release/REL.us.gci ~/.local/share/dolphin-emu/GC/USA/Card\ A/
```

3. **Copy Gecko Code config**:
```bash
# macOS
cp release/GZ2E01.ini ~/Library/Application\ Support/Dolphin/GameSettings/

# Windows
copy release\GZ2E01.ini "%APPDATA%\Dolphin Emulator\GameSettings\"

# Linux
cp release/GZ2E01.ini ~/.local/share/dolphin-emu/GameSettings/
```

4. **Enable cheats in Dolphin**: `Config` → `General` → check `Enable Cheats`

5. **Launch the game** and press **B + Z** to open the mod menu

#### Developer Install (Build from Source)

```bash
git clone --recursive https://github.com/ErwannHillion/Smenu_Zelda_TP_GC_Mod.git
cd Smenu_Zelda_TP_GC_Mod/mod
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=$DEVKITPRO/devkitPPC
export PATH=$DEVKITPPC/bin:$PATH
make us
```

---

## Remerciements / Acknowledgments

Ce projet n'aurait pas été possible sans les outils et le travail remarquable des communautés suivantes :

This project would not have been possible without the tools and remarkable work of the following communities:

- **[zeldaret/tp](https://github.com/zeldaret/tp)** — Le projet de décompilation de Twilight Princess. 100% matching sur le DOL GameCube, un travail colossal de reverse engineering par la Zelda Reverse Engineering Team.

- **[zsrtp](https://github.com/zsrtp)** — Twilight Princess Modding & Development. Créateurs de l'infrastructure de modding TP, notamment :
  - **[libtp_rel](https://github.com/zsrtp/libtp_rel)** — La bibliothèque C++ permettant d'accéder aux données et fonctions du jeu depuis un module REL custom
  - **[REL Template](https://github.com/zsrtp/REL)** — Le template de projet pour créer des mods TP
  - **[Randomizer](https://github.com/zsrtp/Randomizer)** — Le randomizer TP, qui a servi de référence technique
  - **REL Loader** — Le Gecko Code qui charge les modules custom depuis la memory card

- **[devkitPro](https://devkitpro.org/)** — La toolchain de compilation cross-platform pour GameCube/Wii (devkitPPC). Sans eux, pas de compilation PowerPC possible.

- **[pyelf2rel](https://pypi.org/project/pyelf2rel/)** — L'outil de conversion ELF → REL pour les modules GameCube.

- **[Dolphin Emulator](https://dolphin-emu.org/)** — L'émulateur GameCube/Wii qui rend tout ce travail de modding testable et accessible.

- **La communauté gc-forever** — Pour la documentation des Gecko Codes et du format GameCube.

Un merci particulier au **Discord zsrtp** pour la documentation sur le wiki du Randomizer et les ressources techniques partagées.

---

## Licence / License

**© 2026 SHDXW (Erwann Hillion). Tous droits réservés. / All rights reserved.**

Ce projet est **open source**. Vous êtes libre de :
- 📖 Lire, étudier et apprendre du code source
- 🔧 Modifier le code pour votre usage personnel
- 🍴 Forker le projet pour créer votre propre version
- 🤝 Soumettre des pull requests pour améliorer le projet

Vous devez :
- Créditer l'auteur original (shdxw) dans tout projet dérivé
- Ne pas vendre ce mod ou tout projet dérivé
- Garder cette notice de licence dans tout fork ou distribution

This project is **open source**. You are free to:
- Read, study, and learn from the source code
- Modify the code for personal use
- Fork the project to create your own version
- Submit pull requests to improve the project

You must:
- Credit the original author (shdxw) in any derivative work
- Not sell this mod or any derivative work
- Keep this license notice in any fork or distribution

---

*Mod developed by **SHDXW** — All rights reserved*
