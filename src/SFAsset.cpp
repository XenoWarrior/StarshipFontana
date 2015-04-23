#include "SFAsset.h"

int SFAsset::SFASSETID=0;

SFAsset::SFAsset(SFASSETTYPE type, std::shared_ptr<SFWindow> window): type(type), sf_window(window) {
  this->id   = ++SFASSETID;

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
      sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/projectile.png");
      break;
  }

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

// Setting object position
void SFAsset::SetPosition(Point2 & point) {
  Vector2 v(point.getX(), point.getY());
  bbox->SetCentre(v);
}

// Getting object position
Point2 SFAsset::GetPosition() {
  return Point2(bbox->centre->getX(), bbox->centre->getY());
}

// Getting object ID
SFAssetId SFAsset::GetId() {
  return id;
}

// Rendering object
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

// Player movement handling
void SFAsset::GoWest() {
	if(SFASSET_PLAYER == type) {
	  Vector2 c = *(bbox->centre) + Vector2(-5.0f, 0.0f);
	  if(!(c.getX()-32.0f < 0)) {
	    bbox->centre.reset();
	    bbox->centre = make_shared<Vector2>(c);
	  }
	}
}
void SFAsset::GoEast() {
	if(SFASSET_PLAYER == type) {
		int w, h;
		SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

		Vector2 c = *(bbox->centre) + Vector2(5.0f, 0.0f);
		if(!(c.getX()+32.0f > w)) {
		  bbox->centre.reset();
		  bbox->centre = make_shared<Vector2>(c);
		}
	}
}
void SFAsset::GoNorth() {
	if(SFASSET_PLAYER == type) {
		int w, h;
		SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

		Vector2 c = *(bbox->centre) + Vector2(0.0f, 4.0f);

		if(!(c.getY()-18.0f > h)) {
		  bbox->centre.reset();
		  bbox->centre = make_shared<Vector2>(c);
		}
	}
	if(SFASSET_PROJECTILE == type){
	  Vector2 c = *(bbox->centre) + Vector2(0.0f, 10.0f);
    bbox->centre.reset();
    bbox->centre = make_shared<Vector2>(c);
	}
}
void SFAsset::GoSouth() {
	if(SFASSET_PLAYER == type) {
		Vector2 c = *(bbox->centre) + Vector2(0.0f, -3.0f);

		if(!(c.getY() < 64.0f)) {
		  bbox->centre.reset();
		  bbox->centre = make_shared<Vector2>(c);
		}
	}
	if(SFASSET_COIN == type) {
		Vector2 c = *(bbox->centre) + Vector2(0.0f, -1.0f);

		if(!(c.getY() < 0.0f)) {
			bbox->centre.reset();
			bbox->centre = make_shared<Vector2>(c);
		}
		else{
    	auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
      this->SetPosition(pos);
		}
	}
	if(SFASSET_ALIEN == type) {
		Vector2 c = *(bbox->centre) + Vector2(0.0f, -2.0f);

		if(!(c.getY() < 0.0f)) {
			bbox->centre.reset();
			bbox->centre = make_shared<Vector2>(c);
		}
		else{
    	auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
      this->SetPosition(pos);
		}
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

// Handing object collisions
void SFAsset::HandleCollision() {
	// Collisions for projectiles and aliens
  if(SFASSET_PROJECTILE == type) {
    SetNotAlive();
  }

	if(SFASSET_ALIEN == type){
			int canvas_w, canvas_h;
			SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

    	auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
      this->SetPosition(pos);
	}	

	// Collisions for coins
	if(SFASSET_COIN == type) {
			int canvas_w, canvas_h;
			SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

    	auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
      this->SetPosition(pos);
	}
}
