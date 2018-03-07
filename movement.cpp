#include "movement.h"

void moveGame() {
    if(player.invincible != 0 && arduboy.everyXFrames(120)) {
        player.invincible = player.invincible >> 1;
    }
    moveStars();

    if(arduboy.everyXFrames(8)) moveSupplies();
    if(arduboy.everyXFrames(2)) moveBullets();
    if(player.alive) movePlayer();

    moveEnemies();
    if(player.cooldown != 0) player.cooldown--;
    if(player.cooldown == 0 && player.alive) playerShoots();

    enemiesShoot();
}

void moveSupplies() {
    for(byte i; i<noOfSupplies; i++) {
        if(supplies[i].alive)
            supplies[i].x--;
    }
}

void moveBullets() {
    for(byte i=0; i<numberOfBullets; i++) {
        switch(bullets[i].direction) {
            case MOVE_UPLEFT:
                bullets[i].y += bullets[i].speed;
            case MOVE_LEFT:
                bullets[i].x -= bullets[i].speed;
                break;
            
            case MOVE_UPRIGHT:
                bullets[i].x += bullets[i].speed;
            case MOVE_UP:
                bullets[i].y += bullets[i].speed;
                break;

            

            case MOVE_DOWNLEFT:
                bullets[i].x -= bullets[i].speed;
            case MOVE_DOWN:
                bullets[i].y -= bullets[i].speed;
                break;

            case MOVE_DOWNRIGHT:
                bullets[i].x += bullets[i].speed;
            default:
                 bullets[i].x += bullets[i].speed;
        }
    }
}

void movePlayer() {
    if(arduboy.pressed(LEFT_BUTTON) && player.x-player.speed > 0) 
        player.x -= player.speed;

    if(arduboy.pressed(UP_BUTTON) && player.y-player.speed > 0) 
        player.y -= player.speed;

    if(arduboy.pressed(DOWN_BUTTON) 
        && player.y+player.height+player.speed < 63) 
        player.y += player.speed;

    if(arduboy.pressed(RIGHT_BUTTON) 
        && player.x+player.width+player.speed < 127) 
        player.x += player.speed;
}

