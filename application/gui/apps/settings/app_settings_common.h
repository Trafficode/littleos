/*  --------------------------------------------------------------------------
 *  app_settings_common.h
 *  Author: Trafficode
 *  ------------------------------------------------------------------------ */
#ifndef APP_SETTINGS_COMMON_H_
#define APP_SETTINGS_COMMON_H_

typedef enum _settings_sm {
    SettingsRun=0,
    SettingsForceExit=1,
} settings_sm_t;

typedef struct _settings_local {
    settings_sm_t sm;
} settings_local_t;

#endif /* APP_SETTINGS_COMMON_H_ */
/*  --------------------------------------------------------------------------
 *  end of file
 *  ------------------------------------------------------------------------ */
