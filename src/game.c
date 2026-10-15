#include "game.h"

#include <math.h>
#include <stddef.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"

#include "init.h"
#include "menus.h"


double GameScale;
int GameWidth;
int GameHeight;

float GameScaled20;

Texture bcg;
float bcgSrcWidth;
float bcgWidth;

Texture bullet;

double timer;
int ammo;
const int maxAmmo = 10;

int score;
const int defScorePerKill = 50;

float cameraPos;
int maxCameraPos;

const size_t timerStrSize = 3;
char timerStr[3];

float timerFontSize;
float timerFontSpacing;

//Vector2 timerSize;
Vector2 timerPos;

const size_t scoreStrSize = 6;
char scoreStr[6];

float scoreFontSize;
float scoreFontSpacing;

Vector2 scoreSize;
Vector2 scorePos;

Vector2 bulletSize;
float bulletGap;
Vector2 bulletPos;

//hint labels
float gameHintFontSize;
float gameHintFontSpacing;

const char *shootHintStr = "Press LMB to shoot.";
const char *rloadHintStr = "Press RMB to reload.";
const char *lookHintStr = "Move the mouse to look around.";

Vector2 shootHintSize;
Vector2 rloadHintSize;
Vector2 lookHintSize;

Vector2 shootHintPos;
Vector2 rloadHintPos;
Vector2 lookHintPos;

bool drawShootHint = true;
bool drawRloadHint = true;
bool drawLookHint = true;

//sounds
Sound outside;
Sound shot;
Sound empty;
Sound reload;

//enemies
const unsigned int maxEnemies = 30; //the load function divides by this divided by 2 so numbers smaller than 2 will break things
const unsigned int enemyTypeAmount = 3;
const unsigned int respawnTime = 2;

const int defEnemySize = 300;
const float defEnemySpeed = 400;

Enemy *enemy;

Texture enemyTex[3];

void 
RespawnEnemy(int id)
{
  int gap = 50 * GameScale; //so every enemy isnt flying at once 

  if (enemy[id].dir == RIGHT) enemy[id].x = id * -gap - enemy[id].size;
  else enemy[id].x = bcgWidth + id * gap;

  enemy[id].defY = GetRandomValue(0, bcg.height - enemy[id].size);
  enemy[id].y = enemy[id].defY;

  enemy[id].status = FLY;

  TraceLog(LOG_INFO, "Enemy %d of type %d has respawned to %f, %f.", id, enemy[id].type, enemy[id].x, enemy[id].y);
}

void 
LoadEnemies(void) 
{
  enemy = MemAlloc(maxEnemies * sizeof(Enemy));
  
  if (enemy != NULL) 
  {
    for (int i = 0; i < maxEnemies; i++)
    {
      //enums
      //enemy[i].status = FLY;  //set in the function RespawnEnemy
      enemy[i].dir = i % 2 == 0 ? LEFT : RIGHT;

      //integers
      enemy[i].type = i % enemyTypeAmount;
      enemy[i].z = 1 + i / (unsigned int)(maxEnemies / 2); //either 1 or 2

      //floats
      enemy[i].size = ((float)defEnemySize / ((float)enemy[i].z * 1.20)) * GameScale;
  
      RespawnEnemy(i); //sets the position and status
    }
  }
  else 
  {
    TraceLog(LOG_ERROR, "Failed to allocate enemy.");
    CloseGame();
  }
}

void 
UnloadEnemies(void)
{
  MemFree(enemy);
  enemy = NULL;

  TraceLog(LOG_INFO, "Succesfully unloaded enemy.");
}

