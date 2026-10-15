#include "menus.h"

#include "raylib.h"
#include "cJSON.h"
#include <string.h>

#include "init.h"
#include "game.h"

float MenusScale;
float MenusWidth;
float MenusHeight;
const int startKey = KEY_ENTER;

//labels
const char *title = "Sordes-Osores";

float titleFontSize;
float titleFontSpacing;

Vector2 titleSize;

Vector2 titlePos;

const char *play = "Press Enter to play!";

float playFontSize;
float playFontSpacing;

Vector2 playSize;

Vector2 playPos;

void
LoadMenu(void)
{
  MenusScale = GetWindowScale();
  MenusWidth = GetScreenWidth();
  MenusHeight = GetScreenHeight();
  
  //labels 
  titleFontSize = 150 * MenusScale;
  titleFontSpacing = titleFontSize / 10.0;

  titleSize = MeasureTextEx(font, title, titleFontSize, titleFontSpacing);

  titlePos = (Vector2){(MenusWidth - titleSize.x) / 2.0, (MenusHeight - titleSize.y) / 2.0};

  playFontSize = 75 * MenusScale;
  playFontSpacing = playFontSize / 10.0;

  playSize = MeasureTextEx(font, play, playFontSize, playFontSpacing);

  playPos = (Vector2){(MenusWidth - playSize.x) / 2.0, MenusHeight - 2 * playSize.y};
}

void 
UpdateMenu(void) 
{
  if (IsKeyPressed(startKey)) 
  {
    LoadGame();
    ChangeScene(GAME);
  }
}


void
DrawMenu(void) 
{
  //drawGameBackground pls
  DrawRectangle(0, 0, MenusWidth, MenusHeight, Fade(BLACK, .5));

  DrawTextEx(font, title, titlePos, titleFontSize, titleFontSpacing, RED);

  DrawTextEx(font, play, playPos, playFontSize, playFontSpacing, RED);
}

//high
cJSON *json;

const int highScoresShown = 5;

const int nameLen = 6;

char *hsName[5];
int hsScore[5];

//labels

float gap;
float curY;
float lineThick;

const char *highScoreStr = "High Scores";

float highScoreFontSize;
float highScoreFontSpacing;

Vector2 highScoreSize;
Vector2 highScorePos;

float line1Y;

float hsFontSize;
float hsFontSpacing;

const int hsStrLen = 14;
char hsStr[5][14];

Vector2 hsSize[5];
Vector2 hsPos[5];

float line2Y;

char playerScoreStr[14];
int playerScore;

float playerScoreFontSize;
float playerScoreFontSpacing;

Vector2 playerScoreSize;
Vector2 playerScorePos;

bool typedName;

const int hintAmount = 2;

const char *hintStr[2] = {"Type your name and press ENTER to get ranked.", "Press ENTER to play again."};

float hintFontSize;
float hintFontSpacing;

Vector2 hintSize[2];
Vector2 hintPos[2];

void
CenterPlayerScore(void)
{
  playerScoreSize = MeasureTextEx(font, (const char *)playerScoreStr, playerScoreFontSize, playerScoreFontSpacing);

  playerScorePos.x = (MenusWidth - playerScoreSize.x) / 2.0;
}

void
CenterHsScore(int i)
{
  hsSize[i] = MeasureTextEx(font, (const char *)hsStr[i], hsFontSize, hsFontSpacing);

  hsPos[i].x = (MenusWidth - hsSize[i].x) / 2.0;
}

void
LoadHigh(int achiScore)
{
  //loading of the scoreboard
  int dataSize;
  char *data = LoadFileData("assets/high.json", &dataSize);

  json = cJSON_Parse(data);

  if (json == NULL)
  {
    const char *errorPtr = cJSON_GetErrorPtr();

    if (errorPtr != NULL)
    {
      TraceLog(LOG_ERROR, "%s", errorPtr);
    }

    cJSON_Delete(json);
  }

  cJSON *array = cJSON_GetObjectItem(json, "scoreBoard");

  if (array == NULL)
  {
    TraceLog(LOG_ERROR, "Could not load array scoreBoard!");
  }
  else
  {
    TraceLog(LOG_INFO, "Loaded array scoreBoard succesfully!");

    if (cJSON_IsArray(array))
    {
      for (int i = 0; i < cJSON_GetArraySize(array) && i < highScoresShown; i++)
      {
        cJSON *curObject = cJSON_GetArrayItem(array, i);

        if (curObject == NULL)
        {
          TraceLog(LOG_ERROR, "Could not load index %d!", i);
        }
        else
        {
          TraceLog(LOG_INFO, "Loaded index %d!", i);

          cJSON *name = cJSON_GetObjectItem(curObject, "name");
          cJSON *score = cJSON_GetObjectItem(curObject, "score");

          if (cJSON_IsString(name) && name != NULL)
          {
            hsName[i] = name->valuestring;
            TraceLog(LOG_INFO, "Loaded name[%d]!", i);
          }
          else
          {
            TraceLog(LOG_ERROR, "Could not load name[%d]!", i);
          }

          if (cJSON_IsNumber(score))
          {
            hsScore[i] = score->valueint;
            TraceLog(LOG_INFO, "Loaded score[%d]!", i);
          }
          else
          {
            TraceLog(LOG_ERROR, "Could not load score[%d]!", i);
          }
        }
      }
    }
  }

  //labels
  gap = 33.1 * MenusScale; //well calculated

  curY = gap;

  //high Scores label
  highScoreFontSize = 125 * MenusScale;
  highScoreFontSpacing = highScoreFontSize / 10.0;
  
  highScoreSize = MeasureTextEx(font, highScoreStr, highScoreFontSize, highScoreFontSpacing);
  highScorePos = (Vector2){(MenusWidth - highScoreSize.x) / 2.0, curY};

  curY += highScoreSize.y + gap;

  line1Y = curY;
  lineThick = 15 * MenusScale;

  curY += lineThick + gap;

  hsFontSize = 100 * MenusScale;
  hsFontSpacing = hsFontSize / 10.0;

  for (int i = 0; i < highScoresShown; i++)
  {
    const char *hsStrBuff = TextFormat("%5s - %05d", hsName[i], hsScore[i]);

    TextCopy(hsStr[i], hsStrBuff);

    CenterHsScore(i);
    hsPos[i].y = curY;

    curY += hsSize[i].y + gap;
  }

  line2Y = curY;

  curY += lineThick + gap;

  playerScoreFontSize = 50 * MenusScale;
  playerScoreFontSpacing = playerScoreFontSize / 10.0;

  const char *playerScoreBuff = TextFormat("_____ - %05d", achiScore);

  TextCopy(playerScoreStr, playerScoreBuff);

  playerScore = achiScore;

  CenterPlayerScore();
  playerScorePos.y = curY;

  curY += playerScoreSize.y + gap;

  typedName = false;

  //hint
  hintFontSize = 40 * MenusScale;
  hintFontSpacing = hintFontSize / 10.0;

  for (int i = 0; i < hintAmount; i++)
  {
    hintSize[i] = MeasureTextEx(font, hintStr[i], hintFontSize, hintFontSpacing);

    hintPos[i] = (Vector2){(MenusWidth - hintSize[i].x) / 2.0, curY};
  }
}

