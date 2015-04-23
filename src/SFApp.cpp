#include "SFApp.h"
#include "SFEvent.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);

  // Set the player position to the width of the canvas / 2 
  auto player_pos = Point2(canvas_w / 2.0f, 88.0f);
  player->SetPosition(player_pos);

  const int number_of_aliens = 10;
  for(int i = 0; i < number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2(rand() % 600 + 32, rand() % 300 + 200);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

  for(int i = 0; i < 2; i++) {
    auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
    auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
		coin->SetPosition(pos);
    coins.push_back(coin);
  }
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  // Update event
  if(the_event == SFEVENT_UPDATE) {
    OnUpdateWorld();
    OnRender();
  }
  // Quit event
  if(the_event == SFEVENT_QUIT) {
    is_running = false;
    if(currTick > 0){
      cout << endl << "Time Played: " << ((currTick / 60) / 60) << " minute(s) | " << (currTick / 60) << " second(s)" << endl;
    }
    cout << "Enemies Killed: " << enemiesKilled << " | Coins Collected: " << coinsCollected <<  " | Projectiles Fired: " << totalProjectiles << endl << endl;
  }
  // Fire event
  if(the_event == SFEVENT_FIRE){
    if(fire < maxProjectiles){
      totalProjectiles++;
      fire++;
      FireProjectile();
    }
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // if this is an update event, then handle it in SFApp,
    // otherwise punt it to the SFEventDispacher.
    SFEvent sfevent((const SDL_Event) event);
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  int w, h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

  player->HandleInput();

  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();

    // Remove projectile when it hits the top of the screen
    auto p_pos = p->GetPosition();

    if(p_pos.getY() > h) {
      p->HandleCollision();
    }
  }

  for(auto c : coins) {
		c->GoSouth();
    // Check player collision with coin
    if(player->CollidesWith(c)) {
      cout << "Collected coin!" << endl;
      
      c->HandleCollision();
      
      coinsCollected++;
    }
  }

  // Update enemy positions
  for(auto a : aliens) {
		a->GoSouth();
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        cout << "Killed an enemy!" << endl;
        
        p->HandleCollision();
        a->HandleCollision();
        
        enemiesKilled++;
      }
    }
  }

  // Remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> alienTemp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      alienTemp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(alienTemp);

	// Remove all dead projectiles
  list<shared_ptr<SFAsset>> projTemp;
  for(auto p : projectiles) {
    if(p->IsAlive()) {
      projTemp.push_back(p);
    }
    else{
      fire--;
    }
  }
  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(projTemp);

  currTick++;
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // Draw the player
  player->OnRender();

  // Render projectiles that are currently alive
  for(auto p: projectiles) {
    if(p->IsAlive()) {
      p->OnRender();
    }
  }

  // Render aliens that are currently alive
  for(auto a: aliens) {
    if(a->IsAlive()) {
      a->OnRender();
    }
  }

  // Render coins that are currently alive
  for(auto c: coins) {
    c->OnRender();
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