void moveEnemies() {
    for(byte i=1; i<=numberOfEnemies; i++) {
        if(arduboy.everyXFrames(enemies[i-1].speed)) {
            bool blockedUP = false;
            bool blockedDOWN = false;
            bool blockedLEFT = false;
            bool blockedRIGHT = false;

            // Check for collisions.
            for(byte j=1; j<=numberOfEnemies; j++) {
                if(j != i) {
                    if(enemies[j-1].x+enemies[j-1].width == enemies[i-1].x-1 &&
                       (abs(enemies[j-1].y-enemies[i-1].y) < enemies[i-1].height
                       || abs(enemies[j-1].y-enemies[i-1].y) < enemies[j-1].height)) {
                        blockedLEFT = true;
                    }
                    if(enemies[j-1].x == enemies[i-1].x+1+enemies[i-1].width &&
                       (abs(enemies[j-1].y-enemies[i-1].y) < enemies[i-1].height
                       || abs(enemies[j-1].y-enemies[i-1].y) < enemies[j-1].height)) {
                        blockedRIGHT = true;
                    }
                    if(enemies[j-1].y == enemies[i-1].y+1+enemies[i-1].height  &&
                       (abs(enemies[j-1].x-enemies[i-1].x) < enemies[i-1].width
                       || abs(enemies[j-1].x-enemies[i-1].x) < enemies[j-1].width)) {
                        blockedDOWN = true;
                    }
                    if(enemies[j-1].y + enemies[j-1].height == enemies[i-1].y-1 &&
                       (abs(enemies[j-1].x-enemies[i-1].x) < enemies[i-1].width
                       || abs(enemies[j-1].x-enemies[i-1].x) < enemies[j-1].width)) {
                        blockedUP = true;
                    }
                }
            }

            // Ships of type 1 just move to the left.
            // Default is just a safety thing. Move to the left in worst case.
            switch(enemies[i-1].movement) {
                case 1:
                    // Do not change the direction.
                    break;

                // Move up and down but always to the left.
                case 2:
                    if(enemies[i-1].tick < 15) {
                        enemies[i-1].direction = MOVE_UPLEFT;
                        enemies[i-1].tick++;
                    } else {
                        enemies[i-1].direction = MOVE_DOWNLEFT;
                        enemies[i-1].tick = (enemies[i-1].tick+1)%30;
                    }
                    break;

                // Move up, left, down, left, up etc.
                case 4:
                    if(enemies[i-1].tick < 15) {
                        enemies[i-1].direction = MOVE_LEFT;
                        enemies[i-1].tick++;
                    } else if(enemies[i-1].tick < 30) {
                        enemies[i-1].direction = MOVE_UP;
                        enemies[i-1].tick++;
                    } else if(enemies[i-1].tick < 45) {
                        enemies[i-1].direction = MOVE_LEFT;
                        enemies[i-1].tick++;
                    } else if(enemies[i-1].tick < 60) {
                        enemies[i-1].direction = MOVE_DOWN;
                        enemies[i-1].tick = (enemies[i-1].tick+1)%60;
                    }
                    break;

                // Go clockwise and some time to the left.
                case 8:
                    enemies[i-1].tick++;
                    if(enemies[i-1].tick%6 == 0 && enemies[i-1].tick != 48) {
                        enemies[i-1].direction = (enemies[i-1].direction + 1)%8;
                    } else if(enemies[i-1].tick == 48) {
                        enemies[i-1].direction = MOVE_LEFT;
                        enemies[i-1].tick = 0;
                    }
                    break;

                // Move towards the player.
                case 16:
                    enemies[i-1].tick++;
                    if(enemies[i-1].tick%15 == 0) {
                        bool up = false;
                        bool down = false;
                        bool right = false;
                        bool left = false;
                        // Calculate the direction between the player and the enemy.
                        // Should we go up or down?
                        if(player.y-player.height < enemies[i-1].y) {
                            up = true;
                        } else if(player.y > enemies[i-1].y-enemies[i-1].height) {
                            down = true;
                        }
                        // Check for left or right
                        if(player.x-player.width < enemies[i-1].x) {
                            left = true;
                        } else if(player.x > enemies[i-1].x-enemies[i-1].width) {
                            right = true;
                        }
                        if(up) {
                            enemies[i-1].direction = MOVE_UP;
                            if(right)
                                enemies[i-1].direction = MOVE_UPRIGHT;
                            if(left)
                                enemies[i-1].direction = MOVE_UPLEFT;
                        } else if(down) {
                            enemies[i-1].direction = MOVE_DOWN;
                            if(right)
                                enemies[i-1].direction = MOVE_DOWNRIGHT;
                            if(left)
                                enemies[i-1].direction = MOVE_DOWNLEFT;
                        } else if(left) {
                            enemies[i-1].direction = MOVE_LEFT;
                        } else if(right) {
                            enemies[i-1].direction = MOVE_RIGHT;
                        }
                    }
                    break;
                // Move up and down
                case 32:
                    if(enemies[i-1].y > SCREEN_HEIGHT-enemies[i-1].height) {
                        enemies[i-1].direction = MOVE_UP;
                        blockedUP = false;
                    } else if(enemies[i-1].tick < 30) {
                        enemies[i-1].direction = MOVE_UP;
                        enemies[i-1].tick++;
                    } else if(enemies[i-1].tick < 60) {
                        enemies[i-1].direction = MOVE_DOWN;
                        enemies[i-1].tick = (enemies[i-1].tick+1)%60;
                    } 
                    break;

                case 64:

                    break;

                case 128:

                    break;

                default:
                    enemies[i-1].direction = MOVE_LEFT;
            }

            switch(enemies[i-1].direction) {
                // UP and RIGHT+UP
                case MOVE_UPRIGHT:
                    if(!blockedRIGHT) 
                        enemies[i-1].x += 1;
                case MOVE_UP:
                    if(!blockedUP) 
                        enemies[i-1].y -= 1;
                    break;

                // UP+LEFT
                case MOVE_UPLEFT:
                    if(!blockedLEFT) 
                        enemies[i-1].x -= 1;
                    if(!blockedUP) 
                        enemies[i-1].y -= 1;
                    break;

                // Left and LEFT+DOWN
                case MOVE_DOWNLEFT:
                    if(!blockedDOWN) 
                        enemies[i-1].y += 1;
                case MOVE_LEFT:
                    if(!blockedLEFT) 
                        enemies[i-1].x -= 1;
                    break;

                // DOWN and DOWN+RIGHT
                case MOVE_DOWNRIGHT:
                    if(!blockedRIGHT) 
                        enemies[i-1].x += 1;
                case MOVE_DOWN:
                    if(!blockedDOWN) 
                        enemies[i-1].y += 1;
                    break;

                default:
                    if(!blockedLEFT) 
                        enemies[i-1].x -= 1;
            }
        }
    }
}

