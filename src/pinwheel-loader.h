#ifndef PINWHEEL_LOADER_H_
#define PINWHEEL_LOADER_H_

/** @file src/pinwheel-loader.h
 */

#include <stdio.h>

#if defined(_MSC_VER)

#include <windows.h>
#include <WinNT.h>
#include <WinDef.h>
#include <BaseTsd.h>
#include <excpt.h>
#include <tchar.h>
#include <process.h>

typedef CRITICAL_SECTION pinwheel_mutex_t;
typedef unsigned int     pinwheel_thread_t;
typedef HANDLE           pinwheel_thread_handle_t;

#else

#include <pthread.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

typedef pthread_mutex_t pinwheel_mutex_t;
typedef pthread_t       pinwheel_thread_t;
typedef int             pinwheel_thread_handle_t;

#endif

#define MAX_PINWHEEL_LOADER_STATUS_BUFFER 96

struct _PinwheelLoader
{
   FILE                    *stream;
   char                     dash_symbol;
   char                     pulse_symbol[2];
   char                     pinwheel_symbols[4];
   unsigned int             loader_fps;
   unsigned int             pinwheel_rpm;
   unsigned int             min_pulse_velocity;
   unsigned int             max_pulse_velocity;
   volatile int             percent;
   char                     status[MAX_PINWHEEL_LOADER_STATUS_BUFFER];
   volatile int             signal_exit;
   pinwheel_mutex_t        *mutex;
   int                      current_progress;
   int                      minimum_progress;
   int                      maximum_progress;
   pinwheel_thread_handle_t thread;
   pinwheel_thread_t        thread_id;
};
typedef struct _PinwheelLoader PinwheelLoader;
typedef struct _PinwheelLoader *PPinwheelLoader;

PPinwheelLoader create_pinwheel_loader(int min_progress, int max_progress);

void reset_pinwheel_loader(PPinwheelLoader loader, int min_progress, int max_progress);

void destroy_pinwheel_loader(PPinwheelLoader loader);

void pinwheel_start(PPinwheelLoader loader);

void pinwheel_stop(PPinwheelLoader loader);

void pinwheel_update_status(PPinwheelLoader loader, const char * status);

void pinwheel_update_progress(PPinwheelLoader loader, int progress);

void pinwheel_increment_progress(PPinwheelLoader loader);

void pinwheel_update_progress_and_status(PPinwheelLoader loader, int progress, const char * status);

void pinwheel_increment_progress_and_status(PPinwheelLoader loader, const char * status);

void pinwheel_update_percent(PPinwheelLoader loader, int percent);

void pinwheel_increment_percent(PPinwheelLoader loader);

void pinwheel_update_percent_and_status(PPinwheelLoader loader, int percent, const char * status);

void pinwheel_increment_percent_and_status(PPinwheelLoader loader, const char * status);

#endif /* PINWHEEL_LOADER_H_ */
