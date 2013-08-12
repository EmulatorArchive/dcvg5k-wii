//////////////////////////////////////////////////////////////////////////////
// DCVG5KBOUTONS.H - definition des boutons
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

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const int rmask = 0xff000000, gmask = 0x00ff0000;
const int bmask = 0x0000ff00, amask = 0x000000ff;
const int blanc = 0xffffffff, noir  = 0x000000ff;
const int bleu  = 0x202060ff, grist = 0xe0e0e0e4;
const int gris0 = 0xe0e0e0ff;
const int gris1 = 0xd0d0d0ff;
const int gris2 = 0x808080ff;
const int gris3 = 0x606060ff;
#else
const int rmask = 0x000000ff, gmask = 0x0000ff00;
const int bmask = 0x00ff0000, amask = 0xff000000;
const int blanc = 0xffffffff, noir  = 0xff000000;
const int bleu  = 0xff602020, grist = 0xe4e0e0e0;
const int gris0 = 0xffe0e0e0;
const int gris1 = 0xffd0d0d0;
const int gris2 = 0xff808080;
const int gris3 = 0xff606060;
#endif
 
const button bouton[BOUTON_MAX] = {
{"[X]",      16,  14}, //00 
{"[prev]",   16,   8}, //01 
{"[next]",   16,   8}, //02 
{"",          0,   0}, //03 
{"",          0,   0}, //04 
{"",          0,   0}, //05
{"",          0,   0}, //06
{"",          0,   0}, //07
{"Options",  50,  16}, //08
{"?",        15,  16}, //09
{"",         10,  10}, //10
{"k7",       22,  16}, //11
{"fd",       22,  16}, //12
{"memo",     50,  16}, //13
{"{44}",     70,  20}, //14 
{"{45}",     70,  20}, //15
{"{36}",    214,  20}, //16
{"{37}",    214,  20}, //17
{"{38}",    214,  20}, //18
{"Action",   44,  18}, //19
{"list",     22, 16}, //00 list
{"   RESET", 52, 16}, //01 reset
{"Maj",      22, 16}, //02 leftshift
{"[gauche]", 22, 16}, //03 left
{"[droite]", 22, 16}, //04 right
{"[bas]",    22, 16}, //05 down
{"    CTRL", 52, 16}, //06 ctrl
{"insc",     22, 16}, //07 insc
{"run",      22, 16}, //08 run
{" Q",       22, 16}, //09 Q
{"ESPACE",  112, 16}, //0a espace
{"Lck",      22, 16}, //0b shiftlock
{"Maj",      22, 16}, //0c rightshift
{"     RET", 52, 16}, //0d return
{"[haut]",   22, 16}, //0e up
{" A",       22, 16}, //0f A
{" W",       22, 16}, //10 W
{" X",       22, 16}, //11 X
{" C",       22, 16}, //12 C
{" V",       22, 16}, //13 V
{" B",       22, 16}, //14 B
{"#1",       22, 16}, //15 1
{"* :",      22, 16}, //16 :
{" Z",       22, 16}, //17 Z
{" S",       22, 16}, //18 S
{" E",       22, 16}, //19 E
{"\" 3",     22, 16}, //1a 3
{"£ 4",      22, 16}, //1b 4
{"$ 5",      22, 16}, //1c 5
{"%6",       22, 16}, //1d 6
{"! 2",      22, 16}, //1e 2
{"@ ;",      22, 16}, //1f ;
{"_ /",      22, 16}, //20 ./.
{"| x",      22, 16}, //21 x
{" F",       22, 16}, //22 F
{" G",       22, 16}, //23 G
{" U",       22, 16}, //24 U
{" I",       22, 16}, //25 I
{" O",       22, 16}, //26 O
{" P",       22, 16}, //27 P
{"",         22, 16}, //28
{") 0",      22, 16}, //29 0
{" [ ]",     22, 16}, //2a ]
{"pi..",     22, 16}, //2b ..
{"/ ,",      22, 16}, //2c ,
{"& 7",      22, 16}, //2d 7
{"/ 8",      22, 16}, //2e 8 
{"( 9",      22, 16}, //2f 9
{"? -",      22, 16}, //30 -
{". +",      22, 16}, //31 +
{" R",       22, 16}, //32 R
{" T",       22, 16}, //33 T
{" Y",       22, 16}, //34 Y
{"><",       22, 16}, //35 <
{"prt",      22, 16}, //36 prt
{" D",       22, 16}, //37 D
{"^ =",      22, 16}, //38 =
{"effc",     22, 16}, //39 effc
{" N",       22, 16}, //3a N
{" J",       22, 16}, //3b J
{" H",       22, 16}, //3c H
{" K",       22, 16}, //3d K
{" L",       22, 16}, //3e L
{" M",       22, 16}  //3f M
};

const dialogbutton closebutton = {-21, 5, 0};

const dialogbutton statusbutton[STATUSBUTTON_MAX] = {
{  0,  2, 11}, //0x00 "k7",       
{-69,  2,  8}, //0x01 "Options",  
{-17,  2,  9}  //0x02 "?",        
};

