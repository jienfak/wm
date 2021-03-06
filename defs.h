#ifndef __DEFS_H
#define __DEFS_H
/* Keybindings to change, if you need it. */
/* Execute with "/bin/sh" on "XMENU_CMD_KEY_STR" pressing.
	This command is my in-terminal execution wrapper. */
#define CMD_HANDLER_STR ("te shell")
/* In pixels. */
#define DESKTOP_SCROLLING_SPEED (150)
/* It calls your "CMD_HANDLER_STR" by "system". */
#define XMENU_CMD_HOTKEY_STR ("space")
/* Sets layout to $DVORAK_KEYBOARD_LAYOUT. */
#define DVORAK_HOTKEY_STR ("Tab")
/* Sets layout to $DVP_KEYBOARD_LAYOUT. */
#define DVP_HOTKEY_STR ("Return")
/* Sets layout to $QWERTY_KEYBOARD_LAYOUT. */
#define QWERTY_HOTKEY_STR ("a")
/* Sets layout to $NATIVE_KEYBOARD_LAYOUT. */
#define NATIVE_HOTKEY_STR ("BackSpace")
/* Exits the wm. */
#define QUIT_WM_HOTKEY_STR ("Escape")
/* All the hotkeys work with this modiffication key. */
/* Mod4Mask = Super/Win key. */
#define MODKEY (Mod4Mask)

#define MAX_WINDOWS 1024
#endif /* __DEFSH */
