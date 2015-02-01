//Enable POSIX definitions when compiling with C99 instead of GCC.
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */


#include <stdio.h>
#include "elev.h"
#include "time.h"
#include "elevPositionData.h"
#include "elevOrderData.h"
#include "elev_fsm.h"

int main() {
    // Initialize hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }

    fsm_elevInit();
    return 0;


}


