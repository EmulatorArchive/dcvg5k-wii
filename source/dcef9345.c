//////////////////////////////////////////////////////////////////////////////
// DCEF9345.C - Emulation du micro-controleur EF9345
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

#include "../include/ef9345rom.h"

//variables globales
char EF9345ram[0x4000];   //16Ko (logique) pour 8Ko (physique)
char EF9345acc[0x2000];   //8Ko (logique) polices accentuees
char EF9345reg[8];        //registres R0 à R7
char EF9345status;        //registre status  
int  EF9345cycle;         //compteur de cycles  
int  xrepeat;             //nombre de repetitions d'un pixel
char TGS,MAT,PAT,DOR,ROR; //registres acces indirect
char border[80];          //caractere du cadre
char *bloc;               //pointeur bloc courant
char *matrice;            //pointeur matrice caractere courant
char *police[16];         //pointeurs polices de caracteres
int blink = 0;            //etat du clignotement
int cadran;               //cadran en cours d'affichage 
int lastcadran[40];       //dernier cadran affiche
int latchc0;              //couleur c0 latchee
int latchm;               //attribut m(conceal) latche
int latchi;               //attribut i(insert) latche
int latchu;               //attribut u(underline) latche

//fonctions
void (*Makechar)(int x, int y);   //pointeur fonction affichage caractere
void Makechar16x40(int x, int y); //affichage caractere 40 colonnes 16 bits
void Makechar24x40(int x, int y); //affichage caractere 40 colonnes 24 bits
void Makechar12x80(int x, int y); //affichage caractere 80 colonnes 12 bits
extern void Displaychar40(char *character, int x, int y);
extern void Displaychar80(char *character, int x, int y);


// Not programmed function ////////////////////////////////////////////////////
void Todo(int code)
{
 //char m[80];
 //if(code < 0x100) sprintf(m, "EF9345 : fonction %02x invalide", code);   
 //else sprintf(m, "EF9345 : mode d'affichage %01x invalide", code - 0x100);   
 //MessageBox(NULL, m, " EF9345 Erreur", MB_OK | MB_TOPMOST);   
}     

// Changement des modes controles par les registres indirects /////////////////
void SetEF9345mode()
{
 int i, mode;    
 //mode des caracteres
 mode = ((PAT & 0x80) >> 5) | ((TGS & 0xc0) >> 6);
 switch (mode)
 {
  case 0: xrepeat = 3; Makechar = Makechar24x40; break;   
  //case 1: xrepeat = 3; Makechar = Makecharvar40; break;   
  //case 2: xrepeat = 2; Makechar = Makechar8x80; break;   
  case 3: xrepeat = 2; Makechar = Makechar12x80; break;   
  case 4: xrepeat = 3; Makechar = Makechar16x40; break;   
  default: Todo(0x100 + mode); break;   
 }  
 //couleur de bordure
 for(i = 0; i < 80; i++) border[i] = MAT & 0x07;  
 //polices (avec ram 8K)
 police[0x08] = EF9345ram + ((DOR & 0x07) << 11); //G'0
 police[0x09] = police[0x08];                     //G'0 underlined
 police[0x0a] = EF9345ram + ((DOR & 0x30) << 8);  //G'10
 police[0x0b] = police[0x0a] + 0x800;             //G'11
 police[0x0c] = EF9345ram;                        //Quadrichrome
 police[0x0d] = police[0x0c];
 police[0x0e] = police[0x0c];
 police[0x0f] = police[0x0c];
 //adresse bloc memoire courant
 i = ((ROR & 0xf0) >> 4) | ((ROR & 0x40) >> 5) | ((ROR & 0x20) >> 3);
 bloc = EF9345ram + 0x800 * (i & 0x0c);
}    

// Initialisation des polices ////////////////////////////////////////////////
void InitEF9345font()
{
 int i, j;
 //polices accentuees
 for(j = 0; j < 0x10; j++) for(i = 0; i < 0x200; i++)
   EF9345acc[(j << 9) + i] = EF9345rom[0x0600 + i]; 
 for(j = 0; j < 0x200; j += 0x40) for(i = 0; i < 4; i++)
 {
  EF9345acc[0x0200 + j + i +  4] |= 0x1c; //tilde     
  EF9345acc[0x0400 + j + i +  4] |= 0x10; //aigu     
  EF9345acc[0x0400 + j + i +  8] |= 0x08; //aigu     
  EF9345acc[0x0600 + j + i +  4] |= 0x04; //grave     
  EF9345acc[0x0600 + j + i +  8] |= 0x08; //grave     

  EF9345acc[0x0a00 + j + i +  4] |= 0x1c; //tilde    
  EF9345acc[0x0c00 + j + i +  4] |= 0x10; //aigu     
  EF9345acc[0x0c00 + j + i +  8] |= 0x08; //aigu     
  EF9345acc[0x0e00 + j + i +  4] |= 0x04; //grave     
  EF9345acc[0x0e00 + j + i +  8] |= 0x08; //grave     

  EF9345acc[0x1200 + j + i +  4] |= 0x08; //point     
  EF9345acc[0x1400 + j + i +  4] |= 0x14; //trema     
  EF9345acc[0x1600 + j + i + 32] |= 0x08; //cedille     
  EF9345acc[0x1600 + j + i + 36] |= 0x04; //cedille     

  EF9345acc[0x1a00 + j + i +  4] |= 0x08; //point     
  EF9345acc[0x1c00 + j + i +  4] |= 0x14; //trema     
  EF9345acc[0x1e00 + j + i + 32] |= 0x08; //cedille     
  EF9345acc[0x1e00 + j + i + 36] |= 0x04; //cedille     
 }
 //initialisation des pointeurs de polices fixes
 police[0x00] = EF9345rom;          //G0
 police[0x01] = EF9345rom + 0x0800; //G0 underlined
 police[0x02] = EF9345rom + 0x1000; //G11
 police[0x03] = EF9345rom + 0x1800; //G10
 police[0x04] = EF9345acc;          //G20
 police[0x05] = EF9345acc + 0x0800; //G20 underlined
 police[0x06] = EF9345acc + 0x1000; //G21
 police[0x07] = EF9345acc + 0x1800; //G21 underlined
}
         
