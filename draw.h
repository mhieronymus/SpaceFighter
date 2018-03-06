#ifndef DRAW_H
#define DRAW_H

#include <Arduboy2.h>
#include "variables.h"
#include "SpaceFighter_bitmaps.h"

/*
 * @brief Calls all necessary functions to generate the current display.
 * @return True if all explosions have been drawn.
 */
bool drawGame();

/**
 * @brief Titlescreen with sound - SpaceFighter by me.
 * TODO: create nice music.
 */
void showTitle();

/**
 * @brief Draw all stars in the background.
 */
void drawStars();

/**
 * @brief Draw the player in its current state.
 */
void drawPlayer();

/**
 * @brief draw all the enemies.
 */
void drawEnemies();

/*
 * @brief Bullets are 2x2 squares which are drawn in this function.
 */
void drawBullets();

/**
 * @brief Draw supply
 */
void drawSupply();

/**
 * @brief Draw supply messages
 */
void drawExtra();

/*
 * @brief Draws an explosion for destroyed ships.
 * @return: True if all explosions are finished.
 */
bool drawExplosions();

/**
 * @brief Draw score like this: Score: 100
 */
void drawScore();

/**
 * @brief Draw the number of lives like this: symbol heart 5
 * If there are only three or less lives left, draw three hearts.
 */
void drawLives();

/**
 * @brief draw Pause. The Code is taken from ArduBreakout by Sebastian Goscik
 * and has been modified.
 */
void drawPause();

/**
 * @brief draw game over
 */
void drawGameOver();

/**
 * @brief draw a healthbar of the player. Each point is one hit point.
 */
void drawHealthBar();

/**
 * @brief Draw the highscore as seen in 
 * https://github.com/filmote/Steve/blob/master/Steve.ino
 * 
 * @param display the current score of the player after he or she played
 * at least one game
 * 
 * @return true if the title should be shown, false if the game should start
 */
void drawHighscore(bool displayCurrentScore);

#endif 
