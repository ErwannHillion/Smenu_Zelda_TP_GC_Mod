/**
 * @file main.cpp
 * @brief TP Mod Menu
 * B+Z: Toggle | DPad: Nav | A: Add/Execute | X: Remove | Start: Close
 *
 * @author SHDXW (Erwann Hillion)
 *
 * Dedicace à Mr. Cruzol, mon prof de C++.
 * Sans ses cours, ce mod n'existerait pas :).
 * Vous aviez raison, les pointeurs ça sert vraiment.
 */
#include <main.h>
#include <cstdio>
#include <cstring>
#include <patch.h>
#include <display/console.h>
#include <tp/f_ap_game.h>
#include <tp/d_com_inf_game.h>
#include <tp/d_item.h>
#include <tp/d_save.h>
#include <tp/f_op_actor_mng.h>
#include <tp/m_do_controller_pad.h>
#include <tp/m_do_printf.h>
#include <data/items.h>

namespace mod
{
    using namespace libtp::tp::m_do_controller_pad;
    using namespace libtp::tp::d_com_inf_game;
    using namespace libtp::tp::d_item;
    using namespace libtp::data::items;

    Mod* gMod = nullptr;

    struct WeaponEntry { const char* name; uint8_t itemId; };
    struct TeleportEntry { const char* name; const char* stage; int8_t room; int16_t spawn; };

    static const WeaponEntry weapons[] = {
        {"Boomerang",      0x40}, {"Arc",            0x43},
        {"Grappin",        0x44}, {"Spinner",        0x41},
        {"Lanterne",       0x48}, {"Bottes de fer",  0x45},
        {"Boulet",         0x42}, {"Sceptre",        0x46},
        {"Double Grappin", 0x47}, {"Oeil de Faucon", 0x3E},
        {"Sac de bombes",  0x50},
    };
    static const int NUM_WEAPONS = 11;

    static const WeaponEntry bottles[] = {
        {"Bouteille vide",   0x60}, {"Potion rouge",  0x61},
        {"Potion bleue",     0x63}, {"Huile lanterne", 0x66},
        {"Lait",             0x64}, {"Eau",            0x67},
    };
    static const int NUM_BOTTLES = 6;

    static const char* hpOpts[] = {
        "+1 Coeur", "+5 Coeurs", "Full Heal", "20 Max",
        "-1 Coeur", "-5 Coeurs", "1 Coeur only"
    };
    static const int NUM_HP = 7;

    static const char* rupOpts[] = {
        "+10", "+20", "+50", "+100", "MAX",
        "Bourse Max", "-10", "-50", "0 Rupees"
    };
    static const int NUM_RUP = 9;

    static const char* spawnOpts[] = {
        "Bokoblin", "Bulblin", "Deku Baba", "Skulltula",
        "Chuchu", "Helmasaur", "Kargarok", "Stalfos"
    };
    static const uint16_t enemyIDs[] = {
        0x00EF, 0x01B7, 0x01C5, 0x01BF,
        0x00F5, 0x01CF, 0x01E0, 0x01BD
    };
    static const int NUM_SPAWN = 8;

    static const TeleportEntry teleports[] = {
        {"Ordon Village",     "F_SP103",  0, 0},
        {"Source d'Ordinn",   "F_SP104",  0, 0},
        {"Foret de Firone",   "F_SP108",  0, 0},
        {"Cocorico",          "F_SP109",  0, 0},
        {"Montagne Mort",     "F_SP110",  0, 0},
        {"Domaine Zora",      "F_SP111",  0, 0},
        {"Lac Hylia",         "F_SP113",  0, 0},
        {"Citadelle",         "F_SP116",  0, 0},
        {"Bois Sacre",        "F_SP115",  0, 0},
        {"Pics Blancs",       "F_SP112",  0, 0},
        {"Desert Gerudo",     "F_SP118",  0, 0},
        {"Plaine d'Hyrule",  "F_SP117",  0, 0},
        {"Temple Sylvestre",  "D_MN05",   0, 0},
        {"Mines Goron",       "D_MN04",   0, 0},
        {"Temple Abyssal",    "D_MN01",   0, 0},
        {"Temple du Temps",   "D_MN06",   0, 0},
        {"Celestia",          "D_MN07",   0, 0},
        {"Palais du Crepus.", "D_MN08",   0, 0},
        {"Chateau d'Hyrule",  "D_MN09",   0, 0},
    };
    static const int NUM_TP = 19;

