/* TinyWM is written by Nick Welch <mack@incise.org>, 2005.
 *  Window Mover is written by Jien <jienfak@protonmail.com>, 2019.
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

/* One day I understood I don't even use many features of DWM, 
which is really small. Now I want to have REALLY small window manager.
I want it to be as fast as possible. As small as possible. 
I want to easy compile it everywhere. Everything what WM should do
is MANAGING WINDOWS, no FUCKING BARS! If you need bars - 
write separated program for that. Don't implement it in WM, BITCH. 
2000 lines are too many. 50 KB with TCC? WHAT?!  50?]
Are you fucking joking? I need it in smaller! Much smaller.
So get it! */

#include <X11/Xlib.h>
#include <X11/cursorfont.h> 
#include <stdlib.h>
#include <stdio.h>

#define MODKEY (Mod4Mask)
#define XKILL_HOTKEY_STR ("Escape")
#define XMENU_CMD_HOTKEY_STR ("space")
#define DVORAK_HOTKEY_STR ("Tab")
#define DVP_HOTKEY_STR ("Return")
#define QWERTY_HOTKEY_STR ("a")
#define NATIVE_HOTKEY_STR ("Backspace")
#define MAX(a, b) ((a) > (b) ? (a) : (b))

unsigned int strkey(Display *dpy, char *str){
	return XKeysymToKeycode(dpy, XStringToKeysym(str)) ;
}

unsigned int grabkey(Display *dpy, Window rw, char *str){
	unsigned int key = strkey(dpy,str) ;
	XGrabKey( dpy, key, MODKEY, rw,
		True, GrabModeAsync, GrabModeAsync );
	return key ;
}

int main(int argc, char argv[]){
	/* Main display. */
	Display * dpy;
	/* Root window. */
	Window rw;
	/* Cursor shape object. */
	Cursor cur;
	/* Buffer to get window attributes. */
	XWindowAttributes attr;
	/* Buffer to remember delta cursor-X and cursor-Y. */
	XButtonEvent start;
	/* Buffer for X11 events. */
	XEvent ev;

	if(!(dpy = XOpenDisplay(0x0))) return 1;

	rw = DefaultRootWindow(dpy) ;

	/* XGrabKey( dpy, , Mod1Mask, rw,
          	True, GrabModeAsync, GrabModeAsync ); */
	/* Main bindings. */
	unsigned int menu_cmd_key= grabkey(dpy, rw, XMENU_CMD_HOTKEY_STR) ;
	unsigned int xkill_key = grabkey(dpy, rw, XKILL_HOTKEY_STR) ;
	unsigned int dvorak_key = grabkey(dpy, rw, DVORAK_HOTKEY_STR) ;
	unsigned int dvp_key = grabkey(dpy, rw, DVP_HOTKEY_STR) ;
	unsigned int qwerty_key = grabkey(dpy, rw, QWERTY_HOTKEY_STR) ;
		unsigned int native_key = grabkey(dpy, rw, NATIVE_HOTKEY_STR) ;

	/* Mouse bindings. */
	XGrabButton( dpy, 1, MODKEY, rw, True, ButtonPressMask, GrabModeAsync,
          	GrabModeAsync, None, None );
	XGrabButton(dpy, 2, MODKEY,  rw, True, ButtonPressMask, GrabModeAsync,
		GrabModeAsync, None, None );
	XGrabButton( dpy, 3, MODKEY, rw, True, ButtonPressMask, GrabModeAsync,
		GrabModeAsync, None, None );
	/* Cursor creating. */
	cur = XCreateFontCursor(dpy, XC_left_ptr) ;
	XDefineCursor(dpy, rw, cur); 
	for(;;){
		XNextEvent(dpy, &ev);
		if( ev.type == KeyPress /* && ev.xkey.subwindow != None */ ){
			/* Super key is pressed. */ 
			unsigned int key = ev.xkey.keycode ;
			if( key == menu_cmd_key){
					/* The only way to call programs. */
					system("menu_cmd&");
			}else if( key == xkill_key ){
					/* The only way to kill programs. */
					system("xkill -frame&");
			}else if( key == dvorak_key ){
					system("setxkbmap $DVORAK_KEYBOARD_LAYOUT &");
			}else if( key == dvp_key ){
					system("setxkbmap $DVP_KEYBOARD_LAYOUT &");
			}else if( key == native_key ){
					system("setxkbmap $NATIVE_KEYBOARD_LAYOUT &");
			}else if( key == qwerty_key ){
					system("setxkbmap $QWERTY_KEYBOARD_LAYOUT &");
			}
			system("xmodmap $XMODMAP");
		}else if(ev.type == ButtonPress && ev.xbutton.subwindow != None){
				if ( ev.xbutton.button == 2 ){
					/* If middle button pressed. */
					/* Raising window. */
					XRaiseWindow(dpy, ev.xkey.subwindow);
				}
				/* Right or left button is pressed. */
				XGrabPointer( dpy, ev.xbutton.subwindow, True,
					PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
				GrabModeAsync, None, cur, CurrentTime );
				XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
				start = ev.xbutton;
		
		} else if( ev.type == MotionNotify ) {
			/* Window resizing. */
			int xdiff, ydiff;
			while(XCheckTypedEvent(dpy, MotionNotify, &ev))
				;
			xdiff = ev.xbutton.x_root - start.x_root ;
			ydiff = ev.xbutton.y_root - start.y_root ;
			XMoveResizeWindow(dpy, ev.xmotion.window,
				attr.x + (start.button==1 ? xdiff : 0),
				attr.y + (start.button==1 ? ydiff : 0 ),
				MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
				MAX(1, attr.height + (start.button==3 ? ydiff : 0)) );
		} else if(ev.type == ButtonRelease) {
			XUngrabPointer(dpy, CurrentTime);
		}
	}
	XCloseDisplay(dpy);
	return 0 ;
}
