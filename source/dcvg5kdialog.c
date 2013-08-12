///////////////////////////////////////////////////////////////////////////////
// DCVG5KDIALOG.C - Fonctions d'interface utilisateur
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
#include <SDL/SDL_ttf.h> 
#include <string.h>
#include <dirent.h>
#include <math.h>
#include "dcvg5kglobal.h"

#define DIRLIST_LENGTH 32
#define DIRLIST_NMAX 500

//constantes
const SDL_Color textnoir = {0, 0, 0, 255};        //couleur d'ecriture noir  
const SDL_Color textblanc = {255, 255, 255, 255}; //couleur d'ecriture blanc   

//global variables ///////////////////////////////////////////////////////////
SDL_Surface *textbox = NULL;   //surface d'affichage de texte
SDL_Surface *dialogbox = NULL; //surface d'affichage dialogbox
SDL_Surface *statusbar = NULL; //surface de la barre de statut
TTF_Font *vera9 = NULL;        //police d'ecriture taille 9
TTF_Font *vera11 = NULL;       //police d'ecriture taille 11
SDL_Rect dialogrect;           //position dialogbox 
int dialog = 0;                //0=rien 1=message, 2=options 3=clavier 4=menu
int xmouse, ymouse;            //position souris dans fenetre utilisateur
int xmove, ymove;              //position souris dans dialogbox deplacee
int dircount;                  //nombre de fichiers dans le repertoire 
char dirlist[DIRLIST_NMAX][DIRLIST_LENGTH]; //liste des fichiers du repertoire
int dirmin, dirmax;            //plage de numeros de fichiers affiches 

void SDL_error(int n);

//external
extern SDL_Surface *buttonsurfaceup[][LANGUAGE_MAX]; 
extern SDL_Surface *buttonsurfacedown[][LANGUAGE_MAX]; 
extern SDL_Surface *screen;    //surface d'affichage de l'ecran
extern int xclient, yclient;   //taille ecran affiche
extern char *msg[LANGUAGE_MAX][MSG_MAX];  //messages en plusieurs langues
extern int language;
extern int pausez80;
extern button bouton[];
extern int rmask, gmask, bmask, amask;
extern int blanc, noir, bleu, grist, gris0, gris1, gris2;
extern const dialogbutton statusbutton[];
extern const dialogbutton closebutton;

//Tri du repertoire path avec filtre sur l'extension //////////////////////////
void Sortdirectory(char *path, char *extension)
{
 DIR *dir;
 int i, j;   
 struct dirent *entry;
 dircount = 0;
 if ((dir = opendir(path)) == NULL) return;
 while((entry = readdir(dir)))
 {
  strncpy(dirlist[dircount], entry->d_name, DIRLIST_LENGTH);
  dirlist[dircount][DIRLIST_LENGTH - 1] = 0;
  i = strlen(dirlist[dircount]); 
  j = strlen(extension); 
  if(i < j) continue;
  if(strcasecmp(dirlist[dircount] + i - j, extension)) continue; 
  if(++dircount >= DIRLIST_NMAX) break;
 }
 closedir(dir);
 //strcpy(dirlist[dircount++], msg[language][39]); // [décharger]
 qsort(dirlist, dircount, DIRLIST_LENGTH,
 (int(*)(const void*, const void*))strcmp);
}     

//Draw textbox ///////////////////////////////////////////////////////////////
void Drawtextbox(SDL_Surface *surf, char *string, SDL_Rect rect, int c)
{
 //c = couleur du fond et graisse
 //normal : O=gris, 1=blanc, 2=noir, 3=bleu, 4=transparent     
 //bold :  10=gris, 11=blanc, 12=noir, 13=bleu, 14=transparent      
 SDL_Rect textrect;    
 SDL_Color textcolor;
 int r, boxcolor;
 TTF_SetFontStyle(vera11, TTF_STYLE_NORMAL);
 if(c > 9) {c -= 10; TTF_SetFontStyle(vera11, TTF_STYLE_BOLD);}
 switch(c)
 {
  case 0: boxcolor = gris0; textcolor = textnoir; break;
  case 1: boxcolor = blanc; textcolor = textnoir; break;
  case 2: boxcolor = noir; textcolor = textblanc; break;
  case 3: boxcolor = bleu; textcolor = textblanc; break;
  case 4: boxcolor = bleu; textcolor = textnoir; break;
  default: return;
 }         
 if(c != 4) SDL_FillRect(surf, &rect, boxcolor);
 if(string[0] == 0) return;
 SDL_FreeSurface(textbox);
 textbox = TTF_RenderText_Blended(vera11, string, textcolor);
 if(textbox == NULL) {SDL_error(11); return;}
 textrect.x = -2; textrect.w = rect.w - 4;
 textrect.y = 0; textrect.h = rect.h;
 r = SDL_BlitSurface(textbox, &textrect, surf, &rect);
 if(r < 0) SDL_error(12);
}     
	
