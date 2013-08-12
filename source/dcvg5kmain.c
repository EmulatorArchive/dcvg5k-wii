//////////////////////////////////////////////////////////////////////////////
// DCVG5KMAIN.C - dcvg5k v11 emulator main program
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
#include <SDL/SDL_ttf.h> 
#include "../include/policettf.h"
#include "../include/dcvg5kicon.h"
#include "dcvg5kglobal.h"
#include "dcvg5kmsg.h"
#include <fat.h>
#include <gccore.h>
#include <wiikeyboard/keyboard.h>

// global variables //////////////////////////////////////////////////////////
const SDL_VideoInfo *videoinfo;
SDL_AudioSpec audio;
int initialisation =1;
extern int pausez80;
SDL_Rect bien_centre;
SDL_Joystick* joysticks[2];
extern void Joy(int joycode, int state);

// Joysticks initialization //////////////////////////////////////////////////
void Joyinit()
{
}

// Play sound ////////////////////////////////////////////////////////////////
void Playsound(void *udata, Uint8 *stream, int bufferlength)
{
 int i;
 int icycles; //nombre entier de cycles entre deux echantillons
 extern int sound, frequency;
 extern int Run(int n);
 icycles = 1814 * frequency / 10000;
 for(i = 0; i < bufferlength; i++)
 {
  if(pausez80) {stream[i] = 128; continue;}
  
 Run(icycles); stream[i] = sound + 128;
 }
}  

//Event loop /////////////////////////////////////////////////////////////////
void Eventloop()
{
 int curhat, joy_number;
 keyboard_event key;
 SDL_Event event;
 extern int xmouse, ymouse, xmove, ymove;
 extern void Mouseclick(), Dialogmove();
 extern void Resizescreen(int x, int y), Displayscreen();
 extern void Keydown(int keysym, int scancode), Keyup(int keysym, int scancode);

  while(1)
  {
   SDL_JoystickUpdate();
   
   if (KEYBOARD_GetEvent(&key)){
    if (key.type == KEYBOARD_PRESSED) Keydown(key.symbol, key.keycode);
    else {if (key.type == KEYBOARD_RELEASED) Keyup(key.symbol, key.keycode);}}
   
   SDL_PollEvent(&event);
   switch(event.type)
   {
    case SDL_VIDEORESIZE:
         Resizescreen(event.resize.w, event.resize.h); break;
    case SDL_MOUSEBUTTONDOWN:
         Mouseclick(); break;
    case SDL_MOUSEBUTTONUP:
         xmove = ymove = 0; break;
    case SDL_MOUSEMOTION:
         xmouse = event.motion.x - bien_centre.x ; ymouse = event.motion.y - bien_centre.y;
         if(xmove && ymove) Dialogmove(); break;	
    case SDL_QUIT: return;
    case SDL_JOYBUTTONDOWN:
         if (event.jbutton.button == 6) return;
         if (event.jbutton.button == 2 || event.jbutton.button == 3) // bouton action remplacé par bouton 1 et 2 de la Wiimote
            Joy(event.jbutton.which * 8 + 4, 0x00); break;
    case SDL_JOYBUTTONUP:
          if (event.jbutton.button == 2 || event.jbutton.button == 3) // bouton action remplacé par bouton 1 et 2 de la Wiimote
            Joy(event.jbutton.which * 8 + 4, 0x80); break;
    default: break;
   }

   for(joy_number = 0; joy_number < 2; joy_number++) // directions données par la Wiimote
   {
       int hat_number = SDL_JoystickNumHats(joysticks[joy_number]);

       for(curhat = 0; curhat < hat_number; curhat++)
       {
           int joystate = SDL_JoystickGetHat(joysticks[joy_number], curhat);

		   switch (joystate)
           {
		       case SDL_HAT_CENTERED:
		           Joy(joy_number * 8 + 0, 0x80);
                   Joy(joy_number * 8 + 1, 0x80);
                   Joy(joy_number * 8 + 2, 0x80);
                   Joy(joy_number * 8 + 3, 0x80);
               break;
               case SDL_HAT_UP:
                   Joy(joy_number * 8 + 0, 0x00);
                   Joy(joy_number * 8 + 1, 0x80);
                   Joy(joy_number * 8 + 2, 0x80);
                   Joy(joy_number * 8 + 3, 0x80);
               break;
                   
               break;
               case SDL_HAT_RIGHT:
                   Joy(joy_number * 8 + 0, 0x80);
                   Joy(joy_number * 8 + 1, 0x00);
                   Joy(joy_number * 8 + 2, 0x80);
                   Joy(joy_number * 8 + 3, 0x80);
             
               break;
               case SDL_HAT_DOWN:
                   Joy(joy_number * 8 + 0, 0x80);
                   Joy(joy_number * 8 + 1, 0x80);
                   Joy(joy_number * 8 + 2, 0x00);
                   Joy(joy_number * 8 + 3, 0x80);
               break;
               case SDL_HAT_LEFT:
                   Joy(joy_number * 8 + 0, 0x80);
                   Joy(joy_number * 8 + 1, 0x80);
                   Joy(joy_number * 8 + 2, 0x80);
                   Joy(joy_number * 8 + 3, 0x00);
               break;
               case SDL_HAT_RIGHTUP:
                   Joy(joy_number * 8 + 0, 0x00);
                   Joy(joy_number * 8 + 1, 0x00);
                   Joy(joy_number * 8 + 2, 0x80);
                   Joy(joy_number * 8 + 3, 0x80);
               break;
               case SDL_HAT_RIGHTDOWN:
                   Joy(joy_number * 8 + 0, 0x80);
                   Joy(joy_number * 8 + 1, 0x00);
                   Joy(joy_number * 8 + 2, 0x00);
                   Joy(joy_number * 8 + 3, 0x80);
               break;
               case SDL_HAT_LEFTUP:
                   Joy(joy_number * 8 + 0, 0x00);
                   Joy(joy_number * 8 + 1, 0x80);
                   Joy(joy_number * 8 + 2, 0x80);
                   Joy(joy_number * 8 + 3, 0x00);
               break;
               case SDL_HAT_LEFTDOWN:
                   Joy(joy_number * 8 + 0, 0x80);
                   Joy(joy_number * 8 + 1, 0x80);
                   Joy(joy_number * 8 + 2, 0x00);
                   Joy(joy_number * 8 + 3, 0x00);
               break;
               default: break;
           }
		}
   }
  }
}     

