#ifndef __DEFSH
#define __DEFSH
/* Keybindings to change, if you need it. */

/* In pixels. */
	#define DESKTOP_SCROLLING_SPEED (150)
/* It calls your 'menu_cmd' wrapper. */
	#define XMENU_CMD_HOTKEY_STR ("space")
/* Sets layout to $DVORAK_KEYBOARD_LAYOUT. */
	#define DVORAK_HOTKEY_STR ("Tab")
/* Sets layout to $DVP_KEYBOARD_LAYOUT. */
	#define DVP_HOTKEY_STR ("Return")
/* Sets layout to $QWERTY_KEYBOARD_LAYOUT. */
	#define QWERTY_HOTKEY_STR ("a")
/* Sets layout to $NATIVE_KEYBOARD_LAYOUT. */
	#define NATIVE_HOTKEY_STR ("backspace")
/* Exits the wm. */
	#define QUIT_WM_HOTKEY_STR ("Escape")
/* Set focus to window. */
	#define SET_INPUT_FOCUS_HOTKEY_STR ("F1")
/* All the hotkeys work with this modiffication key. */
	/* Mod4Mask = Super/Win key. */
	#define MODKEY (Mod4Mask)

#endif /* __DEFSH */