//Draw message box ////////////////////////////////////////////////////////////
void Drawmessagebox(char *titre, char *text1[], char *text2[])
//text1 est affiché sur fond gris et text2 sur fond blanc
//le dernier des deux textes doit etre une chaine de longueur nulle
{
 SDL_Rect rect;
 int i;
 dialog = 1;  
 //titre
 rect.x = 10; rect.w = dialogbox->w - 32;
 rect.y = 5; rect.h = 15;
 Drawtextbox(dialogbox, titre, rect, 13);
 //texte sur fond gris 
 rect.x = 10; rect.w = dialogbox->w - 20;
 rect.y += 26;
 i = 0;
 while(text1[i][0] != 0)
 {
  Drawtextbox(dialogbox, text1[i], rect, 4);
  rect.y += 13; i++;
 }
 rect.y += 13;
 i = 0; 
 while(text2[i][0] != 0)
 {
  Drawtextbox(dialogbox, text2[i], rect, 1);                   
  rect.y += 13; i++;
 }
}

//Index cassette /////////////////////////////////////////////////////////////
void Drawk7index()
{
 SDL_Rect rect;    
 char index[10];
 extern FILE * fk7;
 extern int k7index, k7indexmax;
 index[0] = 0;
 if(fk7 != NULL) sprintf(index, "%03d/%03d", k7index, k7indexmax);
 rect.x = 25; rect.w = 52; rect.y = 2; rect.h = 15;
 Drawtextbox(statusbar, index, rect, 1);
}

//Draw status bar ////////////////////////////////////////////////////////////
void Drawstatusbar()
{
 SDL_Rect rect;    
 SDL_Surface *surf;    
 int i, r;
 extern char k7name[]; 
 
 if(statusbar == NULL) return;
 //fond
 rect.x = 0; rect.w = xclient; rect.y = 0; rect.h = YSTATUS - 1;
 SDL_FillRect(statusbar, &rect, gris0);
 rect.y = YSTATUS - 1; rect.h = 1;
 SDL_FillRect(statusbar, &rect, blanc);

 //index et nom de la cassette
 Drawk7index();
 rect.x = 80; rect.w = 200; rect.y = 2; rect.h = 15;  
 Drawtextbox(statusbar, k7name, rect, 1); 

 //boutons relaches
 for(i = 0; i < STATUSBUTTON_MAX; i++)
 {
  rect.x = statusbutton[i].x; if(rect.x < 0) rect.x += xclient;
  rect.y = statusbutton[i].y; if(rect.y < 0) rect.y += yclient;
  //Info(rect.x); SDL_Delay(1000);
  surf = buttonsurfaceup[statusbutton[i].n][language];
  r = SDL_BlitSurface(surf, NULL, statusbar, &rect);
  if(r < 0) {SDL_error(13); return;}
 }
}     

//Init status bar ////////////////////////////////////////////////////////////
void Initstatusbar()
{
 statusbar = SDL_CreateRGBSurface(FLAGS, 2048, YSTATUS, 32, rmask, gmask, bmask, amask);
 if(statusbar == NULL) SDL_error(14);
 Drawstatusbar();
} 

