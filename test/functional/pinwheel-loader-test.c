
#include "pinwheel-loader.h"

void pinwheel_update_progress_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_update_progress 1-30\n");
   PPinwheelLoader loader = create_pinwheel_loader(1, 30);
   pinwheel_start(loader);
   for (int i = 0; i <= 31; ++i)
   {
      sprintf(status, "%u/%u", i, 31);
      pinwheel_update_status(loader, status);
      pinwheel_update_progress(loader, i);
      Sleep(100);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_update_progress 98-202\n");
   reset_pinwheel_loader(loader, 98, 202);
   pinwheel_start(loader);
   for (int i = 99; i <= 201; ++i)
   {
      sprintf(status, "%u/%u", i, 201);
      pinwheel_update_status(loader, status);
      pinwheel_update_progress(loader, i);
      Sleep(100);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_increment_progress_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_increment_progress 1-30\n");
   PPinwheelLoader loader = create_pinwheel_loader(1, 30);
   pinwheel_start(loader);
   for (int i = 0; i <= 31; ++i)
   {
      sprintf(status, "%u/%u", i, 31);
      pinwheel_update_status(loader, status);
      pinwheel_increment_progress(loader);
      Sleep(100);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_increment_progress 98-202\n");
   reset_pinwheel_loader(loader, 98, 202);
   pinwheel_start(loader);
   for (int i = 99; i <= 201; ++i)
   {
      sprintf(status, "%u/%u", i, 201);
      pinwheel_update_status(loader, status);
      pinwheel_update_progress(loader, i);
      Sleep(100);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_update_progress_and_status_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_update_progress_and_status 1-30\n");
   PPinwheelLoader loader = create_pinwheel_loader(1, 30);
   pinwheel_start(loader);
   for (int i = 0; i <= 31; ++i)
   {
      sprintf(status, "%u/%u", i, 31);
      pinwheel_update_progress_and_status(loader, i, status);
      Sleep(100);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_update_progress_and_status 98-202\n");
   reset_pinwheel_loader(loader, 98, 202);
   pinwheel_start(loader);
   for (int i = 99; i <= 201; ++i)
   {
      sprintf(status, "%u/%u", i, 201);
      pinwheel_update_progress_and_status(loader, i, status);
      Sleep(100);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_update_percent_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_update_percent 1-99\n");
   PPinwheelLoader loader = create_pinwheel_loader(1, 30);
   pinwheel_start(loader);
   for (int i = 1; i <= 99; ++i)
   {
      sprintf(status, "%u/%u", i, 99);
      pinwheel_update_status(loader, status);
      pinwheel_update_percent(loader, i);
      Sleep(100);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_update_percent -1-101\n");
   reset_pinwheel_loader(loader, 98, 202);
   pinwheel_start(loader);
   for (int i = -1; i <= 101; ++i)
   {
      sprintf(status, "%u/%u", i, 101);
      pinwheel_update_status(loader, status);
      pinwheel_update_percent(loader, i);
      Sleep(100);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_increment_percent_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_increment_percent 1-99\n");
   PPinwheelLoader loader = create_pinwheel_loader(1, 30);
   pinwheel_start(loader);
   for (int i = 1; i <= 99; ++i)
   {
      sprintf(status, "%u/%u", i, 99);
      pinwheel_update_status(loader, status);
      pinwheel_increment_percent(loader);
      Sleep(100);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_increment_percent -1-101\n");
   reset_pinwheel_loader(loader, 98, 202);
   pinwheel_start(loader);
   for (int i = -1; i <= 101; ++i)
   {
      sprintf(status, "%u/%u", i, 101);
      pinwheel_update_status(loader, status);
      pinwheel_update_percent(loader, i);
      Sleep(100);
   }
   pinwheel_stop(loader);

   destroy_pinwheel_loader(loader);
}

void pinwheel_update_percent_and_status_test()
{
   int i = 0;
   char status[16] = { 0 };

   printf("\npinwheel_update_percent_and_status 1-99\n");
   PPinwheelLoader loader = create_pinwheel_loader(1, 30);
   pinwheel_start(loader);
   for (int i = 1; i <= 99; ++i)
   {
      sprintf(status, "%u/%u", i, 99);
      pinwheel_update_percent_and_status(loader, i, status);
      Sleep(100);
   }
   pinwheel_stop(loader);

   printf("\npinwheel_update_percent_and_status -1-101\n");
   reset_pinwheel_loader(loader, 98, 202);
   pinwheel_start(loader);
   for (int i = -1; i <= 101; ++i)
   {
      sprintf(status, "%u/%u", i, 101);
      pinwheel_update_percent_and_status(loader, i, status);
      Sleep(100);
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
   pinwheel_increment_percent_test();
   pinwheel_update_percent_and_status_test();

   return 0;
}