void 
UpdateEnemies(double step, int timer) 
{
  for (int i = 0; i < maxEnemies; i++)
  {
    switch (enemy[i].status)
    {
      case FLY:
        if (enemy[i].dir == RIGHT) 
        {
          if (enemy[i].x - enemy[i].size >= bcgWidth)
          {
            RespawnEnemy(i);
          }
          else 
          {
            enemy[i].x += (defEnemySpeed * (enemy[i].type + 1)) * step * GameScale;
            enemy[i].y = enemy[i].defY + 2000 * sin(enemy[i].x / 100.0) * step * GameScale; // some magic numbers never hurt
          }
        }
        else
        {
          if (enemy[i].x + enemy[i].size <= 0)
          {
            RespawnEnemy(i);
          }
          else 
          {
            enemy[i].x -= (defEnemySpeed * (enemy[i].type + 1)) * step * GameScale;
            enemy[i].y = enemy[i].defY + 2000 * sin(enemy[i].x / 100.0) * step * GameScale; // some magic numbers never hurt
          }
        }
      break;
      case FAL:
        if (enemy[i].y - enemy[i].size >= GameHeight) 
        {
          enemy[i].respawnTime = timer - respawnTime;
          enemy[i].status = DED;
        }
        else 
        {
          enemy[i].y += 2 * defEnemySpeed * step * GameScale;
        }
      break;
      case DED:
        if (timer == enemy[i].respawnTime)
        {
          RespawnEnemy(i);
        }
    }
  }
}

void
DrawEnemies(void)
{
  for (int i = 0; i < maxEnemies; i++)
  {
    if (enemy[i].status != DED && enemy[i].x + enemy[i].size - cameraPos > 0 && enemy[i].x - enemy[i].size - cameraPos < GameWidth)
    {
      DrawTexturePro(enemyTex[enemy[i].type], (Rectangle){0, 0, (float)enemyTex[enemy[i].type].width, (float)enemyTex[enemy[i].type].height}, 
                     (Rectangle){enemy[i].x - cameraPos, enemy[i].y, enemy[i].size, enemy[i].size},
                     (Vector2){0, 0}, 0, WHITE);
    }
  }
}

//scope 

Vector2 lineVPoints[4];
Vector2 lineHPoints[4];

float GameScaled35;
float GameScaled8;

float lineThickness;

float ammoAngle;
float radius;
float radius2;

void
DrawScope(void)
{
  //the update code here since Draw runs at vsync while update at 60FPS
  Vector2 mousePos = (Vector2){(float)GetMouseX(), (float)GetMouseY()};

  lineVPoints[0] = (Vector2){mousePos.x, mousePos.y - GameScaled35};
  lineVPoints[1] = (Vector2){mousePos.x, mousePos.y - GameScaled8};
  lineVPoints[2] = (Vector2){mousePos.x, mousePos.y + GameScaled8};
  lineVPoints[3] = (Vector2){mousePos.x, mousePos.y + GameScaled35};

  lineHPoints[0] = (Vector2){mousePos.x - GameScaled35, mousePos.y};
  lineHPoints[1] = (Vector2){mousePos.x - GameScaled8, mousePos.y};
  lineHPoints[2] = (Vector2){mousePos.x + GameScaled8, mousePos.y};
  lineHPoints[3] = (Vector2){mousePos.x + GameScaled35, mousePos.y};

  ammoAngle = 360 / (float)maxAmmo * ammo + 90;
  
  //Draw
  DrawLineEx(lineVPoints[0], lineVPoints[1], lineThickness, BLACK);
  DrawLineEx(lineVPoints[2], lineVPoints[3], lineThickness, BLACK);

  DrawLineEx(lineHPoints[0], lineHPoints[1], lineThickness, BLACK);
  DrawLineEx(lineHPoints[2], lineHPoints[3], lineThickness, BLACK);

  DrawRing(mousePos, radius, radius + lineThickness, 0, 360, 50, BLACK);

  DrawRing(mousePos, radius, radius + lineThickness, 90, ammoAngle, 50, RED);

  DrawRing(mousePos, radius2, radius2 + lineThickness, 0, 360, 50, BLACK);

  DrawCircleV(mousePos, lineThickness, BLACK);
}

//score labels after kill

bool drawScoreLabel;

float scoreLabelFontSize;
float scoreLabelFontSpacing;

const char *scoreLabelStr = NULL;

Vector2 scoreLabelSize;
Vector2 scoreLabelPos;

int scoreLabelTimer;
const int scoreLabelDuration = 2;

