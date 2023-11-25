#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

void daemon_start();
void daemon_init();
void daemon_stop();
