#ifndef WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#include <winsock2.h>
#include <math.h>
#endif

char *sprobetypes[10] = 
{
	"BLP_P",
	"LIP_P",
	"LDP_P",
	"BLP_A",
	"LIP_A",
	"LDP_A",
	"BLP_AP",
	"LIP_AP",
	"LDP_AP",
	"UNKNOWN"
};
char *sflowtypes[2] = { "P", "A" };

int readwrapper(int sock, char *buf, size_t size)
{
	int ret = 0;
	unsigned int curread = 0;
	fd_set rfds;
	struct timeval tv;
	int retval;

	while(curread < size)
	{
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		tv.tv_sec = 300;
		tv.tv_usec = 0;
		retval = select(sock+1, &rfds, NULL, NULL, &tv);
		if(retval == -1)
		{
			perror("error reading");
			return -1;
		}
		else if(retval == 0)
		{
			return -1;
		}

		ret = recv(sock, buf + curread, (int)size - curread, 0);
		if(ret == -1)
		return ret;
		if(ret == 0)
		return ret;

		curread += ret;
	}

	return curread;
}
int writewrapper(int sock, char *buf, size_t size)
{
	int ret = 0;
	unsigned int curwrite = 0;

	while(curwrite < size)
	{
		ret = send(sock, buf + curwrite, (int)size - curwrite, 0);
		if(ret == -1)
		return ret;

		curwrite += ret;
	}

	return curwrite;
}

#ifdef WIN32
#include <stdio.h>
HANDLE hTimer;
LARGE_INTEGER freq;

int select123(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, 
					 const struct timeval *timeout)
{
	if(readfds == NULL && writefds == NULL && exceptfds == NULL)
	{
		// is this highest-res?
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = -(LONGLONG)(timeout->tv_sec*1e7 + timeout->tv_usec*10);

		// Set a timer to wait
		if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
		{
			printf("SetWaitableTimer failed (%d)\n", GetLastError());
			return -1;
		}
		// Wait for the timer.
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			printf("WaitForSingleObject failed (%d)\n", GetLastError());

		return 0;
	}
	else
	{
		return select(nfds, readfds, writefds, exceptfds, timeout);
	}
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	LARGE_INTEGER t;
	QueryPerformanceCounter((LARGE_INTEGER *)&t);

	tv->tv_sec = (long)floor(1.0*t.QuadPart / freq.QuadPart);
	tv->tv_usec = (long)((1.0*t.QuadPart/freq.QuadPart - tv->tv_sec)*1.0e6);

	return TRUE;
}

#endif