void moveStars() {
    for(byte i=0; i<numberOfStars; i++) {
        stars[i].x--;
    }
}

void enemiesShoot() {
    byte i = 1;

    while(numberOfBullets<MAXBULLETS && i<=numberOfEnemies) {
        switch(enemies[i-1].shipType) {
            case 1:
                if(arduboy.everyXFrames(120) && random(0,100) > 60) {
                    Bullet b;
                    // Shoot the bullet up left from the enemy.
                    b.x = enemies[i-1].x + 1;
                    b.y = enemies[i-1].y + (enemies[i-1].height >> 1);
                    b.height = 2;
                    b.width = 2;
                    b.damage = 3;
                    b.speed = 1;
                    b.alive = true;
                    b.playersBullet = false;
                    b.direction = MOVE_LEFT;
                    bullets[numberOfBullets] = b;
                    numberOfBullets++;
                }
                break;
            case 2:
                if(arduboy.everyXFrames(60) && random(0,100) > 70) {	
                    Bullet b;
                    // Shoot the bullet up left from the enemy.
                    b.x = enemies[i-1].x + 1;
                    b.y = enemies[i-1].y + 1;
                    b.height = 2;
                    b.width = 2;
                    b.damage = 1;
                    b.speed = 1;
                    b.alive = true;
                    b.playersBullet = false;
                    b.direction = MOVE_LEFT;
                    bullets[numberOfBullets] = b;
                    numberOfBullets++;
                }
                break;
            case 4:
                if(arduboy.everyXFrames(240) && random(0,100) > 50) {
                    Bullet b;
                    // Shoot the bullet up left from the enemy.
                    b.x = enemies[i-1].x + 1;
                    b.y = enemies[i-1].y + 1;
                    b.height = 2;
                    b.width = 2;
                    b.damage = 1;
                    b.speed = 1;
                    b.alive = true;
                    b.playersBullet = false;
                    b.direction = MOVE_LEFT;
                    bullets[numberOfBullets] = b;
                    numberOfBullets++;
                }
                break;
            case 8:
		if(arduboy.everyXFrames(20) && random(0,100) > 90) {
                    Bullet b;
                    // Shoot the bullet in front of the enemy.
                    b.x = enemies[i-1].x - 1;
                    b.y = enemies[i-1].y + 2;
                    b.height = 2;
                    b.width = 2;
                    b.damage = 2;
                    b.speed = 2;
                    b.alive = true;
                    b.playersBullet = false;
                    b.direction = MOVE_LEFT;
                    bullets[numberOfBullets] = b;
                    numberOfBullets++;
                }
                break;
            case 16:
                if(arduboy.everyXFrames(20) && random(0,100) > 65) {
                    for(byte j=0; j<2; j++) {
                        if(numberOfBullets<MAXBULLETS) {
                            Bullet b;
                            // Shoot the bullet in front of the enemy.
                            b.x = enemies[i-1].x - 1;
                            b.y = enemies[i-1].y + 3 + j;
                            b.height = 2;
                            b.width = 2;
                            b.damage = 2;
                            b.speed = 1;
                            b.alive = true;
                            b.playersBullet = false;
                            if(j == 0) {
                                    b.direction = MOVE_DOWNLEFT;
                            } else {
                                    b.direction = MOVE_UPLEFT;
                            }
                            bullets[numberOfBullets] = b;
                            numberOfBullets++;
                        }
                    }
                }
                break;
            case 32:
                // Shoot with upper two canons 
                if(arduboy.everyXFrames(60) && random(0,100) > 35) {	
                    if(numberOfBullets<MAXBULLETS) {
                        Bullet b;
                        b.x = enemies[i-1].x + 3;
                        b.y = enemies[i-1].y + 4;
                        b.height = 2;
                        b.width = 2;
                        b.damage = 1;
                        b.speed = 1;
                        b.alive = true;
                        b.playersBullet = false;
                        b.direction = MOVE_LEFT;
                        bullets[numberOfBullets] = b;
                        numberOfBullets++;
                    }
                    if(numberOfBullets<MAXBULLETS) {
                        Bullet b;
                        b.x = enemies[i-1].x + 5;
                        b.y = enemies[i-1].y + 9;
                        b.height = 2;
                        b.width = 2;
                        b.damage = 1;
                        b.speed = 1;
                        b.alive = true;
                        b.playersBullet = false;
                        b.direction = MOVE_LEFT;
                        bullets[numberOfBullets] = b;
                        numberOfBullets++;
                    }
                }
                // Shoot with lower two canons 
                if(arduboy.everyXFrames(30) && random(0,100) > 70) {	
                    if(numberOfBullets<MAXBULLETS) {
                        Bullet b;
                        b.x = enemies[i-1].x + 5;
                        b.y = enemies[i-1].y + 27;
                        b.height = 2;
                        b.width = 2;
                        b.damage = 1;
                        b.speed = 1;
                        b.alive = true;
                        b.playersBullet = false;
                        b.direction = MOVE_LEFT;
                        bullets[numberOfBullets] = b;
                        numberOfBullets++;
                    }
                    if(numberOfBullets<MAXBULLETS) {
                        Bullet b;
                        b.x = enemies[i-1].x + 3;
                        b.y = enemies[i-1].y + 23;
                        b.height = 2;
                        b.width = 2;
                        b.damage = 1;
                        b.speed = 1;
                        b.alive = true;
                        b.playersBullet = false;
                        b.direction = MOVE_LEFT;
                        bullets[numberOfBullets] = b;
                        numberOfBullets++;
                    }
                }
                // Shoot from circle
                if(arduboy.everyXFrames(120)) {
                    for(byte j=0; j<4; j++) {
                        if(numberOfBullets<MAXBULLETS) {
                            Bullet b;
                            // Shoot the bullet in front of the enemy.
                            b.x = enemies[i-1].x + j/2;
                            b.y = enemies[i-1].y + 20 + j%2;
                            b.height = 2;
                            b.width = 2;
                            b.damage = 1;
                            b.speed = 1;
                            b.alive = true;
                            b.playersBullet = false;
                            if(j%2 == 0) {
                                    b.direction = MOVE_DOWNLEFT;
                            } else {
                                    b.direction = MOVE_UPLEFT;
                            }
                            bullets[numberOfBullets] = b;
                            numberOfBullets++;
                        }
                    }
                }
                break;
            default:
                
                break;
        }
        i++;
    }
}

