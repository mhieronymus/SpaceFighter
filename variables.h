#ifndef VARIABLES_H
#define VARIABLES_H

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
    bool alive;
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
    bool alive;
    // Each number is a placeholder for another movement, e.g. straight-forward
    // or sine. Needs to be implemented.
    byte movement;
    byte direction;
    // If true, create a random supply after destruction.
    bool supply;
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
    bool alive;
    bool playersBullet;
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
    bool alive;
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
extern byte frameCounter;
// The stars in the background are saved here.
extern Star stars[MAXSTARS];
extern byte numberOfStars;
extern byte SCREEN_HEIGHT;
extern byte SCREEN_WIDTH;
extern Player player;
extern Arduboy arduboy;
extern AbPrinter text;
extern Enemy enemies[MAXENEMIES];
extern byte numberOfEnemies;
extern Bullet bullets[MAXBULLETS];
extern byte numberOfBullets;
extern Supply supplies[MAXSUPPLY];
extern byte noOfSupplies;
extern bool gameStarted;
extern Explosion explosions[MAXENEMIES+1];

#endif 