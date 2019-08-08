/*
    Bounce is a demo game written to show off features of SDL
    (www.libsdl.org).

    Copyright (C) 2002 Bob Pendleton

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2.0 of
    the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
    
    Bob Pendleton
    Bob@Pendleton.com
*/
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "SDL.h"
#include "SDL_ttf.h"

//-------------------------------------------------

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define abs(a) (((a)<0) ? -(a) : (a))
#define sign(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

//-------------------------------------------------

SDL_TimerID timer = 0;
SDL_Surface *screen = NULL;

int sw = 640;
int sh = 480;

int lastTime = 0;
int now = 0;
int dt = 0;
int minFrameTime = 9;

int flashTime = 0;
bool redWhite = false;
int welcomeTime = 0;

bool done = false;

int minSpeed = 1;
int speedRange = 6;

int myScore = 0;
int myOldScore = 0;

int yourScore = 0;
int yourOldScore = 0;

int winningScore = 30;
int scoreTime = 0;
int scorePause = 30;
char *winner = NULL;

//-------------------------------------------------

Uint32 red = 0;
Uint32 black = 0;
Uint32 white = 0;
Uint32 bg = 0;

//-------------------------------------------------

typedef struct
{
  int minx, maxx;
  int miny, maxy;
  int advance;
  SDL_Surface *pic;
} glyph;

typedef struct
{
  int height;
  int ascent;
  int descent;
  int lineSkip;
  glyph glyphs[128];
} glyphFont;

glyphFont redFont15;
glyphFont whiteFont15;

glyphFont redFont30;
glyphFont whiteFont30;

glyphFont redFont100;
glyphFont whiteFont100;

glyphFont *bounceFont = NULL;

int me30w2;
char meText[] = "ME";

int you30w2;
char youText[] = "YOU";

//-------------------------------------------------

typedef struct
{
  float x, y;
  float dx, dy;
  float ox, oy;
  int w, h;
  int w2, h2;
  SDL_Surface *image;
} movable;

movable earth;
movable moon;
movable sun;

//-------------------------------------------------

float sunx = sw / 2.0;
float suny = sh + 120.0;
float sunRadius = 200.0;

//-------------------------------------------------

enum
  {
    MY_TIMEREVENT,
  };

//-------------------------------------------------

typedef void (*page)(SDL_Event *e);

page currentPage = NULL;

//-------------------------------------------------

void errorExit(char *msg)
{
    printf("%s\n", msg);
    SDL_Quit();
    exit(1);
}

//-------------------------------------------------

void initWhoWon(char *msg);
void drawWhoWon(int dt);
void whoWon(SDL_Event *e);

void initWelcome();
void drawWelcome(int dt);
void welcome(SDL_Event *e);

void initBounce(glyphFont *font);
void finitBounce();
void drawBounce(int dt);
void bounce(SDL_Event *e);

//-------------------------------------------------

void initColors(SDL_PixelFormat *pf)
{
  red = SDL_MapRGB(pf, 0xff, 0x00, 0x00);
  black = SDL_MapRGB(pf, 0x00, 0x00, 0x00);
  white = SDL_MapRGB(pf, 0xff, 0xff, 0xff);

  bg = black;
}

//-------------------------------------------------

void textSize(glyphFont *font, 
                char *text, 
                SDL_Rect *r)
{
  int maxx = 0;
  int advance = 0;

  r->x = 0;
  r->y = 0;
  r->w = 0;
  r->h = font->height;

  while (0 != *text)
  {
    maxx = font->glyphs[*text].maxx;
    advance = font->glyphs[*text].advance;
    r->w += advance;

    text++;
  }

  r->w = r->w - advance + maxx;
}

void eraseText(SDL_Surface *dst, 
                 glyphFont *font, 
                 char *text, 
                 SDL_Rect *r,
                 int x, int y, 
                 Uint32 color)
{
  textSize(font, text, r);
  r->x = x;
  r->y = y;
  SDL_FillRect(screen, r, color);
}

