//////////////////////////////////////////////////////////////////////////////
// DCVG5KDEVICES.C - dcvg5k v11 device emulation
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcvg5k.free.fr
// Created  : December 2007
// Last updated : 2008-01-06
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

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>  
#include "../Z80/Z80.h"

//global variables
FILE *fk7 = NULL;   // pointeur fichier k7
FILE *fprn = NULL;  // pointeur fichier imprimante
int k7bit = 0;      // puissance de 2 designant le bit k7 en cours
int k7octet;        // octet de la cassette en cours de traitement
int k7index;        // compteur du lecteur de cassette
int k7indexmax;     // compteur du lecteur de cassette en fin de bande
int k7protection;   // indicateur lecture seule pour la cassette
char k7name[100] = {0};   // nom du fichier cassette    

//external variables
extern Z80 CPU;
//pointeurs vers fonctions d'acces memoire
extern char (*Mgetc)(unsigned short a);
extern void (*Mputc)(unsigned short a, char c);

// Emulation imprimante //////////////////////////////////////////////////////
void Imprime(char c)
{
 if(fprn == NULL) fprn = fopen("sd:/apps/dcvg5k/dcvg5k-printer.txt", "ab");
 if(fprn != NULL) fputc(c, fprn);
}

// Emulation cassette ////////////////////////////////////////////////////////
void Readoctetk7(void *x)
{
 int byte;
 extern void Erreur(int n);
 extern void Drawk7index();
 extern void Initprog();
 if(fk7 == NULL) {Initprog(); Erreur(11); return;}
 byte = fgetc(fk7); //lecture
 if(byte == EOF)
 {
  Initprog(); Erreur(12);
  fseek(fk7, 0, SEEK_SET); k7index = 0; Drawk7index(); return;
 }
 CPU.AF.W = byte << 8;  //octet dans A et flags a zero
 CPU.PC.W = 0x3b8d;     //retour de la routine de lecture d'un octet
}

void Writeoctetk7()
{
 void Erreur(int n);
 extern void Drawk7index();
 extern void Initprog();
 if(fk7 == NULL) {Initprog(); Erreur(11); return;}
 if(k7protection) {Initprog(); Erreur(13); return;}
 if(fputc(CPU.AF.W >> 8, fk7) == EOF) {Initprog(); Erreur(13); return;}
 CPU.PC.W = 0x3acf; 
 if((ftell(fk7) & 511) == 0) {k7index = ftell(fk7) >> 9; Drawk7index();} 
}

void Loadk7(char *name)
{
 char filename[256];
 extern void Drawk7index();
 if(fk7) {fclose(fk7); fk7 = NULL;} //fermeture cassette éventuellement ouverte
 if(name[0] == 0) return;
 strcpy(k7name, name);
 strcpy(filename, "sd:/apps/dcvg5k/software/");
 strcat(filename, name);
 fk7 = fopen(filename, "rb+");
 if(fk7 == NULL) {k7name[0] = 0; return;}
 k7index = 0;
 fseek(fk7, 0, SEEK_END);   
 k7indexmax = ftell(fk7) >> 9;
 fseek(fk7, 0, SEEK_SET);   
 Drawk7index();
 //k7protection = 1;
}

void Rewindk7()
{
 if(fk7 == NULL) return;
 fseek(fk7, 0, SEEK_SET);
}

// Test de la vitesse de la cassette /////////////////////////////////////////
void Testvitessek7()
{
 CPU.AF.W = 0x0000;  //A et flags a zero
 CPU.PC.W = 0x3b47;  //retour immediat
}

// Lecture boutons souris ////////////////////////////////////////////////////
int Readmousebutton()
{
 extern int mouse, mouselbutton;
 //*A = 3;
 if(!mouse) return 0;
 if(!mouselbutton) return 0;
 //*A = 0; CC |= 0x05;
 return 0;
}

// Lecture position crayon(0) ou souris(1) ///////////////////////////////////
int Readmousexy(int device)
{
 //int x, y;
 //extern int mouse, xmouse, ymouse, xclient, yclient;
 //void Mputw(unsigned short a, short w);
 //if(!mouse) {CC |= 1; return 0;}
 //x = (xclient > 0) ? xmouse * XBITMAP / xclient - 8 : -1;
 //y = (yclient > 0) ? ymouse * YBITMAP / yclient - 10 : -1;
 //if((x < 0) || (x >= 320)) {CC |= 1; return 0;}
 //if((y < 0) || (y >= 250)) {CC |= 1; return 0;}
 //if(cnt[0x7dc] == 0x2a) x <<= 1;
 //if(cnt[0x7dc] == 0x7b) x >>= 1;
 //if(device > 0) {Mputw(0x60d8, x); Mputw(0x60d6, y);} //souris
 //X = x; Y = y; 
 //CC &= 0xfe;
 return 0;
}

