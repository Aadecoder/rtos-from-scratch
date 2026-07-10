#include <stdint.h>

typedef uint32_t TickType_t;

typedef uint32_t StackType_t;

typedef uint32_t TaskHandle_t;

typedef enum{
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
}TaskState;