void 
UnloadHigh(void)
{
  //TODO: implement json saving DONE

  cJSON *root = cJSON_CreateObject();
  cJSON *array = cJSON_CreateArray();

  cJSON_AddItemToObject(root , "scoreBoard", array);

  for (int i = 0; i < highScoresShown; i++)
  {
    cJSON *index = cJSON_CreateObject();

    cJSON_AddStringToObject(index, "name", hsName[i]);
    cJSON_AddNumberToObject(index, "score", hsScore[i]);

    cJSON_AddItemToArray(array, index);
  }

  char *jsonString = cJSON_Print(root);

  SaveFileData("assets/high.json", jsonString, strlen(jsonString));

  MemFree(jsonString);

  cJSON_Delete(root);
  cJSON_Delete(json);
}

void
UpdateHigh(void)
{
  if (typedName)
  {
    if (IsKeyPressed(KEY_ENTER))
    {
      UnloadHigh();
      LoadGame();
      ChangeScene(GAME);
    }
  }
  else
  {
    char key = GetCharPressed();

    if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z'))
    {
      for (int i = 0; i < nameLen - 1; i++)
      {
        if (playerScoreStr[i] == '_') 
        {
          playerScoreStr[i] = key;
          break;
        }
      }

      CenterPlayerScore();
    }
    else if (IsKeyPressed(KEY_BACKSPACE))
    {
      for (int i = nameLen - 2; i >= 0; i--)
      {
        if (playerScoreStr[i] != '_')
        {
          playerScoreStr[i] = '_';
          break;
        }
      }

      CenterPlayerScore();
    }
    else if (IsKeyPressed(KEY_ENTER))
    {
      for (int i = 0; i < nameLen - 1; i++)
      {
        if (playerScoreStr[i] == '_') playerScoreStr[i] = ' ';
      }

      //rank here
      int rank = highScoresShown;

      for (int i = highScoresShown - 1; i >= 0; i--)
      {
        if (hsScore[i] < playerScore) rank--;
        else break;
      }

      TraceLog(LOG_INFO, "rank = %d", rank);

      for (int i = highScoresShown - 1; i >= rank; i--) {
        if (i == rank) {
          hsScore[i] = playerScore;

          memcpy(hsName[i], playerScoreStr, nameLen);

          hsName[i][nameLen - 1] = '\0';

          const char *hsStrBuff = TextFormat("%05s - %05d", hsName[i], hsScore[i]);
          TextCopy(hsStr[i], hsStrBuff);
        }
        else {
          hsScore[i] = hsScore[i - 1];

          TextCopy(hsName[i], hsName[i - 1]);

          const char *hsStrBuff = TextFormat("%05s - %05d", hsName[i], hsScore[i]);
          TextCopy(hsStr[i], hsStrBuff);
        }

        CenterHsScore(i);
      }

      typedName = true;
    }
  }
}

void
DrawHigh(void)
{
  DrawTextEx(font, highScoreStr, highScorePos, highScoreFontSize, highScoreFontSpacing, RED);

  DrawLineEx((Vector2){0, line1Y}, (Vector2){MenusWidth, line1Y}, lineThick, RED);

  for (int i = 0; i < highScoresShown; i++)
  {
    DrawTextEx(font, hsStr[i], hsPos[i], hsFontSize, hsFontSpacing, RED);
  }

  DrawLineEx((Vector2){0, line2Y}, (Vector2){MenusWidth, line2Y}, lineThick, RED);

  DrawTextEx(font, (const char *)playerScoreStr, playerScorePos, playerScoreFontSize, playerScoreFontSpacing, RED);

  if (!typedName) DrawTextEx(font, hintStr[0], hintPos[0], hintFontSize, hintFontSpacing, WHITE);
  else DrawTextEx(font, hintStr[1], hintPos[1], hintFontSize, hintFontSpacing, WHITE);
}
