//////////////////////////////////////////////////////////////////////////////
// DCVG5KDESASS.C - boîte de dialogue pour le desassembleur
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcvg5k.free.fr
// Created  : December 2007
// Last updated : 2008-01-09
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

TTF_Font *mono10 = NULL;   //police d'ecriture mono taille 9
int startaddress = 0xf000; //adresse de debut du desassemblage
int nextaddress;           //adresse de la deuxieme instruction desassemblee
char string[256];    

extern SDL_Surface *dialogbox; //surface d'affichage dialogbox
extern SDL_Surface *buttonsurfaceup[][LANGUAGE_MAX];
extern SDL_Surface *buttonsurfacedown[][LANGUAGE_MAX];
extern int language;
extern int dialog;             //0 ou n°boite de dialogue affichee
extern char *msg[LANGUAGE_MAX][MSG_MAX]; //messages en plusieurs langues
extern const int blanc;
extern const SDL_Color textnoir;
extern SDL_Surface *textbox;
extern button bouton[];
extern const dialogbutton desassbutton[];

extern void Drawtextbox(SDL_Surface *surf, char *string, SDL_Rect rect, int c);
extern void SDL_error(int n);

//Affichage d'une ligne desassemblee /////////////////////////////////////////
void Drawdesass(SDL_Rect rect)
{
 SDL_Rect textrect;    
 int r;
 TTF_SetFontStyle(mono10, TTF_STYLE_NORMAL);
 SDL_FillRect(dialogbox, &rect, blanc);
 SDL_FreeSurface(textbox);
 textbox = TTF_RenderText_Blended(mono10, string, textnoir);
 if(textbox == NULL) {SDL_error(61); return;}
 textrect.x = -2; textrect.w = rect.w - 4;
 textrect.y = 0; textrect.h = rect.h;
 r = SDL_BlitSurface(textbox, &textrect, dialogbox, &rect);
 if(r < 0) SDL_error(62);
}

//Affichage du desassemblage a l'adresse startaddress /////////////////////////
void Displaydesass(int x)
{
 SDL_Rect rect; 
 SDL_Rect textrect;    
 int i, j, n, r;
 char instruction[80];
 char *operande;
 extern int DAsm(char *string, char *A);
 TTF_SetFontStyle(mono10, TTF_STYLE_NORMAL);
 //adresse
 rect.x = 10; rect.y = 40; rect.w = 70; rect.h = 13;
 SDL_FillRect(dialogbox, &rect, blanc);
 sprintf(string, "%1x  %1x  %1x  %1x",
        (startaddress & 0xf000) >> 12, (startaddress & 0x0f00) >> 8,
        (startaddress & 0x00f0) >> 4, (startaddress & 0x000f)); 
 SDL_FreeSurface(textbox);
 textbox = TTF_RenderText_Blended(mono10, string, textnoir);
 if(textbox == NULL) {SDL_error(63); return;}
 textrect.x = -4; textrect.w = rect.w - 4;
 textrect.y = 0; textrect.h = rect.h;
 r = SDL_BlitSurface(textbox, &textrect, dialogbox, &rect);
 if(r < 0) SDL_error(64);
 //desassemblage
 rect.x = 90; rect.y = 15; rect.w = 300; rect.h = 15;
 for(i = 0; i < 20; i++)
 {
  extern char rom[], ram[];
  extern char RdZ80(short w);
  rect.y += 15;     
  sprintf(string, "%04x  ", x);
  n = DAsm(instruction, (x < 0x4000) ? rom + x : ram + x - 0x4000);
  for(j = 0; j < 5; j++)
  {
   if(j < n) sprintf(string + 6 + 2 * j, "%02x", RdZ80(x++) & 0xff);
   else strcat(string, "  ");
  }
  j = 0;
  while(instruction[++j] > 0x30);
  operande = instruction + j;
  if(operande[0]) operande++;
  instruction[j] = 0;
  strcat(string, instruction);  
  while(j++ < 6) strcat(string," ");
  strcat(string, operande);
  Drawdesass(rect); 
  if(i == 0) nextaddress = x;     
 }
}

//Création de la boite de dialogue du desassembleur ///////////////////////////
void Desass()
{
 SDL_Rect rect;
 int i, n;
 extern int pausez80;
 extern void Createdialogbox(int w, int h);
 pausez80 = 1;
 Createdialogbox(405, 345);
 //titre
 rect.x = 10; rect.w = dialogbox->w - 32;
 rect.y = 5; rect.h = 15;
 Drawtextbox(dialogbox, msg[language][32], rect, 13);
 //dessin des boutons
 for(i = 0; i < DESASSBUTTON_MAX; i++)
 {
  n = desassbutton[i].n;
  rect.x = desassbutton[i].x;
  rect.y = desassbutton[i].y;
  SDL_BlitSurface(buttonsurfaceup[n][language], NULL, dialogbox, &rect);
 }
 //affichage du desassemblage
 Displaydesass(startaddress);
 dialog = 5;     
 pausez80 = 0;
}     

//Traitement des clics des boutons de desassemblage ///////////////////////////
void Desassclick()
{
 SDL_Rect rect;
 int i, n, x, y;    
 extern int xmouse, ymouse;
 extern int dialog;
 extern SDL_Rect dialogrect; 
 extern void Displayscreen();

 //recherche du bouton du clic
 for(i = 0; i < DESASSBUTTON_MAX; i++)
 {
  x = dialogrect.x + desassbutton[i].x;     
  y = dialogrect.y + desassbutton[i].y;     
  if(xmouse > x) if(xmouse < (x + bouton[desassbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[desassbutton[i].n].h))
  break;
 }
 if(i >= DESASSBUTTON_MAX) return;
 //dessin du bouton enfonce
 n = desassbutton[i].n;
 rect.x = desassbutton[i].x;
 rect.y = desassbutton[i].y;
 SDL_BlitSurface(buttonsurfacedown[n][language], NULL, dialogbox, &rect);
 Displayscreen();
 //traitement en fonction du bouton
 switch(i)
 {
  case  0: x = startaddress = (startaddress + 0x1000) & 0xffff; break;
  case  1: x = startaddress = (startaddress - 0x1000) & 0xffff; break;
  case  2: x = startaddress = (startaddress + 0x100) & 0xffff; break;
  case  3: x = startaddress = (startaddress - 0x100) & 0xffff; break;
  case  4: x = startaddress = (startaddress + 0x10) & 0xffff; break;
  case  5: x = startaddress = (startaddress - 0x10) & 0xffff; break;
  case  6: x = startaddress = (startaddress + 0x1) & 0xffff; break;
  case  7: x = startaddress = (startaddress - 0x1) & 0xffff; break;
  case  8: x = startaddress; break;
  case  9: x = nextaddress; break;
 } 
 //dessin du bouton relache
 Displaydesass(x); SDL_Delay(200);
 SDL_BlitSurface(buttonsurfaceup[n][language], NULL, dialogbox, &rect);
}
