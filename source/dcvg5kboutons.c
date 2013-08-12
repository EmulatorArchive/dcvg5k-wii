//////////////////////////////////////////////////////////////////////////////
// DCVG5KBOUTONS.C - dessin des boutons et des touches du clavier
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcvg5k.free.fr
// Created  : December 2007
// Last updated : 2008-01-05
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
//////////////////////////////////////////////////////////////////////////////

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h> 
#include <stdio.h>
#include <string.h>
#include "dcvg5kglobal.h"
#include "dcvg5kboutons.h"

//surfaces des boutons relaches et enfonces
SDL_Surface *buttonsurfaceup[BOUTON_MAX][LANGUAGE_MAX];
SDL_Surface *buttonsurfacedown[BOUTON_MAX][LANGUAGE_MAX];

extern SDL_Surface *screen;    //surface d'affichage de l'ecran
extern SDL_Surface *textbox;   //surface d'affichage de texte
extern TTF_Font *vera11;       //police d'ecriture taille 11
extern TTF_Font *vera9;        //police d'ecriture taille 9
extern SDL_Color textnoir;     //couleur d'ecriture noir  
extern char *msg[LANGUAGE_MAX][MSG_MAX];  //messages en plusieurs langues

extern void SDL_error(int n);                      

//Dessin signes previous/next sur une surface /////////////////////////////////
void Drawnext(SDL_Surface *s, int orientation)
{
 SDL_Rect r;
 r.x = 6; r.y = 3; r.w = 3; r.h = 1; SDL_FillRect(s, &r, noir);
 r.x += 1; r.y += orientation; r.w = 1; SDL_FillRect(s, &r, noir);
}

//Dessin d'une croix sur une surface //////////////////////////////////////////
void Drawx(SDL_Surface *s)
{
 SDL_Rect r;
 int i;
 r.x = 4; r.y = 3; r.w = 2; r.h =1;
 for(i = 0; i < 7; i++) {SDL_FillRect(s, &r, noir); r.x++; r.y++;}
 r.x = 10; r.y = 3;
 for(i = 0; i < 7; i++) {SDL_FillRect(s, &r, noir); r.x--; r.y++;}
}

//Dessin d'une fleche sur une surface /////////////////////////////////////////
void Drawarrow(SDL_Surface *s, int orientation)
{
 SDL_Rect r;
 Sint16 *x, *y;
 Uint16 *w, *h; 
 int i, j, k, x0, y0, sens;
 char arrow[] = {0, 0, 1, 0, 1, 1, 1, 1, 0};

 //orientation: 0=gauche 1=droite 2=haut 3=bas
 if(orientation & 1) {sens = -1; x0 = 14; y0 = 9;}
                else {sens = 1; x0 = 2; y0 = 9;}
 if(orientation & 2) {x = &r.y; y = &r.x; w = &r.h; h = &r.w; y0 += 2; x0 -= 1;}
                else {x = &r.x; y = &r.y; w = &r.w; h = &r.h; y0 -= 1; x0 += 2;}
 //dessin de la fleche
 k = 0; *w = 2; *h = 1;
 for(j = 0; j < 3; j++) for(i = 0; i < 3; i++)
 {
  if(arrow[k++] == 0) continue;
  *x = x0 + (2 + i + i) * sens;
  *y = y0 + (j - 3) * sens; SDL_FillRect(s, &r, gris3); 
  *y = y0 - (j - 3) * sens; SDL_FillRect(s, &r, gris3); 
 }      
 *x = x0 - 11 * (1 - sens) / 2; *y = y0; *w = 13; SDL_FillRect(s, &r, noir);
}

//Initialisation image de fond d'un bouton relache ////////////////////////////
void Initbuttonup(SDL_Surface *s, int w, int h)
{
 SDL_Rect rect;
 rect.x = 0; rect.y = 0; rect.w = w;  rect.h = h;
 SDL_FillRect(s, &rect, gris1); 
 rect.x = 1; rect.y = 1; rect.w = w - 1;  rect.h = h - 1;
 SDL_FillRect(s, &rect, noir); 
 rect.x = 0; rect.y = 0; rect.w = w - 1;  rect.h = h - 1;
 SDL_FillRect(s, &rect, blanc); 
 rect.x = 1; rect.y = 1; rect.w = w - 2; rect.h = h - 2;
 SDL_FillRect(s, &rect, gris2); 
 rect.x = 1; rect.y = 1; rect.w = w - 3; rect.h = h - 3;
 SDL_FillRect(s, &rect, gris1);
}     

//Initialisation image de fond d'un bouton enfonce ////////////////////////////
void Initbuttondown(SDL_Surface *s, int w, int h)
{
 SDL_Rect rect;
 rect.x = 0; rect.y = 0; rect.w = w;  rect.h = h;
 SDL_FillRect(s, &rect, noir); 
 rect.x = 1; rect.y = 1; rect.w = w - 1; rect.h = h - 1;
 SDL_FillRect(s, &rect, gris2); 
 rect.x = 2; rect.y = 2; rect.w = w - 2;  rect.h = h - 2;
 SDL_FillRect(s, &rect, blanc); 
 rect.x = 2; rect.y = 2; rect.w = w - 3; rect.h = h - 3;
 SDL_FillRect(s, &rect, gris1);
}     