void drawText(SDL_Surface *dst, 
                glyphFont *font, 
                char *text, 
                SDL_Rect *r,
                int x, int y)
{
  int maxx = 0;
  int advance = 0;
  SDL_Rect at = {0, 0, 0, 0};

  r->x = x;
  r->y = y;
  r->w = 0;
  r->h = font->height;

  at.x = x;
  at.y = y;
  while (0 != *text)
  {
    SDL_BlitSurface(font->glyphs[*text].pic, NULL, screen, &at);

    maxx = font->glyphs[*text].maxx;
    advance = font->glyphs[*text].advance;

    r->w += advance;
    at.x = x + r->w;

    text++;
  }
  r->w = r->w - advance + maxx;
}

void loadFont(glyphFont *font, 
              char *fontName, 
              int ptSize, 
              int style,
              Uint32 fg,
              Uint32 bg,
              SDL_PixelFormat *pf)
{
  int i;
  char letter[2] = {0, 0};
  TTF_Font *ttfFont = NULL;
  SDL_Color foreGround;
  SDL_Color backGround;

  if (TTF_Init() < 0)
  {
    errorExit("Can't init SDL_ttf");
  }

  ttfFont = TTF_OpenFont(fontName, ptSize);
  if (NULL == ttfFont)
  {
    errorExit("Can't open font file");
  }

  TTF_SetFontStyle(ttfFont, style);

  SDL_GetRGB(fg, pf, 
             &foreGround.r, 
             &foreGround.g, 
             &foreGround.b);

  SDL_GetRGB(bg, pf, 
             &backGround.r, 
             &backGround.g, 
             &backGround.b);

  font->height = TTF_FontHeight(ttfFont);
  font->ascent = TTF_FontAscent(ttfFont);
  font->descent = TTF_FontDescent(ttfFont);
  font->lineSkip = TTF_FontLineSkip(ttfFont);

  for (i = 0; i < 128; i++)
  {
    SDL_Surface *g0 = NULL;
    SDL_Surface *g1 = NULL;

    font->glyphs[i].pic = NULL;

    letter[0] = (Uint8)i;

    TTF_GlyphMetrics(ttfFont, 
                     (Uint16)i, 
                     &font->glyphs[i].minx, 
                     &font->glyphs[i].maxx, 
                     &font->glyphs[i].miny, 
                     &font->glyphs[i].maxy, 
                     &font->glyphs[i].advance);

    g0 = TTF_RenderText_Shaded(ttfFont, 
                               letter, 
                               foreGround, 
                               backGround);
    if (NULL != g0)
    {
      g1 = SDL_DisplayFormat(g0);
      SDL_FreeSurface(g0);
    }

    if (NULL != g1)
    {
      SDL_SetColorKey(g1, 
                      (SDL_SRCCOLORKEY | 
                       SDL_RLEACCEL), 
                      bg);
    }

    font->glyphs[i].pic = g1;
  }

  TTF_Quit();
}

//-------------------------------------------------

void initMovable(movable *t, SDL_Surface *s)
{
  t->image = s;
  t->w = s->w;
  t->w2 = t->w / 2;

  t->h = s->h;
  t->h2 = t->h / 2;

  t->x = 0.0;
  t->y = 0.0;

  t->ox = 0.0;
  t->oy = 0.0;

  t->dx = 0.0;
  t->dy = 0.0;
}

int randomRange(int range)
{
  return random() % range;
}

float fiddle(float f)
{
  if (f > 0)
  {
    f = (minSpeed + randomRange(speedRange));
  }
  else
  {
    f = -(minSpeed + randomRange(speedRange));
  }

  return f;
}

float dist2point(float ax, float ay, 
                 float bx, float by)
{
  float dx = bx - ax;
  float dy = by - ay;

  return hypot(dx, dy);
}

float dist2line(float px, float py, 
                float ax, float ay, 
                float bx, float by)
{
  float dx = bx - ax;
  float dy = by - ay;
  float dist = 0.0;
  float denom = (dx * dx) + (dy * dy);

  if (0.0001 > denom)
  {
    return dist2point(px, py, ax, ay);
  }
  else
  {
    float a2 = ((py - ay) * dx) - ((px - ax) * dy);
    dist = sqrt((a2 * a2) / denom);
  }
  
  return dist;
}

