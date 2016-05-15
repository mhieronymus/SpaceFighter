/* 
 * SpaceFighter
 * Copyright (C) 2015 
 * Maicon Hieronymus <mhierony@students.uni-mainz.de>
 * All rights reserved.
 * 
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 * -----------------------------------------------------------------------------
 *  
 * @file        SpaceFighter.ino
 * @date        14.10.2015
 * @version     0.1
 * @author      Maicon Hieronymus <mhierony@students.uni-mainz.de>
 *
 * @brief This program is a game similar to Snake on old mobile phones. 
 * It is coded with Arduboy in mind. Since I do not have an Arduboy yet,
 * I have not tested anything yet. 
 * Enter name and display highscore are taken from Sebastian Goscik as seen in
 * https://github.com/Arduboy/Arduboy/blob/master/examples/ArduBreakout/ArduBreakout.ino
 * and slightly modified.
 * This code uses the old library of Arduboy and has been tested 
 * with a Developer Kit.
 */
#include "Arduboy.h"
#include "SpaceFighter_bitmaps.h"

// Define maximum amount of particles.
#define MAXBULLETS 50
#define MAXSTARS 25
#define MAXENEMIES 10
#define MAXSUPPLY 3
// Define Movements
#define MOVE_UP 45
#define MOVE_UPLEFT 90
#define MOVE_LEFT 135
#define MOVE_DOWNLEFT 180
#define MOVE_DOWN 225
#define MOVE_DOWNRIGHT 255
#define MOVE_RIGHT 1
#define MOVE_UPRIGHT 0
#define DEBUG false

typedef struct
{
    // All coordinates are the upper right corner of an image.
    byte x;
    byte y;
    byte height;
    byte width;
    unsigned int score;
    byte destroyedShips;
    byte lives;
    boolean alive;
    byte numberOfSuperbombs;
    // set invincible to 1 and then every two second a right-shift until 
    // invincible is zero. 
    // This way you are 2 seconds invincible. Or more, if you set
    // invincible to 2, 4 or 8.
    byte invincible;
    byte bulletType;
    byte bulletSpeed;
    // The player cannot fire infinite bullets (and therefore stun the enemy).
    byte bullets;
    byte maxBullets;
    byte speed; // Lower is better.
} Player;

typedef struct
{
    byte x;
    byte y;
} Star;

typedef struct
{
    // All coordinates are the upper right corner of an image.
    byte x;
    byte y;
    byte height;
    byte width;
    // Each number is a placeholder for different ships. These types define
    // the image, bullettype and speed. The highest 1 indicates the type
    // e.g. 0100 is the same type as 0111 or 0101 or 0110.
    byte shipType;
    byte speed; // Lower is better.
    byte lifepoints;
    boolean alive;
    // Each number is a placeholder for another movement, e.g. straight-forward
    // or sine. Needs to be implemented.
    byte movement;
    byte direction;
    // If true, create a random supply after destruction.
    boolean supply;
    byte maxBullets;
} Enemy;

typedef struct
{
    // All coordinates are the upper right corner of an image.
    byte x;
    byte y;
    byte height;
    byte width;
    byte appearance;
    byte damage;
    byte speed;
    boolean alive;
    boolean playersBullet;
    // Use 255 degrees, where 0 is right up, 45 is straight up, 45+180 straight
    // down and 45+90 is to the left, etc. 1 is used for the player's bullets.
    //              90      45      0
    //            135      enemy    1
    //              180     225     255
    byte direction;
} Bullet;

typedef struct
{
    byte x;
    byte y;
    boolean alive;
    // Each number is a placeholder for different supplies, e.g. extra live
    // superbomb, weapon upgrade or invincibility (or even ship upgrade?).
    // 0 is most rare, >8 is most common.
    byte type;
} Supply;

typedef struct
{
    byte x;
    byte y;
    byte tick; // Each tick is part of a different animation
} Explosion;


// Counter the frames. Every 60 frames is one second. This can be used for 
// measuring time.
byte frameCounter;
// The stars in the background are saved here.
Star stars[MAXSTARS];
byte numberOfStars = 0;
byte SCREEN_HEIGHT = 64;
byte SCREEN_WIDTH = 128;
Player player;
Arduboy arduboy;
AbPrinter text(arduboy);
Enemy enemies[MAXENEMIES];
byte numberOfEnemies = 0;
Bullet bullets[MAXBULLETS];
byte numberOfBullets;
Supply supplies[MAXSUPPLY];
byte noOfSupplies = 0;
boolean gameStarted = false;
Explosion explosions[MAXENEMIES+1];

/**
 * @brief Initialize player and enemies
 */
void initGame(boolean newGame)
{
    initPlayer(newGame);
    initEnemies();
    initSupplies();
    initBullets();
    initExplosions();
}

/**
 * @brief Initialize the player's position, size etc.
 */
void initPlayer(boolean newGame)
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

/**
 * @brief Kill all enemies for the beginning.
 */
void initEnemies()
{
    for(byte i=0; i<MAXENEMIES; i++)
    {
        enemies[i].alive=false;
    }
}

/**
 * @brief Destroy all supplies.
 */
void initBullets()
{
    for(byte i=0; i<MAXBULLETS; i++)
    {
        bullets[i].alive=false;
        bullets[i].playersBullet = false;
    }
}

/**
 * @brief Destroy all bullets.
 */
