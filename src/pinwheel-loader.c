
#include "pinwheel-loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char starting_background[] = "      25     50     75     100";

static void pinwheel_lock(PPinwheelLoader loader)
{
#if defined(_MSC_VER)
   EnterCriticalSection(loader->mutex);
#else
   pthread_mutex_lock(loader->mutex);
#endif
}

static void pinwheel_unlock(PPinwheelLoader loader)
{
#if defined(_MSC_VER)
   LeaveCriticalSection(loader->mutex);
#else
   pthread_mutex_unlock(loader->mutex);
#endif
}

void reset_pinwheel_loader(PPinwheelLoader loader, int min_progress, int max_progress)
{
   loader->stream = stdout;
   loader->dash_symbol = '|';
   memcpy(loader->pulse_symbol, "##", sizeof(loader->pulse_symbol));
   memcpy(loader->pinwheel_symbols, "|/-\\", sizeof(loader->pinwheel_symbols));
   loader->loader_fps = 20;
   loader->pinwheel_rpm = 60;
   loader->min_pulse_velocity = 2;
   loader->max_pulse_velocity = 30;
   loader->percent = 0;
   memset(loader->status, 0, sizeof(loader->status));
   loader->signal_exit = 0;
   loader->current_progress = min_progress;
   loader->minimum_progress = min_progress;
   loader->maximum_progress = max_progress;
}

PPinwheelLoader create_pinwheel_loader(int min_progress, int max_progress)
{
   PPinwheelLoader loader         = (PPinwheelLoader)malloc(sizeof(PinwheelLoader));
   reset_pinwheel_loader(loader, min_progress, max_progress);

   loader->mutex = (pinwheel_mutex_t*)malloc(sizeof(pinwheel_mutex_t));
#if defined(_MSC_VER)
   InitializeCriticalSection(loader->mutex);
#else
   pthread_mutex_init(loader->mutex, 0);
#endif
   return loader;
}

void destroy_pinwheel_loader(PPinwheelLoader loader)
{
   if (loader != 0)
   {
      if (loader->mutex)
      {
#if defined(_MSC_VER)
         DeleteCriticalSection(loader->mutex);
#else
         pthread_mutex_destroy(loader->mutex);
#endif
         free(loader->mutex);
      }
      free(loader);
   }
}

void _pinwheel_update_status(PPinwheelLoader loader, const char * status)
{
   if (status != 0)
   {
      strncpy(loader->status, status, sizeof(loader->status));
      loader->status[sizeof(loader->status) - 1] = 0;
   }
   else
   {
      memset(loader->status, 0, sizeof(loader->status));
   }
}

void pinwheel_update_status(PPinwheelLoader loader, const char * status)
{
   pinwheel_lock(loader);
   _pinwheel_update_status(loader, status);
   pinwheel_unlock(loader);
}

static void _pinwheel_update_progress_from_percent(PPinwheelLoader loader, int percent)
{
   loader->percent = percent;
   if (loader->percent < 0) loader->percent = 0;
   if (loader->percent > 100) loader->percent = 100;
   loader->current_progress = (int)(
      ((float)loader->percent/100.0) * (loader->maximum_progress - loader->minimum_progress)
         + loader->minimum_progress);
}

static void _pinwheel_update_percent_from_progress(PPinwheelLoader loader, int progress)
{
   loader->current_progress = progress;
   if (loader->current_progress < loader->minimum_progress)
      loader->current_progress = loader->minimum_progress;
   if (loader->current_progress > loader->maximum_progress)
      loader->current_progress = loader->maximum_progress;
   loader->percent = (int)(100*(
                     ((float)loader->current_progress - (float)loader->minimum_progress) /
                     ((float)loader->maximum_progress - (float)loader->minimum_progress)));
}

void pinwheel_update_progress(PPinwheelLoader loader, int progress)
{
   pinwheel_lock(loader);
   _pinwheel_update_percent_from_progress(loader, progress);
   pinwheel_unlock(loader);
}

void pinwheel_increment_progress(PPinwheelLoader loader)
{
   pinwheel_lock(loader);
   _pinwheel_update_percent_from_progress(loader, loader->current_progress + 1);
   pinwheel_unlock(loader);
}

void pinwheel_update_progress_and_status(PPinwheelLoader loader, int progress, const char * status)
{
   pinwheel_lock(loader);
   _pinwheel_update_percent_from_progress(loader, progress);
   _pinwheel_update_status(loader, status);
   pinwheel_unlock(loader);
}

void pinwheel_increment_progress_and_status(PPinwheelLoader loader, const char * status)
{
   pinwheel_lock(loader);
   _pinwheel_update_percent_from_progress(loader, loader->current_progress + 1);
   _pinwheel_update_status(loader, status);
   pinwheel_unlock(loader);
}

void pinwheel_update_percent(PPinwheelLoader loader, int percent)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress_from_percent(loader, percent);
   pinwheel_unlock(loader);
}

void pinwheel_increment_percent(PPinwheelLoader loader)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress_from_percent(loader, loader->percent + 1);
   pinwheel_unlock(loader);
}

void pinwheel_update_percent_and_status(PPinwheelLoader loader, int percent, const char * status)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress_from_percent(loader, percent);
   _pinwheel_update_status(loader, status);
   pinwheel_unlock(loader);
}

