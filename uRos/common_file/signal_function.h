#ifndef SIGNAL_FUNCTION_H
#define SIGNAL_FUNCTION_H
#include <signal.h>

#define SIGNALSTOP SIGINT
//------------------------------------------------------------------------------//
// Cette fonction desactive les interruptions liées au signal sig				//
// passé en argument															//
//------------------------------------------------------------------------------//
void disable_signal(sigset_t* signals1, int sig);
//------------------------------------------------------------------------------//
// Cette fonction reactive les interruptions liées au signal signals1			//
// passé en argument															//
//------------------------------------------------------------------------------//
void reenable_signal(sigset_t* signals1);
//------------------------------------------------------------------------------//
// Cette teste si un signal particulier sig à ete attrapé						//
// et retour 1 pour oui et 0 pour non											//
//------------------------------------------------------------------------------//
int is_signal_trigered(sigset_t *signals2, int sig);
#endif