void playerShoots() {
    if(arduboy.pressed(B_BUTTON)) {
        if(MAXBULLETS > numberOfBullets
            && player.bullets < player.maxBullets) {
            
            switch(player.firetype) {
                case 0:
                    Bullet b;
                    // A bullet should appear at front of the ship in the middle.
                    b.x = player.x + 1;
                    b.y = player.y + (player.height >> 1);
                    b.damage = player.bulletType;
                    b.speed = player.bulletSpeed;
                    b.alive = true;
                    b.playersBullet = true;
                    b.direction = MOVE_RIGHT;
                    player.bullets++;
                    bullets[numberOfBullets] = b;
                    numberOfBullets++;
                    player.cooldown = player.fireSpeed;
                    break;
                    
                case 4:
                    Bullet b_2;
                    b_2.x = player.x + 1;
                    b_2.y = player.y + (player.height >> 1)+1;
                    b_2.damage = player.bulletType;
                    b_2.speed = player.bulletSpeed;
                    b_2.alive = true;
                    b_2.playersBullet = true;
                    b_2.direction = MOVE_UPRIGHT;
                    player.bullets++;
                    bullets[numberOfBullets] = b_2;
                    numberOfBullets++;
                    
                    if(MAXBULLETS > numberOfBullets) {
                        Bullet b_3;
                        b_3.x = player.x + 1;
                        b_3.y = player.y + (player.height >> 1)-1;
                        b_3.damage = player.bulletType;
                        b_3.speed = player.bulletSpeed;
                        b_3.alive = true;
                        b_3.playersBullet = true;
                        b_3.direction = MOVE_DOWNRIGHT;
                        player.bullets++;
                        bullets[numberOfBullets] = b_3;
                        numberOfBullets++;
                    }
                    
                case 3:
                    if(MAXBULLETS > numberOfBullets) {
                        Bullet b_2;
                        b_2.x = player.x + 1;
                        b_2.y = player.y + (player.height >> 1);
                        b_2.damage = player.bulletType;
                        b_2.speed = player.bulletSpeed;
                        b_2.alive = true;
                        b_2.playersBullet = true;
                        b_2.direction = MOVE_UPRIGHT;
                        player.bullets++;
                        bullets[numberOfBullets] = b_2;
                        numberOfBullets++;
                    }
                    if(MAXBULLETS > numberOfBullets) {
                        Bullet b_2;
                        b_2.x = player.x + 1;
                        b_2.y = player.y + (player.height >> 1);
                        b_2.damage = player.bulletType;
                        b_2.speed = player.bulletSpeed;
                        b_2.alive = true;
                        b_2.playersBullet = true;
                        b_2.direction = MOVE_DOWNRIGHT;
                        player.bullets++;
                        bullets[numberOfBullets] = b_2;
                        numberOfBullets++;
                    }
                    
                case 1:
                    for(byte i=0; i<2; i++) {
                        if(MAXBULLETS > numberOfBullets) {
                            Bullet b;
                            b.x = player.x + 1;
                            b.y = player.y + (player.height >> 1)-1+2*i;
                            b.damage = player.bulletType;
                            b.speed = player.bulletSpeed;
                            b.alive = true;
                            b.playersBullet = true;
                            b.direction = MOVE_RIGHT;
                            player.bullets++;
                            bullets[numberOfBullets] = b;
                            numberOfBullets++;
                            player.cooldown = player.fireSpeed;
                        }
                    }
                    break;
                    
                case 2:
                    Bullet b_4;
                    b_4.x = player.x + 1;
                    b_4.y = player.y + (player.height >> 1);
                    b_4.damage = player.bulletType;
                    b_4.speed = player.bulletSpeed;
                    b_4.alive = true;
                    b_4.playersBullet = true;
                    b_4.direction = MOVE_RIGHT;
                    player.bullets++;
                    bullets[numberOfBullets] = b_4;
                    numberOfBullets++;
                    player.cooldown = player.fireSpeed;
                    if(MAXBULLETS > numberOfBullets) {
                        Bullet b_2;
                        b_2.x = player.x + 1;
                        b_2.y = player.y + (player.height >> 1);
                        b_2.damage = player.bulletType;
                        b_2.speed = player.bulletSpeed;
                        b_2.alive = true;
                        b_2.playersBullet = true;
                        b_2.direction = MOVE_UPRIGHT;
                        player.bullets++;
                        bullets[numberOfBullets] = b_2;
                        numberOfBullets++;
                    }
                    
                    if(MAXBULLETS > numberOfBullets) {
                        Bullet b_2;
                        b_2.x = player.x + 1;
                        b_2.y = player.y + (player.height >> 1);
                        b_2.damage = player.bulletType;
                        b_2.speed = player.bulletSpeed;
                        b_2.alive = true;
                        b_2.playersBullet = true;
                        b_2.direction = MOVE_DOWNRIGHT;
                        player.bullets++;
                        bullets[numberOfBullets] = b_2;
                        numberOfBullets++;
                    }
                    break;
                
                default:
                    Bullet b_5;
                    // A bullet should appear at front of the ship in the middle.
                    b_5.x = player.x + 1;
                    b_5.y = player.y + (player.height >> 1);
                    b_5.damage = player.bulletType;
                    b_5.speed = player.bulletSpeed;
                    b_5.alive = true;
                    b_5.playersBullet = true;
                    b_5.direction = MOVE_RIGHT;
                    player.bullets++;
                    bullets[numberOfBullets] = b_5;
                    numberOfBullets++;
                    player.cooldown = player.fireSpeed;
            }
        }
    }
}