void initSupplies()
{
    for(byte i=0; i<MAXSUPPLY; i++)
    {
        supplies[i].alive=false;
    }
}

/**
 * @brief Stop all explosion animations.
 */
void initExplosions()
{
    for(byte i=0; i<=MAXENEMIES; i++)
    {
        explosions[i].tick=100;
    }
}

/**
 * @brief Titlescreen with sound - SpaceFighter by me.
 * TODO: create nice music.
 */
void showTitle()
{
    boolean showingTitle = true;
    while(showingTitle)
    {
        arduboy.clear();
        // X, Y, name, width, height, color
        arduboy.drawBitmap(0, 0, title, 128, 64, 1); 
        arduboy.display();
        if(arduboy.pressed(A_BUTTON))
            showingTitle = false;
        if(arduboy.pressed(B_BUTTON))
        {
            delay(200);
            boolean showingHighscore = true;
            showHighscore();
            while(showingHighscore)
            {
                if(arduboy.pressed(A_BUTTON)) 
                {
                    showingTitle = false;
                    showingHighscore = false;
                }
                if(arduboy.pressed(B_BUTTON))
                    showingHighscore = false;
            }
        }
        delay(200);
        // arduboy.clear();
        // text.setCursor(2,25);
        // text.print("by Maicon Hieronymus"); 
        // arduboy.display();
        // if(arduboy.pressed(B_BUTTON) || arduboy.pressed(A_BUTTON))
           // break;
        // delay(1000);
        // if(arduboy.pressed(B_BUTTON) || arduboy.pressed(A_BUTTON))
            // break;
    }
}

/*
 * @brief Calls all necessary functions to generate the current display.
 * @return True if all explosions have been drawn.
 */
boolean drawGame()
{
    arduboy.clear();
    generateStar();
    generateEnemy();
    drawEnemies();
    if(player.alive)
        drawPlayer();
    drawSupply();
    drawBullets();
    drawStars();
    drawScore();
    boolean finished = drawExplosions();
    arduboy.display();
    return finished;
}

/**
 * @brief Display Highscore with the scores. 
 * This method is from Sebastian Goscik as seen in
 * https://github.com/Arduboy/Arduboy/blob/master/examples/ArduBreakout/ArduBreakout.ino
 * and slightly modified.
 * Function by nootropic design to display highscores.
 * TODO: Alter for bigger scores.
 */
void showHighscore()
{
    byte y = 10;
    byte x = 24;
    byte score;
    char initials[3]; 
    char textArray[10]; 
    
    // Each block of EEPROM has 10 high scores, and each high score entry
    // is 5 bytes long:  3 bytes for initials and two bytes for score.
    int address = 2*10*5;
    byte hi, lo;
    arduboy.clear();
    text.setCursor(32, 0);
    text.print("HIGH SCORES");
    arduboy.display();

    for(byte i=0; i<10; i++)
    {
        sprintf(textArray, "%2d", i+1);
        text.setCursor(x,y+(i*8));
        text.print(textArray);
        arduboy.display();
        hi = EEPROM.read(address + (5*i));
        lo = EEPROM.read(address + (5*i) + 1);

        if ((hi == 0xFF) && (lo == 0xFF))
        {
            score = 0;
        }
        else
        {
            score = (hi << 8) | lo;
        }

        initials[0] = (char)EEPROM.read(address + (5*i) + 2);
        initials[1] = (char)EEPROM.read(address + (5*i) + 3);
        initials[2] = (char)EEPROM.read(address + (5*i) + 4);

        if (score > 0)
        {
            sprintf(textArray, "%c%c%c %u", initials[0], initials[1], initials[2], score);
            text.setCursor(x + 24, y + (i*8));
            text.print(textArray);
            arduboy.display();
        }
    }
    arduboy.display();
}

/**
 * @brief Draw the player in its current state.
 * TODO: Check for current powerups and choose different ships.
 */
void drawPlayer()
{
    arduboy.drawBitmap(player.x, player.y, ship0, player.height, player.width, 1);
}

/**
 * @brief Generate a random star for the background.
 */
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

/**
 * @brief Draw all stars in the background.
 */
void drawStars()
{
    for(byte i=0; i<numberOfStars; i++)
    {
        arduboy.drawPixel(stars[i].x, stars[i].y, 1);
    }
}

/**
 * @brief Generate random enemies with their strength based on the player's
 * score and the number of enemies on the screen. May generate nothing.
 * You may use this every second.
 * TODO: Check if the room is free to place the new enemy.
 */
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
            e.height = 7;
            e.width = 8;
            e.speed = 10;
            e.maxBullets = 1;
        } else if(e.shipType < 4)
        {
            e.height = 5;
            e.width = 6;
            e.speed = 2;
            e.maxBullets = 3;
        } else if(e.shipType < 8)
        {
            e.height = 7;
            e.width = 8;
            e.speed = 3;
            e.maxBullets = 2;
        } else if(e.shipType < 16)
        {
            e.height = 7;
            e.width = 8;
            e.speed = 10;
            e.maxBullets = 1;
        } else if(e.shipType < 32)
        {
            e.height = 5;
            e.width = 6;
            e.speed = 2;
            e.maxBullets = 3;
        } else if(e.shipType < 64)
        {
            e.height = 7;
            e.width = 8;
            e.speed = 3;
            e.maxBullets = 2;
        } else if(e.shipType < 128)
        {
            e.height = 7;
            e.width = 8;
            e.speed = 10;
            e.maxBullets = 1;
        } else if(e.shipType < 256)
        {
            e.height = 5;
            e.width = 6;
            e.speed = 2;
            e.maxBullets = 3;
        }
        enemies[numberOfEnemies] = e;
        numberOfEnemies++;
    }
}

