#include <X11/Xlib.h>

#define TOON_MESSAGE_LENGTH 128
char toon_message[TOON_MESSAGE_LENGTH];

Window ToonGetRootWindow(Display *display, int screen, Window *clientparent);
