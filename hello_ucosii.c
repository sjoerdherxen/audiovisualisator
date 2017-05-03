#include <stdio.h>
#include "includes.h"
#include "src/Display.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      2

/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata)
{
	srand(1);
  while (1)
  {
	  int* values;
	  values = malloc(sizeof(int) * 10);
	  values[0] = rand()%10;
	  values[1] = rand()%10;
	  values[2] = rand()%10;
	  values[3] = rand()%10;
	  values[4] = rand()%10;
	  values[5] = rand()%10;
	  values[6] = rand()%10;
	  values[7] = rand()%10;
	  values[8] = rand()%10;
	  values[9] = rand()%10;
	  DisplayValues(values);
	  free(values);
	  OSTimeDlyHMSM(0, 0, 1, 100);
  }
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata)
{

  while (1)
  { 
	  volatile alt_u16 *pixelbuffer = 0x08000000;
	  int i;
	  for(i=0;i<320*480;i++){
		  pixelbuffer[i] = 0xFFFF;
	  }
    //printf("☺Hello from task2⌂\n");
    OSTimeDlyHMSM(10000, 0, 3, 0);
  }
}
/* The main function creates two task and starts multi-tasking */
int main(void)
{
  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
              
               
  OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
  OSStart();
  return 0;
}
