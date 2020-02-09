/*  --------------------------------------------------------------------------
 *  xlib_graphic.h    
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef XLIB_GRAPHIC_H_
#define XLIB_GRAPHIC_H_

int xlib_graphic_init( int16_t xsize, int16_t ysize );

void xlib_graphic_flush( int32_t x1, int32_t y1, int32_t x2, int32_t y2, 
                         uint16_t *color );

int xlib_input_init( int16_t xsize, int16_t ysize );

int xlib_input_read( int16_t *x, int16_t *y );

#endif /* XLIB_GRAPHIC_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */