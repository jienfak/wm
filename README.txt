# wm
Windows mover. It can move windows.

Compile it with something smaller than GCC(I use TCC).
To modify keybindings look 'defs.h'.
My wrappers for standard commands you can find in "github.com/jienfak/etc/alias".
Based on TinyWM. Some code is taken from the DWM. Suckless team - thanks.
Usage:
	Every key is used with MODKEY only. Default MODKEY is 'Super' key.
		Button1:
			[none] - drag: interactively move pointed window.
		Button3:
			[none] - drag: interactive window resize.
		Button2:
			[none] - focus input on pointed window.
			Control- toggle moving or not via MouseWheel[Up|Down]+[Shift|Control].
			Shift - close current window.
		MouseWheelUp - scroll to the right.
			[none] - raise pointed window in the stack.
			Shift - scroll to the left.
			Control - scroll up.
		MouseWheeDown:
			Shift  - scroll to the right.
			Control - scroll down.
	Keybindings:
		Tab - set keyboard layout to "$DVORAK_KEYBOARD_LAYOUT".
		a - set keyboard layout to "QWERTY_KEYBOARD_LAYOUT".
		Return - set keyboard layout to "$DVP_KEYBOARD_LAYOUT" .
		Backspace - set keyboard layout to "$NATIVE_KEYBOARD_LAYOUT".
		Space - call "t" wrapper(Usually it calls terminal).
		Esc - quit WM.

	
