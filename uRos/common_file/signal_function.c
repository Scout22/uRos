#include "signal_function.h"


void reenable_signal(sigset_t *signals1){
	sigemptyset(signals1); // empty signals1
	sigprocmask(SIG_SETMASK, signals1, 0); // unblock the signal
}

int is_signal_trigered(sigset_t *signals2,int sig ) {
	sigpending(signals2); // retrieve pending signals
	//if the specific signals was triggerd return 1 else 0
	if (sigismember(signals2, SIGNALSTOP)) {
		return 1;
	}
	return 0;
}

void disable_signal(sigset_t* signals1, int sig) {
	sigemptyset(signals1); // empty signals1
	sigaddset(signals1, sig); // select signal to disable
	sigprocmask(SIG_SETMASK, signals1, 0); // block the signals selected byignals1
}