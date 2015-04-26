#include "SFEvent.h"

/*********************************************************
  Effectively wraps an SDL_Event in our custom event type.

  This can be used to scan for delayed keys, anything important
  should be placed in the new SFAsset::InputHandler (SFAsset.cpp)
**********************************************************/
SFEvent::SFEvent(const SDL_Event & event) {
  // Simply checks through our events
  switch (event.type) {
    // Checks if the QUIT event was initiated
    case SDL_QUIT:
      code = SFEVENT_QUIT;
      break;
    // For the update event
    case SDL_USEREVENT:
      code = SFEVENT_UPDATE;
      break;
    // For pressing keys
    case SDL_KEYDOWN:
      // Check the key pressed
      switch (event.key.keysym.sym) {
        // Firing projectiles key
        case SDLK_SPACE:
          code = SFEVENT_FIRE;
          break;
        // Quitting game
        case SDLK_q:
          code = SFEVENT_QUIT;
          break;
        // Pausing game
        case SDLK_p:
          code = SFEVENT_PAUSE;
          break;
      }
      break;
    // NULL event as there was nothing going on
    default:
      code = SFEVENT_NULL;
      break;
  }
}

SFEVENT SFEvent::GetCode() {
  return code;
}