const dialogbutton optionbutton[OPTIONBUTTON_MAX] = {
{208,  32,  1}, //0x00 "[prev]",  
{208,  40,  2}, //0x01 "[next]",  
{208,  52,  1}, //0x02 "[prev]",  
{208,  60,  2}, //0x03 "[next]",  
{208,  72,  1}, //0x04 "[prev]",  
{208,  80,  2}, //0x05 "[next]", 
{208,  92,  1}, //0x06 "[prev]",  
{208, 100,  2}, //0x07 "[next]", 
{ 10, 120, 18}  //0x08 "defaut", 
};

const dialogbutton desassbutton[DESASSBUTTON_MAX] = {
{10, 30,  1}, //0x00 "[prev]",   
{10, 54,  2}, //0x01 "[next]",   
{28, 30,  1}, //0x02 "[prev]",   
{28, 54,  2}, //0x03 "[next]",   
{46, 30,  1}, //0x04 "[prev]",   
{46, 54,  2}, //0x05 "[next]",  
{64, 30,  1}, //0x06 "[prev]",   
{64, 54,  2}, //0x07 "[next]",  
{10, 70, 14}, //0x08 "[debut]", 
{10, 95, 15}  //0x09 "[suite]", 
};

const dialogbutton keyboardbutton[KEYBOARDBUTTON_MAX] = {
{430,  57, 20}, //00 list
{400,  30, 21}, //01 reset
{ 40, 165, 22}, //02 leftshift
{400, 165, 23}, //03 <
{430, 165, 24}, //04 >
{430, 138, 25}, //05 v
{ 10,  30, 26}, //06 ctrl
{430, 111, 27}, //07 insc
{430,  84, 28}, //08 run
{ 24, 111, 29}, //09 Q
{ 84, 165, 30}, //0a espace
{220, 138, 31}, //0b shiftlock
{220, 165, 32}, //0c rightshift
{280, 138, 33}, //0d return
{400, 138, 34}, //0e ^
{ 10,  84, 35}, //0f A
{ 40, 138, 36}, //10 W
{ 70, 138, 37}, //11 X
{100, 138, 38}, //12 C
{130, 138, 39}, //13 V
{160, 138, 40}, //14 B
{ 70,  30, 41}, //15 1
{ 54,  57, 42}, //16 :
{ 40,  84, 43}, //17 Z
{ 54, 111, 44}, //18 S
{ 70,  84, 45}, //19 E
{130,  30, 46}, //1a 3
{160,  30, 47}, //1b 4
{190,  30, 48}, //1c 5
{220,  30, 49}, //1d 6
{100,  30, 50}, //1e 2
{ 84,  57, 51}, //1f ;
{400,  84, 52}, //20 ./.
{370,  84, 53}, //21 x
{114, 111, 54}, //22 F
{144, 111, 55}, //23 G
{190,  84, 56}, //24 U
{220,  84, 57}, //25 I
{250,  84, 58}, //26 O
{280,  84, 59}, //27 P
{220,  23, 60}, //28
{340,  30, 61}, //29 0
{114,  57, 62}, //2a ]
{144,  57, 63}, //2b ..
{174,  57, 64}, //2c ,
{250,  30, 65}, //2d 7
{280,  30, 66}, //2e 8 
{310,  30, 67}, //2f 9
{400,  57, 68}, //30 -
{370,  57, 69}, //31 +
{100,  84, 70}, //32 R
{130,  84, 71}, //33 T
{160,  84, 72}, //34 Y
{204,  57, 73}, //35 <
{234,  57, 74}, //36 prt
{ 84, 111, 75}, //37 D
{370, 111, 76}, //38 =
{400, 111, 77}, //39 effc
{190, 138, 78}, //3a N
{204, 111, 79}, //3b J
{174, 111, 80}, //3c H
{234, 111, 81}, //3d K
{264, 111, 82}, //3e L
{294, 111, 83}, //3f M
{ 10, 249, 16}, //0x00 "[defaut]",   
{238, 249, 17}  //0x01 "[sauver]",   
};
 
const dialogbutton joystickbutton[JOYSTICKBUTTON_MAX] = {
{135,  30, 34}, //0x00 "[0] haut",    
{160,  55, 24}, //0x01 "[0] droite",  
{135,  80, 25}, //0x02 "[0] bas",     
{110,  55, 23}, //0x03 "[0] gauche",  
{123, 112, 19}, //0x04 "[0] action",  
{  0,   0,  3}, //0x05 "[0] action",  
{  0,   0,  3}, //0x06 "[0] action",  
{  0,   0,  3}, //0x07 "[0] action",  
{293,  30, 34}, //0x08 "[1] haut",    
{318,  55, 24}, //0x09 "[1] droite",  
{293,  80, 25}, //0x0a "[1] bas",     
{268,  55, 23}, //0x0b "[1] gauche",  
{281, 112, 19}, //0x0c "[1] action",  
{  0,   0,  3}, //0x0d "[1] action",  
{  0,   0,  3}, //0x0e "[1] action",  
{  0,   0,  3}, //0x0f "[1] action",  
{ 10, 209, 16}, //0x10 "[defaut]",    
{228, 209, 17}  //0x11 "[sauver]",    
};