// Initialisation EF9345 //////////////////////////////////////////////////////
void InitEF9345()
{//NB: la structure reelle de la memoire n'est pas respectee
 int i;
 //initialisation de la ram
 for(i = 0; i < sizeof(EF9345ram); i++) EF9345ram[i] = 0;
 //initialisation des polices
 InitEF9345font();
 //initialisation des registres
 for(i = 0; i < 8; i++) EF9345reg[i] = 0;
 TGS = MAT = PAT = DOR = ROR = EF9345status = 0;
 SetEF9345mode();
}    

// Calcul de l'adresse en ram /////////////////////////////////////////////////
int Indexram(int r)
{// NB: la structure reelle de la memoire n'est pas respectee
 // Le bit d1 de poids fort du n° de bloc est ignoré (ram limitee à 8 Ko)
 int x, y;
 x = EF9345reg[r]; y = EF9345reg[r - 1]; if(y < 8) y &= 1;
 return ((x&0x3f)|((x&0x40)<<6)|((x&0x80)<<4)|((y&0x1f)<<6)|((y&0x20)<<8));
}     

// Calcul de l'adresse en rom /////////////////////////////////////////////////
int Indexrom(int r)
{// NB: la structure reelle de la memoire n'est pas respectee
 // Les bits d0 et d1 du n° de bloc sont ignorés (rom limitee a 4 Ko) 
 int x, y;
 x = EF9345reg[r]; y = EF9345reg[r - 1]; if(y < 8) y &= 1;
 return((x&0x3f)|((x&0x40)<<6)|((x&0x80)<<4)|((y&0x1f)<<6));
}     

//calcul de l'adresse du caractere x, y dans le bloc courant //////////////////
int Indexbloc(int x, int y)
{
 int i, j;
 i = x;
 j = (y == 0) ? ((TGS & 0x20) >> 5) : ((ROR & 0x1f) + y - 1); 
 //partie droite d'un caractere en double largeur en mode 40 caracteres
 if((TGS & 0x80) == 0) if(x > 0)
 {
  if(lastcadran[x - 1] == 1) i--; //colonne precedente  
  if(lastcadran[x - 1] == 4) i--; //colonne precedente  
  if(lastcadran[x - 1] == 5) i--; //colonne precedente  
 }
 
 //partie basse d'un caractere en double hauteur
 //ne pas prendre le caractere de la ligne precedente
 //(provoque un bug en tete du mat du bateau dans le sphinx d'or) 
 //if(lastcadran[x] == 1) j--;     //ligne precedente
 //if(lastcadran[x] == 2) j--;     //ligne precedente
 //if(lastcadran[x] == 3) j--;     //ligne precedente
 //if(j < 8) j &= 1;    a verifier !!!!!!!!

 return 0x40 * j + i; 
}

/* DCAlice 1.3 -> bug dans Alicetel
// Increment d'un registre X //////////////////////////////////////////////////
void Incrx(int r)
{
 int i;
 i = (EF9345reg[r] & 0x3f) + 1;
 EF9345status &= 0x8f;  //S4, S5, S6 reset
 if(i > 39)
 {
  if(r == 5) EF9345status |= 0x10; //S4=LXa set 
  if(r == 7) EF9345status |= 0x20; //S5=LXm set 
  EF9345status |= 0x40;            //S6=alarm set
  i -= 40;
 } 
 EF9345reg[r] = (EF9345reg[r] & 0xc0) | i;
}   
*/  

/* DCAlice 1.4 -> bug dans Crocky
// Increment d'un registre X //////////////////////////////////////////////////
void Incrx(int r)
{
 int i;
 if(EF9345status & 0x30) EF9345status |= 0x40;  //S6=alarm set
 else EF9345status &= 0xb0;
 i = (EF9345reg[r] & 0x3f) + 1;
 if(i > 39) i -= 40;
 EF9345reg[r] = (EF9345reg[r] & 0xc0) | i;
}     
*/