/**
 * @brief draw all the enemies.
 * TODO: Get the size of each image right.
 */
void drawEnemies()
{
    for(byte i=1; i<=numberOfEnemies; i++)
    {
        if(enemies[i-1].shipType < 2)
        {
            arduboy.drawBitmap(enemies[i-1].x, enemies[i-1].y, enemy0, enemies[i-1].width, enemies[i-1].height, 1);
        } else if(enemies[i-1].shipType < 4)
        {
            arduboy.drawBitmap(enemies[i-1].x, enemies[i-1].y, enemy1, enemies[i-1].width, enemies[i-1].height, 1);
        } else if(enemies[i-1].shipType < 8)
        {
            arduboy.drawBitmap(enemies[i-1].x, enemies[i-1].y, enemy2, enemies[i-1].width, enemies[i-1].height, 1);
        } else if(enemies[i-1].shipType < 16)
        {
            arduboy.drawBitmap(enemies[i-1].x, enemies[i-1].y, enemy0, enemies[i-1].width, enemies[i-1].height, 1);
        } else if(enemies[i-1].shipType < 32)
        {
            arduboy.drawBitmap(enemies[i-1].x, enemies[i-1].y, enemy1, enemies[i-1].width, enemies[i-1].height, 1);
        } else if(enemies[i-1].shipType < 64)
        {
            arduboy.drawBitmap(enemies[i-1].x, enemies[i-1].y, enemy2, enemies[i-1].width, enemies[i-1].height, 1);
        } else if(enemies[i-1].shipType < 128)
        {
            arduboy.drawBitmap(enemies[i-1].x, enemies[i-1].y, enemy0, enemies[i-1].width, enemies[i-1].height, 1);
        } else if(enemies[i-1].shipType < 256)
        {
            arduboy.drawBitmap(enemies[i-1].x, enemies[i-1].y, enemy0, enemies[i-1].width, enemies[i-1].height, 1);
        }
    }
}

/**
 * @brief Create supply at the place where the enemy died.
 * @param[in] x X-Coordinate of the destroyed enemy.
 * @param[in] y Y-Coordinate of the destroyed enemy.
 */
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

/**
 * @brief Draw supply
 */
void drawSupply()
{
    for(byte i=0; i<noOfSupplies; i++)
    {
        if(supplies[i].type < 2)
        {
            // arduboy.drawBitmap(0, 0, supply_00, 128, 64, 1);
        } else if(supplies[i].type < 4)
        {
            // arduboy.drawBitmap(0, 0, supply_01, 128, 64, 1);
        } else if(supplies[i].type < 8)
        {
            // arduboy.drawBitmap(0, 0, supply_02, 128, 64, 1);
        } else if(supplies[i].type < 16)
        {
            // arduboy.drawBitmap(0, 0, supply_03, 128, 64, 1);
        } else if(supplies[i].type < 32)
        {
            // arduboy.drawBitmap(0, 0, supply_04, 128, 64, 1);
        } else if(supplies[i].type < 64)
        {
            // arduboy.drawBitmap(0, 0, supply_05, 128, 64, 1);
        } else if(supplies[i].type < 128)
        {
            // arduboy.drawBitmap(0, 0, supply_06, 128, 64, 1);
        } else if(supplies[i].type < 256)
        {
            // arduboy.drawBitmap(0, 0, supply_07, 128, 64, 1);
        }
    }
}

/*
 * @brief Bullets are 2x2 squares which are drawn in this function.
 */
void drawBullets()
{
    for(byte i=0; i<numberOfBullets; i++)
    {
        arduboy.drawPixel(bullets[i].x, bullets[i].y, 1);
        arduboy.drawPixel(bullets[i].x-1, bullets[i].y, 1);
        arduboy.drawPixel(bullets[i].x, bullets[i].y-1, 1);
        arduboy.drawPixel(bullets[i].x-1, bullets[i].y-1, 1);
    }
}

/*
 * @brief Draws an explosion for destroyed ships.
 * @return: True if all explosions are finished.
 */
