#ifndef MOVMENT_H
#define MOVEMENT_H

#include <Arduboy2.h>
#include "variables.h"

/**
 * @brief Call all functions needed for movement.
 */
void moveGame();

/**
 * @brief Supplies move just straight to the left.
 */
void moveSupplies();

/**
 * @brief Bullets move
 */
void moveBullets();

/**
 * @brief Player moves
 */
void movePlayer();

/**
 * @brief Enemies move
 */
void moveEnemies();

/**
 * @brief Stars move
 */
void moveStars();

/**
 * @brief Enemies shoot
 */
void enemiesShoot();

/**
 * @brief Player shoots
 */
void playerShoots();
#endif