void
LoadGame(void) 
{
  GameScale = GetWindowScale();
  GameWidth = GetRenderWidth();
  GameHeight = GetRenderHeight();


  bcg = LoadTexture("assets/bcg.png");
  SetTextureWrap(bcg, TEXTURE_WRAP_CLAMP); 

  bcgSrcWidth = GameWidth * (bcg.height / (float)GameHeight);
  bcgWidth = bcg.width * (GameHeight / (float)bcg.height); //scaled

  maxCameraPos = bcgWidth - GameWidth;

  timer = 91;
  ammo = 10;
  score = 0;

  cameraPos = 0;

  float GameScaled100 = 100 * GameScale;
  GameScaled20 = 20 * GameScale;

  const char *timerBuff = TextFormat("%02d", (unsigned int)timer);

  memcpy(timerStr, timerBuff, strlen(timerBuff));

  timerFontSize = GameScaled100;
  timerFontSpacing = timerFontSize / 10.0;

  timerPos = (Vector2){GameScaled100, GameScaled100};

  //score label
  const char *scoreBuff = TextFormat("%05d", score);

  memcpy(scoreStr, scoreBuff, strlen(scoreBuff));

  scoreFontSize = GameScaled100;
  scoreFontSpacing = scoreFontSize / 10.0;

  scoreSize = MeasureTextEx(font, scoreStr, scoreFontSize, scoreFontSpacing);
  scorePos = (Vector2){GameWidth - GameScaled100 - scoreSize.x, GameScaled100};

  //bullets
  bullet = LoadTexture("assets/bullet.png");

  bulletSize = (Vector2){bullet.width * GameScale, bullet.height * GameScale};
  bulletGap = bulletSize.x + GameScaled20;
  bulletPos = (Vector2){GameWidth - bulletGap * maxAmmo + GameScaled20 - GameScaled100, GameHeight - bulletSize.y - GameScaled100};

  //hints
  gameHintFontSize = GameScaled100 / 2.0;
  gameHintFontSpacing = gameHintFontSize / 10.0;

  shootHintSize = MeasureTextEx(font, shootHintStr, gameHintFontSize, gameHintFontSpacing);
  rloadHintSize = MeasureTextEx(font, rloadHintStr, gameHintFontSize, gameHintFontSpacing);
  lookHintSize = MeasureTextEx(font, lookHintStr, gameHintFontSize, gameHintFontSpacing);

  shootHintPos = (Vector2){(GameWidth - shootHintSize.x) / 2.0, GameScaled100};
  rloadHintPos = (Vector2){(GameWidth - rloadHintSize.x) / 2.0, shootHintPos.y + shootHintSize.y + GameScaled20};
  lookHintPos = (Vector2){(GameWidth - lookHintSize.x) / 2.0, rloadHintPos.y + rloadHintSize.y + GameScaled20};

  //sounds
  outside = LoadSound("assets/outside.ogg");
  shot = LoadSound("assets/shot.ogg");
  empty = LoadSound("assets/empty.ogg");
  reload = LoadSound("assets/reload.ogg");

  //enemies
  for (int i = 0; i < enemyTypeAmount; i++)
  {
    enemyTex[i] = LoadTexture(TextFormat("assets/enemy%d.png", i));
  }

  LoadEnemies();

  //scope
  GameScaled35 = 1.75 * GameScaled20;
  GameScaled8 = 0.4 * GameScaled20;

  lineThickness = GameScaled20 / 5.0;
  radius = GameScaled20 / 1.5;

  radius2 = radius + 2 * lineThickness;

  //score label
  drawScoreLabel = false;

  scoreLabelStr = NULL;

  scoreLabelFontSize = GameScaled100 / 2.0;
  scoreLabelFontSpacing = scoreLabelFontSize / 10.0;
}

void
UnloadGame(void) 
{
  UnloadTexture(bcg);
  UnloadTexture(bullet);

  for (int i = 0; i < enemyTypeAmount; i++)
  {
    UnloadTexture(enemyTex[i]);
  }

  UnloadEnemies();

  UnloadSound(outside);
  UnloadSound(shot);
  UnloadSound(empty);
  UnloadSound(reload);
}

