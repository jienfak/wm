# wm
Windows mover. It can move windows.
Based on TinyWM. Some code is taken from the DWM. Suckless team - thanks.
Instalation:
	$ [edit 'config.mk' to choose compiler on your machine(Default is choosed TCC).]]
	$ [modify keybindings in 'defs.h' if you need and change CMD-caller.]
	$ make
	# make install
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
		MouseWheelUp:
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
			Space - call cmd-caller program(Usually it calls terminal).
			Esc - quit WM.