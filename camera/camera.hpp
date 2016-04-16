#ifndef _CAMERA_H
#define _CAMERA_H

#include "../thread.h"
#define CAMERA_DT 0.2

int Camera_main(Command_Info *command, Thread_Arg *thread_arg);

/* C/C++混合で書く場合，言語感で関数名の定義が異なるため明示する必要がある */
extern "C" void *Thread_Camera(void *_thread_arg);

#endif
