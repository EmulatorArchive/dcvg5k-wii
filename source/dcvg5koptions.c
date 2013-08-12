//////////////////////////////////////////////////////////////////////////////
// DCVG5KOPTIONS.C - Option setting, save & restore
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
//////////////////////////////////////////////////////////////////////////////

#include <SDL/SDL.h>
#include <stdio.h>
#include <string.h>
#include "dcvg5kglobal.h"
#include <fat.h>

FILE *fpi;        //fichier dcvg5k.ini   
int language;     //0=francais 1=anglais
int frequency;    //frequence z80 en kHz
float echelle;    //zoom

extern SDL_Surface *dialogbox;
extern SDL_Surface *buttonsurfaceup[][LANGUAGE_MAX];
extern SDL_Surface *buttonsurfacedown[][LANGUAGE_MAX];
extern int xclient, yclient; //, vblnumbermax;
extern int k7protection; //protection k7 (0=lecture/ecriture 1=lecture seule)
extern char *msg[LANGUAGE_MAX][MSG_MAX]; //messages en plusieurs langues
extern int rmask, gmask, bmask, amask;
extern button bouton[];
extern const dialogbutton optionbutton[];

extern void Drawtextbox(SDL_Surface *surf, char *string, SDL_Rect rect, int c);

//Draw option box ////////////////////////////////////////////////////////////
void Drawoptionbox()
{
 SDL_Rect rect;
 int i;
 char string[50];
 extern int dialog;
 dialog = 2;     
 //titre
 rect.x = 10; rect.w = dialogbox->w - 32;
 rect.y = 5; rect.h = 15;
 Drawtextbox(dialogbox, msg[language][29], rect, 13);
 //options
 rect.x = 10; rect.w = 194;
 rect.y = 32; rect.h = 15;
 Drawtextbox(dialogbox, msg[language][27], rect, 1); 
 sprintf(string, "Zoom %1.2f", echelle);
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 1); 
 sprintf(string, "%s %i%s", msg[language][28], frequency / 10, "%");
 rect.y += 20; 
 Drawtextbox(dialogbox, string, rect, 1); 
 sprintf(string, "k7 %s", msg[language][(k7protection) ? 42 : 43]);
 rect.y += 20;
 Drawtextbox(dialogbox, string, rect, 1); 
 //dessin des boutons
 for(i = 0; i < OPTIONBUTTON_MAX; i++)
 {
  rect.x = optionbutton[i].x; rect.y = optionbutton[i].y;
  SDL_BlitSurface(buttonsurfaceup[optionbutton[i].n][language], NULL, dialogbox, &rect);
 }
}

//Initialisation des valeurs par defaut //////////////////////////////////////
void Initdefault()
{
 echelle = 1.5f;
 language = 0;            //francais
 xclient = (int)(1.5f * (float)XBITMAP40 );
 yclient = (int)(1.5f * (float)YBITMAP );
 frequency = 1000;        //4 MHz
 k7protection = 1;        //protection cassette 
}