// DCAlice 1.4 sr1 -> Crocky OK et Alicetel OK
// Increment d'un registre X //////////////////////////////////////////////////
void Incrx(int r)
{
 int i;
 i = (EF9345reg[r] & 0x3f) + 1;
 if(i > 39) {i -= 40; EF9345status |= 0x40;}  //S6=alarm set
 EF9345reg[r] = (EF9345reg[r] & 0xc0) | i;
}     

// Increment d'un registre Y //////////////////////////////////////////////////
void Incry(int r)
{
 int i;
 i = (EF9345reg[r] & 0x1f) + 1; if(i > 31) i -= 24;
 EF9345reg[r] = (EF9345reg[r] & 0xe0) | i;      
}     

// Determination du cadran a afficher /////////////////////////////////////////
void Cadran(int x, int attrib)
{
 while(1)
 {
  if(x > 0) if(lastcadran[x-1] == 1) {cadran = 2; break;}  //cadran haut droit 
  if(x > 0) if(lastcadran[x-1] == 5) {cadran = 10; break;} //moitie droite  
  if(lastcadran[x] == 1) {cadran = 4; break;}              //cadran bas gauche
  if(lastcadran[x] == 2) {cadran = 8; break;}              //cadran bas droit
  if(lastcadran[x] == 3) {cadran = 12; break;}             //moitie basse
  if(attrib == 1) {cadran = 5; break;}                     //moitie gauche
  if(attrib == 2) {cadran = 3; break;}                     //moitie haute
  if(attrib == 3) {cadran = 1; break;}                     //cadran haut gauche
  cadran = 0; break;                                       //caractere complet                   
 }     
 lastcadran[x] = cadran;
}

// Zoom sur une partie d'un caractere ////////////////////////////////////////
void Zoom(char *pix, int n)
{
 //pix est le tableau des pixels (8x10=80)
 //n definit la zone (zoomx2y2 pour cadran, zoomx2 ou zoomy2 pour moitie)
 //cadran haut-gauche=1, haut-droit=2, bas-gauche=4, bas-droite=8
 //moitie haute=3, basse=12, gauche=5, droite=10
 int i, j;
 if((n & 0x0a) == 0) for(i = 0; i < 80; i += 8) ///////////////////// 1, 4, 5
  for(j = 7; j > 0; j--) pix[i + j] = pix[i + j / 2];     
 if((n & 0x05) == 0) for(i = 0; i < 80; i += 8) ///////////////////// 2, 8, 10
  for(j =0 ; j < 7; j++) pix[i + j] = pix[i + 4 + j / 2];     
 if((n & 0x0c) == 0) for(i = 0; i < 8; i++) ///////////////////////// 1, 2, 3
  for(j = 9; j > 0; j--) pix[i + 8 * j] = pix[i + 8 * (j / 2)];
 if((n & 0x03) == 0) for(i = 0; i < 8; i++) ///////////////////////// 4, 8, 12
  for(j = 0; j < 9; j++) pix[i + 8 * j] = pix[i + 40 + 8 * (j / 2)];
} 

// Display bichrome character (40 columns) ////////////////////////////////////
void Bichrome40(int ibloc, int x, int y, int c0, int c1, int insert,
                int flash, int conceal, int negative, int underline)
{
 int i, j;
 char pix[80]; //tableau des pixels du caractere
 
 if(flash) if(PAT & 0x40) if(blink) c1 = c0;  //flash
 if(conceal) if(PAT & 0x08) c1 = c0;          //conceal
 if(negative) {i = c1; c1 = c0; c0 = i;}      //negative
 //à vérifier cf instructions en commentaires ci-dessous : insert = 0 ou 1 ??
 if((PAT & 0x30) == 0x30) insert = 0;         //active area mark 
 if(insert == 0) c1 += 8;                     //couleur etendue foreground
 if((PAT & 0x30) == 0x00) insert = 1;         //inlay
 if(insert == 0) c0 += 8;                     //couleur etendue background

 //if(f) if(PAT & 0x40) if(blink) c1 = c0; //flash
 //if(m) if(PAT & 0x08) c1 = c0;           //conceal
 //if(n) {cx = c0; c0 = c1; c1 = cx;}      //negative
 //if((PAT & 0x30) == 0x30) i = 1;         //active area mark
 //if(i == 0) c1 += 8;                     //couleur etendue foreground
 //if((PAT & 0x30) == 0x00) i = 0;         //inlay
 //if(i == 0) c0 += 8;                     //couleur etendue background
     
 //affichage du curseur
 i = (EF9345reg[6] & 0x1f); if(i < 8) i &= 1;
 if(ibloc == 0x40 * i + (EF9345reg[7] & 0x3f)) //position du curseur
 {
  //affichage du curseur si MAT 6 est positionne
  //le clignotement depend de MAT 5, la forme depend de MAT 4 
  switch(MAT & 0x70)
  {
   case 0x40:  //00 = fixed complemented                  
               c0 = (23 - c0) & 15; c1 = (23 - c1) & 15; break;
   case 0x50:  //01 = fixed underlined
               underline = 1; break;
   case 0x60:  //10 = flash complemented
               if(blink) {c0 = (23 - c0) & 15; c1 = (23 - c1) & 15;} break;
   case 0x70:  //11 = flash underlined 
               if(blink) underline = 1; break;
  } 
 }
 
 //initialisation du tableau des pixels du caractere
 j = 0; for(i = 0; i < 10; i++)
 {
  char *k = matrice + 4 * i;
  pix[j++] = (*k & 0x01) ? c1 : c0; pix[j++] = (*k & 0x02) ? c1 : c0;    
  pix[j++] = (*k & 0x04) ? c1 : c0; pix[j++] = (*k & 0x08) ? c1 : c0;    
  pix[j++] = (*k & 0x10) ? c1 : c0; pix[j++] = (*k & 0x20) ? c1 : c0;    
  pix[j++] = (*k & 0x40) ? c1 : c0; pix[j++] = (*k & 0x80) ? c1 : c0;    
 }
  
 //caractere souligne
 if(underline) for(j = 72; j < 80; j++) pix[j] = c1;

 //affichage en fonction du cadran
 //remarque : la double hauteur par le bit 7 du registre MAT
 //est prise en charge par la fonction d'affichage de l'ecran
 //voir Displayscreen() dans le module de traitement de la video
 if(cadran > 0) Zoom(pix, cadran);
 Displaychar40(pix, x + 1 , y + 1);
}

