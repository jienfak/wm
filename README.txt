# wm
Windows mover. It can move windows.
Compile it with something smaller than GCC.
To modify keybindings look 'defs.h'.
My wrappers for standard commands you can find in "github.com/jienfak/etc/alias".
Based on TinyWM.
Usage:
	Every key is used with MODKEY only. Default MODKEY is 'Super' key.
	With pointed window:
		Button1 - drag: interactively move pointed window.
		Button2 - focus input on pointed window.
		Shift+Button2 - close current window.
		MouseWheelUp - raise pointed window in the stack.
		MouseWheeDown - lower pointed window in the stack.
		Button3 - drag: interactive window resize.
	Without pointed window:
		MouseWheelUp - scroll desktop up.
		MouseWheelDown - scroll desktop down.
		Shift+MouseWheelUp - scroll to the right.
		Shift+MouseWheelDown - scroll to the left.
		Button2 - call the "fallmenu_scripts" wrapper.
	Keybindings:
		Tab - set keyboard layout to "$DVORAK_KEYBOARD_LAYOUT".
		a - set keyboard layout to "QWERTY_KEYBOARD_LAYOUT".
		Return - set keyboard layout to "$DVP_KEYBOARD_LAYOUT" .
		Backspace - set keyboard layout to "$NATIVE_KEYBOARD_LAYOUT".
		Space - call "menu_cmd" wrapper.
		Esc - quit WM.

	
