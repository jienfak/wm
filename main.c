/* It moves windows and a bit more.*/

/* TinyWM is written by Nick Welch <mack@incise.org>, 2005.
 *  Window Mover is written by Jien <jienfak@protonmail.com>, 2019.
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

/* WM with all features I really use. Nothing more. */
/* To make it work well you should implement a few
	wrappers(My wrappers are in 'github.com/jienfak/etc/aliases'):
		1) closecurwin : closes current window. 
		2) menu_cmd : runs input command by shell.
		3) fallmenu_scripts : runs scripts from '$HOME/.scripts' or something.*/

#include <X11/Xlib.h>
#include <X11/cursorfont.h> 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "defs.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))


enum{ MouseEmptyFlag,
	MouseWinMoveFlag,
	MouseWinResizeFlag,
	MouseAllWinsMoveFlag,
	MouseFlagsLength};

void setonlyflag(bool flags[], int len, int flag){
	for( int i=0 ; i<len ; ++i ){ flags[i] = false ; }
	flags[flag] = true ;
}
unsigned int strkey(Display *dpy, char *str){
	return XKeysymToKeycode(dpy, XStringToKeysym(str)) ;
}

unsigned int grabkey( Display *dpy, Window rw, char *str){
	unsigned int key = strkey(dpy,str) ;
	XGrabKey( dpy, key, MODKEY, rw,
		True, GrabModeAsync, GrabModeAsync );
	return key ;
}

void grabbutton(Display *dpy, Window rw, int button){
	XGrabButton( dpy, button, MODKEY, rw, True, ButtonPressMask|ButtonReleaseMask, GrabModeAsync,
          	GrabModeAsync, None, None );
	XGrabButton( dpy, button, MODKEY|ShiftMask, rw, True, ButtonPressMask|ButtonReleaseMask, GrabModeAsync,
          	GrabModeAsync, None, None );
}

void movewinarr(Display *dpy, Window wins[], int len, int xdiff, int ydiff){
	XWindowAttributes attr;
	for( int i=0 ; i<len ; ++i ){
		XGetWindowAttributes(dpy, wins[i], &attr);
		XMoveWindow(dpy, wins[i], attr.x+xdiff, attr.y+ydiff);
	}
}

int moveallwins(Display *dpy, Window rw, int xdiff, int ydiff){
	Window dw;
	Window *wins;
	unsigned int n;
	if(! XQueryTree(dpy, rw, &dw, &dw,&wins, &n ) ){ return 1; }
	movewinarr(dpy, wins, n, xdiff, ydiff);
	XFree(wins);
	return 0 ;
}