void pinwheel_increment_percent_and_status(PPinwheelLoader loader, const char * status)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress_from_percent(loader, loader->percent + 1);
   _pinwheel_update_status(loader, status);
   pinwheel_unlock(loader);
}

static void pinwheel_clearline(PPinwheelLoader loader, int n)
{
   int clears = 0;
   for (clears = 0; clears < n; ++clears)
   {
      fprintf(loader->stream, "\b \b");
   }
}

static char _spin_wheel(PPinwheelLoader loader, unsigned int *state)
{
   *state = (*state + 1) % sizeof(loader->pinwheel_symbols);
   return loader->pinwheel_symbols[*state];
}

#if defined(_MSC_VER)
#define COMMON_THREAD_ROUTINE(name, argname) unsigned __stdcall  name(void * argname)
#define COMMON_THREAD_EXIT(argname) { _endthreadex((unsigned)argname); return (unsigned)argname; }
#define USLEEP(x) Sleep(x/1000)
#else 
#define COMMON_THREAD_ROUTINE(name, argname) void *  name(void * argname)
#define COMMON_THREAD_EXIT(argname) return argname;
#define USLEEP(x) usleep(x)
#endif

COMMON_THREAD_ROUTINE(pinwheel_loader, ploader)
{
   PPinwheelLoader loader = (PPinwheelLoader)ploader;

   const int sleep_in_useconds          = 1000000 / loader->loader_fps;
   int pinwheel_cycle_count =
      loader->loader_fps /
      (2 * sizeof(loader->pinwheel_symbols)*(loader->pinwheel_rpm / 60));
   int            pulse_cycle_count = 0;
   const int      pulse_velocity_diff = loader->max_pulse_velocity - loader->min_pulse_velocity;
   const unsigned background_size     = strlen(starting_background) + 1;
   char          *drawn_loader        = (char*)malloc(background_size);
   int            last_line_size      = 0;
   unsigned int   pinwheel_i          = 0;
   char           pinwheel            = _spin_wheel(loader, &pinwheel_i);
   long int       pulse_i             = -1 * (long int)sizeof(loader->pulse_symbol);
   unsigned       dash_count          = 0;
   float          current_percent     = loader->percent / (float)100.0;
   unsigned       cycle_count         = 0;
   unsigned int   i                   = 0;

   if (pinwheel_cycle_count <= 0)
   {
      pinwheel_cycle_count = 1;
   }
   for (cycle_count = 0; !loader->signal_exit; cycle_count++)
   {
      current_percent = loader->percent / (float)100.0;
      strcpy(drawn_loader, starting_background);

      pulse_cycle_count = (current_percent == 0.0)
         ? (int)(loader->loader_fps / loader->min_pulse_velocity)
         : (int)(loader->loader_fps / (loader->min_pulse_velocity +
            (current_percent * pulse_velocity_diff)));
      if (pulse_cycle_count <= 0)
      {
         pulse_cycle_count = 1;
      }
      dash_count = (unsigned)(current_percent*(background_size - 1));
      for (i = 0; i < background_size-1; ++i)
      {
         if (i < dash_count + 1)
         {
            if (dash_count > sizeof(loader->pulse_symbol)
               && (long)i >= pulse_i
               && i < pulse_i + sizeof(loader->pulse_symbol))
            {
               drawn_loader[i] = loader->pulse_symbol[i - pulse_i];
            }
            else if (!isdigit(drawn_loader[i]))
            {
               drawn_loader[i] = loader->dash_symbol;
            }
         }
         else if (i == dash_count + 1)
         {
            drawn_loader[i] = pinwheel;
         }
         else
         {
            break;
         }
      }

      if (cycle_count % pinwheel_cycle_count == 0)
         pinwheel = _spin_wheel(loader, &pinwheel_i);
      if (dash_count > sizeof(loader->pulse_symbol)
         && pulse_cycle_count > 0
         && cycle_count % pulse_cycle_count == 0)
      {
         if (++pulse_i > (long)dash_count)
            pulse_i = -1 * (long int)sizeof(loader->pulse_symbol);
      }

      pinwheel_clearline(loader, last_line_size);
      last_line_size = fprintf(loader->stream, "[%s] %3d%% %s",
         drawn_loader, (int)(100 * current_percent), loader->status);
      USLEEP(sleep_in_useconds);
   }
   free(drawn_loader);
   COMMON_THREAD_EXIT(ploader)
}

void pinwheel_stop(PPinwheelLoader loader)
{
   pinwheel_lock(loader);
   loader->signal_exit = 1;
#if defined(_MSC_VER)
   WaitForSingleObject(loader->thread, INFINITE);
#else
   pthread_join(loader->thread_id, 0);
#endif
   pinwheel_unlock(loader);
}

void pinwheel_start(PPinwheelLoader loader)
{
   loader->percent = 0;
   loader->current_progress = loader->minimum_progress;
   pinwheel_update_status(loader, "");
   loader->signal_exit = 0;
#if defined(_MSC_VER)
   loader->thread = (pinwheel_thread_handle_t)_beginthreadex(NULL, 0, pinwheel_loader, loader, CREATE_SUSPENDED, &loader->thread_id);
   ResumeThread(loader->thread);
#else
   pthread_create(&loader->thread_id, 0, pinwheel_loader, loader);
#endif
}