float dist2seg(float px, float py, 
               float ax, float ay, 
               float bx, float by)
{
  float dx = bx - ax;
  float dy = by - ay;

  float t;

  t = ((px - ax) * dx) + ((py - ay) * dy);
  if (t < 0.0) // out past 
  {
    return dist2point(px, py, ax, ay);
  }

  t = ((bx - px) * dx) + ((by - py) * dy);
  if (t < 0.0)
  {
    return dist2point(px, py, bx, by);
  }
  
  return dist2line(px, py, ax, ay, bx, by);
}

void pointBounce(movable *t, float hx, float hy)
{
  float dx = t->x - hx;
  float dy = t->y - hy;
  float dist = hypot(dx, dy);
  float speed = 2 *(minSpeed + 
                    randomRange(speedRange));

  dx /= dist;
  dy /= dist;

  t->dx = dx * speed;
  t->dy = dy * speed;
}

void wallBounce(movable *t)
{
  if (0 == t->dx)
  {
    t->dx = minSpeed;
  }

  if (0 == t->dy)
  {
    t->dy = minSpeed;
  }

  if ((t->x < t->w2) || (t->x > (sw - t->w2)))
  {
    t->x = max(t->x, t->w2);
    t->x = min(t->x, sw - (t->w2));

    t->dx = -(t->dx);
    t->dx = fiddle(t->dx);
  }

  if ((t->y < t->h2) || (t->y > (sh - t->h2)))
  {
    t->y = max(t->y, t->h2);
    t->y = min(t->y, sh - (t->h2));

    t->dy = -(t->dy);
    t->dy = fiddle(t->dy);
  }
}

void demoBounce()
{
  float dist;

  dist = dist2seg(moon.x, moon.y,
                  earth.ox, earth.oy, 
                  earth.x, earth.y);
  if (dist <= ((float)(earth.w2 + moon.w2)))
  {
    pointBounce(&earth, moon.x, moon.y);
    pointBounce(&moon, earth.x, earth.y);
  }

  dist = dist2point(earth.x, earth.y, 
                    sunx, suny);
  if (dist <= (sunRadius + earth.w2))
  {
    pointBounce(&earth, sunx, suny);
  }

  dist = dist2point(moon.x, moon.y, 
                    sunx, suny);
  if (dist <= (sunRadius + moon.w2))
  {
    pointBounce(&moon, sunx, suny);
  }
}

void clipMovable(movable *t)
{
  if ((t->x < t->w2) || (t->x > (sw - t->w2)))
  {
    t->x = max(t->x, t->w2);
    t->x = min(t->x, (sw - t->w2));
  }

  if ((t->y < t->h2) || (t->y > (sh - t->h2)))
  {
    t->y = max(t->y, t->h2);
    t->y = min(t->y, (sh - t->h2));
  }

}

void drawMovable(movable *t, SDL_Rect *r)
{
  r->x = (Sint16)(t->x - t->w2);
  r->y = (Sint16)(t->y - t->h2);
  r->w = t->w;
  r->h = t->h;
  SDL_BlitSurface(t->image, NULL, screen, r);
}

void eraseMovable(movable *t, SDL_Rect *r)
{
  r->x = (Sint16)(t->ox - t->w2);
  r->y = (Sint16)(t->oy - t->h2);
  r->w = t->w;
  r->h = t->h;
  SDL_FillRect(screen, r, bg);
}

//-------------------------------------------------

void initBounce(glyphFont *font)
{
  SDL_Rect s;

  srandom(time(NULL));

  earth.dx = randomRange(speedRange);
  earth.dy = randomRange(speedRange);

  earth.x = earth.ox = (sw / 2.0) - earth.w2;
  earth.y = earth.oy = (sh / 2.0) - earth.h2;

  moon.x = sw / 2.0;
  moon.y = sh - (moon.w * 3.0);

  myScore = 0;
  myOldScore = 0;

  yourScore = 0;
  yourOldScore = 0;

  scoreTime = 0;

  bounceFont = font;

  textSize(bounceFont, meText, &s);
  me30w2 = s.w / 2;

  textSize(bounceFont, youText, &s);
  you30w2 = s.w / 2;

  sun.x = sw / 2;
  sun.y = sh - sun.h2;

  SDL_ShowCursor(SDL_DISABLE);

  SDL_FillRect(screen, NULL, bg);
  SDL_UpdateRect(screen, 0, 0, 0, 0);

  drawBounce(0);
}

