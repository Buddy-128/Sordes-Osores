#include "raylib.h"

#include "init.h"
#include "menus.h"
#include "game.h"

int 
main() 
{
  InitGame();

  double accu = 0;
  const double step = 1 / 60.0;

	while(!WindowShouldClose() && !ShouldGameClose()){
    float dt = GetFrameTime();
    if (dt > .25) dt = .25;
    
    accu += dt;


    for (;accu >= step;) {
      //update here and pass step
      switch (GetCurScene()) {
        case MENU:
          UpdateMenu();
        break;
        case GAME:
          UpdateGame(step);
        break;
        case HIGH:
          UpdateHigh();
      }

      accu -= step;
    }


		BeginDrawing();
      ClearBackground(BLACK);

      switch (GetCurScene()) {
        case MENU:
          DrawMenu();
        break;
        case GAME:
          DrawGame();
        break;
        case HIGH:
          DrawHigh();
      }
		EndDrawing();
	}

  EndGame();
	return 0;
}
