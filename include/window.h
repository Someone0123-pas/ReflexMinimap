#ifndef WINDOW_H
#define WINDOW_H

/*
 * Functions with this return type draw to the screen with Raylib, so they should only be called in between BeginDrawing() and EndDrawing() control
 * flows.
 */
typedef void window_draw;
typedef window_draw (*window_draw_func)(void);
#define DRAW_NONE NULL

enum WindowState {
    WindowState_AttemptConnection,
    WindowState_Connected,
    WindowState_Minimap,
    WindowState_Received,
    WindowState_Error
};

window_draw draw_try_connect();
window_draw draw_connected();
window_draw draw_minimap();
window_draw draw_error();

#endif  // WINDOW_H
