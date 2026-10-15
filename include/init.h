#ifndef INIT_H_
#define INIT_H_

#include "raylib.h"

typedef enum Scene {MENU, HIGH, GAME} Scene;

void InitGame(void);
void EndGame(void);

float GetWindowScale(void);

Scene GetCurScene(void);
void ChangeScene(Scene next);

bool ShouldGameClose(void);
void CloseGame(void);

extern Font font;

#endif
