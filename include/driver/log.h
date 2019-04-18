#include "osapi.h"

#define log(tag, s, ...) os_printf("[%s:%s:%d:%s] " s "\n", __FILE__, __FUNCTION__, __LINE__, tag, ##__VA_ARGS__)