    static const char* catNames[] = {"ARMES", "BOTTLE", "VIE", "RUPEES", "SPAWN", "WARP"};
    static const int NUM_CATS = 6;

    static int getOptCount(int cat) {
        switch (cat) {
        case 0: return NUM_WEAPONS;
        case 1: return NUM_BOTTLES;
        case 2: return NUM_HP;
        case 3: return NUM_RUP;
        case 4: return NUM_SPAWN;
        case 5: return NUM_TP;
        } return 0;
    }

    static bool hasItem(uint8_t itemId) {
        auto& inv = dComIfG_gameInfo.save.save_file.player.player_item;
        for (int i = 0; i < 24; i++)
            if (inv.item[i] == itemId) return true;
        return false;
    }

    static void removeItem(uint8_t itemId) {
        auto& inv = dComIfG_gameInfo.save.save_file.player.player_item;
        for (int i = 0; i < 24; i++)
            if (inv.item[i] == itemId) { inv.item[i] = 0xFF; break; }
    }

    void main() { Mod* mod = new Mod(); mod->init(); }
    void exit() {}

    Mod::Mod(): menuActive(false), category(0), option(0), inputDelay(0) {}

    // Shortcut for logging
    #define LOG(fmt, ...) libtp::tp::m_Do_printf::OSReport("[SMenu] " fmt "\n", ##__VA_ARGS__)

    void Mod::init()
    {
        gMod = this;
        // Re-enable OSReport (the game disables it after boot)
        libtp::tp::m_Do_printf::OSReportEnable();
        LOG("=== SMenu v1.0 by SHDXW ===");
        LOG("Initializing mod menu...");
        libtp::display::setConsole(false, 25);
        libtp::display::setConsoleColor(10, 10, 40, 180);
        return_fapGm_Execute =
            libtp::patch::hookFunction(libtp::tp::f_ap_game::fapGm_Execute,
                                        []() { return gMod->procNewFrame(); });
        LOG("Hook on fapGm_Execute installed");
        LOG("Ready! Press B+Z to open menu");
    }

    void Mod::procNewFrame()
    {
        uint32_t hold = cpadInfo[PAD_1].mButtonFlags;
        uint32_t trig = cpadInfo[PAD_1].mPressedButtonFlags;

        if ((hold & Button_B) && (trig & Button_Z))
        {
            menuActive = !menuActive;
            if (menuActive) {
                category = 0; option = 0;
                libtp::display::setConsole(true, 25);
                LOG("Menu opened");
            } else {
                libtp::display::setConsole(false, 25);
                libtp::display::clearConsole(0, 0);
                LOG("Menu closed");
            }
            inputDelay = 10;
        }

        if (menuActive)
        {
            if (inputDelay > 0) inputDelay--;
            else handleInput();
            drawMenu();
            cpadInfo[PAD_1].mButtonFlags = 0;
            cpadInfo[PAD_1].mPressedButtonFlags = 0;
        }

        return_fapGm_Execute();
    }

    void Mod::handleInput()
    {
        uint32_t trig = cpadInfo[PAD_1].mPressedButtonFlags;
        int count = getOptCount(category);

        if (trig & Button_DPad_Up)    { option = (option - 1 + count) % count; inputDelay = 5; }
        if (trig & Button_DPad_Down)  { option = (option + 1) % count; inputDelay = 5; }
        if (trig & Button_DPad_Left)  { category = (category - 1 + NUM_CATS) % NUM_CATS; option = 0; inputDelay = 5; }
        if (trig & Button_DPad_Right) { category = (category + 1) % NUM_CATS; option = 0; inputDelay = 5; }
        if (trig & Button_A)          { executeAction(); inputDelay = 12; }

        if ((trig & Button_X) && (category == 0 || category == 1))
        {
            uint8_t id = (category == 0) ? weapons[option].itemId : bottles[option].itemId;
            const char* name = (category == 0) ? weapons[option].name : bottles[option].name;
            removeItem(id);
            LOG("Removed item: %s (0x%02X)", name, id);
            inputDelay = 12;
        }

        if (trig & Button_Start)
        {
            menuActive = false;
            libtp::display::setConsole(false, 25);
            libtp::display::clearConsole(0, 0);
            inputDelay = 10;
        }
    }