//Create box /////////////////////////////////////////////////////////////////
void Createbox(int color)
{
 SDL_Rect rect;
 SDL_Delay(100);
 if(dialogbox != NULL) SDL_FreeSurface(dialogbox);
 dialogbox = SDL_CreateRGBSurface(FLAGS, dialogrect.w, dialogrect.h,
                                  32, rmask, gmask, bmask, amask);
 //fond 
 rect.x = 0; rect.w = dialogbox->w;
 rect.y = 0; rect.h = dialogbox->h;
 SDL_FillRect(dialogbox, &rect, color);
 //bordure haut blanche      
 rect.h = 1;
 SDL_FillRect(dialogbox, &rect, blanc);
 //bordure gauche blanche      
 rect.x = 0; rect.w = 1;
 rect.y = 0; rect.h = dialogbox->h - 1;
 SDL_FillRect(dialogbox, &rect, blanc);
 //bordure bas gris fonce
 rect.x = 1; rect.w = dialogbox->w - 2;
 rect.y = dialogbox->h - 2; rect.h = 1; 
 SDL_FillRect(dialogbox, &rect, gris2);
 //bordure bas noire
 rect.x += 1; rect.y += 1; 
 SDL_FillRect(dialogbox, &rect, noir);
 //bordure droite gris fonce
 rect.x = dialogbox->w - 2; rect.w = 1;
 rect.y = 1; rect.h = dialogbox->h - 3;
 SDL_FillRect(dialogbox, &rect, gris2);
 //bordure droite noire
 rect.x += 1; rect.y += 1;
 SDL_FillRect(dialogbox, &rect, noir);
}

//Create dialog box //////////////////////////////////////////////////////////
void Createdialogbox(int w, int h)
{
 SDL_Rect rect;
 int r;
 dialogrect.x = 10; dialogrect.w = w;
 dialogrect.y = YSTATUS + 10; dialogrect.h = h;
 xmove = ymove = 0; 
 Createbox(grist);
 //barre de titre bleue
 rect.x = 4; rect.w = dialogbox->w - 8;
 rect.y = 3; rect.h = 18;
 SDL_FillRect(dialogbox, &rect, bleu);
 //bouton de fermeture
 rect.x = closebutton.x + dialogbox->w;
 rect.y = closebutton.y;
 r = SDL_BlitSurface(buttonsurfaceup[closebutton.n][language], NULL, dialogbox, &rect);
 if(r < 0) SDL_error(15);
}