//Main program ///////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
 SDL_RWops *rw;
 unsigned char cursor[] = {
  128,0,192,0,160,0,144,0,136,0,132,0,130,0,129,0,128,128,128,64,128,32,129,240,
  137,0,152,128,164,128,196,64,130,64,2,32,1,32,1,16,0,176,0,192,0,0,0,0,128,0,
  192,0,224,0,240,0,248,0,252,0,254,0,255,0,255,128,255,192,255,224,255,240,255,
  0,231,128,199,128,131,192,3,192,3,224,1,224,1,240,0,240,0,192,0,0,0,0};
 extern int language;
 extern TTF_Font *mono10;
 extern TTF_Font *vera9;
 extern TTF_Font *vera11;
 extern int xclient, yclient, screencount;
 extern void Initbuttonsurfaces(), Initstatusbar();
 extern void Initoptions(), Saveoptions(), SDL_error(int n);
 extern void Init6809registerpointers(), Initfilenames(), Hardreset();
 extern void Resizescreen(int x, int y), Displayscreen(), Keyboardinit();
 
 //initialisations
 fatInitDefault();
 Joyinit();       //Joysticks initialization
 Initoptions();   //Option initialization
 Keyboardinit();  //Keyboard initialization
 Hardreset();     //Initialization


 //initialize SDL ttf
 if(TTF_Init() == -1) SDL_error(1);
 rw = SDL_RWFromMem(veramono, sizeof(veramono));
 mono10 = TTF_OpenFontRW(rw, 0, 10);
 rw = SDL_RWFromMem(verattf, sizeof(verattf));
 vera9 = TTF_OpenFontRW(rw, 0, 9);
 rw = SDL_RWFromMem(verattf, sizeof(verattf));
 vera11 = TTF_OpenFontRW(rw, 0, 11);
 
 //initialize SDL video and keyboard
 if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) SDL_error(2);
 atexit(SDL_Quit);
 
 if(KEYBOARD_Init(NULL)) atexit(SDL_Quit); 
 joysticks[0] = SDL_JoystickOpen(0);
 joysticks[1] = SDL_JoystickOpen(1);
 
 rw = SDL_RWFromMem(dcvg5kicon, sizeof(dcvg5kicon));
 SDL_WM_SetIcon(SDL_LoadBMP_RW(rw, 1), NULL);  //icone fenetre
 SDL_WM_SetCaption(msg[language][0], NULL);    //titre fenetre
 SDL_SetCursor(SDL_CreateCursor(cursor, cursor + 48, 16, 24, 0, 0)); //curseur
 SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
 
 Resizescreen(xclient, yclient + YSTATUS);
 screencount = 0;
  
 Initbuttonsurfaces(); //Initialisation des surfaces des boutons
 Initstatusbar();      //Initialisation de la surface de la barre de statut  
 Displayscreen();      //Affichage ecran avec la barre de statut
 
 //initialize SDL audio
 if(SDL_Init(SDL_INIT_AUDIO) < 0) SDL_error(3);
 audio.freq = 22050;
 audio.format = AUDIO_U8;
 audio.channels = 1;
 audio.samples = 1024; 
 audio.callback = Playsound;
 audio.userdata = NULL;
 if(SDL_OpenAudio(&audio, NULL) < 0 ) SDL_error(4);
 SDL_PauseAudio(0);    //Lance l'emulation

 //test events
 Eventloop();
 
 //quit
 pausez80 = 1;
 SDL_PauseAudio(1);
 Saveoptions();
 SDL_Delay(100);
 TTF_Quit();
 SDL_JoystickClose(joysticks[0]);
 SDL_JoystickClose(joysticks[1]);
 SDL_Quit();
 return 0;
}
