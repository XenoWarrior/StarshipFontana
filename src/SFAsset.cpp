/*********************************************************
  This is the asset class that is responsible for making
  object for in-game. 

  It will actually create, render and control objects.

  If any objects are to be modified externally, you need to
  get the object from an array or know what variable it is stored as
  and call:
    object->Action();
  For modifying directly, it is simply:
    this->Action();

  It is "this" because when object->Action(); is called, it
  sends a message from the object to here and knows what it
  was called from.
*********************************************************/

#include "SFAsset.h"

// Set ID numbers to each asset
int SFAsset::SFASSETID=0;

/*********************************************************
  This is the main code for SFAsset.cpp

  It will be used to setup each object in-game by giving
  it a sprite based on the passed type and applying it to
  the window specified.
*********************************************************/
SFAsset::SFAsset(SFASSETTYPE type, std::shared_ptr<SFWindow> window): type(type), sf_window(window) {

  // Set the asset ID.
  this->id   = ++SFASSETID;

  // Setup what sprite this asset will use
  switch (type) {
    case SFASSET_PLAYER:
      sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/player.png");
      break;
    case SFASSET_PROJECTILE:
      sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/projectile.png");
      break;
    case SFASSET_ALIEN:
      sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/alien.png");
      break;
    case SFASSET_COIN:
      sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/coin.png");
      break;
    case SFASSET_WALL: 
      sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/wall.png");
      break;
    case SFASSET_STARS: 
      sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/stars.png");
      break;
  }

  // If the sprite was not set, then throw an error as it may not exist
  if(!sprite) {
    cerr << "Could not load asset of type " << type << endl;
    throw SF_ERROR_LOAD_ASSET;
  }

  // Get texture width & height
  int w, h;
  SDL_QueryTexture(sprite, NULL, NULL, &w, &h);

  // Initialise bounding box
  bbox = make_shared<SFBoundingBox>(SFBoundingBox(Vector2(0.0f, 0.0f), w, h));
}

SFAsset::SFAsset(const SFAsset& a) {
  sprite = a.sprite;
  sf_window = a.sf_window;
  bbox   = a.bbox;
  type   = a.type;
}

SFAsset::~SFAsset() {
  bbox.reset();
  if(sprite) {
    SDL_DestroyTexture(sprite);
    sprite = nullptr;
  }
}

/**
 * The logical coordinates in the game assume that the screen
 * is indexed from 0,0 in the bottom left corner.  The blittable
 * coordinates of the screen map 0,0 to the top left corner. We
 * need to convert between the two coordinate spaces.  We assume
 * that there is a 1-to-1 quantisation.
 */
Vector2 GameSpaceToScreenSpace(SDL_Renderer* renderer, Vector2 &r) {
  int w, h;
  SDL_GetRendererOutputSize(renderer, &w, &h);

  return Vector2 (r.getX(), (h - r.getY()));
}

/*********************************************************
  sets the asset position on screen
*********************************************************/
void SFAsset::SetPosition(Point2 & point) {
  Vector2 v(point.getX(), point.getY());
  bbox->SetCentre(v);
}

/*********************************************************
  Returns position of object
*********************************************************/
Point2 SFAsset::GetPosition() {
  return Point2(bbox->centre->getX(), bbox->centre->getY());
}

/*********************************************************
  Will get the object ID
*********************************************************/
SFAssetId SFAsset::GetId() {
  return id;
}

/*********************************************************
  Will get the object health points
*********************************************************/
int SFAsset::GetHealth() {
  return objHP;
}

/*********************************************************
  Will set the object health points
*********************************************************/
void SFAsset::SetHealth(int val) {
  this->objHP = val;
}

/*********************************************************
  Will get the player score
*********************************************************/
int SFAsset::GetScore() {
  return playerScore;
}

/*********************************************************
  Will set the player score
*********************************************************/
void SFAsset::SetScore(int val) {
  this->playerScore = val;
  cout << "Set score to: " << val << endl;
}

/*********************************************************
  Display the object on the render window
*********************************************************/
void SFAsset::OnRender() {
  // 1. Get the SDL_Rect from SFBoundingBox
  SDL_Rect rect;

  Vector2 gs = (*(bbox->centre) + (*(bbox->extent_x) * -1)) + (*(bbox->extent_y) * -1);
  Vector2 ss = GameSpaceToScreenSpace(sf_window->getRenderer(), gs);
  rect.x = ss.getX();
  rect.y = ss.getY();
  rect.w = bbox->extent_x->getX() * 2;
  rect.h = bbox->extent_y->getY() * 2;

  // 2. Blit the sprite onto the level
  SDL_RenderCopy(sf_window->getRenderer(), sprite, NULL, &rect);
}

/********************************************************* 
  Makes the objects move

  To reduce amount of code needed, you can just do:
    obj->MoveHorizontal();

  Then in the MoveHorizontal() method, define a new SFAsset so each
  different assets have individual movement patterns rather than
  making a new method to handle movement for other assets.

  This is the same for all below methods:
    MoveHorizontal();
    MoveVertical();

  Each movement is similar, but they're all set to different
  speeds in Vector2
  (Vector2(XMove, YMove))
*********************************************************/
void SFAsset::MoveHorizontal(float speed) {
  // For this to stop instances going off-screen
  // need to get height and width of screen
	int w, h;
	SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

  // Handle movement for type player
  if(SFASSET_PLAYER == type) {
    // Setup our new vector
		Vector2 c = *(bbox->centre) + Vector2(speed, 0.0f);

    // If not at the left of screen, allow it to move
		if(!(c.getX()+32.0f > w) && !(c.getX()-32.0f < 0)) {
		  bbox->centre.reset();
		  bbox->centre = make_shared<Vector2>(c);
		}
	}
}