boolean drawExplosions()
{
    boolean finished = true;
    // Not very efficiently...
    for(byte i=0; i<=MAXENEMIES; i++)
    {
        if(explosions[i].tick < 15)
        {
            finished = false;
            arduboy.drawPixel(explosions[i].x, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-1, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y, 1);
            explosions[i].tick++;
        } else if(explosions[i].tick < 30)
        {
            finished = false;
            arduboy.drawPixel(explosions[i].x, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-1, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y, 1);
            
            arduboy.drawPixel(explosions[i].x, explosions[i].y+2, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-2, 1);
            arduboy.drawPixel(explosions[i].x-2, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x+2, explosions[i].y, 1);
            // diagonal lines
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y-1, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y-1, 1);
            
            explosions[i].tick++;
        } else if(explosions[i].tick < 45)
        {
            finished = false;
            arduboy.drawPixel(explosions[i].x, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-1, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y, 1);
            
            arduboy.drawPixel(explosions[i].x, explosions[i].y+2, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-2, 1);
            arduboy.drawPixel(explosions[i].x-2, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x+2, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y+3, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-3, 1);
            arduboy.drawPixel(explosions[i].x-3, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x+3, explosions[i].y, 1);
            
            // diagonal lines
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y-1, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y-1, 1);
            arduboy.drawPixel(explosions[i].x+2, explosions[i].y+2, 1);
            arduboy.drawPixel(explosions[i].x+2, explosions[i].y-2, 1);
            arduboy.drawPixel(explosions[i].x-2, explosions[i].y+2, 1);
            arduboy.drawPixel(explosions[i].x-2, explosions[i].y-2, 1);
            
            explosions[i].tick++;
        } else if(explosions[i].tick < 60)
        {
            finished = false;
            arduboy.drawPixel(explosions[i].x, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-1, 1);
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y, 1);
            
            arduboy.drawPixel(explosions[i].x, explosions[i].y+4, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-4, 1);
            arduboy.drawPixel(explosions[i].x+4, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x-4, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y+3, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-3, 1);
            arduboy.drawPixel(explosions[i].x+3, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x-3, explosions[i].y, 1);
            
            // diagonal lines
            arduboy.drawPixel(explosions[i].x+3, explosions[i].y+3, 1);
            arduboy.drawPixel(explosions[i].x+3, explosions[i].y-3, 1);
            arduboy.drawPixel(explosions[i].x-3, explosions[i].y+3, 1);
            arduboy.drawPixel(explosions[i].x-3, explosions[i].y-3, 1);
            arduboy.drawPixel(explosions[i].x+2, explosions[i].y+2, 1);
            arduboy.drawPixel(explosions[i].x+2, explosions[i].y-2, 1);
            arduboy.drawPixel(explosions[i].x-2, explosions[i].y+2, 1);
            arduboy.drawPixel(explosions[i].x-2, explosions[i].y-2, 1);
            
            explosions[i].tick++;
        } else if(explosions[i].tick < 75)
        {
            finished = false;
            arduboy.drawPixel(explosions[i].x, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y+1, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-1, 1);
            arduboy.drawPixel(explosions[i].x+1, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x-1, explosions[i].y, 1);
            
            arduboy.drawPixel(explosions[i].x, explosions[i].y+4, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-4, 1);
            arduboy.drawPixel(explosions[i].x+4, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x-4, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y+5, 1);
            arduboy.drawPixel(explosions[i].x, explosions[i].y-5, 1);
            arduboy.drawPixel(explosions[i].x+5, explosions[i].y, 1);
            arduboy.drawPixel(explosions[i].x-5, explosions[i].y, 1);
            
            // diagonal lines
            arduboy.drawPixel(explosions[i].x+3, explosions[i].y+3, 1);
            arduboy.drawPixel(explosions[i].x+3, explosions[i].y-3, 1);
            arduboy.drawPixel(explosions[i].x-3, explosions[i].y+3, 1);
            arduboy.drawPixel(explosions[i].x-3, explosions[i].y-3, 1);
            arduboy.drawPixel(explosions[i].x+4, explosions[i].y+4, 1);
            arduboy.drawPixel(explosions[i].x+4, explosions[i].y-4, 1);
            arduboy.drawPixel(explosions[i].x-4, explosions[i].y+4, 1);
            arduboy.drawPixel(explosions[i].x-4, explosions[i].y-4, 1);
            
            explosions[i].tick++;
        }
    }
    return finished;
}

void moveGame()
{
    if(arduboy.everyXFrames(120) && player.invincible != 0)
    {
        player.invincible = player.invincible >> 1;
    }
    if(arduboy.everyXFrames(2))
        moveStars();
    
    moveSupplies();
    if(arduboy.everyXFrames(3))
        moveBullets();
    if(arduboy.everyXFrames(player.speed) && player.alive)
    {
        movePlayer();
    }
    moveEnemies();
    if(arduboy.everyXFrames(10) && player.alive)
    {
        playerShoots();
    }
    enemiesShoot();
}

/**
 * @brief Supplies move just straight to the left.
 */
void moveSupplies()
{
    for(byte i; i<noOfSupplies; i++)
    {
        supplies[i].x++;
    }
}

/**
 * @brief Bullets move
 * TODO: Check speed.
 */
void moveBullets()
{
    for(byte i=0; i<numberOfBullets; i++)
    {
        switch(bullets[i].direction)
        {
            case 0:
                bullets[i].x = bullets[i].x - 1 - bullets[i].speed;
                bullets[i].y = bullets[i].y + 1 + bullets[i].speed;
                break;
                
            case 45:
                bullets[i].y = bullets[i].y + 1 + bullets[i].speed;
                break;
                
            case 90:
                 bullets[i].x = bullets[i].x + 1 + bullets[i].speed;
                bullets[i].y = bullets[i].y + 1 + bullets[i].speed;
                break;
                
            case MOVE_LEFT:
                bullets[i].x = bullets[i].x - 1 - bullets[i].speed;
                break;
                
            case 180:
                bullets[i].x = bullets[i].x - 1 - bullets[i].speed;
                bullets[i].y = bullets[i].y - 1 - bullets[i].speed;
                break;
                
            case 225:
                bullets[i].y = bullets[i].y - 1 - bullets[i].speed;
                break;
                
            case 255:
                bullets[i].x = bullets[i].x + 1 + bullets[i].speed;
                bullets[i].y = bullets[i].y - 1 - bullets[i].speed;
                break;
            // MOVE_RIGHT
            default:
                 bullets[i].x = bullets[i].x + 1 + bullets[i].speed;
        }
    }
}

