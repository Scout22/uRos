#ifndef SIGNAL_FUNCTION_H
#define SIGNAL_FUNCTION_H
#include <signal.h>

#define SIGNALSTOP SIGINT
//------------------------------------------------------------------------------//
// Cette fonction desactive les interruptions li�es au signal sig				//
// pass� en argument															//
//------------------------------------------------------------------------------//
void disable_signal(sigset_t* signals1, int sig);
//------------------------------------------------------------------------------//
// Cette fonction reactive les interruptions li�es au signal signals1			//
// pass� en argument															//
//------------------------------------------------------------------------------//
void reenable_signal(sigset_t* signals1);
//------------------------------------------------------------------------------//
// Cette teste si un signal particulier sig � ete attrap�						//
// et retour 1 pour oui et 0 pour non											//
//------------------------------------------------------------------------------//
int is_signal_trigered(sigset_t *signals2, int sig);
#endif