void finitBounce()
{
  SDL_ShowCursor(SDL_ENABLE);
}

void checkScore()
{

  if (myScore >= winningScore)
  {
    finitBounce();
    initWhoWon("You Lost!");
    currentPage = whoWon;
  }
  else if (yourScore >= winningScore)
  {
    finitBounce();
    initWhoWon("You WON!");
    currentPage = whoWon;
  }
}

void testHits(int dt)
{
  float dist = 0.0;

  scoreTime -= dt;
  scoreTime = max(scoreTime, 0);

  // did Earth hit the sun?
  dist = dist2point(earth.x, earth.y, 
                    sunx, suny);
  if (dist <= (sunRadius + earth.w2))
  {
    if (0 == scoreTime)
    {
      myScore++;
    }
    pointBounce(&earth, sunx, suny);
    scoreTime = scorePause;
  }

  // did earth hit the moon?
  dist = dist2seg(moon.x, moon.y,
                  earth.ox, earth.oy, 
                  earth.x, earth.y);

  if (dist <= ((float)(earth.w2 + moon.w2)))
  {
    if (0 == scoreTime)
    {
      yourScore++;
    }
    pointBounce(&earth, moon.x, moon.y);
    scoreTime = scorePause;
    return;
  }
}

void drawBounce(int dt)
{
  SDL_Rect size;
  int posSize = 20;
  SDL_Rect pos[posSize];
  int np = -1;
  int drop = 20;
  int line = 25;
  char score[] = "0000000000000";

  // erase the score
  np++;
  sprintf(score, "%2d", yourOldScore);
  textSize(bounceFont, score, &size);
  eraseText(screen, 
            bounceFont, 
            score, 
            &pos[np], 
            160 - (size.w / 2), drop + line,
            bg);
  yourOldScore = yourScore;

  np++;
  sprintf(score, "%2d", myOldScore);
  textSize(bounceFont, score, &size);
  eraseText(screen, 
            bounceFont, 
            score, 
            &pos[np], 
            480 - (size.w / 2), drop + line,
            bg);
  myOldScore = myScore;

  // erase the earth
  np++;
  earth.ox = earth.x;
  earth.oy = earth.y;
  eraseMovable(&earth, &pos[np]);

  // erase the moon
  np++;
  eraseMovable(&moon, &pos[np]);

  // draw the score
  np++;
  drawText(screen, 
           bounceFont, 
           youText,
           &pos[np], 
           160 - you30w2, drop);

  np++;
  sprintf(score, "%2d", yourScore);
  textSize(bounceFont, score, &size);
  drawText(screen, 
           bounceFont, 
           score,
           &pos[np], 
           160 - (size.w / 2), drop + line);

  np++;
  drawText(screen, 
           bounceFont, 
           meText, 
           &pos[np], 
           480 - me30w2, drop);

  np++;
  sprintf(score, "%2d", myScore);
  textSize(bounceFont, score, &size);
  drawText(screen, 
           bounceFont, 
           score, 
           &pos[np], 
           480 - (size.w / 2), drop + line);

  // draw the sun
  np++;
  drawMovable(&sun, &pos[np]);

  // draw the earth
  earth.x += (earth.dx * dt) / 5.0;
  earth.dy += (dt / 200.0); // gravity
  earth.y += (earth.dy * dt) / 5.0;

  wallBounce(&earth);

  np++;
  drawMovable(&earth, &pos[np]);

  // draw the moon
  clipMovable(&moon);

  moon.ox = moon.x;
  moon.oy = moon.y;
  np++;
  drawMovable(&moon, &pos[np]);

  SDL_UpdateRects(screen, np + 1, pos);

  if (np >= posSize)
  {
    errorExit("Increase posSize you lazy...\n");
  }
}

void bounce(SDL_Event *e)
{
  switch (e->type)
  {
    case SDL_USEREVENT:
      switch (e->user.code)
      {
      case MY_TIMEREVENT:
        now = SDL_GetTicks();
        dt = now - lastTime;
        if (dt >= minFrameTime)
        {
          drawBounce(dt);
          testHits(dt);
          checkScore();

          lastTime = now;
        }
        break;
      }
      break;

  case SDL_MOUSEMOTION:
    moon.x = e->motion.x;
    moon.y = e->motion.y;
    break;
  }
}