/**
 * @brief Player moves
 */
void movePlayer()
{
    if(arduboy.pressed(LEFT_BUTTON) && player.x > 0)
    {
        player.x--;
    }
    if(arduboy.pressed(UP_BUTTON) && player.y > 0)
    {
        player.y--;
    }
    if(arduboy.pressed(DOWN_BUTTON) && player.y+player.height < 63)
    {
        player.y++;
    }
    if(arduboy.pressed(RIGHT_BUTTON) && player.x+player.width < 127)
    {
        player.x++;
    }
}


/**
 * @brief Enemies move
 * TODO: Alter the directions. Check for speed. 
 */
void moveEnemies()
{
    for(byte i=1; i<=numberOfEnemies; i++)
    {
        if(arduboy.everyXFrames(enemies[i-1].speed))
        {
        
            boolean blockedUP = false;
            boolean blockedDOWN = false;
            boolean blockedLEFT = false;
            boolean blockedRIGHT = false;
            
            // Check for collisions.
            for(byte j=1; j<=numberOfEnemies; j++)
            {
                if(j != i)
                {
                    if(enemies[j-1].x+enemies[j-1].width == enemies[i-1].x-1 &&
                       (abs(enemies[j-1].y-enemies[i-1].y) < enemies[i-1].height 
                       || abs(enemies[j-1].y-enemies[i-1].y) < enemies[j-1].height))
                    {
                        blockedLEFT = true;
                    }
                    if(enemies[j-1].x == enemies[i-1].x+1+enemies[i-1].width &&
                       (abs(enemies[j-1].y-enemies[i-1].y) < enemies[i-1].height 
                       || abs(enemies[j-1].y-enemies[i-1].y) < enemies[j-1].height))
                    {
                        blockedRIGHT = true;
                    }
                    if(enemies[j-1].y == enemies[i-1].y+1+enemies[i-1].height  &&
                       (abs(enemies[j-1].x-enemies[i-1].x) < enemies[i-1].width 
                       || abs(enemies[j-1].x-enemies[i-1].x) < enemies[j-1].width))
                    {
                        blockedDOWN = true;
                    }
                    if(enemies[j-1].y + enemies[j-1].height == enemies[i-1].y-1 &&
                       (abs(enemies[j-1].x-enemies[i-1].x) < enemies[i-1].width 
                       || abs(enemies[j-1].x-enemies[i-1].x) < enemies[j-1].width))
                    {
                        blockedUP = true;
                    }
                }
            }
            
            //TODO Alter direction of enemy
            switch(enemies[i-1].direction)
            {
                // UP and RIGHT+UP
                case MOVE_UPRIGHT:
                    if(!blockedRIGHT) enemies[i-1].x = enemies[i-1].x + 1;
                case MOVE_UP:
                    if(!blockedUP) enemies[i-1].y = enemies[i-1].y - 1;
                    break;
                    
                // UP+LEFT
                case MOVE_UPLEFT:
                    if(!blockedLEFT) enemies[i-1].x = enemies[i-1].x - 1;
                    if(!blockedUP) enemies[i-1].y = enemies[i-1].y - 1;
                    break;
                    
                // Left and LEFT+DOWN
                case MOVE_DOWNLEFT:
                    if(!blockedDOWN) enemies[i-1].y = enemies[i-1].y + 1;
                case MOVE_LEFT:
                    if(!blockedLEFT) enemies[i-1].x = enemies[i-1].x - 1;
                    break;
                    
                // DOWN and DOWN+RIGHT
                case MOVE_DOWNRIGHT:
                    if(!blockedRIGHT) enemies[i-1].x = enemies[i-1].x + 1;
                case MOVE_DOWN:
                    if(blockedDOWN) enemies[i-1].y = enemies[i-1].y + 1;
                    break;
                    
                default:
                    if(!blockedLEFT) enemies[i-1].x = enemies[i-1].x - 1;
            }
        }
    }
}

/**
 * @brief Stars move
 */
void moveStars()
{
    for(byte i=0; i<numberOfStars; i++)
    {
        stars[i].x--;
    }
}

/**
 * @brief Enemies shoot
 */
void enemiesShoot()
{
    byte i = 1;

    while(numberOfBullets<MAXBULLETS && i<numberOfEnemies)
    {
        if(enemies[i-1].shipType < 2 && arduboy.everyXFrames(120) && random(0,100) > 60)
        {
            Bullet b;
            // Shoot the bullet up left from the enemy.
            b.x = enemies[i-1].x + 1;
            b.y = enemies[i-1].y + (enemies[i-1].height >> 1);
            // Height and width depend on appearance which correlates to the
            // bitmap.
            b.appearance = 1;
            b.height = 2;
            b.width = 2;
            // There are no lifepoints for the player. Every hit is a kill.
            b.damage = 0;
            b.speed = 0;
            b.alive = true;
            b.playersBullet = false;
            b.direction = MOVE_LEFT;
            bullets[numberOfBullets] = b;
            numberOfBullets++;
        } else if(enemies[i-1].shipType < 4 && arduboy.everyXFrames(60) && random(0,100) > 70)
        {
            Bullet b;
            // Shoot the bullet up left from the enemy.
            b.x = enemies[i-1].x + 1;
            b.y = enemies[i-1].y + 1;
            // Height and width depend on appearance which correlates to the
            // bitmap.
            b.appearance = 1;
            b.height = 2;
            b.width = 2;
            // There are no lifepoints for the player. Every hit is a kill.
            b.damage = 0;
            b.speed = 0;
            b.alive = true;
            b.playersBullet = false;
            b.direction = MOVE_LEFT;
            bullets[numberOfBullets] = b;
            numberOfBullets++;
        } else if(enemies[i-1].shipType < 8 && arduboy.everyXFrames(240) && random(0,100) > 50)
        {
            Bullet b;
            // Shoot the bullet up left from the enemy.
            b.x = enemies[i-1].x + 1;
            b.y = enemies[i-1].y + 1;
            // Height and width depend on appearance which correlates to the
            // bitmap.
            b.appearance = 1;
            b.height = 2;
            b.width = 2;
            // There are no lifepoints for the player. Every hit is a kill.
            b.damage = 0;
            b.speed = 0;
            b.alive = true;
            b.playersBullet = false;
            b.direction = MOVE_LEFT;
            bullets[numberOfBullets] = b;
            numberOfBullets++;
        } else if(enemies[i-1].shipType < 16)
        {

        } else if(enemies[i-1].shipType < 32)
        {

        } else if(enemies[i-1].shipType < 64)
        {

        } else if(enemies[i-1].shipType < 128)
        {

        } else if(enemies[i-1].shipType < 256)
        {
            
        }
        i++;
    }
}

