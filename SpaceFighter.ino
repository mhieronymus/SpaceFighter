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

#include "variables.h"
#include "draw.h"
#include "movement.h"
#include "collision.h"
#include "generator.h"

Arduboy arduboy;
byte frameCounter;
// The stars in the background are saved here.
Star stars[MAXSTARS];
byte numberOfStars = 0;
byte SCREEN_HEIGHT = 64;
byte SCREEN_WIDTH = 128;
Player player;
Enemy enemies[MAXENEMIES];
byte numberOfEnemies = 0;
Bullet bullets[MAXBULLETS];
byte numberOfBullets;
Supply supplies[MAXSUPPLY];
byte noOfSupplies = 0;
bool gameStarted = false;
Explosion explosions[MAXENEMIES+1];

AbPrinter text(arduboy);

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

void setup()
{
    arduboy.begin();
    arduboy.setFrameRate(60);
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
            generateStar();
            generateEnemy();
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
