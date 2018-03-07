#include "generator.h"

void initGame(bool newGame) {
    initPlayer(newGame);
    initEnemies();
    initSupplies();
    initBullets();
    initExplosions();
    boss_coming = false;
    extra_tick = 0;
    last_boss = 0;
}

void initPlayer(bool newGame) {
    player.x = 0;
    player.y = 0;
    player.width = 14;
    player.height = 14;
    player.invincible = 2;

    if(newGame) {
        player.score = 0;
        player.speed = 1; // Higher is better.
        player.lives = 4;
        player.bulletType = 1;
        player.bulletSpeed = 1;
        player.maxBullets = 3;
        player.fireSpeed = 80;
    }

    player.lifepoints = 3;
    player.alive = true;
    player.bullets = 0;
    player.cooldown = 0;
    player.firetype = 0;
}

void initEnemies() {
    for(byte i=0; i<MAXENEMIES; ++i) {
        enemies[i].alive=false;
    }
}

void initBullets() {
    for(byte i=0; i<MAXBULLETS; ++i) {
        bullets[i].alive=false;
        bullets[i].playersBullet = false;
    }
}

void initSupplies() {
    for(byte i=0; i<MAXSUPPLY; ++i) {
        supplies[i].alive=false;
    }
}

void initExplosions() {
    for(byte i=0; i<=MAXENEMIES; ++i) {
        explosions[i].tick=200;
    }
}

void generateStar() {
    byte rand = random(1, 100);
    // Make sure there are not too many stars.
    if(rand<5 && numberOfStars < MAXSTARS) {
        Star s;
        s.x = SCREEN_WIDTH-1;
        // Create a y-coordinate between bottom and top.
        s.y = random(0, SCREEN_HEIGHT);
        stars[numberOfStars] = s;
        numberOfStars++;
    }
}

void generateBoss() {
    if(numberOfEnemies == 0) {
        Enemy e;
        e.x = SCREEN_WIDTH-13;
        e.y = 96;
        e.y = 90;
        e.alive = true;
        e.movement = 32;
        e.tick = 0;
        e.supply = true;
        e.shipType = 32;
        e.height = 32;
        e.width = 13;
        e.speed = 10;
        e.lifepoints = 30 + player.score/10;
        enemies[numberOfEnemies] = e;
        numberOfEnemies++;
    }
}

void generateEnemy() {
    byte max_level_enemies = 4 + player.score/100;
    if(random(0, max_level_enemies) > numberOfEnemies
        && numberOfEnemies < MAXENEMIES && random(0,100) > 85)  {

        Enemy e;
        e.x = SCREEN_WIDTH;
        e.y = random(10, SCREEN_HEIGHT-10);
        byte rnd = random(0, 5);
        e.alive = true;
        e.direction = MOVE_LEFT;
        e.tick = 0;
        if(random(0, 100) > 85 && MAXSUPPLY > noOfSupplies) {
            e.supply = true;
        }
        switch(rnd) {
            case 0:
                e.movement = 4;
                e.shipType = 1;
                e.height = 7;
                e.width = 8;
                e.speed = 7;
                e.lifepoints = random(5, 5+player.destroyedShips);
                break;
            case 1:
                e.movement = 8;
                e.shipType = 2;
                e.height = 5;
                e.width = 6;
                e.speed = 4;
                e.lifepoints = 1 + player.score/50;
                break;
            case 2:
                e.movement = 2;
                e.direction = MOVE_DOWNLEFT;
                e.shipType = 4;
                e.height = 7;
                e.width = 8;
                e.speed = 5;
                e.lifepoints = 1 + player.score/60;
                break;
            case 3:
                e.shipType = 8;
                e.height = 5;
                e.width = 4;
                e.speed = 1; 
                e.lifepoints = 1;
                break;
            case 4:
                e.movement = 8;
                e.shipType = 16;
                e.height = 8;
                e.width = 5;
                e.speed = 3;
                e.lifepoints = 1 + player.score/10;
                break;
            case 5:
                e.shipType = 32;
                e.height = 7;
                e.width = 8;
                e.speed = 1;
                e.lifepoints = 1 + player.score/20;
                break;
            case 6:
                e.movement = 16;
                e.shipType = 64;
                e.height = 7;
                e.width = 8;
                e.speed = 1;
                e.lifepoints = 1 + player.score/30;
                break;
            default:
                e.movement = 0; // Straight line
                e.shipType = 128;
                e.height = 5;
                e.width = 6;
                e.speed = 1;//
                e.lifepoints = 8;
                break;
        }
        enemies[numberOfEnemies] = e;
        numberOfEnemies++;
    }
}

void createSupply(byte x, byte y) {
    Supply s;
    s.x = x;
    s.y = y;
    s.alive = true;
    supplies[noOfSupplies] = s;
    noOfSupplies++;
    drawSupply();
}
