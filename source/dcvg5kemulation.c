//////////////////////////////////////////////////////////////////////////////
// DCVG5KEMULATION.C - dcvg5k v11 - vg5000 emulation
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

#include <stdio.h>
#include <string.h>
#include "../Z80/Z80.h"
#include "dcvg5kglobal.h"
#include "../include/vg5000rom.h"
#include <SDL/SDL.h>

// memory
char ram[0xc000];       //ram VG5000 16K(base) + 32K(extension maxi) = 48K
char ioport[0x100];     //ports d'E/S = 256 octets
//Z80
Z80 CPU;
//autres variables globales
int nline;           //numero de la ligne ecran a afficher
int cycleline;       //nombre de cycles depuis le dernier affichage ligne
int cycleirq;        //nombre de cycles depuis la dernière IRQ
int cycleblink;      //nombre de cycles depuis le dernier clignotement
int cyclesperline;   //nombre de cycles par affichage ligne
int cyclesperirq;    //nombre de cycles par irq
int cyclesperblink;  //nombre de cycles par clignotement
int keyscan[256];    //disposition actuelle du clavier en cours d'édition
int sound;           //niveau du haut-parleur
int pausez80;        //processor pause state

extern int bordercolor; //couleur bordure
extern char message[];
extern int pausez80;

extern void Imprime(char c);
extern void Displayscreen();   //affichage de l'ecran
extern void WriteEF9345(char r, char c);
extern char ReadEF9345(char r);
extern void Blinking();

// Traitement keyup et keydown ///////////////////////////////////////////////
void Key(int keycode, int state)
{
 int i, j, n;
 void Hardreset();
 n = 0x80 + (keycode >> 3);
 j = 1; for(i = 0; i < (keycode & 7); i++) j += j;
 if(state) ioport[n] |= j; else ioport[n] &= ~j;
 if((ioport[0x80] & 0x42) == 0) Hardreset(); //Ctrl-Reset
}

void Joy(int joycode, int state)
{
 int i, j, n;
 n = 0x07 + (joycode >> 3);
 j = 1; for(i = 0; i < (joycode & 7); i++) j += j;
 if(state) ioport[n] |= j; else ioport[n] &= ~j;
 return;
}

// Joystick move /////////////////////////////////////////////////////////////
void Joymove(int n, int x, int y)
{
 //n est le numero du joystick (1 ou 2)   
 n += 6; //numero du port 7=joystick1 8=joystick2 
 if(y < 27768) ioport[n] &= 0xfe; else ioport[n] |= 0x01;
 if(x < 27768) ioport[n] &= 0xf7; else ioport[n] |= 0x08;
 if(y > 37767) ioport[n] &= 0xfb; else ioport[n] |= 0x04;
 if(x > 37767) ioport[n] &= 0xfd; else ioport[n] |= 0x02;
}

//Initialisation palette /////////////////////////////////////////////////////
void Initpalette(void)
{
 int i;
 int r[16]={0,15,0,15,0,15,0,15,0,15,0,15,0,15,0,15};
 int v[16]={0,0,15,15,0,0,15,15,0,0,15,15,0,0,15,15};
 int b[16]={0,0,0,0,15,15,15,15,0,0,0,0,15,15,15,15};
 extern void Palette(int n, int r, int v, int b);
 //0=noir, 1=rouge, 2=vert, 3=jaune, 4=bleu, 5=magenta, 6=cyan, 7=blanc
 for(i = 0; i < 16; i++) Palette(i, r[i], v[i], b[i]);
}

// Initialisation programme de l'ordinateur émulé ////////////////////////////
void Initprog()
{
 int i;   
 CPU.PC.W = 0x0033;
 sound = 0;
 for(i = 0x07; i < 0x09; i++) ioport[i] = 0xff; //joysticks au repos
 for(i = 0x80; i < 0x88; i++) ioport[i] = 0xff; //touches relachées
}