/**
 * @brief Player shoots
 * TODO Add a delay so the player cannot shoot too much at once maybe?
 * Add different directions with different bullets.
 */
void playerShoots()
{
    if(arduboy.pressed(B_BUTTON))
    {
        if(MAXBULLETS > numberOfBullets 
            && player.bullets < player.maxBullets)
        {
            Bullet b;
            // A bullet should appear at front of the ship in the middle.
            b.x = player.x + 1;
            b.y = player.y + (player.height >> 1);
            b.appearance = player.bulletType;
            b.damage = player.bulletType;
            b.speed = player.bulletSpeed;
            b.alive = true;
            b.playersBullet = true;
            b.direction = MOVE_RIGHT;
            player.bullets++;
            bullets[numberOfBullets] = b;
            numberOfBullets++;
        }
    }
}

/**
 * @brief Call all collision functions.
 */
void checkCollision()
{
    checkCollisionPlayer();
    checkCollisionEnemy();
    checkEnemiesInFrame();
    checkBulletsInFrame();
}

/**
 * @brief Player: Check collision with bullet, enemy and supply.
 */
void checkCollisionPlayer()
{
    text.setCursor(10,10);
    text.print(player.invincible);
    
    if(player.alive)
    {
        for(byte i=0; i<numberOfBullets; i++)
        {
            // First check if player and enemy/bullet collide on the x-coordinate
            // and then on the y-coordinate. 
            if((abs(player.x-bullets[i].x) < player.width-3
                && bullets[i].x >= player.x)
                &&(abs(player.y-bullets[i].y) < player.height-1
                && bullets[i].y >= player.y)
                && !bullets[i].playersBullet && player.invincible == 0)
            {
                player.alive = false;
                bullets[i].alive = false;
                player.lives--;
                if(DEBUG)
                {
                    arduboy.clear();
                    text.setCursor(0,0);
                    text.print("BULLET");
                    text.setCursor(0,20);
                    text.print(bullets[i].x);
                    text.setCursor(0,40);
                    text.print(bullets[i].y);
                    text.setCursor(32,20);
                    text.print(bullets[i].width);
                    text.setCursor(32,40);
                    text.print(bullets[i].height);
                    
                    text.setCursor(64,0);
                    text.print("PLAYER");
                    text.setCursor(64,20);
                    text.print(player.x);
                    text.setCursor(64,40);
                    text.print(player.y);
                    text.setCursor(96,20);
                    text.print(player.width);
                    text.setCursor(96,40);
                    text.print(player.height);
                    
                    arduboy.display();
                }
                break;
            }
        }
    }
    if(player.alive)
    {
        for(byte i=0; i<numberOfEnemies; i++)
        {
            if(((enemies[i].x-player.x < player.width && enemies[i].x > player.x)
                || (player.x-enemies[i].x < enemies[i].width && enemies[i].x < player.x))
                && ((enemies[i].y-player.y < player.height && enemies[i].y > player.y)
                || (player.y-enemies[i].y < enemies[i].height && enemies[i].y < player.y))
                && player.invincible == 0)
            {
                player.alive = false;
                enemies[i].alive = false;
                player.lives--;
                if(DEBUG)
                {
                    arduboy.clear();
                    text.setCursor(0,0);
                    text.print("ENEMY");
                    text.setCursor(0,20);
                    text.print(enemies[i].x);
                    text.setCursor(0,40);
                    text.print(enemies[i].y);
                    text.setCursor(32,20);
                    text.print(enemies[i].width);
                    text.setCursor(32,40);
                    text.print(enemies[i].height);
                    
                    text.setCursor(64,0);
                    text.print("PLAYER");
                    text.setCursor(64,20);
                    text.print(player.x);
                    text.setCursor(64,40);
                    text.print(player.y);
                    text.setCursor(96,20);
                    text.print(player.width);
                    text.setCursor(96,40);
                    text.print(player.height);
                    
                    arduboy.display();
                }
                break;
            }
        }
    }
}

/**
 * @brief Enemy: Check shot.
 * TODO: if(noOfSupplies = supplie.length) no supply
 */
