#include "collision.h"

void checkCollision()
{
    checkCollisionPlayer();
    checkCollisionEnemy();
    checkEnemiesInFrame();
    checkBulletsInFrame();
}

void checkCollisionPlayer()
{    
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