//Traitement des clics souris dans la barre de statut ////////////////////////
void Statusclick()
{
 SDL_Rect rect;
 int i, r, x, y;	
 void Drawmenubox(), Drawdirlistbox(int);
 extern void Keyboard(), About();
 
 //recherche du bouton concerne par le clic
 for(i = 0; i < STATUSBUTTON_MAX; i++)
 {
  x = statusbutton[i].x; if(x < 0) x += xclient;     
  y = statusbutton[i].y; if(y < 0) y += yclient;     
  if(xmouse > x) if(xmouse < (x + bouton[statusbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[statusbutton[i].n].h))
  break;
 }
 if(i >= STATUSBUTTON_MAX) {dialog = 0; return;}

 //dessin du bouton enfonce
 rect.x = statusbutton[i].x; if(rect.x < 0) rect.x += xclient;
 rect.y = statusbutton[i].y; if(rect.y < 0) rect.y += yclient;
 r = SDL_BlitSurface(buttonsurfacedown[statusbutton[i].n][language], NULL, screen, &rect);
 if(r < 0) {SDL_error(16); return;}
            
 //traitement           
 switch(i)
 {
  case 0: dirmin = 0; Drawdirlistbox(0); break;
  case 1: Drawmenubox(); break;
  case 2: About(); break;
 }
 Drawstatusbar();
}

//Deplacement de la boite de dialogue ////////////////////////////////////////
void Dialogmove()
{
 int ytotal;
 ytotal = yclient + YSTATUS;
 dialogrect.x = xmouse - xmove;	  
 dialogrect.y = ymouse - ymove;	  
 if(dialogrect.x > (xclient - dialogrect.w))
   dialogrect.x = xclient - dialogrect.w;
 if(dialogrect.y > (ytotal - dialogrect.h))
    dialogrect.y = ytotal - dialogrect.h;
 if(dialogrect.x < 0)
    dialogrect.x = 0;
 if(dialogrect.y < YSTATUS)
    dialogrect.y = YSTATUS;
}

//Traitement des clics souris dans la boite de dialogue //////////////////////
void Dialogclick()
{
 int x, y;    
 extern void Optionclick(), Keyclick(), Joyclick(), Desassclick();

 //clic bouton de fermeture
 x = dialogrect.x + dialogrect.w + closebutton.x;     
 y = dialogrect.y + closebutton.y;     
 if(xmouse > x) if(xmouse < (x + bouton[closebutton.n].w))
 if(ymouse > y) if(ymouse < (y + bouton[closebutton.n].h))
 {dialog = 0; xmove = ymove = 0; return;}

 //clic bandeau superieur
 if(ymouse < (dialogrect.y + 22))
 {
  xmove = xmouse - dialogrect.x;
  ymove = ymouse - dialogrect.y;
  return;
 }

 //traitements particuliers
 if(dialog == 2) Optionclick();  //boite de dialogue Parametres
 if(dialog == 3) Keyclick();     //boite de dialogue Clavier
 if(dialog == 4) Joyclick();     //boite de dialogue Manettes
 if(dialog == 5) Desassclick();  //boite de dialogue Desassemblage
}

//Traitement des clics souris ////////////////////////////////////////////////
void Mouseclick()
{
 void Menuclick();
 pausez80 = 1;

 //clic dans la barre de statut
 if(ymouse < YSTATUS)
 {Statusclick(); pausez80 = 0; return;}
  
 //clic dans la boite de dialogue
 if(dialog > 0)
 if(xmouse > dialogrect.x) if(xmouse < (dialogrect.x + dialogrect.w))
 if(ymouse > dialogrect.y) if(ymouse < (dialogrect.y + dialogrect.h))
 {if(dialog > 999) Menuclick(); else Dialogclick();}

 //redemarrage de l'emulation, meme si elle etait arretee
 //sinon il n'y a pas d'affichage
 pausez80 = 0;
}

//Draw menu box ////////////////////////////////////////////////////////////
void Drawmenubox()
{
 SDL_Rect rect;
 int i;
 char string[50];
 dialogrect.x = xclient -122; dialogrect.w = 120;
 dialogrect.y = YSTATUS; dialogrect.h = 4 * 16 + 10;
 Createbox(gris0);
 rect.x = 10; rect.w = 105; rect.y = 4; rect.h = 14;
 for(i = 29; i < 33; i++)
 {
  sprintf(string, "%s...", msg[language][i]);   
  Drawtextbox(dialogbox, string, rect, 0);
  rect.y += 16;
 } 
 dialog = 1003; 
}

//Draw directory listbox /////////////////////////////////////////////////////
void Drawdirlistbox(int n)
{
 SDL_Rect rect;
 int i;
 void *string;
 switch(n)
 {
  case 0: Sortdirectory("sd:/apps/dcvg5k/software", ".k7"); break;
  case 1: Sortdirectory("sd:/apps/dcvg5k/software", ".fd"); break;
  case 2: Sortdirectory("sd:/apps/dcvg5k/software", ".rom"); break;
  default: return;    
 }
 if(dircount <= 0) return;
 dirmax = dirmin + 14;
 if(dirmax > dircount) dirmax = dircount;
 dialogrect.x = statusbutton[n].x; dialogrect.w = 200;
 dialogrect.y = YSTATUS; dialogrect.h = 16 * (dirmax - dirmin) + 26;
 if((dirmax < dircount) || (dirmin > 0)) dialogrect.h += 16; //[suite]/[debut]
 Createbox(gris0);
 rect.x = 10; rect.w = dialogrect.w - 12;
 rect.y = 4; rect.h = 14;
 Drawtextbox(dialogbox, msg[language][39], rect, 0); 
 for(i = dirmin; i < dirmax; i++)
 {
  rect.y += 16;
  Drawtextbox(dialogbox, dirlist[i], rect, 0); 
 }     
 if((dirmax < dircount) || (dirmin > 0))
 {
  rect.y += 16;
  string = msg[language][(dirmax == dircount) ? 41 : 40];
  Drawtextbox(dialogbox, string, rect, 0); 
 } 
 dialog = 1000 + n; 
}

//Traitement des clics dans un menu deroulant /////////////////////////////////
void Menuclick()
{
 int i, n, ymax;    
 extern void Options(), Clavier(), Manettes(), Desass();;
 extern void Loadk7(char *name);
 
 n = dialog - 1000;
 if(n == 3) //menu options
 {
  if(ymouse < (YSTATUS + 20)) {Options(); return;}
  if(ymouse < (YSTATUS + 36)) {Clavier(); return;}
  if(ymouse < (YSTATUS + 52)) {Manettes(); return;}
  if(ymouse < (YSTATUS + 68)) {Desass(); return;}
  return;
 } 

 while(1) //dir listbox
 {
  ymax = YSTATUS + 20;
  if(ymouse < ymax) {Loadk7(" "); break;} 
  for(i = dirmin; i < dirmax; i++)
  {ymax += 16; if(ymouse < ymax) {Loadk7(dirlist[i]); break;}} 
  if(i < dirmax) break;
  ymax += 16; if(ymouse < ymax)
  {dirmin += 20; if(dirmin >= dircount) dirmin = 0; Drawdirlistbox(n);}
  return; 
 }
 Drawstatusbar();
 dialog = 0;         
}

// About message box /////////////////////////////////////////////////////////
void About()
{
 int i;    
 char *text1[9], *text2[6];     
 Createdialogbox(324, 231); 
 for(i = 0; i < 8; i++) text1[i] = msg[language][i + 2]; text1[i] = "";
 text1[i] = "";
 for(i = 0; i < 5; i++) text2[i] = msg[language][i + 20]; text2[i] = "";

 /*test police/////////////////////////////////////////////
 char string[8][33];
 for(i = 0; i < 8; i++)
 {
  int j;
  for(j = 0; j < 32; j++) string[i][j] = 32 + 32 * i + j;
  string[i][j] = 0;
  text2[i] = string[i];
 } 
 */

 /*version SDL/////////////////////////////////////////////
 const SDL_version *vdll = SDL_Linked_Version();
 SDL_version vsdl;
 SDL_VERSION(&vsdl);
 char versdl[50], verdll[50];
 sprintf(versdl, "Version SDL : %u.%u.%u",
         vsdl.major, vsdl.minor, vsdl.patch);
 sprintf(verdll, "Version DLL : %u.%u.%u",
         vdll->major, vdll->minor, vdll->patch);
 text2[0] = versdl;
 text2[1] = verdll;
 i = 2;
 */

 /*affichage de controle de variables d'affichage /////////////////////////
 char string0[20], string1[20], string2[20];
 extern int videolinenumber, currentlinesegment, currentvideomemory;
 sprintf(string0, "videolinenumber....: %i", videolinenumber);
 sprintf(string1, "currentlinesegment.: %i", currentlinesegment);
 sprintf(string2, "currentvideomemory.: %i", currentvideomemory);
 text2[0] = string0;
 text2[1] = string1;
 text2[2] = string2;
 text2[3] = "";
 */
 
 Drawmessagebox(msg[language][1], text1, text2);
}    

// Message d'erreur emulateur ////////////////////////////////////////////////
void Erreur(int n)
{
 char string[256];    
 char *text1[3], *text2[1];     
 Createdialogbox(300, 90);    
 sprintf(string, "%i", n); 
 text1[0] = string;
 text1[1] = msg[language][n];
 text1[2] = "";
 text2[0] = "";
 Drawmessagebox(msg[language][10], text1, text2);
}    

// Affichage de controle d'une valeur ///////////////////////////////////////
void Info(int n)
{
 char string[32];   
 char *text1[4], *text2[1];     
 sprintf(string, "0x%04x (%i)", n, n);
 Createdialogbox(120, 60);    
 text1[0] = string;
 text1[1] = "";
 text2[0] = "";
 Drawmessagebox("Info", text1, text2);
}    

// Message d'erreur SDL //////////////////////////////////////////////////////
void SDL_error(int n)
{
 char string[256];    
 char *text1[3], *text2[1];     
 Createdialogbox(300, 90);    
 sprintf(string, "%i", n); 
 text1[0] = string;
 text1[1] = SDL_GetError();
 text1[2] = "";
 text2[0] = "";
 Drawmessagebox("SDL error", text1, text2);
 SDL_Delay(1000);
}