// Hardreset de l'ordinateur émulé ///////////////////////////////////////////
void Hardreset()
{
 int i;
 int pausestatus; 
 void InitEF9345();
 void Loadcartridge(int mode);
 void Loadk7(int mode);
 void Loaddisk(int mode);
 pausestatus = pausez80; pausez80 = 1;     //save pause status and stop Z80  
 InitEF9345();                             //initialisation EF9345  
 //raz de la ram
 for(i = 0; i < sizeof(ram); i++) ram[i] = 0;        //ram a zero
 //patch de la rom
 rom[0x3aa9] = 0xed; rom[0x3aaa] = 0xfe;    //ecriture cassette
 rom[0x3af3] = 0xed; rom[0x3af4] = 0xfe;    //test vitesse k7
 rom[0x3b48] = 0xed; rom[0x3b49] = 0xfe;    //lecture octet cassette
 rom[0x1e00] = 0xaf; rom[0x1e01] = 0x1f;    //fin cassette Basicode
 rom[0x3a81] = 0xc9;                        //signaux de synchro
 //initialisation clavier
 for(i = 0x07; i < 0x09; i++) ioport[i] = 0xff;      //joysticks au repos
 for(i = 0x80; i < 0x88; i++) ioport[i] = 0xff;      //touches relachées
 //autres initialisations
 Initpalette();                            //initialisation palette
 ResetZ80(&CPU);                           //initialisation processeur
 cycleline = 0; nline = 0;                 //groupe de 10 lignes video  
 cycleirq = 0; cyclesperirq = 80000;       //IRQ: 20 millisecondes
 cycleblink = 0; cyclesperblink = 2000000; //clignotement: 500 millisecondes
 cyclesperline = 400000 / 31;
 sound = 0;                                //valeur initiale d'un echantillon
 pausez80 = pausestatus;                   //restore pause status
}

// Execution n cycles processeur z80 /////////////////////////////////////////
int Run(int n)
{
 int oldICount;   
 extern int EF9345cycle;   
 oldICount = CPU.ICount;               //sauvegarde du compteur de cycles
 CPU.IPeriod = n;                      //nombre de cycles a executer
 RunZ80(&CPU);                         //execution d'environ n cycles 
 n += oldICount - CPU.ICount;          //nombre de cycles reellement executes 
 if(EF9345cycle > 0) EF9345cycle -= n; //cycles EF9345
 cycleline += n;                       //increment compteur ligne  
 cycleirq += n;                        //increment compteur IRQ
 cycleblink += n;                      //increment compteur clignotement
 return 0;
}  

// Ecriture memoire VG5000 ////////////////////////////////////////////////////
void WrZ80(word w, byte c)
{
 int a = w & 0xffff;
 if(a >= 0x4000) ram[a - 0x4000] = c;
}  

// Lecture memoire VG5000 /////////////////////////////////////////////////////
byte RdZ80(word w)
{
 int a = w & 0xffff;   
 return((a < 0x4000) ? rom[a] : ram[a - 0x4000]);
}

// IO output //////////////////////////////////////////////////////////////////
void OutZ80(word w, byte c)
{
 w &= 0xff;   
 ioport[w] = c;
 if(w == 0x11) {Imprime(c); return;} //ecriture imprimante
 if(w == 0xcf) {WriteEF9345(ioport[0x8f], c); return;} //ecriture EF9345
 if(w == 0xaf) {sound = c << 2; return;} //sortie son et cassette
 if(w == 0xef) {return;} //?????
}  

// IO input ///////////////////////////////////////////////////////////////////
byte InZ80(word w)
{
 w &= 0xff;   
 if(w == 0xcf) return ReadEF9345(ioport[0x8f]); //lecture EF9345 
 if((w > 0x7f) && (w < 0x88)) return ioport[w]; //lecture clavier
 if((w == 0) || (w == 1)) return ioport[8 - w]; //joysticks ports 1 et 0    
 if((w == 7) || (w == 8)) return ioport[w];     //joysticks ports 7 et 8   
 return 0;
}

// Loop ///////////////////////////////////////////////////////////////////////
word LoopZ80(register Z80 *R)
{
 extern void Displayline(int n);   
 if(cycleirq > cyclesperirq)      //lancement interruption
   {cycleirq -= cyclesperirq; IntZ80(R, INT_IRQ);}  
 if(cycleblink > cyclesperblink)  //clignotement curseur
   {cycleblink -= cyclesperblink; Blinking();}  
 if(cycleline > cyclesperline)    //affichage d'une ligne
   {cycleline -= cyclesperline; Displayline(nline++);
   
   if(nline > 30) {nline = 0;
   Displayscreen();
   }}  
 return INT_QUIT; 
}

// Patch //////////////////////////////////////////////////////////////////////
void PatchZ80(register Z80 *R)
{
 extern void Writeoctetk7(), Testvitessek7(), Readoctetk7();
 if(R->PC.W == 0x3aab) Writeoctetk7();  //ecriture cassette
 if(R->PC.W == 0x3af5) Testvitessek7(); //test vitesse k7
 if(R->PC.W == 0x3b4A) Readoctetk7();   //lecture cassette
}    