void checkCollisionEnemy()
{
    for(byte i=0; i<numberOfEnemies; i++)
    {
        for(byte j=0; j<numberOfBullets; j++)
        {
            if((abs(enemies[i].x-bullets[j].x) < enemies[i].width 
            && bullets[j].x >= enemies[i].x)
            &&(abs(enemies[i].y-bullets[j].y) < enemies[i].height 
            && bullets[j].y >= enemies[i].y)
            && bullets[j].playersBullet)                                                          
            {
                bullets[j].alive = false;
                enemies[i].alive = false;
                player.score++;
            }
        }
    }
}

/**
 * @brief Enemy: Check if still in frame
 */
void checkEnemiesInFrame()
{
    for(byte i=0; i<numberOfEnemies; i++)
    {
        if(enemies[i].x > 128 || enemies[i].y > 64 || enemies[i].x == 0 || enemies[i].y == 0)
        {
            enemies[i].alive = false;
        }
    }
}

/**
 * @brief Bullet: Check if still in frame
 */
void checkBulletsInFrame()
{
    for(byte i=0; i<numberOfBullets; i++)
    {
        if(bullets[i].x > 128 || bullets[i].y > 64 || bullets[i].x == 0 || bullets[i].y == 0)
        {
            bullets[i].alive = false;
        }
    }
}

/**
 * @brief Draw score
 */
void drawScore()
{
    // We use this for debugging. Draw numberOfBullets:
    text.setCursor(110, 0);
    text.print(player.score);
    if(DEBUG)
    {
       // text.print(numberOfBullets);
        byte test = 0;
        for(byte i=0; i<numberOfBullets; i++)
        {
            if(bullets[i].alive)
            {
                test = test + 1;
            }
        }
        text.setCursor(70, 0);
        text.print(test);
        text.setCursor(3, 0);
        text.print(player.bullets);
    }
}

/**
 * @brief draw Pause. The Code is taken from ArduBreakout by Sebastian Goscik
 * and has been modified.
 */
void drawPause()
{
    boolean paused = true;
    //Draw pause to the screen
    text.setCursor(52, 45);
    text.print("PAUSE");
    arduboy.display();
    while(paused)
    {
        delay(200);
        //Unpause if FIRE is pressed
        if(arduboy.getInput() == A_BUTTON)
        {
            arduboy.fillRect(52, 45, 30, 11, 0);
            paused=false;
        }
    }
    delay(150);
}

/**
 * @brief game over
 */
void gameOver()
{
    drawGameOver();
    delay(4000);
    enterHighScore(2);
}

/**
 * @brief draw game over
 */
void drawGameOver()
{
    text.setCursor(52, 42);
    text.print( "Game");
    text.setCursor(52, 54);
    text.print("Over");
    arduboy.display();
}

/**
 * @brief Function by nootropic design to add high scores. The Code is taken from ArduBreakout by Sebastian Goscik.
 */
void enterInitials()
{
    char index = 0;
    char initials[3];
    char text_buffer[10];

    arduboy.clear();
    initials[0] = ' ';
    initials[1] = ' ';
    initials[2] = ' ';
    while (true)
    {
        arduboy.display();
        arduboy.clear();

        text.setCursor(16,0);
        text.print("HIGH SCORE");
        sprintf(text_buffer, "%u", player.score);
        text.setCursor(88, 0);
        text.print(text_buffer);
        text.setCursor(56, 20);
        text.print(initials[0]);
        text.setCursor(64, 20);
        text.print(initials[1]);
        text.setCursor(72, 20);
        text.print(initials[2]);
        for(byte i = 0; i < 3; i++)
        {
            arduboy.drawLine(56 + (i*8), 27, 56 + (i*8) + 6, 27, 1);
        }
        arduboy.drawLine(56, 28, 88, 28, 0);
        arduboy.drawLine(56 + (index*8), 28, 56 + (index*8) + 6, 28, 1);
        delay(150);

        if (arduboy.pressed(LEFT_BUTTON) || arduboy.pressed(B_BUTTON))
        {
            index--;
            if (index < 0)
            {
                index = 0;
            } 
        }

        if (arduboy.pressed(RIGHT_BUTTON))
        {
            index++;
            if (index > 2)
            {
                index = 2;
            }
        }

        if (arduboy.pressed(DOWN_BUTTON))
        {
            initials[index]++;
            // A-Z 0-9 :-? !-/ ' '
            if (initials[index] == '0')
            {
                initials[index] = ' ';
            }
            if (initials[index] == '!')
            {
                initials[index] = 'A';
            }
            if (initials[index] == '[')
            {
                initials[index] = '0';
            }
            if (initials[index] == '@')
            {
                initials[index] = '!';
            }
        }

        if (arduboy.pressed(UP_BUTTON))
        {
            initials[index]--;
            if (initials[index] == ' ') {
                initials[index] = '?';
            }
            if (initials[index] == '/') {
                initials[index] = 'Z';
            }
            if (initials[index] == 31) {
                initials[index] = '/';
            }
            if (initials[index] == '@') {
                initials[index] = ' ';
            }
        }

        if (arduboy.pressed(A_BUTTON))
        {
            if (index < 2)
            {
                index++;
            } else {
                return;
            }
        }
    }
}

/*
 * @brief Enter HighScore. The Code is taken from ArduBreakout by Sebastian Goscik.
 */