    void Mod::drawMenu()
    {
        auto& s = dComIfG_gameInfo.save.save_file.player.player_status_a;
        auto& rec = dComIfG_gameInfo.save.save_file.player.player_item_record;

        libtp::display::print(0, ".----------------------------------------.");

        char title[62];
        snprintf(title, sizeof(title),
                 "| ~ TWILIGHT PRINCESS ~  MOD MENU        |");
        libtp::display::print(1, title);

        char stats[62];
        snprintf(stats, sizeof(stats),
                 "| HP:%02d/%02d  $:%04d  Arrows:%03d  Bombs:%02d |",
                 s.currentHealth/2, s.maxHealth/2,
                 s.currentRupees, rec.bow_ammo, rec.bomb_bag_1_ammo);
        libtp::display::print(2, stats);
        libtp::display::print(3, "|----------------------------------------|");

        // Category tabs
        char tabLine[62] = {};
        int p = 0;
        tabLine[p++] = '|';
        for (int i = 0; i < NUM_CATS; i++)
        {
            if (p >= 40) break;
            if (i == category) tabLine[p++] = '<';
            else tabLine[p++] = ' ';
            const char* n = catNames[i];
            while (*n && p < 40) tabLine[p++] = *n++;
            if (i == category) tabLine[p++] = '>';
            else tabLine[p++] = ' ';
        }
        while (p < 41) tabLine[p++] = ' ';
        tabLine[p++] = '|';
        tabLine[p] = '\0';
        libtp::display::print(4, tabLine);
        libtp::display::print(5, "|----------------------------------------|");

        int count = getOptCount(category);
        for (int i = 0; i < count && i < 11; i++)
        {
            char opt[62] = {};
            const char* cur = (i == option) ? ">>" : "  ";

            if (category == 0) {
                snprintf(opt, sizeof(opt), "| %s [%s] %-29s|",
                         cur, hasItem(weapons[i].itemId) ? "+" : "-", weapons[i].name);
            } else if (category == 1) {
                snprintf(opt, sizeof(opt), "| %s [%s] %-29s|",
                         cur, hasItem(bottles[i].itemId) ? "+" : "-", bottles[i].name);
            } else if (category == 2) {
                snprintf(opt, sizeof(opt), "| %s %-33s|", cur, hpOpts[i]);
            } else if (category == 3) {
                snprintf(opt, sizeof(opt), "| %s %-33s|", cur, rupOpts[i]);
            } else if (category == 4) {
                snprintf(opt, sizeof(opt), "| %s %-33s|", cur, spawnOpts[i]);
            } else {
                snprintf(opt, sizeof(opt), "| %s %-33s|", cur, teleports[i].name);
            }
            libtp::display::print(static_cast<uint8_t>(6 + i), opt);
        }
        for (int i = count; i < 11; i++)
            libtp::display::print(static_cast<uint8_t>(6 + i),
                                  "|                                        |");

        libtp::display::print(17, "|----------------------------------------|");
        if (category == 0 || category == 1)
            libtp::display::print(18, "| A:Donner X:Retirer  DPad:Nav  St:Close |");
        else if (category == 5)
            libtp::display::print(18, "| A:Teleporter  DPad:Nav  Start:Close  |");
        else
            libtp::display::print(18, "| A:Executer  DPad:Naviguer  St:Close  |");
        libtp::display::print(19, "'----------------------------------------'");
        libtp::display::print(20, "");
        libtp::display::print(21, "  Mod developed by shdxw");
        libtp::display::print(22, "  All rights reserved");
        libtp::display::print(23, "");
        libtp::display::print(24, "");
    }

