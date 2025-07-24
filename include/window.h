#ifndef WINDOW_H
#define WINDOW_H

#include <raylib.h>

#include "error.h"
#include "types.h"

result_t window_update(u8*);

// TODO: Should all be called with Font as parametre
/*
 * Functions with this return type draw to the screen with Raylib, so they should only be called in between BeginDrawing() and EndDrawing() control
 * flows.
 */
typedef void window_draw;
typedef window_draw (*window_draw_func)(Font*);
#define DRAW_NONE NULL

enum WindowState {
    WindowState_AttemptConnection,
    WindowState_Connected,
    WindowState_ReceivedMsg,
    WindowState_Error
};

window_draw draw_try_connect(Font*);
window_draw draw_connected(Font*);
window_draw draw_minimap(Font*);
window_draw draw_error(Font*);


#endif  // WINDOW_H
