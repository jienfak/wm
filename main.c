/* TinyWM is written by Nick Welch <mack@incise.org>, 2005.
 *  Window Mover is written by Jien <jienfak@protonmail.com>, 2019.
 * This software is in the public domain
 * and is provided AS IS, with NO WARRANTY. */

#include <X11/Xlib.h>
#include <X11/cursorfont.h> 
#include <stdlib.h>

#define MODKEY (Mod4Mask)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main()
{
	Cursor cur;
	Display * dpy;
	Window rw;
	XWindowAttributes attr;
	XButtonEvent start;
	XEvent ev;

	if(!(dpy = XOpenDisplay(0x0))) return 1;

	rw = DefaultRootWindow(dpy);

	/* XGrabKey( dpy, XKeysymToKeycode(dpy, XStringToKeysym("F1")), Mod1Mask, rw,
          	True, GrabModeAsync, GrabModeAsync ); */

	/* Main bindings. */
	XGrabButton( dpy, 1, MODKEY, rw, True, ButtonPressMask, GrabModeAsync,
          	GrabModeAsync, None, None );
	XGrabButton(dpy, 2, MODKEY,  rw, True, ButtonPressMask, GrabModeAsync,
		GrabModeAsync, None, None);
	XGrabButton( dpy, 3, MODKEY, rw, True, ButtonPressMask, GrabModeAsync,
		GrabModeAsync, None, None );
	/* Cursor creating. */
	cur = XCreateFontCursor(dpy, XC_left_ptr) ;
	XDefineCursor(dpy, rw, cur); 
	for(;;){
		XNextEvent(dpy, &ev);
		if( ev.type == KeyPress && ev.xkey.subwindow != None ){
		}else if(ev.type == ButtonPress && ev.xbutton.subwindow != None){
			if( ev.xbutton.button == 2 ){
				XRaiseWindow(dpy, ev.xkey.subwindow);
				continue;
			}
			XGrabPointer( dpy, ev.xbutton.subwindow, True,
				PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
				GrabModeAsync, None, cur, CurrentTime );
			XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
			start = ev.xbutton;
		} else if( ev.type == MotionNotify ) {
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
}