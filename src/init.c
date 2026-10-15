#include "init.h"
#include "menus.h"
#include "game.h"

#include <stddef.h>

#include "raylib.h"

  

Font font;

Scene curScene;

const int defHeight = 1080;

float scale;

bool close;

//Game
void 
InitGame(void) 
{
  curScene = MENU;

  SetConfigFlags(FLAG_VSYNC_HINT);
  SetConfigFlags(FLAG_MSAA_4X_HINT);

  SetTraceLogLevel(LOG_ERROR);

  InitWindow(800, 600, "Chicken");
  InitAudioDevice();

  font = LoadFontEx("assets/font.ttf", 150, NULL, 223);

  int monitor = GetCurrentMonitor();

  int width = GetMonitorWidth(monitor);
  int height = GetMonitorHeight(monitor);

  ToggleFullscreen();
  SetWindowSize(width, height);

  scale = height / (float)defHeight;

  close = false;

  HideCursor();
  LoadMenu();
}

void
EndGame() 
{
  switch (curScene) {
    case MENU:
      //UnloadMenu();
    break;
    case HIGH:
      UnloadHigh();
    break;
    case GAME:
      UnloadGame();
  }

  UnloadFont(font);
  CloseAudioDevice();
  CloseWindow();
}

float 
GetWindowScale(void) 
{
  return scale;
}

//Scenes
Scene
GetCurScene(void) 
{
  return curScene;
}

void 
ChangeScene(Scene next) 
{
  curScene = next;
}

bool
ShouldGameClose(void)
{
  return close;
}

void
CloseGame(void)
{
  close = true;
}
