#include <stdio.h>
#include <nl.h>
#include "server.h"

void printErrorExit(void)
{
    NLenum err = nlGetError();
    
    if(err == NL_SYSTEM_ERROR)
        printf("System error: %s\n", nlGetSystemErrorStr(nlGetSystemError()));
    else
        printf("HawkNL error: %s\n", nlGetErrorStr(err));

    nlShutdown();
    exit(1);
}

int main()
{
    NLsocket serversock;
    NLenum   type = NL_IP;/* default network type */
    NLint    exitcode;

    if (!nlInit()) printErrorExit();

    printf("nlGetString(NL_VERSION) = %s\n", nlGetString(NL_VERSION));
    printf("nlGetString(NL_NETWORK_TYPES) = %s\n", nlGetString(NL_NETWORK_TYPES));

    if (!nlSelectNetwork(type)) printErrorExit();

    serversock = nlOpen(2000, NL_RELIABLE);
    
    if (serversock == NL_INVALID) printErrorExit();
    
    if (!nlListen(serversock))       /* let's listen on this socket */
    {
        nlClose(serversock);
        printErrorExit();
    }

    ServerDispatch *server = new ServerDispatch();
    server->Run(serversock);
    delete server;

    nlShutdown();
    return 0;
}
