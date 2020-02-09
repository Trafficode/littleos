/*  --------------------------------------------------------------------------
 *  xlib_graphic.c    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#include <X11/Xlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lvgl.h"
#include "system.h"
#include "xlib_graphic.h"

static Display *dis;
static Window win;
static GC gc;

static pthread_t input_tsk;
static void *input_tsk_worker(void *param);

static int16_t last_x = 0;
static int16_t last_y = 0;
static int pressed = 0;

static pthread_mutex_t input_lock;

static void xlib_window_wait( void );

int xlib_input_init( int16_t xsize, int16_t ysize ) {
    xsize = (int16_t)xsize;
    ysize = (int16_t)ysize;
    return(0); /* Default success */
}

int xlib_input_read( int16_t *x, int16_t *y ) {
    int _pressed;

    pthread_mutex_lock( &input_lock );
    _pressed = pressed;
    *x = last_x;
    *y = last_y;
    pthread_mutex_unlock( &input_lock );

    return( _pressed );
}

int xlib_graphic_init( int16_t xsize, int16_t ysize ) {
    unsigned long valuemask = 0;    /* which values in 'values' to  */
    XGCValues values;               /* initial values for the GC.   */
    int rc = 1; /* Init as initialize error */

    rc = XInitThreads();
    if( 0 == rc ) {
        unix_board_error("[xlib_graphic_init] XInitThreads RC:%d\n", rc);
        rc = 1;
        goto exit;
    }
    
    dis = XOpenDisplay(NULL);
    if( NULL == dis ) {
        unix_board_error("[xlib_graphic_init] XOpenDisplay RC:%d\n", rc);
        rc = 1;
        goto exit;
    }

    XLockDisplay(dis);

    win = XCreateSimpleWindow(  dis, RootWindow(dis, 0), 1, 1, (uint32_t)xsize, 
                                (uint32_t)ysize, 0, 0x000000, 0xFFFFFF);
    XMapWindow(dis, win);

    gc = XCreateGC(dis, win, valuemask, &values);
    if ( 0 > gc ) {
        unix_board_error("[xlib_graphic_init] XCreateGC RC:%d\n", gc);
        rc = 1;
        goto exit;
    }

    XSync(dis, True);
    XFlush(dis);
    
    xlib_window_wait();

    XUnlockDisplay(dis);

    /* Must be called after XInitThreads so xlib_input_init() is empty
     * to avoid any problems */
    XSelectInput(dis,win,PointerMotionMask|ButtonPressMask|ButtonReleaseMask);

    pthread_mutex_init( &input_lock, NULL );
    pthread_mutex_unlock( &input_lock );

    rc = pthread_create(&input_tsk, NULL, input_tsk_worker, NULL);
    if ( 0 != rc ) {
        unix_board_error("[xlib_graphic_init]"  \
                         " input_tsk_worker create RC: %d\n", rc);
        rc = 1;
        goto exit;         
    }

exit:
    return(rc);    
}

void xlib_graphic_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                        uint16_t *color)
{
    uint32_t x=0, y=0, i=0, r=0, g=0, b=0, _color=0;
    
    XLockDisplay(dis);
    for( y=y1; y<=y2; y++ ) {
        for( x=x1; x<=x2; x++ ) {
            /* Just get index in table */
            i = (x2-x1+1)*(y-y1)+(x-x1);

            _color = (uint32_t)color[i] & 0x0000FFFF;

            /* Get RGB565 colors */
            r = (_color & 0x0000F800) << (5+3);
            g = (_color & 0x000007E0) << (3+2);
            b = (_color & 0x0000001F) << 3;

            /* Make RGB888 colors */
            r = r | ((r & 0x00E00000) >> 5);
            g = g | ((g & 0x0000C000) >> 6);
            b = b | ((b & 0x000000E0) >> 5);
            
            /* Set draw context */
            XSetForeground(dis, gc, r|g|b);
            XDrawPoint(dis, win, gc, x, y);
        }
    }
    XFlush(dis);
    XUnlockDisplay(dis);
    return;
}

static void xlib_window_wait( void ) {
    XWindowAttributes xwa;

    do {
        XGetWindowAttributes(dis, win, &xwa);
        unix_system_sleep_ms(1);
    } while(xwa.map_state != IsViewable);
}

static void *input_tsk_worker(void *param) {
    XEvent event;

    for( ;; ) {
        XNextEvent( dis, &event );
        pthread_mutex_lock( &input_lock );
        switch( event.type ) {
            case MotionNotify:
                last_x = (int16_t)event.xmotion.x;
                last_y = (int16_t)event.xmotion.y;
                break;
            case ButtonPressMask:
                pressed = 1;
                break;
            case ButtonPressMask|KeyPressMask:
                pressed = 0;
                break;
            default:
                break;
        }
        pthread_mutex_unlock( &input_lock );
    }

    return(0);
}

/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
