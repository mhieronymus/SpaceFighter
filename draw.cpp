#include "draw.h"

bool drawGame()
{
    arduboy.clear();
    drawScore();
    drawLives();
    drawEnemies();
    if(player.alive)
        drawPlayer();
    drawSupply();
    drawBullets();
    drawStars();
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
    arduboy.setCursor(32, 0);
    arduboy.print("HIGH SCORES");
    arduboy.display();

    for(byte i=0; i<10; i++)
    {
        sprintf(textArray, "%2d", i+1);
        arduboy.setCursor(x,y+(i*8));
        arduboy.print(textArray);
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
            arduboy.setCursor(x + 24, y + (i*8));
            arduboy.print(textArray);
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
        if(supplies[i].alive)
        {
            arduboy.drawPixel(supplies[i].x, supplies[i].y, 1);
            arduboy.drawPixel(supplies[i].x-1, supplies[i].y, 1);
            arduboy.drawPixel(supplies[i].x+1, supplies[i].y, 1);
            arduboy.drawPixel(supplies[i].x, supplies[i].y+1, 1);
            arduboy.drawPixel(supplies[i].x, supplies[i].y-1, 1);
            
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
    arduboy.setCursor(70, 0);
    arduboy.print("Score:");
    arduboy.setCursor(110, 0);
    arduboy.print(player.score);
    if(DEBUG)
    {
       // arduboy.print(numberOfBullets);
        byte test = 0;
        for(byte i=0; i<numberOfBullets; i++)
        {
            if(bullets[i].alive)
            {
                test = test + 1;
            }
        }
        arduboy.setCursor(70, 0);
        arduboy.print(test);
        arduboy.setCursor(3, 0);
        arduboy.print(player.bullets);
    }
}

void drawLives()
{
    // Draw multiple hearts.
    if(player.lives < 4)
    {
        for(byte i=0; i<player.lives; i++)
        {
            arduboy.drawPixel(i*7+2, 1, 1);
            arduboy.drawPixel(i*7+3, 1, 1);
            arduboy.drawPixel(i*7+1, 0, 1);
            arduboy.drawPixel(i*7+4, 0, 1);
            arduboy.drawPixel(i*7, 1, 1);
            arduboy.drawPixel(i*7+5, 1, 1);
            arduboy.drawPixel(i*7, 2, 1);
            arduboy.drawPixel(i*7+5, 2, 1);
            arduboy.drawPixel(i*7+1, 3, 1);
            arduboy.drawPixel(i*7+4, 3, 1);
            arduboy.drawPixel(i*7+2, 4, 1);
            arduboy.drawPixel(i*7+3, 4, 1);
        }
    } else
    {
        // Draw an heart.
        arduboy.drawPixel(2, 1, 1);
        arduboy.drawPixel(3, 1, 1);
        arduboy.drawPixel(1, 0, 1);
        arduboy.drawPixel(4, 0, 1);
        arduboy.drawPixel(0, 1, 1);
        arduboy.drawPixel(5, 1, 1);
        arduboy.drawPixel(0, 2, 1);
        arduboy.drawPixel(5, 2, 1);
        arduboy.drawPixel(1, 3, 1);
        arduboy.drawPixel(4, 3, 1);
        arduboy.drawPixel(2, 4, 1);
        arduboy.drawPixel(3, 4, 1);
        // Draw a cross.
        arduboy.drawPixel(7, 1, 1);
        arduboy.drawPixel(8, 2, 1);
        arduboy.drawPixel(9, 3, 1);
        arduboy.drawPixel(7, 3, 1);
        arduboy.drawPixel(9, 1, 1);
        // Get the number of lives.
        arduboy.setCursor(11, 0);
        arduboy.print(player.lives);
    }
}

void drawPause()
{
    bool paused = true;
    //Draw pause to the screen
    arduboy.setCursor(52, 45);
    arduboy.print("PAUSE");
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
    arduboy.setCursor(52, 42);
    arduboy.print( "Game");
    arduboy.setCursor(52, 54);
    arduboy.print("Over");
    arduboy.display();
}