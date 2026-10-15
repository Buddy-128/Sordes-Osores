#ifndef GAME_H_
#define GAME_H_

typedef enum Status {
  FLY, FAL, DED
} Status;

typedef enum Direction {
  LEFT, RIGHT
} Direction;

typedef struct Enemy {
  Direction dir;
  Status status;

  int type;
  int z;
  int respawnTime;

  float size;
  float x;
  float y;
  float defY;
} Enemy;

void LoadGame(void);
void UnloadGame(void);
void UpdateGame(double step);
void DrawGame(void);

#endif
