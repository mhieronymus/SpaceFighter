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

// Define buttons.
#define FIRE_BUTTON 1
#define PAUSE_BUTTON 2
#define DOWN 64
#define RIGHT 4
#define UP 16
#define LEFT 32
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
    // set invincible to 1 and then every two second a left-shift until 
    // invincible is zero. 
    // This way you are 16 seconds invincible. Or less, if you set
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
Enemy enemies[MAXENEMIES];
byte numberOfEnemies = 0;
Bullet bullets[MAXBULLETS];
byte numberOfBullets;
Supply supplies[MAXSUPPLY];
byte noOfSupplies = 0;
boolean gameStarted = false;

/**
 * @brief Initialize player and enemies
 */
void initGame()
{
    initPlayer();
    initEnemies();
    initSupplies();
    initBullets();
}

/**
 * @brief Initialize the player's position, size etc.
 */
void initPlayer()
{   
    player.x = 0;
    player.y = 0;
    player.width = 14;
    player.height = 14;
    player.score = 0;
    player.numberOfSuperbombs = 0;
    player.invincible = 0;
    player.bulletType = 1;
    player.bulletSpeed = 0;
    player.bullets = 0;
    player.maxBullets = 3;
    player.speed = 3; // Lower is better. 
    player.alive = true;
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
 * @brief Titlescreen with sound - SpaceFighter by me.
 * TODO: create nice music.
 */
void showTitle()
{
    while(true)
    {
        arduboy.clearDisplay();
        // X, Y, name, width, height, color
        arduboy.drawBitmap(0, 0, title, 128, 64, 1); 
        arduboy.display();
        if(arduboy.pressed(FIRE_BUTTON) || arduboy.pressed(PAUSE_BUTTON))
            break;
        // delay(2000);
        // arduboy.clearDisplay();
        // arduboy.setCursor(2,25);
        // arduboy.print("by Maicon Hieronymus"); 
        // arduboy.display();
        // if(arduboy.pressed(FIRE_BUTTON) || arduboy.pressed(PAUSE_BUTTON))
           // break;
        // delay(1000);
        // if(arduboy.pressed(FIRE_BUTTON) || arduboy.pressed(PAUSE_BUTTON))
            // break;
    }
}

/*
 * @brief Calls all necessary functions to generate the current display.
 */
void drawGame()
{
    arduboy.clearDisplay();
    generateStar();
    generateEnemy();
    drawEnemies();
    drawPlayer();
    drawSupply();
    drawBullets();
    drawStars();
    arduboy.display();
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
    // byte y = 10;
    // byte x = 24;
    // // Each block of EEPROM has 10 high scores, and each high score entry
    // // is 5 bytes long:  3 bytes for initials and two bytes for score.
    // int address = 2*10*5;
    // byte hi, lo;
    // arduboy.clearDisplay();
    // arduboy.drawBitmap(0, 0, highscore, 128, 24, 1);
    // arduboy.display();

    // for(int i=0; i<10; i++)
    // {
        // sprintf(text, "%2d", i+1);
        // arduboy.setCursor(x,y+(i*8));
        // arduboy.print(text);
        // arduboy.display();
        // hi = EEPROM.read(address + (5*i));
        // lo = EEPROM.read(address + (5*i) + 1);

        // if ((hi == 0xFF) && (lo == 0xFF))
        // {
            // score = 0;
        // }
        // else
        // {
            // score = (hi << 8) | lo;
        // }

        // initials[0] = (char)EEPROM.read(address + (5*i) + 2);
        // initials[1] = (char)EEPROM.read(address + (5*i) + 3);
        // initials[2] = (char)EEPROM.read(address + (5*i) + 4);

        // if (score > 0)
        // {
            // sprintf(text, "%c%c%c %u", initials[0], initials[1], initials[2], score);
            // arduboy.setCursor(x + 24, y + (i*8));
            // arduboy.print(text);
            // arduboy.display();
        // }
    // }
    // arduboy.display();
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
    if(random(0, 4) > numberOfEnemies && numberOfEnemies <= MAXENEMIES) 
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
        } else if(e.shipType < 4)
        {
            e.height = 5;
            e.width = 6;
            e.speed = 2;
        } else if(e.shipType < 8)
        {
            e.height = 7;
            e.width = 8;
            e.speed = 3;
        } else if(e.shipType < 16)
        {
            e.height = 7;
            e.width = 8;
            e.speed = 10;
        } else if(e.shipType < 32)
        {
            e.height = 5;
            e.width = 6;
            e.speed = 2;
        } else if(e.shipType < 64)
        {
            e.height = 7;
            e.width = 8;
            e.speed = 3;
        } else if(e.shipType < 128)
        {
            e.height = 7;
            e.width = 8;
            e.speed = 5;
        } else if(e.shipType < 256)
        {
            e.height = 5;
            e.width = 6;
            e.speed = 2;
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

void moveGame()
{
    moveStars();
//   moveSupplies();
    moveBullets();
    if(arduboy.everyXFrames(player.speed))
    {
        movePlayer();
    }
    moveEnemies();
    if(arduboy.everyXFrames(10))
    {
        playerShoots();
        // enemiesShoot();
    }
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
                
            case 135:
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
    // byte input = arduboy.getInput();
    switch(arduboy.getInput())
    {
        case (LEFT + UP):
        case (LEFT + UP + FIRE_BUTTON):
        case (LEFT + UP + PAUSE_BUTTON):
        case (LEFT + UP + FIRE_BUTTON + PAUSE_BUTTON):
            if(player.x > 0)
                player.x--;
        case UP:
        case (UP + FIRE_BUTTON):
        case (UP + PAUSE_BUTTON):
        case (UP + PAUSE_BUTTON + FIRE_BUTTON):
            if(player.y > 0)
                player.y--;
            break;
            
        case (RIGHT + DOWN):
        case (RIGHT + DOWN + FIRE_BUTTON):
        case (RIGHT + DOWN + PAUSE_BUTTON):
        case (RIGHT + DOWN + PAUSE_BUTTON + FIRE_BUTTON):
            if(player.x+player.width < 127)
                player.x++;
        case DOWN:
        case (DOWN + FIRE_BUTTON):
        case (DOWN + PAUSE_BUTTON):
        case (DOWN + PAUSE_BUTTON + FIRE_BUTTON):
            if(player.y+player.height < 63)
                player.y++;
            break;
            
        case (DOWN + LEFT):
        case (DOWN + LEFT + FIRE_BUTTON):
        case (DOWN + LEFT + PAUSE_BUTTON):
        case (DOWN + LEFT + PAUSE_BUTTON + FIRE_BUTTON):
            if(player.y+player.height < 63)
                player.y++;
        case LEFT:
        case (LEFT + FIRE_BUTTON):
        case (LEFT + PAUSE_BUTTON):
        case (LEFT + PAUSE_BUTTON + FIRE_BUTTON):
            if(player.x > 0)
                player.x--;
            break;
            
        case (UP + RIGHT):
        case (UP + RIGHT + FIRE_BUTTON):
        case (UP + RIGHT + PAUSE_BUTTON):
        case (UP + RIGHT + PAUSE_BUTTON + FIRE_BUTTON):
            if(player.y > 0)
                player.y--;
        case RIGHT:
        case (RIGHT + FIRE_BUTTON):
        case (RIGHT + PAUSE_BUTTON):
        case (RIGHT + PAUSE_BUTTON + FIRE_BUTTON):
            if(player.x+player.width < 127)
                player.x++;
            break;
            
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
    byte i = 0;

    while(numberOfBullets<MAXBULLETS && i<numberOfEnemies)
    {
        //TODO Check if it is time for the enemy to shoot. Might depend on
        // shiptype.
        if(enemies[i].shipType < 2)
        {
            Bullet b;
            // Shoot the bullet up left from the enemy.
            b.x = enemies[i].x + 1;
            b.y = enemies[i].y + 1;
            // Height and width depend on appearance which correlates to the
            // bitmap.
            b.appearance = 1;
            b.height = 2;
            b.width = 2;
            // There are no lifepoints for the player. Every hit is a kill.
            b.damage = 0;
            b.speed = (player.destroyedShips << 3) + 2;
            b.alive = true;
            b.playersBullet = false;
            b.direction = 90;
            bullets[numberOfBullets-1] = b;
            numberOfBullets++;
        } else if(supplies[i].type < 4)
        {
         // TODO   
        } else if(supplies[i].type < 8)
        {

        } else if(supplies[i].type < 16)
        {

        } else if(supplies[i].type < 32)
        {

        } else if(supplies[i].type < 64)
        {

        } else if(supplies[i].type < 128)
        {

        } else if(supplies[i].type < 256)
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
    if( arduboy.getInput()%2 == FIRE_BUTTON)
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
            b.direction = 1;
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
    boolean alreadyDead = false;
    for(byte i=0; i<numberOfBullets; i++)
    {
        // First check if player and enemy/bullet collide on the x-coordinate
        // and then on the y-coordinate. 
        if((abs(player.x-bullets[i].x) < player.width 
            && bullets[i].x >= player.x)
            &&(abs(player.y-bullets[i].y) < player.height 
            && bullets[i].y >= player.y)
            && bullets[i].playersBullet == false)
        {
            player.alive = false;
            if(!alreadyDead) 
            {
                player.lives--;
                alreadyDead = true;
            }
            bullets[i].alive = false;
        }
    }
    
    for(byte i=0; i<numberOfEnemies; i++)
    {
        if((abs(enemies[i].x-player.x) < enemies[i].width 
            || abs(player.x-enemies[i].x) < player.width)
            &&( abs(enemies[i].y-player.y) < enemies[i].height 
            || abs(player.y-enemies[i].y) < player.height))
        {
            player.alive = false;
            if(!alreadyDead) 
            {
                player.lives--;
                alreadyDead = true;
            }
            enemies[i].alive = false;
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
            && bullets[j].y >= enemies[i].y))                                                          
            {
                bullets[j].alive = false;
                enemies[i].alive = false;
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
    
}

/**
 * @brief Pause
 */
void pause()
{
    
}

/**
 * @brief draw Pause
 */
void drawPause()
{
    
}

/**
 * @brief game over
 */
void gameOver()
{
    
}

/**
 * @brief draw game over
 */
void drawGameOver()
{
    
}

/**
 * @brief Destroy all dead objects.
 */
void checkAlive()
{
    byte movedObjects = 1;
    for(byte i=numberOfBullets-1; i>=0; i--)
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
    for(byte i=numberOfEnemies-1; i>=0; i--)
    {
        if(!enemies[i].alive)
        {
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
        gameStarted = false; // Alter it.
    }
}
 
void setup()
{
    arduboy.start();
    arduboy.setFrameRate(60);
      // setRGBled(10,0,0);
    for(int8_t y = -18; y<=24; y++) {
        arduboy.clearDisplay();
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
        drawGame();
        moveGame();
        checkCollision();
        checkAlive();
    } else
    {
        showTitle();
        gameStarted = true;
        initGame();
    }
}
