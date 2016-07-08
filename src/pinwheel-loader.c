
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

void reset_pinwheel_progress_loader(PPinwheelLoader loader, int num_tasks)
{
   loader->stream = stdout;
   loader->dash_symbol = '|';
   strncpy(loader->pulse_symbol, "##", sizeof(loader->pulse_symbol));
   strncpy(loader->pinwheel_symbols, "|/-\\", sizeof(loader->pinwheel_symbols));
   loader->loader_fps = 20;
   loader->pinwheel_rpm = 60;
   loader->min_pulse_velocity = 2;
   loader->max_pulse_velocity = 30;
   memset(loader->status, 0, sizeof(loader->status));
   loader->signal_exit = 0;
   loader->current_task = 0;
   loader->number_of_tasks = num_tasks;
}

PPinwheelLoader create_pinwheel_progress_loader(int num_tasks)
{
   PPinwheelLoader loader         = (PPinwheelLoader)malloc(sizeof(PinwheelLoader));
   reset_pinwheel_progress_loader(loader, num_tasks);

   loader->mutex = (pinwheel_mutex_t*)malloc(sizeof(pinwheel_mutex_t));
#if defined(_MSC_VER)
   InitializeCriticalSection(loader->mutex);
#else
   pthread_mutex_init(loader->mutex, 0);
#endif
   return loader;
}

PPinwheelLoader create_pinwheel_percent_loader()
{
   return create_pinwheel_progress_loader(100);
}

void reset_pinwheel_percent_loader(PPinwheelLoader loader)
{
   reset_pinwheel_progress_loader(loader, 100);
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
   if (percent < 0) percent = 0;
   if (percent > 100) percent = 100;
   loader->current_task = (int)(((float)percent/100.0) * loader->number_of_tasks);
}

static void _pinwheel_update_progress(PPinwheelLoader loader, int progress)
{
   if (progress < 0)
      progress = 0;
   if (progress > loader->number_of_tasks)
      progress = loader->number_of_tasks;
   loader->current_task = progress;
}

void pinwheel_update_progress(PPinwheelLoader loader, int progress)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress(loader, progress);
   pinwheel_unlock(loader);
}

void pinwheel_increment_progress(PPinwheelLoader loader)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress(loader, loader->current_task + 1);
   pinwheel_unlock(loader);
}

void pinwheel_update_progress_and_status(PPinwheelLoader loader, int progress, const char * status)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress(loader, progress);
   _pinwheel_update_status(loader, status);
   pinwheel_unlock(loader);
}

void pinwheel_increment_progress_and_status(PPinwheelLoader loader, const char * status)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress(loader, loader->current_task + 1);
   _pinwheel_update_status(loader, status);
   pinwheel_unlock(loader);
}

void pinwheel_update_percent(PPinwheelLoader loader, int percent)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress_from_percent(loader, percent);
   pinwheel_unlock(loader);
}