//-------------------------------------------------

void initWhoWon(char *msg)
{
  winner = msg;

  earth.dx = randomRange(speedRange);
  earth.dy = randomRange(speedRange);

  earth.x = earth.ox = (sw / 2.0) - earth.w2;
  earth.y = earth.oy = (sh / 2.0) - earth.h2;

  moon.dx = randomRange(speedRange);
  moon.dy = randomRange(speedRange);

  moon.x = sw / 2.0;
  moon.y = sh - (moon.w * 3.0);

  sun.x = sw / 2;
  sun.y = sh - sun.h2;

  redWhite = false;
  flashTime = 200;
  welcomeTime = 30 * 1000;

  SDL_FillRect(screen, NULL, bg);
  SDL_UpdateRect(screen, 0, 0, 0, 0);

  drawWhoWon(0);
}

void drawWhoWon(int dt)
{
  SDL_Rect size;
  int posSize = 20;
  SDL_Rect pos[posSize];
  int np = -1;
  int drop = 20;
  int line = 25;
  char score[] = "0000000000000";

  welcomeTime -= dt;
  if (welcomeTime <= 0)
  {
    initWelcome();
    currentPage = welcome;
    return;
  }

  flashTime -= dt;
  if (flashTime <= 0)
  {
    redWhite = !redWhite;
    flashTime = 200;
  }

  // erase the earth
  np++;
  earth.ox = earth.x;
  earth.oy = earth.y;
  eraseMovable(&earth, &pos[np]);

  // erase the moon
  np++;
  moon.ox = moon.x;
  moon.oy = moon.y;
  eraseMovable(&moon, &pos[np]);

  // draw the score
  np++;
  drawText(screen, 
           bounceFont, 
           youText,
           &pos[np], 
           160 - you30w2, drop);

  np++;
  sprintf(score, "%2d", yourScore);
  textSize(bounceFont, score, &size);
  drawText(screen, 
           bounceFont, 
           score,
           &pos[np], 
           160 - (size.w / 2), drop + line);

  np++;
  drawText(screen, 
           bounceFont, 
           meText, 
           &pos[np], 
           480 - me30w2, drop);

  np++;
  sprintf(score, "%2d", myScore);
  textSize(bounceFont, score, &size);
  drawText(screen, 
           bounceFont, 
           score, 
           &pos[np], 
           480 - (size.w / 2), drop + line);

  // draw the earth
  earth.x += (earth.dx * dt) / 5.0;
  earth.dy += (dt / 200.0); // gravity
  earth.y += (earth.dy * dt) / 5.0;

  wallBounce(&earth);

  np++;
  drawMovable(&sun, &pos[np]);

  np++;
  drawMovable(&earth, &pos[np]);

  // draw the moon
  moon.x += (moon.dx * dt) / 5.0;
  moon.y += (moon.dy * dt) / 5.0;

  wallBounce(&moon);
  demoBounce();

  np++;
  drawMovable(&moon, &pos[np]);

  if (redWhite)
  {
    np++;
    textSize(&redFont100, winner, &size);
    drawText(screen,
             &redFont100, 
             winner, 
             &pos[np], 
             (sw - size.w) / 2, 100);
  }
  else
  {
    np++;
    textSize(&whiteFont100, winner, &size);
    drawText(screen,
             &whiteFont100, 
             winner, 
             &pos[np], 
             (sw - size.w) / 2, 100);
  }

  np++;
  textSize(&whiteFont15, "CLICK TO BEGIN", &size);
  drawText(screen,
           &whiteFont15, 
           "CLICK TO BEGIN", 
           &pos[np], 
           (sw - size.w) / 2, 260);

  np++;
  drawText(screen,
           &whiteFont15, 
           "ESC - To Exit", 
           &pos[np], 
           (sw - size.w) / 2, 
           260 + (2 * whiteFont15.lineSkip));

  np++;
  drawText(screen,
           &whiteFont15, 
           "F1 - Pause Game", 
           &pos[np], 
           (sw - size.w) / 2,
           260 + (3 * whiteFont15.lineSkip));

  SDL_UpdateRects(screen, np + 1, pos);

  if (np >= posSize)
  {
    errorExit("Increase posSize you lazy...\n");
  }
}