void enterHighScore(byte file)
{
    // Each block of EEPROM has 10 high scores, and each high score entry
    // is 5 bytes long:  3 bytes for initials and two bytes for score.
    int address = file * 10 * 5;
    byte hi, lo;
    char tmpInitials[3];
    char initials[3];
    unsigned int tmpScore = 0;

    // High score processing
    for(byte i = 0; i < 10; i++)
    {
        hi = EEPROM.read(address + (5*i));
        lo = EEPROM.read(address + (5*i) + 1);
        if ((hi == 0xFF) && (lo == 0xFF))
        {
            // The values are uninitialized, so treat this entry
            // as a score of 0.
            tmpScore = 0;
        } else
        {
            tmpScore = (hi << 8) | lo;
        }
        if (player.score > tmpScore)
        {
            enterInitials();
            for(byte j=i;j<10;j++)
            {
                hi = EEPROM.read(address + (5*j));
                lo = EEPROM.read(address + (5*j) + 1);

                if ((hi == 0xFF) && (lo == 0xFF))
                {
                    tmpScore = 0;
                }
                else
                {
                    tmpScore = (hi << 8) | lo;
                }

                tmpInitials[0] = (char)EEPROM.read(address + (5*j) + 2);
                tmpInitials[1] = (char)EEPROM.read(address + (5*j) + 3);
                tmpInitials[2] = (char)EEPROM.read(address + (5*j) + 4);

                // write score and initials to current slot
                EEPROM.write(address + (5*j), ((player.score >> 8) & 0xFF));
                EEPROM.write(address + (5*j) + 1, (player.score & 0xFF));
                EEPROM.write(address + (5*j) + 2, initials[0]);
                EEPROM.write(address + (5*j) + 3, initials[1]);
                EEPROM.write(address + (5*j) + 4, initials[2]);

                // tmpScore and tmpInitials now hold what we want to
                //write in the next slot.
                player.score = tmpScore;
                initials[0] = tmpInitials[0];
                initials[1] = tmpInitials[1];
                initials[2] = tmpInitials[2];
            }
            player.score = 0;
            initials[0] = ' ';
            initials[1] = ' ';
            initials[2] = ' ';
            return;
        }
    }
}

/**
 * @brief Destroy all dead objects.
 */
void checkAlive()
{
    byte movedObjects = 1;
    // int is needed or else i <- 0-1 = 255
    for(int i=numberOfBullets-1; i>=0; i--)
    {
        if(!bullets[i].alive)
        {
            if(bullets[i].playersBullet)
            {
                player.bullets--;
            } 
            if(i<numberOfBullets-movedObjects) 
            {
                bullets[i] = bullets[numberOfBullets-movedObjects];
                movedObjects++;
            } 
            numberOfBullets--;
        }
    }
    
    movedObjects = 1;
    // For all enemies, if he is not alive, check if he is within the frame.
    for(int i=numberOfEnemies-1; i>=0; i--)
    {
        if(!enemies[i].alive)
        {
            byte j=0;
            if(enemies[i].x <= 128 && enemies[i].y <= 64 && enemies[i].x > 0 && enemies[i].y > 0)
            {
                // Search the first free slot in explosions and add a new explosion.
                for(byte j = 0; j <= MAXENEMIES; j++)
                {
                    if(explosions[j].tick >= 75)
                    {
                        explosions[j].tick = 0;
                        explosions[j].x = enemies[i].x + (enemies[i].width >> 1);
                        explosions[j].y = enemies[i].y + (enemies[i].height >> 1);
                        j = 255;
                    }
                }
           }
            if(i<numberOfEnemies-movedObjects) 
            {
                enemies[i] = enemies[numberOfEnemies-movedObjects];
                movedObjects++;
            } 
            numberOfEnemies--;
        }
    }
    if(!player.alive)
    {
        // Add an explosion for the player.
        for(byte j = 0; j <= MAXENEMIES; j++)
        {
            if(explosions[j].tick >= 75)
            {
                explosions[j].tick = 0;
                explosions[j].x = player.x + (player.width >> 1);
                explosions[j].y = player.y + (player.height >> 1);
                j = 255;
            }
        }
    }
}
 
void setup()
{
    arduboy.begin();
    arduboy.setFrameRate(60);
      // setRGBled(10,0,0);
    for(int8_t y = -18; y<=24; y++) {
        arduboy.clear();
        arduboy.drawBitmap(20,y, arduboy_logo, 88, 16, WHITE);
        arduboy.display();
        delay(27);
        // longer delay post boot, we put it inside the loop to
        // save the flash calling clear/delay again outside the loop
        if (y==-16) 
        {
            delay(250);
        }
    }
    delay(750);
    arduboy.initRandomSeed();
}

/**
 * @brief TODO Check for alive player and set invincibility.
 */
void loop()
{
    // No rendering needed until the next frame is needed. Duh!
    if(!(arduboy.nextFrame()))
        return;
    if(gameStarted)
    {
        if(player.alive)
        {
            drawGame();
            moveGame();
            checkCollision();
            checkAlive();
            if(arduboy.getInput() == A_BUTTON)
            { 
                delay(200);
                drawPause();
            }
        } else 
        {
            if(drawGame())
            {
                gameStarted = false;
                delay(1000);
            } else {
                moveGame();
            }
           
        }
        if(!gameStarted)
        {
            if(player.lives == 0)
            {
                gameOver();
            } else
            {
                initGame(false);
                gameStarted = true;
            }
        }
    } else
    {
        showTitle();
        gameStarted = true;
        initGame(true);
    }
}
