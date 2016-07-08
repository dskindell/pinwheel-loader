
#include "pinwheel-loader.h"

#define PROGRESS_RATE 100

#if defined(_MSC_VER)
#define OS_MSLEEP(x) Sleep(x)
#else
#define OS_MSLEEP(x) usleep(1000*x)
#endif

void pinwheel_update_progress_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_update_progress 0-31 of 30 tasks\n");
   PPinwheelLoader loader = create_pinwheel_progress_loader(31);
   pinwheel_start(loader);
   for (i = 0; i <= 31; ++i)
   {
      sprintf(status, "%u/%u", i, 31);
      pinwheel_update_status(loader, status);
      pinwheel_update_progress(loader, i);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_update_progress 1-29 of 30 tasks\n");
   reset_pinwheel_progress_loader(loader, 30);
   pinwheel_start(loader);
   for (i = 1; i <= 29; ++i)
   {
      sprintf(status, "%u/%u", i, 29);
      pinwheel_update_status(loader, status);
      pinwheel_update_progress(loader, i);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_increment_progress_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_increment_progress 0-31 of 30 tasks\n");
   PPinwheelLoader loader = create_pinwheel_progress_loader(30);
   pinwheel_start(loader);
   for (i = 0; i <= 31; ++i)
   {
      sprintf(status, "%u/%u", i, 31);
      pinwheel_update_status(loader, status);
      pinwheel_increment_progress(loader);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_increment_progress 1-29 of 30 tasks\n");
   reset_pinwheel_progress_loader(loader, 30);
   pinwheel_start(loader);
   for (i = 1; i <= 29; ++i)
   {
      sprintf(status, "%u/%u", i, 29);
      pinwheel_update_status(loader, status);
      pinwheel_increment_progress(loader);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_update_progress_and_status_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_update_progress_and_status 0-31 of 30 tasks\n");
   PPinwheelLoader loader = create_pinwheel_progress_loader(30);
   pinwheel_start(loader);
   for (i = 0; i <= 31; ++i)
   {
      sprintf(status, "%u/%u", i, 31);
      pinwheel_update_progress_and_status(loader, i, status);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_update_progress_and_status 1-29 of 30 tasks\n");
   reset_pinwheel_progress_loader(loader, 30);
   pinwheel_start(loader);
   for (i = 1; i <= 29; ++i)
   {
      sprintf(status, "%u/%u", i, 29);
      pinwheel_update_progress_and_status(loader, i, status);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_update_percent_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_update_percent 1-99\n");
   PPinwheelLoader loader = create_pinwheel_percent_loader();
   pinwheel_start(loader);
   for (i = 1; i <= 99; ++i)
   {
      sprintf(status, "%u/%u", i, 99);
      pinwheel_update_status(loader, status);
      pinwheel_update_percent(loader, i);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_update_percent -1-101\n");
   reset_pinwheel_percent_loader(loader);
   pinwheel_start(loader);
   for (i = -1; i <= 101; ++i)
   {
      sprintf(status, "%u/%u", i, 101);
      pinwheel_update_status(loader, status);
      pinwheel_update_percent(loader, i);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_update_percent_and_status_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_update_percent_and_status 1-99\n");
   PPinwheelLoader loader = create_pinwheel_percent_loader();
   pinwheel_start(loader);
   for (i = 1; i <= 99; ++i)
   {
      sprintf(status, "%u/%u", i, 99);
      pinwheel_update_percent_and_status(loader, i, status);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_update_percent_and_status -1-101\n");
   reset_pinwheel_percent_loader(loader);
   pinwheel_start(loader);
   for (i = -1; i <= 101; ++i)
   {
      sprintf(status, "%u/%u", i, 101);
      pinwheel_update_percent_and_status(loader, i, status);
      OS_MSLEEP(PROGRESS_RATE);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

int main()
{

   pinwheel_update_progress_test();
   pinwheel_increment_progress_test();
   pinwheel_update_progress_and_status_test();

   pinwheel_update_percent_test();
   pinwheel_update_percent_and_status_test();

   return 0;
}
