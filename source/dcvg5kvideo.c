///////////////////////////////////////////////////////////////////////////////
// DCVG5KVIDEO.C - Fonctions d'affichage pour dcvg5k
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcvg5k.free.fr
// Created  : December 2007
// Last updated : 2008-01-11
//
// This file is part of DCVG5K v2.
// 
// DCVG5K v2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DCVG5K v2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DCVG5K v2.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////

#include <SDL/SDL.h>
#include <string.h>
#include "dcvg5kglobal.h"

#define VIDEO_MODE SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE

// global variables //////////////////////////////////////////////////////////
SDL_Surface *ecran = NULL;
SDL_Surface *screen = NULL;    //surface d'affichage de l'ecran
int xclient;                   //largeur fenetre utilisateur 
int yclient;                   //hauteur ecran MO5 dans fenetre utilisateur
SDL_Rect bien_centre;          // centrage de la fenetre utilisateur
struct pix {char b, g, r, a;}; //structure pixel BGRA
struct pix pcolor[20];         //couleurs BGRA de la palette
int currentvideomemory;        //index octet courant en memoire video thomson
int currentlinesegment;        //numero de l'octet courant dans la ligne video
int xpixel40[XBITMAP40 + 1];   //abscisses des pixels en mode 40 colonnes
int xpixel80[XBITMAP80 + 1];   //abscisses des pixels en mode 80 colonnes
int ypixel[YBITMAP + 1];       //ordonnees des pixels pour tous les modes
int *pcurrentpixel;            //pointeur ecran : pixel courant
int *pcurrentline;             //pointeur ecran : debut ligne courante
int *pmin;                     //pointeur ecran : premier pixel
int *pmax;                     //pointeur ecran : dernier pixel + 1
int screencount;               //nbre ecrans affiches entre 2 affichages status

//definition des intensites pour correction gamma
const int g[16]={0,60,90,110,130,148,165,180,193,205,215,225,230,235,240,255};

extern char *pagevideo;

extern void Drawstatusbar();
extern void SDL_error(int n);

// Modification de la palette ////////////////////////////////////////////////
void Palette(int n, int r, int v, int b)
{
 pcolor[n].a = g[b];
 pcolor[n].r = g[v];
 pcolor[n].g = g[r];
 pcolor[n].b = 0xff;
}

//Display screen /////////////////////////////////////////////////////////////
void Displayscreen()
{
 extern SDL_Surface *dialogbox;
 extern SDL_Surface *statusbar;
 extern SDL_Rect dialogrect;  
 extern int dialog;
 
 if(dialog > 0)
   if(SDL_BlitSurface(dialogbox, NULL, screen, &dialogrect) < 0) SDL_error(31);
 if(--screencount < 0)  //1 fois sur 10 pour diminuer le temps d'affichage
 {
  if(statusbar != NULL)
  if(SDL_BlitSurface(statusbar, NULL, screen, NULL) < 0) SDL_error(32);
  screencount = 9;
 }  
 
 bien_centre.x = 320 - xclient / 2;
 bien_centre.y = 240 - (yclient + YSTATUS) / 2;
 SDL_BlitSurface(screen, NULL, ecran, &bien_centre);
 if(SDL_Flip(ecran) < 0) SDL_error(33); 

}     

// Affichage d'un caractere en mode 40 caracteres par ligne ///////////////////
void Displaychar40(char *c, int x, int y)
{
 int i, j;
 int nsegment, nligne, x0, w;   
 int *p0, *p1, *p2, *p;
 nligne = 10 * y;
 p0 = pmin + ypixel[nligne] * xclient;   //pointeur debut de la ligne courante
 nsegment = 8 * x;                       //numero du segment courant
 x0 = xpixel40[nsegment];                //position debut segment dans la ligne
 w = 4 * (xpixel40[nsegment + 8] - x0);  //largeur du caractere en octets     
 if(SDL_MUSTLOCK(screen))
   if(SDL_LockSurface(screen) < 0) {SDL_error(35); return;}
 for(i = 0; i < 10; i++)
 {
  p = p0 + x0;                           //pointeur segment courant
  nsegment = 8 * x;                      //numero du segment courant
  for(j = 0; j < 8; j++)
  {
   p1 = p0 + xpixel40[++nsegment];       //pointeur debut du segment suivant     
   while(p < p1) memcpy(p++, pcolor + c[8 * i + j], 4);
  }
  p0 += xclient;
  p2 = pmin + xclient * ypixel[++nligne]; //pointeur debut de la ligne suivante
  while(p0 < p2)
  {
   memcpy(p0 + x0, p0 + x0 - xclient, w);
   p0 += xclient;
  }
 }
 SDL_UnlockSurface(screen);
}

// Affichage d'un caractere en mode 80 caracteres par ligne ///////////////////
void Displaychar80(char *c, int x, int y)
{
 int *p;   
 int i, j, k = 0;
 p = pmin + 10080 * y + 12 * x; //2+80+2=84 caracteres de 12 pixels
 for(i = 0; i < 10; i++)
 {
  for(j = 0; j < 6; j++)
  {
   memcpy(p++, pcolor + c[k], 4);
   memcpy(p++, pcolor + c[k], 4);
   k++;
  }
  p += 996; //ligne suivante : sauter 83 caracteres de 12 pixels
 }
}

// Resize screen //////////////////////////////////////////////////////////////
void Resizescreen(int x, int y)
{
 int i, savepausez80;    
 extern int pausez80;
 extern int dialog;
 extern void Drawoptionbox();
 extern void Displayline(int n);
 savepausez80 = pausez80;
 pausez80 = 1; //SDL_Delay(200);
 //effacement surface de l'ecran
 if(screen != NULL)
 {
  pmin = (int*)(screen->pixels);
  pmax = pmin + screen->w * screen->h;
  for(pcurrentpixel = pmin; pcurrentpixel < pmax; pcurrentpixel++)
      memcpy(pcurrentpixel, pcolor, 4);
  SDL_Flip(ecran);
 } 
 //creation nouvelle surface
 SDL_FreeSurface(screen);
 y -= YSTATUS;	
 xclient = (x < 336) ? 336 : x;    
 yclient = (y < 216) ? 216 : y;
 ecran = SDL_SetVideoMode(640, 480, 32, VIDEO_MODE);
 screen = SDL_CreateRGBSurface(VIDEO_MODE, xclient, yclient + YSTATUS, 32, 0, 0, 0, 0);
 
 if(screen == NULL || ecran == NULL)
 {
  SDL_WM_SetCaption(" Erreur fatale : Mode video non compatible", NULL);
  SDL_error(37);
 }
 pmin = (int*)(screen->pixels) + YSTATUS * xclient;
 pmax = pmin + yclient * xclient;
 //calcul des abscisses des pixels en mode 40 et 80 colonnes
 for(i = 0; i <= XBITMAP40; i++) xpixel40[i] = i * xclient / XBITMAP40;
 for(i = 0; i <= XBITMAP80; i++) xpixel80[i] = i * xclient / XBITMAP80;
 for(i = 0; i <= YBITMAP; i++) ypixel[i] = i * yclient / YBITMAP;
 //rafraichissement de l'écran
 for(i = 0; i < 24; i++) Displayline(i);
 Drawstatusbar();
 if(dialog == 2) Drawoptionbox();
 screencount = 0;
 Displayscreen();
 pausez80 = savepausez80;
}     