void whoWon(SDL_Event *e)
{
  switch (e->type)
  {
    case SDL_USEREVENT:
      switch (e->user.code)
      {
      case MY_TIMEREVENT:
        now = SDL_GetTicks();
        dt = now - lastTime;
        if (dt >= minFrameTime)
        {
          drawWhoWon(dt);
          lastTime = now;
        }
        break;
      }
      break;

  case SDL_MOUSEBUTTONDOWN:
    initBounce(&redFont30);
    currentPage = bounce;
    break;
  }
}

//-------------------------------------------------

void initWelcome()
{
  earth.dx = randomRange(speedRange);
  earth.dy = randomRange(speedRange);

  earth.x = earth.ox = (sw / 2.0) - earth.w2;
  earth.y = earth.oy = (sh / 2.0) - earth.h2;

  moon.dx = randomRange(speedRange);
  moon.dy = randomRange(speedRange);

  moon.x = sw / 2.0;
  moon.y = sh - (moon.w * 3.0);

  sun.x = sw / 2;
  sun.y = sh - sun.h2;

  redWhite = false;
  flashTime = 200;

  SDL_FillRect(screen, NULL, bg);
  SDL_UpdateRect(screen, 0, 0, 0, 0);

  drawWelcome(0);
}

void drawWelcome(int dt)
{
  SDL_Rect size;
  int posSize = 20;
  SDL_Rect pos[posSize];
  int np = -1;

  flashTime -= dt;
  if (flashTime < 0)
  {
    redWhite = !redWhite;
    flashTime = 200;
  }

  // erase the earth
  np++;
  earth.ox = earth.x;
  earth.oy = earth.y;
  eraseMovable(&earth, &pos[np]);

  // erase the moon
  np++;
  moon.ox = moon.x;
  moon.oy = moon.y;
  eraseMovable(&moon, &pos[np]);

  // draw the earth
  earth.x += (earth.dx * dt) / 5.0;
  earth.dy += (dt / 200.0); // gravity
  earth.y += (earth.dy * dt) / 5.0;

  wallBounce(&earth);

  np++;
  drawMovable(&sun, &pos[np]);

  np++;
  drawMovable(&earth, &pos[np]);

  // draw the moon
  moon.x += (moon.dx * dt) / 5.0;
  moon.y += (moon.dy * dt) / 5.0;

  wallBounce(&moon);
  demoBounce();

  np++;
  drawMovable(&moon, &pos[np]);

  if (redWhite)
  {
    np++;
    textSize(&redFont100, "Bounce!", &size);
    drawText(screen,
             &redFont100, 
             "Bounce!", 
             &pos[np], 
             (sw - size.w) / 2, 100);
  }
  else
  {
    np++;
    textSize(&whiteFont100, "Bounce!", &size);
    drawText(screen,
             &whiteFont100, 
             "Bounce!", 
             &pos[np], 
             (sw - size.w) / 2, 100);
  }

  np++;
  textSize(&whiteFont15, "CLICK TO BEGIN", &size);
  drawText(screen,
           &whiteFont15, 
           "CLICK TO BEGIN", 
           &pos[np], 
           (sw - size.w) / 2, 260);

  np++;
  drawText(screen,
           &whiteFont15, 
           "ESC - To Exit", 
           &pos[np], 
           (sw - size.w) / 2, 
           260 + (2 * whiteFont15.lineSkip));

  np++;
  drawText(screen,
           &whiteFont15, 
           "F1 - Pause Game", 
           &pos[np], 
           (sw - size.w) / 2,
           260 + (3 * whiteFont15.lineSkip));

  SDL_UpdateRects(screen, np + 1, pos);

  if (np >= posSize)
  {
    errorExit("Increase posSize you lazy...\n");
  }
}