// Display quadrichrome character (40 columns) ////////////////////////////////
void Quadrichrome40(int c, int b, int a, int x, int y)
{
 //C0-6= character code
 //B0= insert             not yet implemented !!!
 //B1= low resolution 
 //B2= subset index (low resolution only)
 //B3-5 = set number
 //A0-6 = 4 color palette    
 char *k;      //pointeur table des caracteres
 char pix[80]; //tableau des pixels a afficher
 int i, j, n, col[8];
 int lowresolution = (b & 0x02) >> 1; 
 int ramx, ramy, rambloc;
 char *ramindex;
 //pas de double hauteur ou largeur pour les quadrichromes
 lastcadran[x] = 0;
 //color table initialization (seules les couleurs 0-3 sont utilisées)
 j = 1; n = 0;
 for(i = 0; i < 4; i++) col[i] = 7; //blanc par defaut
 for(i = 0; i < 8; i++) {if(a & j) col[n++] = i; j <<= 1;}
 //find bloc number i in ram
 rambloc = 0;
 if(b & 0x20) rambloc |= 4;      //B5
 if(b & 0x08) rambloc |= 2;      //B3
 if(b & 0x10) rambloc |= 1;      //B4
 //find character address in ram
 ramx = c & 0x03; ramy =(c & 0x7f) >> 2;
 ramindex = police[0x0c] + 0x800 * rambloc + 0x40 * ramy + ramx;
 if(lowresolution) ramindex += 5 * (b & 0x04);
 //fill pixel table
 j = 0; for(i = 0; i < 10; i++) 
 {
  k = ramindex + 4 * (i >> lowresolution);
  pix[j] = pix[j + 1] = col[(*k & 0x03) >> 0]; j += 2;    
  pix[j] = pix[j + 1] = col[(*k & 0x0c) >> 2]; j += 2;    
  pix[j] = pix[j + 1] = col[(*k & 0x30) >> 4]; j += 2;    
  pix[j] = pix[j + 1] = col[(*k & 0xc0) >> 6]; j += 2;    
 }
 //display character
 Displaychar40(pix, x + 1, y + 1);
}    

// Display bichrome character (80 columns) ////////////////////////////////////
void Bichrome80(int c, int a, int x, int y)
{
 char *k;      //pointeur table des caracteres    
 char pix[60]; //tableau des pixels a afficher
 int i, j, d, c0, c1;    
 c1 = (a & 1) ? (DOR >> 4) & 7 : DOR & 7; //foreground color = DOR
 c0 =  MAT & 7;                           //background color = MAT
 switch(c & 0x80)
 {
  case 0: //alphanumeric G0 set        
   //A0: D = color set
   //A1: U = underline 
   //A2: F = flash  
   //A3: N = negative
   //C0-6: character code    
   j = 0; 
   if(a & 4) if(PAT & 0x40) if(blink) c1 = c0; //flash
   if(a & 8) {i = c1; c1 = c0; c0 = i;}        //couleurs en mode inverse
   d = ((c & 0x7f) >> 2) * 0x40 + (c & 0x03);  //deplacement dans table police
   for(i = 0; i < 10; i++)
   {
    k = EF9345rom + d + 4 * i;
    pix[j++] = (*k & 0x01) ? c1 : c0; pix[j++] = (*k & 0x02) ? c1 : c0;    
    pix[j++] = (*k & 0x04) ? c1 : c0; pix[j++] = (*k & 0x08) ? c1 : c0;    
    pix[j++] = (*k & 0x10) ? c1 : c0; pix[j++] = (*k & 0x20) ? c1 : c0;    
   }
   if(a & 2) for(j = 54; j < 60; j++) pix[j] = c1; //underline
   break;
  default: //dedicated mosaic set
   //A0: D = color set
   //A1-3: 3 blocs de 6 pixels
   //C0-6: 7 blocs de 6 pixels
   pix[ 0] = (c & 0x01) ? c1 : c0; pix[ 3] = (c & 0x02) ? c1 : c0;
   pix[12] = (c & 0x04) ? c1 : c0; pix[15] = (c & 0x08) ? c1 : c0;
   pix[24] = (c & 0x10) ? c1 : c0; pix[27] = (c & 0x20) ? c1 : c0;
   pix[36] = (c & 0x40) ? c1 : c0; pix[39] = (a & 0x02) ? c1 : c0;
   pix[48] = (a & 0x04) ? c1 : c0; pix[51] = (a & 0x08) ? c1 : c0;
   for(i = 0; i < 60; i += 12) {pix[i + 6] = pix[i]; pix[i + 9] = pix[i + 3];}
   for(i = 0; i < 60; i += 3) pix[i + 2] = pix[i + 1] = pix[i];
   break;
 } 
 Displaychar80(pix, x, y);
}     

