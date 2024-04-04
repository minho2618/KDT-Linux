#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

#define MODULE_FILENAME "/dev/k_wq_list_driver"

int main(int argc, char *argv[])
{
    int dev;
    char buff = (argc > 1) ? atoi(argv[1]) : 1;

    printf("PID: %d\n", getpid());

    dev = open(MODULE_FILENAME, O_RDWR | O_NDELAY);
    if (dev < 0) {
        printf("module open error\n");
        exit(1);
    }

    if (write(dev, &buff, 1) < 0) {
        printf("write error\n");
        goto err;
    }

    if (read(dev, &buff, 1) < 0) {
        printf("read error\n");
    }

    printf("read data: %c\n", buff);

err:
    close(dev);

    return 0;
}
