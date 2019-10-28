
#ifndef _TIMER_H_
#define _TIMER_H_

int timer1Setup(void);
unsigned long timer1GetTicks(void);

void UsSleep( unsigned long Delay );
#define MsSleep(d) UsSleep((unsigned long)d*1000U)


#endif // _TIMER_H_
