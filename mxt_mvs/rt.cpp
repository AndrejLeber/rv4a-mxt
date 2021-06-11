
#include "includes.h"

int init_rt_mvs_thread(int prio, void* data) {

    struct sched_param param{};
    pthread_attr_t attr;
    pthread_t t1;
    int ret;

    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        printf("mlockall failed: %m\n");
        exit(-2);
    }

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
        printf("init pthread attributes failed\n");
        goto out;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, 16384);
    if (ret) {
        printf("pthread setstacksize failed\n");
        goto out;
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
        printf("pthread setschedpolicy failed\n");
        goto out;
    }
    param.sched_priority = prio;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
        printf("pthread setschedparam failed\n");
        goto out;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
        printf("pthread setinheritsched failed\n");
        goto out;
    }

    /* Create a pthread t1 with specified attributes */
    ret = pthread_create(&t1, &attr, mxt_mvs_pos, data);
    if (ret) {
        printf("create pthread failed\n");
        std::cout << strerror(errno) << std::endl;
        goto out;
    }

    /* Join the thread and wait until it is done */
    ret = pthread_join(t1, nullptr);
    if (ret) {
        printf("joining pthread t1 failed: %m\n");
    }
out:
    munlockall();
    return ret;
}
