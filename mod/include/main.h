#pragma once
#include <tp/f_ap_game.h>
#include <cinttypes>

namespace mod
{
    void main();
    void exit();

    class Mod
    {
       public:
        Mod();
        void init();

       private:
        // Trampoline to original fapGm_Execute
        void (*return_fapGm_Execute)() = nullptr;

        // Menu state
        bool menuActive;
        int category;
        int option;
        int inputDelay;

        // Called each frame
        void procNewFrame();
        // Input handling
        void handleInput();
        // Draw menu
        void drawMenu();
        // Execute selected action
        void executeAction();

        // Actions
        void giveAllItems();
        void giveWeapons();
        void giveBottles();
        void fillAmmo();
        void fullHeal();
        void addHeart();
        void maxHearts();
        void maxRupees();
        void maxWallet();
        void spawnEnemy(int idx);
    };
}
