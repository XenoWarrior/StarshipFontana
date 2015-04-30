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
  player->SetScore(1200);

  const int number_of_aliens = 10;
  for(int i = 0; i < number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);

    // Make enemy at position and set it's health
    alien->SetPosition(pos);
    alien->SetHealth(15);
    alien->SetFired(0);

    aliens.push_back(alien);

    cout << "Created enemy with " << alien->GetHealth() << endl;
  }

  for(int i = 0; i < 2; i++) {
    // Spawn in coins
    auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
    auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600); 
    coins.push_back(coin);
  }

  for(int i = 0; i < 2; i++){
    auto star = make_shared<SFAsset>(SFASSET_STARS, sf_window);
    auto pos = (i == 0 ? Point2(320, 1000) : Point2(320, 1600));
    star->SetPosition(pos);
    stars.push_back(star);
  }

  auto hpBar = make_shared<SFAsset>(SFASSET_HEALTHBAR, sf_window);
  auto pos = Point2(92, 488);
  hpBar->SetPosition(pos);
  healthBar.push_back(hpBar);

  cout << endl << "Welcome to the game, you have " << player->GetHealth() << " HP." << endl;
  cout << "You start with " << player->GetScore() << " points, use these points wisely as each bullet will use 1 point." << endl;
  cout << "Hitting enemy will give you back the point, killing will give you 10 points." << endl << "Running out of points or death is game over!" << endl << endl;
  cout << endl << "Collect 1500 points to win the game!" << endl;
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
    // This is the update event returned from Point2(rand() % 600 + 32, rand() % 400 + 600);SFEvent::GetCode();
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
          FireProjectile(player->GetPosition(), true);
        }
      }
      // Break out of statement.
      break;
    }
    break;
  }
}

