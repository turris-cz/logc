#include <unistd.h>

struct fakesyslog {
	int priority;
	char *msg;
};

extern struct fakesyslog *fakesyslog;
extern size_t fakesyslog_cnt;

void fakesyslog_reset();
void fakesyslog_free();