void welcome(SDL_Event *e)
{
  switch (e->type)
  {
    case SDL_USEREVENT:
      switch (e->user.code)
      {
      case MY_TIMEREVENT:
        now = SDL_GetTicks();
        dt = now - lastTime;
        if (dt >= minFrameTime)
        {
          drawWelcome(dt);
          lastTime = now;
        }
        break;
      }
      break;

  case SDL_MOUSEBUTTONDOWN:
    initBounce(&redFont30);
    currentPage = bounce;
    break;
  }
}

//-------------------------------------------------

Uint32 timerCallback(Uint32 interval, void *param)
{
  SDL_Event event;

  event.type = SDL_USEREVENT;
  event.user.code = MY_TIMEREVENT;
  event.user.data1 = (void *)SDL_GetTicks();
  event.user.data2 = (void *)0;

  SDL_PushEvent(&event);

  return interval;
}

//-------------------------------------------------

void loadImage(char *name, movable *m)
{
  SDL_Surface *s0, *s1;

  s0 = SDL_LoadBMP(name);
  if (NULL == s0)
  {
    errorExit("Can't load image file");
  }

  s1 = SDL_DisplayFormat(s0);
  if (NULL == s1)
  {
    errorExit("Can't convert image file");
  }
  SDL_SetColorKey(s1,
                  (SDL_SRCCOLORKEY | 
                   SDL_RLEACCEL), 
                  black);

  initMovable(m, s1);
  SDL_FreeSurface(s0);
}

void loadImages()
{
  loadImage("./earth50.bmp", &earth);
  loadImage("./moon25.bmp", &moon);
  loadImage("./sunslice.bmp", &sun);
}

//-------------------------------------------------

void loadFonts(SDL_PixelFormat *pf)
{
  loadFont(&redFont30, 
           "luxisr.ttf", 
           30, 
           TTF_STYLE_BOLD, 
           red, bg,
           pf);

  loadFont(&whiteFont30, 
           "luxisr.ttf", 
           30, 
           TTF_STYLE_BOLD, 
           white, bg,
           pf);

  loadFont(&redFont100, 
           "luxisr.ttf", 
           100, 
           TTF_STYLE_BOLD | TTF_STYLE_ITALIC, 
           red, bg,
           pf);

  loadFont(&whiteFont100, 
           "luxisr.ttf", 
           100, 
           TTF_STYLE_BOLD | TTF_STYLE_ITALIC, 
           white, bg,
           pf);

  loadFont(&redFont15, 
           "luxisr.ttf", 
           15, 
           TTF_STYLE_NORMAL, 
           red, bg,
           pf);

  loadFont(&whiteFont15, 
           "luxisr.ttf", 
           15, 
           TTF_STYLE_NORMAL, 
           white, bg,
           pf); 
}

//-------------------------------------------------

int main(int argc, char **argv)
{
  SDL_PixelFormat *pf = NULL;
  SDL_Event event;
  int options = SDL_ANYFORMAT;
  bool play = true;

  int i;
  for (i = 1; i < argc; i++)
  {
    if (0 == strcmp("-fullscreen", argv[i]))
    {
      options |= SDL_FULLSCREEN;
    }
  }

  if (-1 == SDL_Init((SDL_INIT_VIDEO |
                      SDL_INIT_TIMER |
                      SDL_INIT_EVENTTHREAD)))
  {
    errorExit("Can't initialize SDL");
  }
  atexit(SDL_Quit);

  screen = SDL_SetVideoMode(sw, sh, 
                            0,
                            options);

  if (NULL == screen)
  {
    errorExit("Can't set video mode");
  }

  SDL_WM_SetCaption("Bounce", "Bounce");

  pf = screen->format;
  initColors(pf);

  loadImages();
  loadFonts(pf);
  initWelcome();

  timer = SDL_AddTimer(10, timerCallback, NULL);

  lastTime = SDL_GetTicks();
  now = lastTime;
  dt = 0;

  currentPage = welcome;

  while ((!done) && SDL_WaitEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT:
      done = true;
      break;

    case SDL_KEYDOWN:
      switch(event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
        done = true;
        break;

      case SDLK_F1:
        play = !play;

        lastTime = SDL_GetTicks();
        now = lastTime;
        dt = 0;
        break;

      default:
        break;
      }
      break;
    }

    if (play && 
        (!done) &&
        (NULL != currentPage))
    {
      currentPage(&event);
    }
  }
    
  exit(0);
}
