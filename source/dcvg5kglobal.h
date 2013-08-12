//////////////////////////////////////////////////////////////////////////////
// DCVG5KGLOBAL.H - declarations globales
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

#define FLAGS SDL_HWSURFACE | SDL_DOUBLEBUF |SDL_RESIZABLE

#define XBITMAP40 336         //42 caracteres de 8 pixels 
#define XBITMAP80 492         //82 caracteres de 6 pixels
#define YBITMAP 270 
#define YSTATUS 20

#define KEYBOARDKEY_MAX 64    //nombre de touches du clavier vg5k
#define JOYSTICKKEY_MAX 16    //nombre total de contacts des deux manettes 
#define KEYBOARDBUTTON_MAX 66 //nombre de boutons boite de dialogue clavier 
#define JOYSTICKBUTTON_MAX 18 //nombre de boutons boite de dialogue manettes 
#define STATUSBUTTON_MAX 3    //nombre de boutons de la barre de statut 
#define OPTIONBUTTON_MAX 9    //nombre de boutons boite de dialogue options 
#define DESASSBUTTON_MAX 10   //nombre de boutons boite de dialogue desass 
#define LANGUAGE_MAX 2        //nombre de langues differentes 
#define MSG_MAX 50            //nombre de textes dans chaque langue 

#define BOUTON_MAX 104        //nombre de boutons et touches

typedef struct
{
 int x;         //ordonnee du coin haut-gauche
 int y;         //abscisse du coin haut-gauche
 int n;         //numero de la surface SDL
} dialogbutton;

typedef struct
{
 char name[12]; //nom du bouton
 int w;         //largeur en pixels
 int h;         //hauteur en pixels
} button;

