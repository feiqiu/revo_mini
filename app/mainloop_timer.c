#include "stm32f4xx_hal.h"
#include "mainloop_timer.h"
#include "event_dispatcher.h"
#include "event_list.h"

////////////////////////////////////////////////////////////////////////////////
//
// private prototypes
//
////////////////////////////////////////////////////////////////////////////////
static void mainloop_timer_1ms_handler(uint32_t event);

////////////////////////////////////////////////////////////////////////////////
//
// module privates
//
////////////////////////////////////////////////////////////////////////////////
static SoftTimer      _mainloop_timer;

////////////////////////////////////////////////////////////////////////////////
//
// task timer driver
//
////////////////////////////////////////////////////////////////////////////////
static void
mainloop_timer_1ms_handler(uint32_t event)
{
  soft_timer_drive(&_mainloop_timer);
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
mainloop_timer_init(void)
{
  soft_timer_init(&_mainloop_timer, 1);
  event_register_handler(mainloop_timer_1ms_handler, DISPATCH_EVENT_TIMER_TICK);
}

void
mainloop_timer_schedule(SoftTimerElem* elem, int expires)
{
  soft_timer_add(&_mainloop_timer, elem, expires);
}

void
mainloop_timer_cancel(SoftTimerElem* elem)
{
  soft_timer_del(&_mainloop_timer, elem);
}