void pinwheel_update_percent_and_status(PPinwheelLoader loader, int percent, const char * status)
{
   pinwheel_lock(loader);
   _pinwheel_update_progress_from_percent(loader, percent);
   _pinwheel_update_status(loader, status);
   pinwheel_unlock(loader);
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

static char _spin_wheel(char * pinwheel_symbols, unsigned int *state)
{
   *state = (*state + 1) % sizeof(pinwheel_symbols);
   return pinwheel_symbols[*state];
}

static void pinwheel_clearline(FILE* stream, int n)
{
   int clears = 0;
   for (clears = 0; clears < n; ++clears)
   {
      fprintf(stream, "\b \b");
   }
}

COMMON_THREAD_ROUTINE(pinwheel_loader, ploader)
{
   const PPinwheelLoader loader = (const PPinwheelLoader)ploader;
   PPinwheelLoader loader_copy = (PPinwheelLoader)malloc(sizeof(PinwheelLoader));

   const unsigned background_size = strlen(starting_background) + 1;
   char          *drawn_loader = (char*)malloc(background_size);
   int            sleep_in_useconds    = 0;
   int            pinwheel_cycle_count = 0;
   int            pulse_cycle_count    = 0;
   int            pulse_velocity_diff  = 0;
   int            last_line_size       = 0;
   unsigned int   pinwheel_i           = 0;
   unsigned       dash_count           = 0;
   unsigned       cycle_count          = 0;
   unsigned int   i                    = 0;
   char           pinwheel             = 0;
   float          current_percent      = 0.0;
   long int       pulse_i              = 0;
   long int       pulse_symbol_count   = 0;

   pinwheel_lock(loader);
   memcpy(loader_copy, loader, sizeof(PinwheelLoader));
   pinwheel_unlock(loader);
   pinwheel = _spin_wheel(loader_copy->pinwheel_symbols, &pinwheel_i);

   if (pinwheel_cycle_count <= 0)
   {
      pinwheel_cycle_count = 1;
   }
   for (cycle_count = 0; !loader_copy->signal_exit; cycle_count++)
   {
      pinwheel_lock(loader);
      memcpy(loader_copy, loader, sizeof(PinwheelLoader));
      pinwheel_unlock(loader);

      current_percent = ((float)loader_copy->current_task / (float)loader_copy->number_of_tasks);
      sleep_in_useconds = 1000000 / loader_copy->loader_fps;
      pinwheel_cycle_count = loader_copy->loader_fps / (2 * sizeof(loader_copy->pinwheel_symbols)*(loader_copy->pinwheel_rpm / 60));
      pulse_velocity_diff = loader_copy->max_pulse_velocity - loader_copy->min_pulse_velocity;
      pulse_cycle_count = (current_percent == 0.0)
         ? (int)(loader_copy->loader_fps / loader_copy->min_pulse_velocity)
         : (int)(loader_copy->loader_fps / (loader_copy->min_pulse_velocity +
            (current_percent * pulse_velocity_diff)));
      pulse_symbol_count = (long int)strlen(loader_copy->pulse_symbol);
      pulse_i = -1 * pulse_symbol_count;

      strcpy(drawn_loader, starting_background);
      if (pulse_cycle_count <= 0)
      {
         pulse_cycle_count = 1;
      }

      dash_count = (unsigned)(current_percent*(background_size - 1));
      for (i = 0; i < background_size-1; ++i)
      {
         if (i < dash_count + 1)
         {
            if ((long int)dash_count > pulse_symbol_count
               && (long int)i >= pulse_i
               && (long int)i < pulse_i + pulse_symbol_count)
            {
               drawn_loader[i] = loader_copy->pulse_symbol[i - pulse_i];
            }
            else if (!isdigit(drawn_loader[i]))
            {
               drawn_loader[i] = loader_copy->dash_symbol;
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
         pinwheel = _spin_wheel(loader_copy->pinwheel_symbols, &pinwheel_i);
      if ((long int)dash_count > pulse_symbol_count
         && pulse_cycle_count > 0
         && cycle_count % pulse_cycle_count == 0)
      {
         if (++pulse_i > (long)dash_count)
            pulse_i = -1 * pulse_symbol_count;
      }

      pinwheel_clearline(loader_copy->stream, last_line_size);
      last_line_size = fprintf(loader_copy->stream, "[%s] %3d%% %s",
         drawn_loader, (int)(100 * current_percent), loader_copy->status);
      USLEEP(sleep_in_useconds);
   }
   free(drawn_loader);
   free(loader_copy);
   COMMON_THREAD_EXIT(ploader)
}

void pinwheel_stop(PPinwheelLoader loader)
{
   pinwheel_lock(loader);
   loader->signal_exit = 1;
   pinwheel_unlock(loader);
#if defined(_MSC_VER)
   WaitForSingleObject(loader->thread, INFINITE);
#else
   pthread_join(loader->thread_id, 0);
#endif
}

void pinwheel_start(PPinwheelLoader loader)
{
   loader->current_task = 0;
   pinwheel_update_status(loader, "");
   loader->signal_exit = 0;
#if defined(_MSC_VER)
   loader->thread = (pinwheel_thread_handle_t)_beginthreadex(NULL, 0, pinwheel_loader, loader, CREATE_SUSPENDED, &loader->thread_id);
   ResumeThread(loader->thread);
#else
   pthread_create(&loader->thread_id, 0, pinwheel_loader, loader);
#endif
}