//Traitement des clics des boutons d'options /////////////////////////////////
void Optionclick()
{
 SDL_Rect rect;
 int i, x, y, step;    
 extern int xmouse, ymouse;
 extern int dialog;
 extern SDL_Rect dialogrect; 
 extern void Displayscreen();
 extern void Resizescreen(int x, int y);

 //recherche du bouton du clic
 for(i = 0; i < OPTIONBUTTON_MAX; i++)
 {
  x = dialogrect.x + optionbutton[i].x;     
  y = dialogrect.y + optionbutton[i].y;     
  if(xmouse > x) if(xmouse < (x + bouton[optionbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[optionbutton[i].n].h))
  break;
 }
 if(i >= OPTIONBUTTON_MAX) return;
 //dessin du bouton enfonce
 rect.x = optionbutton[i].x; rect.y = optionbutton[i].y;
 SDL_BlitSurface(buttonsurfacedown[optionbutton[i].n][language], NULL, dialogbox, &rect);
 Displayscreen(); SDL_Delay(100);
 //traitement en fonction du bouton           
 switch(i)
 {
  case 0: if(--language < 0) language = LANGUAGE_MAX - 1; //previous language
          SDL_WM_SetCaption(msg[language][0], NULL);
          break;
  case 1: if(++language >= LANGUAGE_MAX) language = 0;  //next language 
          SDL_WM_SetCaption(msg[language][0], NULL);
          break;
  case 2: echelle += 0.1f; if(echelle > 1.9f) echelle = 1.9f; //zoom in
          Resizescreen((int)(XBITMAP40 * echelle), YSTATUS + (int)(YBITMAP * echelle));
          break;
  case 3: echelle -= 0.1f; if(echelle < 1.0f) echelle = 1.0f; //zoom out
          Resizescreen((int)(XBITMAP40 * echelle), YSTATUS + (int)(YBITMAP * echelle));
          break;
  case 4: step = 100; frequency = frequency * 10 / 10; //faster
          if(frequency > 1999) step = 1000; 
          if(frequency < 9999 - step) frequency += step;
          break;
  case 5: step = 100; frequency = frequency * 10 / 10; //slower
          if(frequency > 2000) step = 1000; 
          if(frequency > 9 + step) frequency -= step;
          break;
  case 6: k7protection = 1 - k7protection; //unprotect k7
          break;
  case 7: k7protection = 1 - k7protection; //protect k7
          break;
  case 8: Initdefault();    //restore default values
          //Restorewindow(); //lire les commentaires de cette fonction
          Resizescreen(xclient, YSTATUS + yclient);
          break;
 } 
 if(dialog == 2) Drawoptionbox();
}

//Option setting /////////////////////////////////////////////////////////////
void Options()
{
 extern void Createdialogbox(int w, int h);
 Createdialogbox(236, 156);
 Drawoptionbox();
}     

//Option initialization //////////////////////////////////////////////////////
void Initoptions()
{
 char string[256];    
 Initdefault();
 //ouverture fichier dcvg5k.ini
 fpi = fopen("sd:/apps/dcvg5k/dcvg5k.ini", "r+");                 //s'il existe ouverture
 if(fpi == NULL) fpi = fopen("sd:/apps/dcvg5k/dcvg5k.ini", "w+"); //s'il n'existe pas : creation
 fread(string, 12, 1, fpi);                        //lecture identifiant  
 if(strcmp("dcvg5kini-2", string) != 0) return;
 //initialisation des options
 fread(&language, 4, 1, fpi);  //langue 
 fread(&frequency, 4, 1, fpi); //frequence z80
 fread(&xclient, 4, 1, fpi);   //largeur ecran
 fread(&yclient, 4, 1, fpi);   //hauteur ecran
 fread(&k7protection, 4, 1, fpi); 
 //controle
 if(language < 0) language = 0;
 if(language > (LANGUAGE_MAX - 1)) language = LANGUAGE_MAX - 1;
 if(frequency < 100) frequency = 100;
 if(frequency > 9000) frequency = 9000;
 if(xclient < (XBITMAP40 / 2)) xclient = XBITMAP40 / 2; 
 if(xclient > (2 * XBITMAP40)) xclient = 2 * XBITMAP40; 
 if(yclient < YBITMAP) yclient = YBITMAP; 
 if(yclient > (4 * YBITMAP)) yclient = 4 * YBITMAP;
 if(k7protection) k7protection = 1; 
} 

//Option save ////////////////////////////////////////////////////////////////
void Saveoptions()
{
 int i = 0;    
 fseek(fpi, 0, SEEK_SET);
 fwrite("dcvg5kini-2", 12, 1, fpi);
 fwrite(&language, 4, 1, fpi);  //langue 
 fwrite(&frequency, 4, 1, fpi); //frequence z80
 fwrite(&xclient, 4, 1, fpi);   //largeur ecran
 fwrite(&yclient, 4, 1, fpi);   //hauteur ecran
 fwrite(&k7protection, 4, 1, fpi); 
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fwrite(&i, 4, 1, fpi);         //reserve
 fclose(fpi); 
} 
