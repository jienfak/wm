/* It moves windows and a bit more.*/

/* TinyWM is written by Nick Welch <mack@incise.org>, 2005.
	Window Mover is written by Jien <jienfak@protonmail.com>, 2019.
	This software is in the public domain
	and is provided AS IS, with NO WARRANTY. */

/* Some functions are taken from the DWM. Thanks guys from 'suckless' project. */

/* WM with all features I really use. Nothing more. */
/* To make it work well you should implement a few
	wrappers(My wrappers are in 'github.com/jienfak/etc/aliases'):
		1) closecurwin : closes current window. 
		2) menu_cmd : runs input command by shell.
		3) fallmenu_scripts : runs scripts from '$HOME/.scripts' or something.*/

#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h> 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "defs.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Default atoms. */
enum { WMProtocols,
	WMDelete, WMState,
	WMTakeFocus,
	WMLast } ;
/* EWMH atoms. */
enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
       NetWMFullscreen, NetActiveWindow, NetWMWindowType,
       NetWMWindowTypeDialog, NetClientList, NetLast };
/* Mouse flags. */
enum { MouseEmptyFlag,
	MouseWinMoveFlag,
	MouseWinResizeFlag,
	MouseAllWinsMoveFlag,
	MouseFlagsLast };

int xerrordummy(Display *dpy, XErrorEvent *ee){
	return 0 ;
}

