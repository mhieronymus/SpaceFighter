#include "draw.h"

bool drawGame()
{
    arduboy.clear();
    drawEnemies();
    if(player.alive)
        drawPlayer();
    drawSupply();
    drawBullets();
    drawStars();
    drawScore();
    bool finished = drawExplosions();
    arduboy.display();
    return finished;
}

void showTitle()
{
    bool showingTitle = true;
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
            bool showingHighscore = true;
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

void showHighscore()
{
    byte y = 10;
    byte x = 24;
    byte score;
    char initials[3]; 
    char textArray[10]; 
    
    // Each block of EEPROM has 10 high scores, and each high score entry
    // is 5 byte long:  3 byte for initials and two byte for score.
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

void drawStars()
{
    for(byte i=0; i<numberOfStars; i++)
    {
        arduboy.drawPixel(stars[i].x, stars[i].y, 1);
    }
}

void drawPlayer()
{
    arduboy.drawBitmap(player.x, player.y, ship0, player.height, player.width, 1);
}

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

bool drawExplosions()
{
    bool finished = true;
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

void drawPause()
{
    bool paused = true;
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

void drawGameOver()
{
    text.setCursor(52, 42);
    text.print( "Game");
    text.setCursor(52, 54);
    text.print("Over");
    arduboy.display();
}