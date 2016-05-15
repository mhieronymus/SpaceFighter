#include "generator.h"

void initGame(bool newGame)
{
    initPlayer(newGame);
    initEnemies();
    initSupplies();
    initBullets();
    initExplosions();
}

void initPlayer(bool newGame)
{   
    player.x = 0;
    player.y = 0;
    player.width = 14;
    player.height = 14;
    if(newGame)
        player.score = 0;
    player.numberOfSuperbombs = 0;
    player.invincible = 1;
    player.bulletType = 1;
    player.bulletSpeed = 0;
    player.bullets = 0;
    player.maxBullets = 2;
    player.speed = 3; // Lower is better. 
    player.alive = true;
    if(newGame)
        player.lives = 3;
}

void initEnemies()
{
    for(byte i=0; i<MAXENEMIES; i++)
    {
        enemies[i].alive=false;
    }
}

void initBullets()
{
    for(byte i=0; i<MAXBULLETS; i++)
    {
        bullets[i].alive=false;
        bullets[i].playersBullet = false;
    }
}

void initSupplies()
{
    for(byte i=0; i<MAXSUPPLY; i++)
    {
        supplies[i].alive=false;
    }
}

void initExplosions()
{
    for(byte i=0; i<=MAXENEMIES; i++)
    {
        explosions[i].tick=100;
    }
}

void generateStar()
{
    byte rand = random(1, 100); 
    // Make sure there are not too many stars.
    if(rand<5 && numberOfStars < MAXSTARS)
    {
        Star s;
        s.x = SCREEN_WIDTH-1;
        // Create a y-coordinate between bottom and top.
        s.y = random(0, SCREEN_HEIGHT);
        stars[numberOfStars] = s;
        numberOfStars++;
    }
}

void generateEnemy()
{
    if(random(0, 4) > numberOfEnemies && numberOfEnemies <= MAXENEMIES && random(0,100) > 85) 
    {   
        Enemy e;
        e.x = SCREEN_WIDTH;
        e.y = random(10, SCREEN_HEIGHT-10);
        e.shipType = random(0, 8); 
        e.lifepoints = random(1+player.destroyedShips, 10+player.destroyedShips);
        e.alive = true;
        e.direction = MOVE_LEFT; //random(0, 8)
        e.movement = 0; // Straight line
        if(random(0, 100) > 95 && MAXSUPPLY >= noOfSupplies)
        {
            e.supply = true;
        }
        if(e.shipType < 2)
        {
            e.shipType = 1;
            e.height = 7;
            e.width = 8;
            e.speed = 10;
            e.maxBullets = 1;
        } else if(e.shipType < 4)
        {
            e.shipType = 2;
            e.height = 5;
            e.width = 6;
            e.speed = 2;
            e.maxBullets = 3;
        } else if(e.shipType < 8)
        {
            e.movement = 2;
            e.direction = MOVE_DOWNLEFT;
            e.shipType = 4;
            e.height = 7;
            e.width = 8;
            e.speed = 3;
            e.maxBullets = 2;
        } else if(e.shipType < 16)
        {
            e.shipType = 8;
            e.height = 7;
            e.width = 8;
            e.speed = 10;
            e.maxBullets = 1;
        } else if(e.shipType < 32)
        {
            e.shipType = 16;
            e.height = 5;
            e.width = 6;
            e.speed = 2;
            e.maxBullets = 3;
        } else if(e.shipType < 64)
        {
            e.shipType = 32;
            e.height = 7;
            e.width = 8;
            e.speed = 3;
            e.maxBullets = 2;
        } else if(e.shipType < 128)
        {
            e.shipType = 64;
            e.height = 7;
            e.width = 8;
            e.speed = 10;
            e.maxBullets = 1;
        } else if(e.shipType < 256)
        {
            e.shipType = 128;
            e.height = 5;
            e.width = 6;
            e.speed = 2;
            e.maxBullets = 3;
        }
        enemies[numberOfEnemies] = e;
        numberOfEnemies++;
    }
}

void createSupply(byte x, byte y)
{
    Supply s;
    s.x = x;
    s.y = y;
    s.type = random(0,8);
    s.alive = true;
    supplies[noOfSupplies] = s;
    drawSupply();
}