void SFAsset::MoveVertical(float speed) {
  // For this to stop instances going off-screen
  // need to get height and width of screen
  int w, h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

  // Handle movement for type player
	if(SFASSET_PLAYER == type) {
		Vector2 c = *(bbox->centre) + Vector2(0.0f, speed);

		if(!(c.getY() < 64.0f) && !(c.getY()-18.0f > h)) {
		  bbox->centre.reset();
		  bbox->centre = make_shared<Vector2>(c);
		}
	}

  // Handle movement for type projectile
  if(SFASSET_PROJECTILE == type){
    Vector2 c = *(bbox->centre) + Vector2(0.0f, speed);
    if(!(c.getY() > h + 32.0f)) {
      bbox->centre.reset();
      bbox->centre = make_shared<Vector2>(c);
    }
    else {
      this->SetNotAlive();
    }
  }

  // Handle movement for type coin
  if(SFASSET_COIN == type) {
    Vector2 c = *(bbox->centre) + Vector2(0.0f, speed);

    if(!(c.getY() < 0.0f)) {
      bbox->centre.reset();
      bbox->centre = make_shared<Vector2>(c);
    }
    else{
      auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
      this->SetPosition(pos);
    }
  }

  // Handle movement for the background
  if(SFASSET_STARS == type) {
    Vector2 c = *(bbox->centre) + Vector2(0.0f, speed);

    if(!(c.getY() < 0.0f)) {
      bbox->centre.reset();
      bbox->centre = make_shared<Vector2>(c);
    }
    else{
      cout << "Resetting gb pos." << endl;
      auto pos  = Point2(320, 1800);
      this->SetPosition(pos);
    }
  }

  // Handle movement for type alien
	if(SFASSET_ALIEN == type) {     
		Vector2 c = *(bbox->centre) + Vector2(0.0f, speed);

		if(!(c.getY() < 0.0f)) {
			bbox->centre.reset();
			bbox->centre = make_shared<Vector2>(c);
		}
		else{
    	auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
      this->SetPosition(pos);
      this->SetHealth(10);
		}
	}
}

/*********************************************************
  As mentioned in SFApp.cpp, this is my own movement
  handler.

  It works by checking the keyboard state and scanning for
  the code.

  It basically consits of:
    Keyboard movement
    Mouse movement
*********************************************************/
void SFAsset::HandleInput(){
  int w, h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

  // This section of the code handles keyboard input
  // Used for nice player movement.
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
  if(keyboardState[SDL_SCANCODE_DOWN]) {
    this->MoveVertical(-2.0f);
  }
  if(keyboardState[SDL_SCANCODE_UP]) {
    this->MoveVertical(4.0f);
  }
  if(keyboardState[SDL_SCANCODE_LEFT]) {
    this->MoveHorizontal(-5.0f);
  }
  if(keyboardState[SDL_SCANCODE_RIGHT]) {
    this->MoveHorizontal(5.0f);
  }
}

// Collision detection
bool SFAsset::CollidesWith(shared_ptr<SFAsset> other) {
  return bbox->CollidesWith(other->bbox);
}

// Get bounding box of instance
shared_ptr<SFBoundingBox> SFAsset::GetBoundingBox() {
  return bbox;
}

// Setting and getting object status
void SFAsset::SetNotAlive() {
  type = SFASSET_DEAD;
}

bool SFAsset::IsAlive() {
  return (SFASSET_DEAD != type);
}

// Specific collision handling for enemy to player
void SFAsset::HandlePlayerCollision(){
  if(SFASSET_ALIEN == type){
    auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
    this->SetPosition(pos);
  }
}

// Handing object collisions
int SFAsset::HandleCollision() {
	// Collisions for projectiles
  if(SFASSET_PROJECTILE == type) {
    SetNotAlive();
  }

  // Collisions for aliens
  if(SFASSET_ALIEN == type){
    int canvas_w, canvas_h;
    SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

    // For removing enemy health and checking if it died
    if(this->GetHealth() > 0){
      // Hurt the enemy for 5 HP
      this->SetHealth(this->GetHealth() - 5);

      // Get HP as string
      int tempHP = this->GetHealth();

      // Convert the HP to string form
      string HPTempString;
      ostringstream HPConvert;
      HPConvert << tempHP;
      HPTempString = HPConvert.str();

      // Tell player it was hurt
      cout << "Hurt enemy " << this->GetId() << " for 5 HP. (EnemyHP: " << (this->GetHealth() <= 0 ? "DEAD" : HPTempString) << ")" << endl;

      // Do another check because we can reach 0, but it won't check until next collision.
      if(this->GetHealth() <= 0){
        // Enemy died, so set new position and health
        auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
        this->SetPosition(pos);
        this->SetHealth(10);

        // Return special condition back to call
        return 1;
      }
    }
    else{
      // Enemy died, so set new position and health
      auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
      this->SetPosition(pos);
      this->SetHealth(10);

      // Tell player
      cout << "Enemy " << this->GetId() << " died!" << endl;

      // Return special condition back to call
      return 1;
    }
  }

	// Collisions for coins
	if(SFASSET_COIN == type) {
    // Kill the coin
    this->SetNotAlive();
    return 1;
	}
  return 0;
}
