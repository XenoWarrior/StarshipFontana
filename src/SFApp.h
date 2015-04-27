/*********************************************************
  HEADER FILE FOR SFApp.cpp

  This file contains any external requirements to run the 
  code.

  Any variables or methods will be defined here before they 
  can be used in the main game.
*********************************************************/

#ifndef SFAPP_H
#define SFAPP_H

// Pull in important libraries that the game will use
#include <memory>   // Pull in std::shared_ptr (for pointers)
#include <iostream> // Pull in std::cerr, std::endl (for output)
#include <list>     // Pull in list (for array)
#include <sstream>  // pull in sstream (for strings) (String Stream?)

// So we don't have to keep doing std::string etc
using namespace std;

// Get game related headers
#include "SFCommon.h"
#include "SFEvent.h"
#include "SFAsset.h"

/**
 * Represents the StarshipFontana application. It has responsibilities for
 * * Creating and destroying the app window
 * * Processing game events
 */
class SFApp {
public:
  SFApp(std::shared_ptr<SFWindow>);
  virtual ~SFApp();

  // Define any new methods for SFApp.cpp to use below.
  void    OnEvent(SFEvent &);
  int     OnExecute();
  void    OnUpdateWorld();
  void    OnRender();
  void    FireProjectile();
  void    EndGame();
  void    PauseGame();
  void    GameDifficultyModifier(int diff);
private:
  // Define any variables to use in SFApp.cpp below.
  SDL_Surface             * surface;
  bool                    is_running;
  bool                    is_paused = false;

  // Window pointer
  shared_ptr<SFWindow>        sf_window;

  // Player and bounding box
  shared_ptr<SFAsset>         player;
  shared_ptr<SFBoundingBox>   app_box;

  // The object lists for our game instances
  list<shared_ptr<SFAsset>> projectiles;
  list<shared_ptr<SFAsset>> aliens;
  list<shared_ptr<SFAsset>> coins;
  list<shared_ptr<SFAsset>> walls;
  list<shared_ptr<SFAsset>> stars;

  // For projectile handling
  int fire;                   // Total fired
  int maxProjectiles = 5;     // Max allowed
  int totalProjectiles = 0;   // Total in session

  // For calculating time
  int currTick = 0;

  // For game difficulty
  int gameDifficulty = 0;

  // For counting enemies and coins collected
  int enemiesKilled = 0;
  int coinsCollected = 0;

  SFError OnInit();
};
#endif