    void Mod::executeAction()
    {
        auto& s = dComIfG_gameInfo.save.save_file.player.player_status_a;

        LOG("Execute action: cat=%d (%s) opt=%d", category, catNames[category], option);

        switch (category)
        {
        case 0:
            LOG("Give weapon: %s (0x%02X)", weapons[option].name, weapons[option].itemId);
            execItemGet(weapons[option].itemId);
            break;
        case 1:
            LOG("Give bottle: %s (0x%02X)", bottles[option].name, bottles[option].itemId);
            execItemGet(bottles[option].itemId);
            break;
        case 2:
            switch (option) {
            case 0: s.maxHealth += 2; if(s.maxHealth>80) s.maxHealth=80; s.currentHealth=s.maxHealth; break;
            case 1: s.maxHealth += 10; if(s.maxHealth>80) s.maxHealth=80; s.currentHealth=s.maxHealth; break;
            case 2: s.currentHealth = s.maxHealth; break;
            case 3: s.maxHealth=40; s.currentHealth=40; break;
            case 4: if(s.maxHealth>2) s.maxHealth-=2; if(s.currentHealth>s.maxHealth) s.currentHealth=s.maxHealth; break;
            case 5: if(s.maxHealth>10) s.maxHealth-=10; else s.maxHealth=2; if(s.currentHealth>s.maxHealth) s.currentHealth=s.maxHealth; break;
            case 6: s.maxHealth=2; s.currentHealth=2; break;
            }
            LOG("Health: %s -> HP=%d/%d", hpOpts[option], s.currentHealth/2, s.maxHealth/2);
            break;
        case 3:
            switch (option) {
            case 0: s.currentRupees+=10; break;
            case 1: s.currentRupees+=20; break;
            case 2: s.currentRupees+=50; break;
            case 3: s.currentRupees+=100; break;
            case 4: { uint16_t m=300; if(s.currentWallet==BIG_WALLET)m=600; if(s.currentWallet==GIANT_WALLET)m=1000; s.currentRupees=m; break; }
            case 5: s.currentWallet=GIANT_WALLET; s.currentRupees=1000; break;
            case 6: if(s.currentRupees>=10) s.currentRupees-=10; else s.currentRupees=0; break;
            case 7: if(s.currentRupees>=50) s.currentRupees-=50; else s.currentRupees=0; break;
            case 8: s.currentRupees=0; break;
            }
            LOG("Rupees: %s -> $%d", rupOpts[option], s.currentRupees);
            break;
        case 4:
            LOG("Spawn enemy: %s (ID=0x%04X)", spawnOpts[option], enemyIDs[option]);
            spawnEnemy(option);
            break;
        case 5: // Teleport
            if (option >= 0 && option < NUM_TP) {
                auto& tp = teleports[option];
                LOG("Warp to: %s (stage=%s room=%d spawn=%d)", tp.name, tp.stage, tp.room, tp.spawn);
                dComIfGp_setNextStage(tp.stage, tp.spawn, tp.room, -1, 0.0f, 0, 1, 0x0F, 0, 0, 0);
                menuActive = false;
                libtp::display::setConsole(false, 25);
                libtp::display::clearConsole(0, 0);
            }
            break;
        }
    }

    void Mod::giveAllItems() {}
    void Mod::giveWeapons() {}
    void Mod::giveBottles() {}

    void Mod::fillAmmo()
    {
        auto& rec = dComIfG_gameInfo.save.save_file.player.player_item_record;
        auto& mx = dComIfG_gameInfo.save.save_file.player.player_item_max;
        rec.bow_ammo = 100; mx.max_arrow_capacity = 100;
        rec.bomb_bag_1_ammo = 60; rec.bomb_bag_2_ammo = 60; rec.bomb_bag_3_ammo = 60;
    }

    void Mod::fullHeal() {}
    void Mod::addHeart() {}
    void Mod::maxHearts() {}
    void Mod::maxRupees() {}
    void Mod::maxWallet() {}

    void Mod::spawnEnemy(int idx)
    {
        if (idx < 0 || idx >= NUM_SPAWN) return;
        float pos[3] = {0.0f, 0.0f, 0.0f};
        int16_t rot[3] = {0, 0, 0};
        float scale[3] = {1.0f, 1.0f, 1.0f};
        libtp::tp::f_op_actor_mng::fopAcM_create(
            static_cast<int16_t>(enemyIDs[idx]), 0, pos, 0, rot, scale, -1);
    }
}
