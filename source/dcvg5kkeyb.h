//////////////////////////////////////////////////////////////////////////////
// DCVG5KKEYB.H - VG5000 keyboard definition
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


int pckeycode[KEYBOARDKEY_MAX]=
{
 0x47, //00 list
 0x2a, //01 reset
 0xe1, //02 leftshift
 0x4b, //03 <
 0x4d, //04 >
 0x50, //05 v
 0xe0, //06 ctrl
 0x52, //07 insc
 0x4f, //08 run
 0x04, //09 Q
 0x2c, //0a espace
 0x39, //0b shiftlock
 0xe5, //0c rightshift
 0x28, //0d return   
 0x48, //0e ^
 0x14, //0f A
 0x1d, //10 W
 0x1b, //11 X
 0x06, //12 C
 0x19, //13 V
 0x05, //14 B
 0x1e, //15 1
 0x37, //16 :
 0x1a, //17 Z
 0x16, //18 S
 0x08, //19 E
 0x20, //1a 3
 0x21, //1b 4
 0x22, //1c 5
 0x23, //1d 6
 0x1f, //1e 2
 0x36, //1f ;
 0x3b, //20 ./.
 0x32, //21 x
 0x09, //22 F
 0x0a, //23 G
 0x18, //24 U
 0x0c, //25 I
 0x12, //26 O
 0x13, //27 P
 0x29, //28
 0x27, //29 0
 0x38, //2a ]
 0x30, //2b ..
 0x10, //2c ,
 0x24, //2d 7
 0x25, //2e 8 
 0x26, //2f 9
 0x2d, //30 -
 0x2e, //31 +
 0x15, //32 R
 0x17, //33 T
 0x1c, //34 Y
 0x64, //35 <
 0xe4, //36 prt
 0x07, //37 D
 0x1a, //38 =
 0x53, //39 effc
 0x11, //3a N
 0x0d, //3b J
 0x0b, //3c H
 0x0e, //3d K
 0x0f, //3e L
 0x33  //3f M
};

//scancode hardware du PC Windows pour chaque fonction des manettes VG5000
//(+ 0x40 pour les touches du pave numerique pour les distinguer des autres)
int pcjoycode[JOYSTICKKEY_MAX]=
{
 0x94, //0 manette 0 HAUT 
 0x99, //1 manette 0 DROITE 
 0x98, //2 manette 0 BAS
 0x97, //3 manette 0 GAUCHE
 0xa8, //4 manette 0 ACTIONA
 0xff, //5 manette 0 ACTIONB
 0xff, //6 manette 0 ACTIONC
 0xff, //7 manette 0 ACTIOND
 0xaa, //8 manette 1 HAUT 
 0x72, //9 manette 1 DROITE
 0x90, //a manette 1 BAS
 0x8f, //b manette 1 GAUCHE
 0x96, //c manette 1 ACTIONA
 0xff, //d manette 1 ACTIONB
 0xff, //e manette 1 ACTIONC
 0xff  //f manette 1 ACTIOND
};