//Arrondir les coins des touches relachees ////////////////////////////////////
void Roundcornerup(SDL_Surface *s, int w, int h)
{
 SDL_Rect rect;
 rect.w = 1;  rect.h = 1;
 rect.x = 0;     rect.y = 0;     SDL_FillRect(s, &rect, gris0); 
 rect.x = w - 2; rect.y = 0;     SDL_FillRect(s, &rect, gris0);
 rect.x = w - 1; rect.y = 0;     SDL_FillRect(s, &rect, gris0);
 rect.x = 1;     rect.y = 1;     SDL_FillRect(s, &rect, blanc);
 rect.x = w - 3; rect.y = 1;     SDL_FillRect(s, &rect, blanc);
 rect.x = w - 1; rect.y = 1;     SDL_FillRect(s, &rect, gris0);
 rect.x = 1;     rect.y = h - 3; SDL_FillRect(s, &rect, blanc);
 rect.x = w - 3; rect.y = h - 3; SDL_FillRect(s, &rect, gris2);
 rect.x = 0;     rect.y = h - 2; SDL_FillRect(s, &rect, gris0);
 rect.x = w - 2; rect.y = h - 2; SDL_FillRect(s, &rect, noir);
 rect.x = 0;     rect.y = h - 1; SDL_FillRect(s, &rect, gris0);
 rect.x = 1;     rect.y = h - 1; SDL_FillRect(s, &rect, gris0);
 rect.x = w - 1; rect.y = h - 1; SDL_FillRect(s, &rect, gris0);
}     

//Arrondir les coins des touches enfoncees ////////////////////////////////////
void Roundcornerdown(SDL_Surface *s, int w, int h)
{
 SDL_Rect rect;
 rect.w = 1;  rect.h = 1;
 rect.x = 0;     rect.y = 0;     SDL_FillRect(s, &rect, gris0); 
 rect.x = w - 1; rect.y = 0;     SDL_FillRect(s, &rect, gris0); 
 rect.x = 1;     rect.y = 1;     SDL_FillRect(s, &rect, noir); 
 rect.x = 2;     rect.y = 2;     SDL_FillRect(s, &rect, gris2); 
 rect.x = w - 2; rect.y = h - 2; SDL_FillRect(s, &rect, blanc); 
 rect.x = 0;     rect.y = h - 1; SDL_FillRect(s, &rect, gris0); 
 rect.x = w - 1; rect.y = h - 1; SDL_FillRect(s, &rect, gris0); 
}     

//Initialisation des surfaces des boutons ////////////////////////////////////
void Initbuttonsurfaces()
{
 SDL_Rect rect;
 int i, j, w, h;
 const char *string; 
 for(i = 0; i < BOUTON_MAX; i++)
 {
  if(bouton[i].name[0] == 0) continue; //bouton non créé 
  w = bouton[i].w;
  h = bouton[i].h;
  //creation du bouton dans chaque langue
  for(j = 0; j < LANGUAGE_MAX; j++)
  {
   string = bouton[i].name;
   //boutons non traduits     
   if(j > 0) if(string[0] != '{')
   {
    buttonsurfaceup[i][j] = buttonsurfaceup[i][0];
    buttonsurfacedown[i][j] = buttonsurfacedown[i][0];
    continue;
   }
   //creation de la surface et du fond
   buttonsurfaceup[i][j] = SDL_CreateRGBSurface
    (FLAGS, w, h, 32, rmask, gmask, bmask, amask);
   buttonsurfacedown[i][j] = SDL_CreateRGBSurface
    (FLAGS, w, h, 32, rmask, gmask, bmask, amask);
   Initbuttonup(buttonsurfaceup[i][j], w, h);
   if(i > 18) Roundcornerup(buttonsurfaceup[i][j], w, h);
   Initbuttondown(buttonsurfacedown[i][j], w, h);
   if(i > 18) Roundcornerdown(buttonsurfacedown[i][j], w, h);
   //ajout d'un bitmap
   if(string[0] == '[')
   {
    switch(string[1])
    {
     case 'p': Drawnext(buttonsurfaceup[i][j], -1);                                           
               Drawnext(buttonsurfacedown[i][j], -1); break;                                           
     case 'n': Drawnext(buttonsurfaceup[i][j], 1);                                           
               Drawnext(buttonsurfacedown[i][j], 1); break;                                           
     case 'h': Drawarrow(buttonsurfaceup[i][j], 2);                                           
               Drawarrow(buttonsurfacedown[i][j], 2); break;                                           
     case 'b': Drawarrow(buttonsurfaceup[i][j], 3);                                           
               Drawarrow(buttonsurfacedown[i][j], 3); break;                                           
     case 'g': Drawarrow(buttonsurfaceup[i][j], 0);                                           
               Drawarrow(buttonsurfacedown[i][j], 0); break;                                           
     case 'd': Drawarrow(buttonsurfaceup[i][j], 1);                                           
               Drawarrow(buttonsurfacedown[i][j], 1); break;                                           
     default : Drawx(buttonsurfaceup[i][j]);                                           
               Drawx(buttonsurfacedown[i][j]); break;                                           
    }                                            
    continue;                     
   } 
   //recherche du texte traduit en plusieurs langues
   if(string[0] == '{')
   {
    int imsg;                   
    imsg = strtol(string + 1, NULL, 10);
    if(imsg < 0) imsg = 0;
    if(imsg > MSG_MAX) imsg = 0;
    string = msg[j][imsg];
   } 
   //creation de l'image du texte
   SDL_FreeSurface(textbox);
   rect.x = (i > 19) ? 3 : 4;  //3 pour les touches, 4 pour les autres
   rect.y = (i > 19) ? 2 : 1;  //2 pour les touches, 1 pour les autres
   textbox = TTF_RenderText_Blended((i > 19) ? vera9 : vera11, string, textnoir);
   if(textbox == NULL) {SDL_error(25); return;}
   //application du texte sur la surface du bouton 
   if(SDL_BlitSurface(textbox, NULL, buttonsurfaceup[i][j], &rect) < 0) SDL_error(26);
   if(SDL_BlitSurface(textbox, NULL, buttonsurfacedown[i][j], &rect) < 0) SDL_error(27);
  }
 }
}     