/*********************************
    ec->OnRender();
  }
**************************
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

	player->HandleInput();

  // Handle game-over conditions
  if(player->GetHealth() <= 0 || player->GetScore() <= 0){
    cout << endl <<  "Game Over! " << (player->GetHealth() <= 0 ? "You have died!" : (player->GetScore() <= 0 ? "No more points left!" : "")) << endl << "Check your statistics below!" << endl;
    EndGame();
    is_running = false;
  }

  if(player->GetScore() > 1200 && gameDifficulty != 5) {
    gameDifficulty = 5;
    GameDifficultyModifier(gameDifficulty);
  }
  if(player->GetScore() > 1000 && player->GetScore() < 1200 && gameDifficulty != 4) {
    gameDifficulty = 4;
    GameDifficultyModifier(gameDifficulty);
  }
  if(player->GetScore() > 850 && player->GetScore() < 1000 && gameDifficulty != 3) {
    gameDifficulty = 3;
    GameDifficultyModifier(gameDifficulty);
  }
  if(player->GetScore() > 600 && player->GetScore() < 850 && gameDifficulty != 2) {
    gameDifficulty = 2;
    GameDifficultyModifier(gameDifficulty);
  }
  if(player->GetScore() > 300 && player->GetScore() < 600 && gameDifficulty != 1) {
    gameDifficulty = 1;
    GameDifficultyModifier(gameDifficulty);
  }
	if(player->GetScore() >= 1500){
		cout << endl <<  "Game Over! You have won the game and saved Earth's code!" << endl;
		EndGame();
		is_running = false;	
	}

  // Update projectile positions
  for(auto pp: pProjectiles) {
    // Move projectile north
    pp->MoveVertical(10.0f);
  }
  for(auto ep: eProjectiles) {
    // Move projectile north
    ep->MoveVertical(-5.0f - gameDifficulty);
  }

  for(auto s : stars){ 
    s->MoveVertical(-0.5f - (gameDifficulty / 2));
  }

  for(auto power: powers){
    power->MoveVertical(-3.0f);

    if(player->CollidesWith(power)){
      firePower = 1;
      firePowerTime = 300;
      power->HandleCollision();
    }
  }

  if(firePower == 1){
    if(firePowerTime > 1){
      firePowerTime--;
    }
    else{
      firePower = 0;
    }
  }

  // Update collectible positions and check collisions
  for(auto c : coins) {
    // Move collectible coin south
		c->MoveVertical(-1.0f);

    // Check player collision with coin
    if(player->CollidesWith(c)) {
      // Output a message
      cout << "Power up! You can now fire more projectiles!" << endl;
      
      // Handle the collision
      if(c->HandleCollision()){
        // Add to our counter for this session
        coinsCollected++;

        // allow more firepower for the player
        maxProjectiles += 1;
      }
    }
  }

  // Update enemy positions and check player collisions
  for(auto a : aliens) {
    // Move the enemy south
    a->MoveVertical(-2.0f - gameDifficulty);

    if(a->HandleProjectile()){
      FireProjectile(a->GetPosition(), false);
      cout << "Enemy fired projectile" << endl;
    }

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
      cout << "Crashed with an enemy " << a->GetId() << "! Taking 10 damage. (PlayerHP: " << player->GetHealth() << ")" << endl;
    }
  }

  // Check for collisions on projectiles
  for(auto p : pProjectiles) {
    // Check through all enemies
    for(auto a : aliens) {
      // If the projectile collides with the alien
      if(p->CollidesWith(a)) {
        // Get the alien position
        auto aPos = a->GetPosition();
        
        // Handle the collisions for both projectile and enemy
        p->HandleCollision();

        // Set the score back up as the projectile hit
        player->SetScore(player->GetScore() + 1);
        
        // If HandleCollision returns a special value (1) to show enemy run out of HP
        if(a->HandleCollision() == 1){
          // Add 10 points to score
          player->SetScore(player->GetScore() + 10);

          // Add to our counter for the kill
          enemiesKilled++;

          // Decide if a collectible should be dropped
          int check = (rand() % 600 + 32);
          if(check >= 0 && check <= 200){
            // Output some message
            cout << "Coin dropped!" << endl;

            // Drop some loot
            auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
            auto pos  = Point2(aPos);
            coin->SetPosition(pos);
            coins.push_back(coin);
          }
          else if(check >= 200 && check <= 300) {
            cout << "Powerup dropped!" << endl;

            // Drop some loot
            auto power = make_shared<SFAsset>(SFASSET_POWERUP, sf_window);
            auto pos  = Point2(aPos);
            power->SetPosition(pos);
            powers.push_back(power);
          }
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

  // Remove all dead (player) projectiles
  list<shared_ptr<SFAsset>> pProjTemp;
  // For each projectile
  for(auto p : pProjectiles) {
    // Check if alive
    if(p->IsAlive()) {
      // Add alive to new temp array
      pProjTemp.push_back(p);
    }
    else{
      // Decrease the counter for total bullets on screen
      fire--;
    }
  }
  // Clear old bullets and set alive ones back to array
  pProjectiles.clear();
  pProjectiles = list<shared_ptr<SFAsset>>(pProjTemp);

  // Remove all dead (enemy) projectiles
  list<shared_ptr<SFAsset>>eProjTemp;
  // For each projectile
  for(auto p : eProjectiles) {
    // Check if alive
    if(p->IsAlive()) {
      // Add alive to new temp array
      eProjTemp.push_back(p);
    }
    else{
      // Decrease the counter for total bullets on screen
      fire--;
    }
  }
  // Clear old bullets and set alive ones back to array
  eProjectiles.clear();
  eProjectiles = list<shared_ptr<SFAsset>>(eProjTemp);

  // Remove all dead collectibles
  list<shared_ptr<SFAsset>> collTemp;
  // For each coin
  for(auto c : coins) {
    // Check if alive
    if(c->IsAlive()) {
      // Add alive to new temp array
      collTemp.push_back(c);
    }
  }
  // Clear old coins and set alive ones back to array
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(collTemp);


  // Remove all dead powerups
  list<shared_ptr<SFAsset>> powTemp;
  // For each powerup
  for(auto power : powers) {
    // Check if alive
    if(power->IsAlive()) {
      // Add alive to new temp array
      powTemp.push_back(power);
    }
  }
  // Clear old powerups and set alive ones back to array
  powers.clear();
  powers = list<shared_ptr<SFAsset>>(powTemp);

  // Clear-out the HPBlock list and update it
  healthBlocks.clear();
  stage.clear();
  DrawHud();

  // Increase the tick counter (used to calculate time played)
  currTick++;
}

/***********************************************************
  This will setup any UI related assets to the screen

  It mainly updates the state of the player health
  and the game difficulty indicator (1 to 5)
***********************************************************/
void SFApp::DrawHud(){
  int totalBlocks = player->GetHealth() / 10;

  for(int i = 0; i < totalBlocks; i++) {
    if(player->GetHealth() >= 70) { 
      auto hpBlock = make_shared<SFAsset>(SFASSET_HEALTHBLOCKG, sf_window);
      auto pos = Point2(20 + (16 * i), 450);
      hpBlock->SetPosition(pos);
      healthBlocks.push_back(hpBlock);
    }
    else if(player->GetHealth() >= 30) {
      auto hpBlock = make_shared<SFAsset>(SFASSET_HEALTHBLOCKY, sf_window);
      auto pos = Point2(20 + (16 * i), 450);
      hpBlock->SetPosition(pos);
      healthBlocks.push_back(hpBlock);
    }
    else {
      auto hpBlock = make_shared<SFAsset>(SFASSET_HEALTHBLOCKR, sf_window);
      auto pos = Point2(20 + (16 * i), 450);
      hpBlock->SetPosition(pos);
      healthBlocks.push_back(hpBlock); 
    }
  }

  for(int i = 0; i < gameDifficulty; i++){
    // Since the indicator will use the same sprite, it won't matter to use HEALTHBLOCKG
    auto stageIndicator = make_shared<SFAsset>(SFASSET_HEALTHBLOCKG, sf_window);
    auto pos = Point2(20 + (16 * i), 60);
    stageIndicator->SetPosition(pos);
    stage.push_back(stageIndicator);
  }
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

  // Render backgrounds
  for(auto s: stars) {
    s->OnRender();
  }

  // Draw the player (SFAsset::OnRender();)!
  player->OnRender();

  // Render projectiles that are currently alive
  for(auto p: pProjectiles) {
    if(p->IsAlive()) {
      p->OnRender();
    }
  }
  for(auto p: eProjectiles) {
    if(p->IsAlive()) {
      p->OnRender();
    }
  }

  for(auto power: powers) {
    if(power->IsAlive()) {
      power->OnRender();
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

  // Render healthbar
  for(auto hp: healthBlocks) {
    hp->OnRender();
  }
  for(auto bar: healthBar) {
    bar->OnRender();
  }

  // Just to show what stage we're on
  for(auto st: stage) {
    st->OnRender();
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

/***********************************************************
  This method is exactly what it says... It fires bullets.
***********************************************************/
void SFApp::FireProjectile(Point2 position, bool isPlayer) {
  if(isPlayer){

    if(firePower == 1){
      // Make the projectiles
      auto p1 = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
      auto p2 = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);

      int baseX = position.getX()-12;
      int baseY = position.getY();

      auto pos1 = Point2(baseX, baseY);
      auto pos2 = Point2(baseX+24, baseY);

      // Set the projectile to the position
      p1->SetPosition(pos1);
      p2->SetPosition(pos2);
      pProjectiles.push_back(p1);
      pProjectiles.push_back(p2);
    }
    else{
      // Make the projectiles
      auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);

      // Set the projectile to the position
      pb->SetPosition(position);
      pProjectiles.push_back(pb);
    }
    player->SetScore(player->GetScore() - 1);
  }
  else{
      // Make the projectiles
      auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);

      // Set the projectile to the position
      pb->SetPosition(position);
    eProjectiles.push_back(pb);
  }
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

void SFApp::GameDifficultyModifier(int diff) {
  cout << "Changing game stage... Stage " << diff << " of 5." << endl;
  if(gameDifficulty < diff) {
    int number_of_aliens;
    if(diff == 1){
      number_of_aliens = 2;
    }
    else if(diff == 2){
      number_of_aliens = 3;
    }
    else if(diff == 3){
      number_of_aliens = 5;
    }
    else if(diff == 4){
      number_of_aliens = 9;
    }

    for(int i = 0; i < number_of_aliens; i++) {
      // place an alien at width/number_of_aliens * i
      auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
      auto pos  = Point2(rand() % 600 + 32, rand() % 400 + 600);

      // Make enemy at position and set it's health
      alien->SetPosition(pos);
      alien->SetHealth(15);
      alien->SetFired(0);

      aliens.push_back(alien);

      cout << "Created enemy with " << alien->GetHealth() << endl;
    }
  }
}
