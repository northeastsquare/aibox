#include "ai_comm_log.h"

int ai_func_log_flag[64] = {0};

int ai_set_func_log_flag(unsigned long flag)
{
    if (flag == 0) {
        memset(ai_func_log_flag, 0, 64);
    } else if (flag == 1) {
        memset(ai_func_log_flag, 1, 64);
    } else {
        for (int i = 0; i < 64; i++) {
            if (((flag >> i) & ((unsigned long) 1)) == ((unsigned long) 1)) {
                ai_func_log_flag[i] = 1;
            } else {
                ai_func_log_flag[i] = 0;
            }
        }
    }

    return 0;
}