// Affichage d'un caractere 16 bits 40 colonnes ///////////////////////////////
void Makechar16x40(int x, int y)
{
 int i, a, b, type, c0, c1, f, m, n, u, ibloc;

 //recherche des octets b, a (precedent si double largeur)
 ibloc = Indexbloc(x, y);
 a = bloc[ibloc]; b = bloc[ibloc + 0x800];   

 //determination attribut HL et numero du cadran a afficher
 Cadran(x, (a & 0x80) ? 0 : (((a & 0x20) >> 5) | ((a & 0x10) >> 3))); 

 //adresse de la matrice de definition du caractere
 //le if de la ligne suivante provoquait un bug dans le fou volant vg5000
 //verifier que sa suppression n'introduit pas de bug pour les jeux Alice
 //type = ((b & 0x80) >> 4) | ((a & 0x80) >> 6); if(type == 2) type++;
 type = ((b & 0x80) >> 4) | ((a & 0x80) >> 6);
 matrice = police[type] + ((b & 0x7f) >> 2) * 0x40 + (b & 0x03);
 if((b & 0xe0) == 0x80) matrice = police[0x03]; //negative space

 //attributs latches
 if(x == 0) latchm = latchi = latchu = latchc0 = 0;
 if(type == 4) {latchm = b & 1; latchi = (b & 2) >> 1; latchu = (b & 4) >> 2;} 
 if(a & 0x80) latchc0 = (a & 0x70) >> 4;

 //attributs du caractere
 c0 = latchc0;                           //background
 c1 = a & 0x07;                          //foreground
 i = latchi;                             //insert mode
 f  = (a & 0x08) >> 3;                   //flash
 m = latchm;                             //conceal
 n  = (a & 0x80) ? 0: ((a & 0x40) >> 6); //negative
 u = latchu;                             //underline

 //affichage
 Bichrome40(ibloc, x, y, c0, c1, i, f, m, n, u);
}     

// Affichage d'un caractere 24 bits 40 colonnes ///////////////////////////////
void Makechar24x40(int x, int y)
{
 int i, a, b, c, c0, c1, f, m, n, u, ibloc;

 //recherche des octets c, b, a
 //ibloc = (MAT & 0x80) ? Indexbloc(x, y / 2) : Indexbloc(x, y);
 //la double hauteur est maintenant prise en compte dans Displayscreen()
 ibloc = Indexbloc(x, y);
 c = bloc[ibloc]; b = bloc[ibloc + 0x800]; a = bloc[ibloc + 0x1000];   
 if((b & 0xc0) == 0xc0) {Quadrichrome40(c, b, a, x, y); return;}
 
 //determination attribut HL et numero du cadran a afficher
 Cadran(x, (b & 0x02) + ((b & 0x08) >> 3)); 
 
 //adresse de la matrice de definition du caractere
 matrice = police[(b & 0xf0) >> 4] + ((c & 0x7f) >> 2) * 0x40 + (c & 0x03);

 //attributs du caractere
 c0 = a & 0x07;        //background
 c1 = (a & 0x70) >> 4; //foreground
 i = b & 0x01;         //insert
 f = (a & 0x08) >> 3;  //flash
 m = (b & 0x04) >> 2;  //conceal
 n = (a & 0x80) >> 7;  //negative
 u = (((b & 0x60) == 0) || ((b & 0xc0) == 0x40)) ? ((b & 0x10) >> 4) : 0;

 //affichage
 Bichrome40(ibloc, x, y, c0, c1, i, f, m, n, u);
}     

// Affichage d'un caractere variable 40 colonnes //////////////////////////////
void Makecharvar40(int x, int y)
{
}

// Affichage de deux caracteres 8 bits 80 colonnes ////////////////////////////
void Makechar8x80(int x, int y)
{
}    