void
UpdateGame(double step)
{
  if (!IsSoundPlaying(outside))
  {
    PlaySound(outside);
  }

  //updating the timer and score text
  if (TextToInteger(timerStr) != (int)timer) 
  {
    const char *timerBuff = TextFormat("%02d", (unsigned int)timer);

    memcpy(timerStr, timerBuff, strlen(timerBuff));
  }

  if (TextToInteger(scoreStr) != score)
  {
    const char *scoreBuff = TextFormat("%05d", score);

    memcpy(scoreStr, scoreBuff, strlen(scoreBuff));
  }

  //movement
  float fifthOfScreen = GameWidth / 5.0;

  if (GetMouseX() < fifthOfScreen) 
  {
    cameraPos = Clamp(cameraPos - (fifthOfScreen - GetMouseX()) / 4.0, 0, maxCameraPos);
    drawLookHint = false;
  }
  else if (GetMouseX() > 4 * fifthOfScreen) 
  {
    cameraPos = Clamp(cameraPos + (GetMouseX() - 4 * fifthOfScreen) / 4.0, 0, maxCameraPos);
    drawLookHint = false;
  }

  //ammo logic
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
  {
    drawShootHint = false;

    if (ammo <= 0) 
    {
      PlaySound(empty);
    }
    else 
    {
      ammo--;
      PlaySound(shot);

      for (int i = 0; i < maxEnemies; i++)
      {
        if (enemy[i].status == FLY)
        {
          if (CheckCollisionPointRec((Vector2){(float)GetMouseX() , (float)GetMouseY()}, (Rectangle){enemy[i].x - cameraPos, enemy[i].y, enemy[i].size, enemy[i].size}))
          {

            enemy[i].status = FAL;
            int scoreGained = defScorePerKill * enemy[i].z * ((enemy[i].type + 1) / 2.5);
            score += scoreGained;

            drawScoreLabel = true;

            scoreLabelStr = TextFormat("%d", scoreGained);
            scoreLabelSize = MeasureTextEx(font, scoreLabelStr, scoreLabelFontSize, scoreLabelFontSpacing);

            scoreLabelPos = (Vector2){enemy[i].x - scoreLabelSize.x / 2.0, enemy[i].y - scoreLabelSize.y / 2.0};
            scoreLabelTimer = timer - scoreLabelDuration;

            break;
          }
        }
      }
    }
  }
  else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) 
  {
    drawRloadHint = false;

    ammo = maxAmmo;
    PlaySound(reload);
  }

  UpdateEnemies(step, timer); //this line needs to be before UnloadGame() otherwise segfault for using freed memory
  
  //timer update
  if (timer <= 0) 
  {
    UnloadGame();
    LoadHigh(score);
    ChangeScene(HIGH);
  }
  else timer -= step;

  //scoreLabelUpdates
  if (drawScoreLabel)
  {
    if (scoreLabelTimer >= (int)timer) 
    {
      drawScoreLabel = false;
      scoreLabelStr = NULL;
    }
  }
}

void 
DrawGame(void) 
{
  DrawTexturePro(bcg, (Rectangle){cameraPos / GameScale, 0, bcgSrcWidth, bcg.height}, (Rectangle){0, 0, GameWidth, GameHeight}, (Vector2){0, 0}, 0, WHITE);

  DrawEnemies();

  DrawScope();

  DrawTextEx(font, timerStr, timerPos, timerFontSize, timerFontSpacing, RED);

  DrawTextEx(font, scoreStr, scorePos, scoreFontSize, scoreFontSpacing, RED);

  for (int i = 0; i < ammo; i++) 
  {
    DrawTexturePro(bullet, (Rectangle){0, 0, bullet.width, bullet.height}, (Rectangle){bulletPos.x + bulletGap * i, bulletPos.y, bulletSize.x, bulletSize.y}, (Vector2){0, 0}, 0, RED);
  }

  DrawTextEx(font, scoreLabelStr, (Vector2){scoreLabelPos.x - cameraPos, scoreLabelPos.y}, scoreLabelFontSize, scoreLabelFontSpacing, RED);

  //hints
  if (drawShootHint) DrawTextEx(font, shootHintStr, shootHintPos, gameHintFontSize, gameHintFontSpacing, RED);
  if (drawRloadHint) DrawTextEx(font, rloadHintStr, rloadHintPos, gameHintFontSize, gameHintFontSpacing, RED);
  if (drawLookHint) DrawTextEx(font, lookHintStr, lookHintPos, gameHintFontSize, gameHintFontSpacing, RED);
}
