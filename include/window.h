#ifndef WINDOW_H
#define WINDOW_H

#include <raylib.h>

#include "error.h"
#include "types.h"

extern int g_fps;

/*
 * Empty s_kirbystate.
 */ 
void kirbystate_init();

/*
 * Mutate s_kirbystate with received message. 
 */
result_t kirbystate_update(u8* message);
#define KIRBYSTATE_UPDATE_UNKNOWN_MSGTYPE 1

/*
 * Functions with this return type draw to the screen with Raylib, so they should only be called in between BeginDrawing() and
 * EndDrawing() control flows.
 */
typedef void window_draw;
typedef window_draw (*window_draw_func)(Font*, Camera2D*);
#define DRAW_NONE NULL

enum WindowState { WindowState_AttemptConnection, WindowState_Connected, WindowState_ReceivedMsg, WindowState_Error };

window_draw draw_try_connect(Font*, Camera2D*);
window_draw draw_connected(Font*, Camera2D*);
window_draw draw_minimap(Font*, Camera2D*);
window_draw draw_error(Font*, Camera2D*);

constexpr int COOR_SCALING = 4;
constexpr int PIXELSIZE = 8;

#endif  // WINDOW_H