int main(int argc, char argv[]){
	/* Main display. */
	Display * dpy;
	/* Root window. */
	Window rw;
	/* Cursor shape object. */
	Cursor cur;
	int but;
	bool mouse_flags[MouseFlagsLength] = { false, false } ;
	Window sw;
	/* Buffer to get window attributes. */
	XWindowAttributes attr;
	/* Buffer to remember delta cursor-X and cursor-Y. */
	XButtonEvent start;
	/* Buffer for X11 events. */
	XEvent ev;
	/* Buffer for key pressing. */
	unsigned int key,
		state;
	/* Buffer for dedlta coordinates. */
	int xdiff, ydiff;

	if(! (dpy = XOpenDisplay(NULL)) ){
		fprintf(stderr, "%s: Can't open display\n", argv[0]);
		return EXIT_FAILURE ;
	}

	rw = DefaultRootWindow(dpy) ;

	/* Application runner and keyboard layout keybindings. */
	unsigned int menu_cmd_key= grabkey(dpy, rw, XMENU_CMD_HOTKEY_STR) ,
		dvorak_key = grabkey(dpy, rw, DVORAK_HOTKEY_STR) ,
		dvp_key = grabkey(dpy, rw, DVP_HOTKEY_STR) ,
		qwerty_key = grabkey(dpy, rw, QWERTY_HOTKEY_STR) ,
		native_key = grabkey(dpy, rw, NATIVE_HOTKEY_STR) ,
		quit_wm_key = grabkey(dpy, rw, QUIT_WM_HOTKEY_STR)
	;
	

	/* Mouse bindings. */
	grabbutton(dpy, rw, 1);
	grabbutton(dpy, rw, 2);
	grabbutton(dpy, rw, 3);
	grabbutton(dpy, rw, 4);
	grabbutton(dpy, rw, 5);

	/* Cursor creating and defining. */
	cur = XCreateFontCursor(dpy, XC_left_ptr) ;
	XDefineCursor(dpy, rw, cur); 
	for(;;){
		XNextEvent(dpy, &ev);
		switch( ev.type ){
		case KeyPress :
			/* Super key is pressed. */ 
			key = ev.xkey.keycode ;
			sw = ev.xkey.subwindow ;
			if( key == menu_cmd_key){
				/* The only way to call programs. */
				system("menu_cmd &");
			}else if( key == dvorak_key ){
				system("setxkbmap $DVORAK_KEYBOARD_LAYOUT ; xmodmap $XMODMAP ");
			}else if( key == dvp_key ){
				system("setxkbmap $DVP_KEYBOARD_LAYOUT ; xmodmap $XMODMAP ");
			}else if( key == native_key ){
				system("setxkbmap $NATIVE_KEYBOARD_LAYOUT ; xmodmap $XMODMAP ");
			}else if( key == qwerty_key ){
				system("setxkbmap $QWERTY_KEYBOARD_LAYOUT ; xmodmap $XMODMAP");
			}else if( key == quit_wm_key ){
				goto success_exit ;
			}
		break;

		case ButtonPress :
			but = ev.xbutton.button ;
			sw = ev.xbutton.subwindow ;
			state = ev.xbutton.state ;
			if(sw != None){
				XGrabPointer( dpy, sw, True,
					PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
					GrabModeAsync, None, cur, CurrentTime );
				XGetWindowAttributes(dpy, sw, &attr);
			}
			start = ev.xbutton ;
			switch( but ){
			case 1 :
				if( sw != None ){
					setonlyflag(mouse_flags, MouseFlagsLength, MouseWinMoveFlag);
				}else{
					setonlyflag(mouse_flags, MouseFlagsLength, MouseAllWinsMoveFlag);
				}
			break;

			case 2 :
				if( sw != None ){
					if( state&ShiftMask ){
						system("closecurwin &");
					}else{
						XSetInputFocus(dpy, sw, RevertToPointerRoot, CurrentTime);
					}
				}else{
					system("fallmenu_scripts &");
				}
			break;

			case 3 :
				if( sw != None ){
					setonlyflag(mouse_flags, MouseFlagsLength, MouseWinResizeFlag);
				}else{
					system("fallmenu_scripts &");
				}
			break;

			case 4 :
				if( sw != None ){
					XRaiseWindow(dpy, sw);
				}else{
					if( state&ShiftMask ){
						moveallwins(dpy, rw, DESKTOP_SCROLLING_SPEED, 0);
					}else{
						moveallwins(dpy, rw, 0, DESKTOP_SCROLLING_SPEED);
					}
				}
			break;
					
			case 5 :
				if( sw != None ){
					XLowerWindow(dpy, sw);
				}else{
					if( state&ShiftMask ){
						moveallwins(dpy, rw, -DESKTOP_SCROLLING_SPEED, 0);

					}else{
						moveallwins(dpy, rw, 0, -DESKTOP_SCROLLING_SPEED);
					}
				}
			break;

			}
		break;

		case ButtonRelease :
			but = ev.xbutton.button ;
			sw = ev.xbutton.subwindow ;

			switch(but){
			case 1 :
				if( sw != None ){
					setonlyflag(mouse_flags, MouseFlagsLength, MouseEmptyFlag) ;
				}else{
				}
			break;

			case 2 :
				if( sw != None){
				}else{
				}
			break;

			case 3 :
				if( sw != None ){
					setonlyflag(mouse_flags, MouseFlagsLength, MouseEmptyFlag) ;
				}else{
				}
			break;

			case 4 :
				if( sw != None ){
				}else{
				}
			break;
					
			case 5 :
				if( sw != None ){
				}else{
				}
			break;
			}
			XUngrabPointer(dpy, CurrentTime);
		break;

		case MotionNotify :
			while(XCheckTypedEvent(dpy, MotionNotify, &ev))
				/* Buffer skipping. */ ;
			sw = ev.xmotion.window ;
			xdiff = ev.xbutton.x_root - start.x_root ;
			ydiff = ev.xbutton.y_root - start.y_root ;
			if( mouse_flags[MouseWinMoveFlag] ){
				/* Moving about just one window. */
				XMoveWindow(dpy, sw, attr.x+xdiff, attr.y+ydiff);
			}else if( mouse_flags[MouseWinResizeFlag]){
				/* Resizing with protection from oversizing. */
				XResizeWindow(dpy, sw,
					MAX(1, attr.width+xdiff ),
					MAX(1, attr.height+ydiff) );
			}else if( mouse_flags[MouseAllWinsMoveFlag]) {
				/* The way we can move all the desktop, so we don't need 
					multiple work areas, we have just one
				 	global. */
				moveallwins(dpy, rw, xdiff, ydiff);
			}
		break;
		}
	}
	success_exit:
	XCloseDisplay(dpy);
	return EXIT_SUCCESS ;
}