int xerror(Display *dpy, XErrorEvent *ee){
	if (ee->error_code == BadWindow
			|| (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
			|| (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
			|| (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
			|| (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
			|| (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
			|| (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
			|| (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
			|| (ee->request_code == X_CopyArea && ee->error_code == BadDrawable) ){
		return 0;
	}
	fprintf( stderr, "wm: Fatal error: Request code=%d, error code=%d.\n",
		ee->request_code, ee->error_code );
	return xerror(dpy, ee) ; /* May call exit. */
}

int sendevent(Display *dpy, Window win, Atom *wmatom, Atom proto){
	int n;
	Atom *protocols;
	int exists = 0 ;
	XEvent ev;

	if(XGetWMProtocols(dpy, win, &protocols, &n)){
		while (!exists && n--)
			exists = protocols[n] == proto ;
		XFree(protocols);
	}
	if(exists){
		ev.type = ClientMessage ;
		ev.xclient.window = win ;
		ev.xclient.message_type = wmatom[WMProtocols] ;
		ev.xclient.format = 32 ;
		ev.xclient.data.l[0] = proto;
		ev.xclient.data.l[1] = CurrentTime ;
		XSendEvent(dpy, win, False, NoEventMask, &ev) ;
	}
	return exists ;
}

void killwin(Display *dpy, Window win, Atom *wmatom){
	if (!sendevent(dpy, win, wmatom, wmatom[WMDelete])) {
		XGrabServer(dpy);
		XSetErrorHandler(xerrordummy);
		XSetCloseDownMode(dpy, DestroyAll);
		XKillClient(dpy, win);
		XSync(dpy, False);
		XSetErrorHandler(xerror);
		XUngrabServer(dpy);
	}
}

int xerrorotherwm(Display *dpy, XErrorEvent *ee){
	/* Startup Error handler to check if another window manager
		is already running. */
	fputs(stderr, "wm: Another window manager is already running.");
	exit(1);
	return -1 ;
}

void checkotherwm(Display *dpy){
	XSetErrorHandler(xerrorotherwm);
	/* This causes an error if some other window manager is running */
	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
	XSync(dpy, False);
	XSetErrorHandler(xerror);
	XSync(dpy, False);
}

void setfocus(Display *dpy, Window root, Window win, Atom *wmatom, Atom *netatom){
	XSetInputFocus(dpy, win, RevertToPointerRoot, CurrentTime);
	XChangeProperty(dpy, root, netatom[NetActiveWindow],
		XA_WINDOW, 32, PropModeReplace,
		(unsigned char *) &(win), 1);
	sendevent(dpy, win, wmatom, wmatom[WMTakeFocus]);
}

void setonlyflag(bool flags[], int len, int flag){
	for( int i=0 ; i<len ; ++i ){ flags[i] = false ; }
	flags[flag] = true ;
}
unsigned int strkey(Display *dpy, char *str){
	return XKeysymToKeycode(dpy, XStringToKeysym(str)) ;
}

unsigned int grabmodkey( Display *dpy, Window rw, char *str){
	unsigned int key = strkey(dpy,str) ;
	XGrabKey( dpy, key, MODKEY, rw,
		True, GrabModeAsync, GrabModeAsync );
	return key ;
}

void grabmodbutton(Display *dpy, Window rw, int button){
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
	/* Atoms. */
	Atom wmatom[WMLast], netatom[NetLast];
	/* Root window. */
	Window rw;
	/* Cursor shape object. */
	Cursor cur;
	int but;
	bool mouse_flags[MouseFlagsLast];
	setonlyflag(mouse_flags, MouseFlagsLast, MouseEmptyFlag);
	/* Subwindow. */
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
	/*checkotherwm(dpy);*/
	wmatom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", False) ;
	wmatom[WMDelete] = XInternAtom(dpy, "WM_DELETE_WINDOW", False) ;
	wmatom[WMState] = XInternAtom(dpy, "WM_STATE", False) ;
	wmatom[WMTakeFocus] = XInternAtom(dpy, "WM_TAKE_FOCUS", False);
	netatom[NetActiveWindow] = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False) ;
	netatom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", False) ;
	netatom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", False) ;
	netatom[NetWMState] = XInternAtom(dpy, "_NET_WM_STATE", False) ;
	netatom[NetWMCheck] = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False) ;
	netatom[NetWMFullscreen] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False) ;
	netatom[NetWMWindowType] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False) ;
	netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False) ;
	netatom[NetClientList] = XInternAtom(dpy, "_NET_CLIENT_LIST", False) ;
	/* Application runner and keyboard layout keybindings. */
	unsigned int menu_cmd_key= grabmodkey(dpy, rw, XMENU_CMD_HOTKEY_STR) ,
		dvorak_key = grabmodkey(dpy, rw, DVORAK_HOTKEY_STR) ,
		dvp_key = grabmodkey(dpy, rw, DVP_HOTKEY_STR) ,
		qwerty_key = grabmodkey(dpy, rw, QWERTY_HOTKEY_STR) ,
		native_key = grabmodkey(dpy, rw, NATIVE_HOTKEY_STR) ,
		quit_wm_key = grabmodkey(dpy, rw, QUIT_WM_HOTKEY_STR)
	;
	

	/* Mouse bindings. */
	grabmodbutton(dpy, rw, 1);
	grabmodbutton(dpy, rw, 2);
	grabmodbutton(dpy, rw, 3);
	grabmodbutton(dpy, rw, 4);
	grabmodbutton(dpy, rw, 5);

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
					setonlyflag(mouse_flags, MouseFlagsLast, MouseWinMoveFlag);
				}else{
					setonlyflag(mouse_flags, MouseFlagsLast, MouseAllWinsMoveFlag);
				}
			break;

			case 2 :
				if( sw != None ){
					if( state&ShiftMask ){
						killwin(dpy, sw, wmatom);
					}else{
						setfocus(dpy, rw,  sw, wmatom, netatom);
					}
				}else{
					system("fallmenu_scripts &");
				}
			break;

			case 3 :
				if( sw != None ){
					setonlyflag(mouse_flags, MouseFlagsLast, MouseWinResizeFlag);
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
					setonlyflag(mouse_flags, MouseFlagsLast, MouseEmptyFlag) ;
				}else{
					setonlyflag(mouse_flags, MouseFlagsLast, MouseEmptyFlag);
				}
			break;

			case 2 :
				if( sw != None){
				}else{
				}
			break;

			case 3 :
				if( sw != None ){
					setonlyflag(mouse_flags, MouseFlagsLast, MouseEmptyFlag) ;
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
				XMoveWindow(dpy, sw, attr.x+xdiff, attr.y+ydiff);
			}else if( mouse_flags[MouseWinResizeFlag]){
				XResizeWindow(dpy, sw,
					MAX(1, attr.width+xdiff ),
					MAX(1, attr.height+ydiff) );
			}else if( mouse_flags[MouseAllWinsMoveFlag]) {
				moveallwins(dpy, rw, xdiff, ydiff);
			}
		break;
		}
	}
	success_exit:
	XCloseDisplay(dpy);
	return EXIT_SUCCESS ;
}
