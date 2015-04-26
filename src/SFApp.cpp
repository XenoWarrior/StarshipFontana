/*********************************************************
  This is where all our game fun happens.

  It is responsible for:
    Making new instances in SFApp()
    Checking collisions in OnUpdateWorld()
    Checking events in OnEvent()
    Executing the main game in OnExecute()
    Rendering objects in OnRender()
    Firing projectiles in FireProjectile()

  These methods keep the game running smoothly.
*********************************************************/

#include "SFApp.h"
#include "SFEvent.h"

/***********************************************************
  This is the initial constructor for the class.

  This will setup the spawning positions of the objects
  such as players, enemies and any other instances in-game
***********************************************************/
SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);

  // Set the player position to the width of the canvas / 2 
  auto player_pos = Point2(canvas_w / 2.0f, 88.0f);
  player->SetPosition(player_pos);
  player->SetHealth(100);
  player->SetScore(10);

  const int number_of_aliens = 10;
  for(int i = 0; i < number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);

    // Make enemy at position and set it's health
    alien->SetPosition(pos);
    alien->SetHealth(10);

    aliens.push_back(alien);

    cout << "Created enemy with " << alien->GetHealth() << endl;
  }

  for(int i = 0; i < 2; i++) {
    auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
    auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);
    coin->SetPosition(pos);
    coins.push_back(coin);
  }
  cout << endl << "Welcome to the game, you have " << player->GetHealth() << " HP." << endl;
  cout << "You start with " << player->GetScore() << " points, use these points wisely as each bullet will use 1 point." << endl;
  cout << "Hitting enemy will give you back the point, killing will give you 10 points." << endl << "Running out of points or death is game over!" << endl << endl;
}

SFApp::~SFApp() {
}

/***********************************************************
  Handle all events that come from SDL.
  These are timer or keyboard events.

  For this method, I have actually removed the player
  movement code and moved it into:
    OnUpdateWorld();
  This ultimately helps to ensure smooth movement.
  The original code would only check for an event then break.

  The events were all individual (up/left/down/right) and to
  add ones like upleft or downright would mean to put in more
  code that isn't really needed.

  OnUpdateWorld is called each tick so it is faster.
***********************************************************/
void SFApp::OnEvent(SFEvent& event) {
  // Get the event code from SFEvent::GetCode(); (SFEvent.cpp handles this)
  SFEVENT the_event = event.GetCode();

  // Now check what event we're processing.
  switch(the_event) {
    // This is the update event returned from SFEvent::GetCode();
    case SFEVENT_UPDATE: {
      // Update world and renderer.
      
      // Check if the game is paused
      if(!is_paused){
        // Not paused, update world.
        OnUpdateWorld();
      }

      // Render objects
      OnRender();

      // Break out of switch statement.
      break;
    }
    // This event handles quitting the game.
    case SFEVENT_QUIT: {
      is_running = false;
      EndGame();
      // Break out of switch statement.
      break;
    }
    // This allows the game to be paused
    case SFEVENT_PAUSE:{
      PauseGame();
      break;
    }
    // This handles the firing of projectiles, this has been left here so it only checks and delays rapid firing.
    case SFEVENT_FIRE: {
      // Make sure game is not paused
      if(!is_paused){
        // Check if we can fire (maxProjectiles limits the total on screen allowed)
        if(fire < maxProjectiles){
          // Count how many projectiles were fired in the entire session.
          totalProjectiles++;

          // Add to the fire limit counter.
          fire++;

          // Fire a projectile.
          FireProjectile();
        }
      }
      // Break out of statement.
      break;
    }
    break;
  }
}

/***********************************************************
  This is called to keep the program running.

  It simply sends out our events into the core of the game
  and keeps it running while the is_running var is true
***********************************************************/
int SFApp::OnExecute() {
  // Setup SDL event
  SDL_Event event;

  // While our program is running and waiting for events
  while (SDL_WaitEvent(&event) && is_running) {
    // Process the event as an SFEvent (SFEvent.cpp)
    SFEvent sfevent((const SDL_Event) event);

    // Now process our event in the SFApp::OnEvent(); method (SFApp.cpp)
    OnEvent(sfevent);
  }
}

