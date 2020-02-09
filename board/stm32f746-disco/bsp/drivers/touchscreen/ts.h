/*  --------------------------------------------------------------------------
 *  ts.h   
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef TS_H_
#define TS_H_

#define TS_MONO_TOUCH_SUPPORTED     (1)

typedef struct
{  
  void       (*Init)(uint16_t);
  uint16_t   (*ReadID)(uint16_t);
  void       (*Reset)(uint16_t);
  void       (*Start)(uint16_t);
  uint8_t    (*DetectTouch)(uint16_t);
  void       (*GetXY)(uint16_t, uint16_t*, uint16_t*);
  void       (*EnableIT)(uint16_t);
  void       (*ClearIT)(uint16_t);
  uint8_t    (*GetITStatus)(uint16_t);
  void       (*DisableIT)(uint16_t);
} TS_DrvTypeDef;

int ts_init( int16_t xsize, int16_t ysize );
int ts_read( int16_t *x, int16_t *y );

#endif /* TS_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