// Affichage de deux caracteres 12 bits 80 colonnes ///////////////////////////
void Makechar12x80(int x, int y)
{
 int ibloc;
 ibloc = Indexbloc(x, y);
 Bichrome80(bloc[ibloc], (bloc[ibloc + 0x1000] >> 4) & 0x0f, 2 * x + 2, y + 1);
 Bichrome80(bloc[ibloc + 0x800], bloc[ibloc + 0x1000] & 0x0f, 2 * x + 3, y + 1);
}    

// Clignotement  /////////////////////////////////////////////////////////////
void Blinking(void) {blink = 1 - blink;}

// Display one screen line (0-24) /////////////////////////////////////////////
void Displayline(int n)
{
 int i;

 if(n == 25) EF9345status &= 0xfb;  //S2=VBL status set (bit2=0)   
 if(n > 24) return; //lignes 25-30 non affichees (video blanking) 
 EF9345cycle = 416; //104 microsecondes (temps de chargement ligne)
 //bordures gauche et droite
 Displaychar40(border, 0, n + 1);
 Displaychar40(border, 41, n + 1);
 //bordure haute et service row
 if(n == 0)
 {
  EF9345status |= 0x04;   //S2=VBL status reset (bit2=1)
  for(i = 0; i < 42; i++) Displaychar40(border, i, 0);      //bordure haute    
  if(PAT & 1) for(i = 0; i < 40; i++) Makechar(i, n);   //ligne affichee
  else for(i = 0; i < 42; i++) Displaychar40(border, i, 1); //ligne=bordure 
  return;
 }   
 //upper bulk
 if(n < 12)
 {
  if(PAT & 2) for(i = 0; i < 40; i++) Makechar(i, n);   //ligne affichee 
  else for(i = 0; i < 42; i++) Displaychar40(border, i, n); //ligne=bordure
  return;
 }  
 //lower bulk
 if(PAT & 4) for(i = 0; i < 40; i++) Makechar(i, n);        //ligne affichee
 else for(i = 0; i < 42; i++) Displaychar40(border, i, n);  //ligne=bordure
 //derniere ligne
 if(n == 24)
 {for(i = 0; i < 42; i++) Displaychar40(border, i, 26);}    //bordure basse   
}

// Lecture-ecriture en memoire EF9345 /////////////////////////////////////////
void OCTwrite(int r, int i) //registre R1
{
 EF9345cycle = 16; //4 microsecondes
 EF9345ram[Indexram(r)] = EF9345reg[1]; if(i == 0) return;
 Incrx(r); if((EF9345reg[r] & 0x3f) == 0) if(r == 7) Incry(6); //increment
}     
void OCTread(int r, int i)
{
 EF9345cycle = 18; //4.5 microsecondes
 EF9345reg[1] = EF9345ram[Indexram(r)];  if(i == 0) return;  
 Incrx(r); if((EF9345reg[r] & 0x3f) == 0) if(r == 7) Incry(6); //increment
}     

void KRGwrite(int i) //mode 40 caracteres 16 bits
{
 int a = Indexram(7);
 EF9345cycle = 22; //5.5 microsecondes
 EF9345ram[a] = EF9345reg[1];             
 EF9345ram[a + 0x0800] = EF9345reg[2];   
 if(i) Incrx(7);
}
void KRGread(int i)
{
 int a = Indexram(7);
 EF9345cycle = 30; //7.5 microsecondes
 EF9345reg[1] = EF9345ram[a];
 EF9345reg[2] = EF9345ram[a + 0x0800];     
 if(i) Incrx(7);
}     

void KRFwrite(int i) //mode 40 caracteres 24 bits
{
 int a = Indexram(7);
 EF9345cycle = 16; //4 microsecondes
 EF9345ram[a] = EF9345reg[1];             
 EF9345ram[a + 0x0800] = EF9345reg[2];   
 EF9345ram[a + 0x1000] = EF9345reg[3];  
 if(i) Incrx(7);
}
void KRFread(int i)
{
 int a = Indexram(7);
 EF9345cycle = 30; //7.5 microsecondes
 EF9345reg[1] = EF9345ram[a];
 EF9345reg[2] = EF9345ram[a + 0x0800];     
 EF9345reg[3] = EF9345ram[a + 0x1000];     
 if(i) Incrx(7);
}     

void KRLwrite(int i) //mode 80 caracteres 12 bits
{
 int a = Indexram(7);
 EF9345cycle = 50; //12.5 microsecondes
 EF9345ram[a] = EF9345reg[1];
 switch((a / 0x800) & 1)
 {
  case 0: EF9345ram[a + 0x1000] &= 0x0f;
          EF9345ram[a + 0x1000] |= (EF9345reg[3] & 0xf0); break;
  case 1: EF9345ram[a + 0x0800] &= 0xf0;
          EF9345ram[a + 0x0800] |= (EF9345reg[3] & 0x0f); break;
 } 
 if(i)
 {
  if((EF9345reg[7] & 0x80) == 0x00) {EF9345reg[7] |= 0x80; return;}
  EF9345reg[7] &= 0x80;
  Incrx(7);
 } 
}

void KRLread(int i)
{
 int a = Indexram(7);
 EF9345cycle = 46; //11.5 microsecondes
 EF9345reg[1] = EF9345ram[a];             
 switch((a / 0x800) & 1)
 {
  case 0: EF9345reg[3] = EF9345ram[a + 0x1000]; break;
  case 1: EF9345reg[3] = EF9345ram[a + 0x0800]; break;
 } 
 if(i)
 {
  if((EF9345reg[7] & 0x80) == 0x00) {EF9345reg[7] |= 0x80; return;}
  EF9345reg[7] &= 0x80;
  Incrx(7);
 } 
}

// Move buffer ////////////////////////////////////////////////////////////////
void MVB(int n, int r1, int r2, int s)
{
 //s=1 pour mode normal, s=0 pour remplir tout le buffer   
 int i, a1, a2;    
 EF9345cycle = 8; //2 microsecondes
 for(i = 0; i < 1280; i++)
 {
  a1 = Indexram(r1); a2 = Indexram(r2);  
  EF9345ram[a2] = EF9345ram[a1]; EF9345cycle += 16 * n;
  if(n > 1) EF9345ram[a2 + 0x0800] = EF9345ram[a1 + 0x0800];
  if(n > 2) EF9345ram[a2 + 0x1000] = EF9345ram[a1 + 0x1000];
  Incrx(r1); Incrx(r2);
  if((EF9345reg[5] & 0x3f) == 0) {if(s) break;}
  if((EF9345reg[7] & 0x3f) == 0) {if(s) break; else Incry(6);}
 }  
 EF9345status &= 0x8f;  //reset S4(LXa), S5(LXm), S6(Al) 
}     

// Increment registre R6 //////////////////////////////////////////////////////
void INY()
{
 EF9345cycle = 8; //2 microsecondes     
 Incry(6);
 EF9345status &= 0x8f;  //reset S4(LXa), S5(LXm), S6(Al) 
}     

// Lecture/Ecriture des registres indirects ///////////////////////////////////
void INDwrite(int x)
{
 EF9345cycle = 8; //2 microsecondes
 switch(x)
 {
  case 1: TGS = EF9345reg[1]; break;
  case 2: MAT = EF9345reg[1]; break;
  case 3: PAT = EF9345reg[1]; break;
  case 4: DOR = EF9345reg[1]; break;
  case 7: ROR = EF9345reg[1]; break;
 }        
 SetEF9345mode();
 EF9345status &= 0x8f;  //reset S4(LXa), S5(LXm), S6(Al) 
}     

void INDread(int x)
{
 EF9345cycle = 14; //3.5 microsecondes
 switch(x)
 {
  case 0: EF9345reg[1] = EF9345rom[Indexrom(7) & 0x1fff]; break;  
  case 1: EF9345reg[1] = TGS; break;  
  case 2: EF9345reg[1] = MAT; break;  
  case 3: EF9345reg[1] = PAT; break;  
  case 4: EF9345reg[1] = DOR; break;
  case 7: EF9345reg[1] = ROR; break;  
  case 8: EF9345reg[1] = ROR; break;  
 }        
 EF9345status &= 0x8f;  //reset S4(LXa), S5(LXm), S6(Al) 
}     