/***********************************************************
  This is where the action happens.

  In here, I have added my own custom movement handler called:
    HandleInput(); (SFAsset::HandleInput();) (SFAsset.cpp)

  This new handler implements smooth movement and a very
  work in progress mouse movement handler that still needs some
  tuning up but it is not important at the moment.
***********************************************************/
void SFApp::OnUpdateWorld() {
  // Get the width and height of our renderer
  int w, h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

  // Run the new movement handler for our player
  player->HandleInput();

  if(player->GetHealth() <= 0 || player->GetScore() <= 0){
    cout << endl <<  "Game Over! " << (player->GetHealth() <= 0 ? "You have died!" : (player->GetScore() <= 0 ? "No more points left!" : "")) << endl << "Check your statistics below!" << endl;
    EndGame();
    is_running = false;
  }

  // Update projectile positions
  for(auto p: projectiles) {
    // Move projectile north
    p->GoNorth();

  }

  // Update collectible positions and check collisions
  for(auto c : coins) {
    // Move collectible coin south
		c->GoSouth();

    // Check player collision with coin
    if(player->CollidesWith(c)) {
      // Output a message
      cout << "Collected coin!" << endl;
      
      // Handle the collision
      c->HandleCollision();
        
      // Add to our counter for this session
      coinsCollected++;
    }
  }

  // Update enemy positions and check player collisions
  for(auto a : aliens) {
    // Move the enemy south
    a->GoSouth();

    // Check if player collides with enemy
    if(player->CollidesWith(a)) {
      // Remove 10 health
      player->SetHealth(player->GetHealth() - 10);
      
      // Special collisions detection for player colliding with enemies (instant kill enemy + removed 10HP from player)
      if(a->IsAlive()){
        a->HandlePlayerCollision();
      }

      // Add one to enemy kill counter
      enemiesKilled++;

      // Output left over health after collision
      cout << "Crashed with an enemy" << a->GetId() << "! Taking 10 damage. (PlayerHP: " << player->GetHealth() << ")" << endl;
    }
  }

  // This is a new idea I am implementing, the walls will actually move along screen and hit the player
  // Not sure if I will keep this, but let us see how it goes.
  for(auto w: walls) {
    // Move wall east
    w->GoEast();
  }

  // Check for collisions on projectiles
  for(auto p : projectiles) {
    // Check through all enemies
    for(auto a : aliens) {
      // If the projectile collides with the alien
      if(p->CollidesWith(a)) {
        // Handle the collisions for both projectile and enemy
        p->HandleCollision();
        player->SetScore(player->GetScore() + 1);
        
        // If HandleCollision returns a special value (1) to show enemy run out of HP
        if(a->HandleCollision() == 1){
          // Add to our counter for 
          player->SetScore(player->GetScore() + 10);
          enemiesKilled++;
        }
      }
    }
  }

  // Removing dead enemies from the array (long method, could be quicker using a list, maybe...)
  list<shared_ptr<SFAsset>> alienTemp;
  // For each enemy in the array
  for(auto a : aliens) {
    // Check if it is alive
    if(a->IsAlive()) {
      // If alive, push it to the new temp array
      alienTemp.push_back(a);
    }
  }
  // Any leftover enemies will be cleared
  aliens.clear();
  // Set the alive enemies back into the main array
  aliens = list<shared_ptr<SFAsset>>(alienTemp);

  // Remove all dead projectiles
  list<shared_ptr<SFAsset>> projTemp;
  // For each projectiles
  for(auto p : projectiles) {
    // Check if alive
    if(p->IsAlive()) {
      // Add alive to new temp array
      projTemp.push_back(p);
    }
    else{
      // Decrease the counter for total bullets on screen
      fire--;
    }
  }
  // Clear old bullets and set alive ones back to array
  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(projTemp);

  // Increase the tick counter (used to calculate time played)
  currTick++;
}

/***********************************************************
  This function renders all of our objects such as:
    Player.
    Enemies.
    Coins.
    And any others added.
  When making a new SFAsset, ensure to create at the top in
    SFApp::SFApp() (SFApp.cpp)

  Don't forget, X->OnRender does not call SFApp::OnRender();
  It actually calls SFAsset::OnRender(); because X is an object
  from SFAsset.cpp

  SFAsset->OnRender();
  p is an object from SFAsset, so:
  p->OnRender();
***********************************************************/
void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // Draw the player (SFAsset::OnRender();)!
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

  // Render walls that are currently alive
  for(auto w: walls) {
    w->OnRender();
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

/***********************************************************
  This method is exactly what it says... It fires bullets.

  It could be possible to move this method into SFAsset.cpp
  and have it so any object (within reason) can fire projectiles.

  So it could be similar to:
    SFAsset->FireProjectile(); to call
  And:
    if(SFASSET_ALIEN == type) to check
***********************************************************/
void SFApp::FireProjectile() {
  // Make the projectiles
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);

  // Get the player position
  auto v  = player->GetPosition();

  // Set the projectile to the player pos
  pb->SetPosition(v);

  // Push back to array
  projectiles.push_back(pb);

  player->SetScore(player->GetScore() - 1);
}

/***********************************************************
  Ends the game.

  This was moved into a method so the game could be ended
  in different ways. Since the player can die, it would be
  crucial to have this accessible by any of the methods.

  Also using it as a method prevents re-using the same code
  two times in different places.
***********************************************************/
void SFApp::EndGame(){
  // This will simply calculate the time that the player has spent playing the game.
  // To ensure we don't do a a division by 0 (very unlikely).
  if(currTick > 0){
    // Setup the minutes/seconds variables.
    int min = 0, sec = 0;

    // Calculate is we've played for more than a minute so we can calculate minutes.
    if(currTick > 600){
      // Division by 60 to seconds then 60 to minutes.
      min = ((currTick / 60) / 60);

      // Remove the total ticks that we took from the tick counter.
      currTick -= ((min * 60) * 60);
    }

    // This will now check if there are more than 60 ticks, possible to calculate seconds.
    if(currTick > 60){
      // Just divide the ticks by 60 to get seconds
      sec = (currTick / 60);
    }
    // Output some statistics to console for the player to see.
    cout << endl << "Time Played: " << min << " minute(s) | " << sec << " second(s)" << endl;
  }
  // This will show the player what they did during their session.
  cout << "Enemies Killed: " << enemiesKilled << " | Coins Collected: " << coinsCollected <<  " | Projectiles Fired: " << totalProjectiles << endl << endl;
  cout << endl << "Total Score: " << player->GetScore() << endl;
}

void SFApp::PauseGame(){
  if(is_paused){
    is_paused = false;
    cout << "Unpaused game!" << endl;
  }
  else{
    is_paused = true;
    cout << "Paused game!" << endl;
  }
}