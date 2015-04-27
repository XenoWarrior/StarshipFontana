#ifndef SFASSET_H
#define SFASSET_H

#include <string>
#include <memory>
#include <iostream>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

#include "SFCommon.h"
#include "SFEvent.h"
#include "SFWindow.h"
#include "SFBoundingBox.h"

/**
 * We could create SFPlayer, SFProjectile and SFAsset which are subclasses
 * of SFAsset.  However, I've made an implementation decision to use this
 * enum to mark the type of the SFAsset.  If we add more asset types then
 * the subclassing strategy becomes a better option.
 */
enum SFASSETTYPE {SFASSET_DEAD, SFASSET_PLAYER, SFASSET_PROJECTILE, SFASSET_ALIEN, SFASSET_COIN, SFASSET_WALL, SFASSET_STARS};

class SFAsset {
public:
  SFAsset(const SFASSETTYPE, const std::shared_ptr<SFWindow>);
  SFAsset(const SFAsset&);
  virtual ~SFAsset();

  virtual void      SetPosition(Point2 &);
  virtual Point2    GetPosition();
  virtual SFAssetId GetId();
  virtual void      OnRender();

  // Now that speed is being passed to this, I could actually pack it into a single method.
  virtual void      GoEast(float speed);
  virtual void      GoWest(float speed); 
  virtual void      GoNorth(float speed);
  virtual void      GoSouth(float speed);
  
  virtual void      SetNotAlive();
  virtual bool      IsAlive();
  virtual int       HandleCollision();
  virtual void      HandlePlayerCollision();
  virtual void      HandleInput();
  virtual int       GetHealth();
  virtual void      SetHealth(int val);
  virtual int       GetScore();
  virtual void      SetScore(int val);
  
  virtual bool      CollidesWith(shared_ptr<SFAsset>);;
  virtual shared_ptr<SFBoundingBox> GetBoundingBox();
private:
  // it would be nice if we could make this into a smart pointer,
  // but, because we need to call SDL_FreeSurface on it, we can't.
  // (or we could use a std::shared_ptr with a custom Deleter, but
  // that's a little too much right now)
  SDL_Texture               * sprite;
  shared_ptr<SFBoundingBox>   bbox;
  SFASSETTYPE                 type;
  SFAssetId                   id;
  std::shared_ptr<SFWindow>   sf_window;

  int                         objHP;
  int                         playerScore;

  static int SFASSETID;
};

#endif