// Execution commande EF9345 //////////////////////////////////////////////////
void ExecEF9345(int opcode)
{
 EF9345status = 0;  
 if((EF9345reg[5] & 0x3f) == 39) EF9345status |= 0x10; //S4(LXa) set 
 if((EF9345reg[7] & 0x3f) == 39) EF9345status |= 0x20; //S5(LXm) set 
 //S4-S6 sont remis a zero par IND, MVB, MVD, MVT, VSM, INY
 //S6 est positionne dans la fonction Incrx()
  
 switch(opcode)
 {
  case 0x00: KRFwrite(0); break;     //KRF: R1,R2,R3->ram              
  case 0x01: KRFwrite(1); break;     //KRF: R1,R2,R3->ram + increment              
  case 0x02: KRGwrite(0); break;     //KRG: R1,R2->ram              
  case 0x03: KRGwrite(1); break;     //KRG: R1,R2->ram + increment              
  case 0x05: Todo(opcode); break;    //CLF: Clear page 24 bits              
  case 0x07: Todo(opcode); break;    //CLG: Clear page 16 bits              
  case 0x08: KRFread(0); break;      //KRF: ram->R1,R2,R3              
  case 0x09: KRFread(1); break;      //KRF: ram->R1,R2,R3 + increment              
  case 0x0a: KRGread(0); break;      //KRG: ram->R1,R2              
  case 0x0b: KRGread(1); break;      //KRG: ram->R1,R2 + increment              
  case 0x30: OCTwrite(7, 0); break;  //OCT: R1->RAM, main pointer 
  case 0x31: OCTwrite(7, 1); break;  //OCT: R1->RAM, main pointer + inc 
  case 0x34: OCTwrite(5, 0); break;  //OCT: R1->RAM, aux pointer 
  case 0x35: OCTwrite(5, 1); break;  //OCT: R1->RAM, aux pointer + inc
  case 0x38: OCTread(7, 0); break;   //OCT: RAM->R1, main pointer 
  case 0x39: OCTread(7, 1); break;   //OCT: RAM->R1, main pointer + inc 
  case 0x3c: OCTread(5, 0); break;   //OCT: RAM->R1, aux pointer 
  case 0x3d: OCTread(5, 1); break;   //OCT: RAM->R1, aux pointer + inc
  case 0x40: Todo(opcode); break;    //KRC: R1 -> ram              
  case 0x41: Todo(opcode); break;    //KRC: R1 -> ram + inc              
  case 0x48: Todo(opcode); break;    //KRC: 80 characters - 8 bits              
  case 0x49: Todo(opcode); break;    //KRC: 80 characters - 8 bits              
  case 0x50: KRLwrite(0); break;     //KRL: 80 char - 12 bits write              
  case 0x51: KRLwrite(1); break;     //KRL: 80 char - 12 bits write + inc             
  case 0x58: KRLread(0); break;      //KRL: 80 char - 12 bits read              
  case 0x59: KRLread(1); break;      //KRL: 80 char - 12 bits read + inc             
  case 0x80: break;                  //IND: R1->ROM (impossible ?)
  case 0x81: INDwrite(1); break;     //IND: R1->TGS
  case 0x82: INDwrite(2); break;     //IND: R1->MAT
  case 0x83: INDwrite(3); break;     //IND: R1->PAT
  case 0x84: INDwrite(4); break;     //IND: R1->DOR
  case 0x87: INDwrite(7); break;     //IND: R1->ROR
  case 0x88: INDread(0); break;      //IND: ROM->R1
  case 0x89: INDread(1); break;      //IND: TGS->R1
  case 0x8a: INDread(2); break;      //IND: MAT->R1
  case 0x8b: INDread(3); break;      //IND: PAT->R1
  case 0x8c: INDread(4); break;      //IND: DOR->R1
  case 0x8f: INDread(7); break;      //IND: ROR->R1
  case 0x90: break;                  //NOP: no operation
  case 0x91: break;                  //NOP: no operation
  case 0x95: break;                  //VRM: vertical sync mask reset
  case 0x99: break;                  //VSM: vertical sync mask set
  case 0xb0: INY(); break;           //INY: increment Y
  case 0xd5: MVB(1, 7, 5, 1); break; //MVB: move buffer MP->AP stop
  case 0xd6: MVB(1, 7, 5, 0); break; //MVB: move buffer MP->AP nostop
  case 0xd9: MVB(1, 5, 7, 1); break; //MVB: move buffer AP->MP stop
  case 0xda: MVB(1, 5, 7, 0); break; //MVB: move buffer AP->MP nostop
  case 0xe5: MVB(2, 7, 5, 1); break; //MVD: move double buffer MP->AP stop
  case 0xe6: MVB(2, 7, 5, 0); break; //MVD: move double buffer MP->AP nostop
  case 0xe9: MVB(2, 5, 7, 1); break; //MVD: move double buffer AP->MP stop
  case 0xea: MVB(2, 5, 7, 0); break; //MVD: move double buffer AP->MP nostop
  case 0xf5: MVB(3, 7, 5, 1); break; //MVT: move triple buffer MP->AP stop
  case 0xf6: MVB(3, 7, 5, 0); break; //MVT: move triple buffer MP->AP nostop
  case 0xf9: MVB(3, 5, 7, 1); break; //MVT: move triple buffer AP->MP stop
  case 0xfa: MVB(3, 5, 7, 0); break; //MVT: move triple buffer AP->MP nostop
  default: Todo(opcode);
 }
}

// Lecture EF9345 /////////////////////////////////////////////////////////////
char ReadEF9345(char r)
{
 if(r & 8) ExecEF9345(EF9345reg[0] & 0xff); //execution d'une commande
 if(r & 7) return EF9345reg[r & 7];         //retour du registre

 //Positionnement du status
 //reste a affiner le calcul des bits S4(LXa), S5(LXm), S6(Al)
 //toutes les commandes ne les positionnent pas
 //verifier si le calcul du status doit etre effectue
 //avant ou apres l'execution de la commande
 //S0 et S1 inutilises et laisses a zero
 //S2=vertical synchronization signal state (masquable par commande VRM)
 //(a programmer)
 //S3=MSB value of R1 positionne uniquement par commande KRV
 //       if(EF9345reg[1] & 0x80) EF9345status |= 0x08; 
 //S4=LXa et S5=LXm positionnes en debut de commande
 //       et remis a zero par IND, MVB, MVD, MVT, VSM, INY
 //S5=Al  positionne lors de l'incrementation : fonction Incrx()
 //       et remis a zero par IND, MVB, MVD, MVT, VSM, INY

 if(EF9345cycle > 0) EF9345status |= 0x80; else EF9345status &= 0x7f; //S7=busy       
 return EF9345status; //retour du status
}

// Ecriture EF9345 ////////////////////////////////////////////////////////////
void WriteEF9345(char r, char c)
{
 EF9345reg[r & 7] = c;                      //mise à jour du registre  
 if(r & 8) ExecEF9345(EF9345reg[0] & 0xff); //execution d'une commande
}
