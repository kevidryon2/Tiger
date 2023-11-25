#include "daemon.h"

extern int create_daemon;

//first half of daemonization
void daemon_start() {
	printf("Started Daemon.\n");
	
	//create dir /run/tiger/
	mkdir("/run/tiger", 0777);
	
	//delete log files
	unlink("/run/tiger/info.log");
	unlink("/run/tiger/err.log");
	
	FILE *info = fopen("/run/tiger/info.log", "a");
	FILE *err = fopen("/run/tiger/err.log", "a");
	
	dup2(fileno(info), STDOUT_FILENO);
	dup2(fileno(err), STDERR_FILENO);
	
	create_daemon = true;
}

//second half of daemonization
void daemon_init() {
	int pid;
	
	pid = fork();
	if (pid < 0) exit(1); // error
	if (pid > 0) exit(0); // let the parent terminate
	
	setsid();
	
	//ignore signals
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	
	pid = fork();
	if (pid < 0) exit(1); // error
	if (pid > 0) exit(0); // let the parent terminate
	
	umask(0);
	
	//we already chdir'd
	
	//we are now a daemon!
	
	//write pidfile '/run/tiger/pid'
	FILE *fp = fopen("/run/tiger/pid", "w");
	fprintf(fp, "%d\n", getpid());
	fclose(fp);
}

void daemon_stop() {
	printf("Stopping Daemon...\n");
	FILE *fp = fopen("/run/tiger/pid", "r");
	
	if (!fp) {
		printf("Tiger hasn't yet started.\n");
		exit(1);
	}
	
	int pid;
	fscanf(fp, "%d", &pid);
	
	printf("PID: %d\n", pid);
	
	kill(pid, SIGTERM);
	
	fclose(fp);
	
	//remove pidfile
	unlink("/run/tiger/pid");
}
