#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "kernel.h"
#include "kernel_id.h"

/* OSEK declarations */
DeclareCounter(SysTimerCnt);

DeclareTask(EventDispatcherTask);
DeclareTask(EventBigButtonTask);
DeclareTask(EventRightButtonTask);
DeclareTask(LCDTask);
DeclareTask(MainTask);

DeclareEvent(BigButtonOnEvent); /* Event declaration */
DeclareEvent(BigButtonOffEvent); /* Event declaration */
DeclareEvent(RightButtonOnEvent);
DeclareEvent(RightButtonOffEvent);

DeclareResource(ResourceLightThreshold);
DeclareResource(ResourceDirection);
DeclareResource(ResourceBoxesRemaining);
DeclareResource(ResourceNextBoxTarget);
DeclareResource(ResourceCurrentBox);

#endif //MAIN_TASK_H