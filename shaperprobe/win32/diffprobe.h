#ifndef _DIFFPROBE_H_
#define _DIFFPROBE_H_

#include <stdio.h>

#define VERSION 5
#define RATE_FACTOR 0.9
#define RATE_DROP_FACTOR 2
#define LOSS_RATE_THRESH 0.2

#define SELECTPORT 55000
#define NUM_SELECT_SERVERS 3
#define SERV_PORT (55005)
#define SERV_PORT_UDP (55005)
#define MAX_NLIPS 1//5

#define LISTENQ (10)
#define TRAIN_LENGTH 50 
#define NITERATIONS 10
#define SUBTRAINLEN 6
#define TRAINGGTHRESH 0.5
#define TRAINEQTHRESH 0.1
#define GG(A,B) (A/B > 1+TRAINGGTHRESH)
#define EQ(A,B) (A/B > 1-TRAINEQTHRESH && A/B < 1+TRAINEQTHRESH)

#define TBDURATION 60
//#define TB_RATE_AVG_INTERVAL 0.3
#define TB_RATE_LOG_INTERVAL 0.05
#define TB_NPRIOR 3
#define TB_NPOSTERIOR 8
#define TB_NTOTLOSSPOSTERIOR 20
#define TB_RATERATIO 1.10 //1.25
#define TB_LOSSRATE 0.1
#define TB_TOTLOSSRATE 0.01
#define TB_SMOOTH_WINDOW 11
#define TB_SMOOTH_WINDOW_HALF 5
#define TB_SMOOTH_WINDOW_HALF_HALF 2
#define TB_SMOOTH_THRESH TB_RATERATIO
#define TB_MAX_TRAINLEN 5

#define MFLOWDURATION 5

#define UDPIPHEADERSZ 28


int prober_bind_port(int port);

double prober_sleep_resolution();
void prober_swait(struct timeval, double sleepRes);
void prober_sbusywait(struct timeval);
struct timeval prober_packet_gap(struct timeval y, struct timeval x);

int tbdetectReceiver(int tcpsock, int udpsock, double capacity, double sleepRes, unsigned int *result, unsigned int *minbktdepth, unsigned int *maxbktdepth, double *tbrate, unsigned int *abortflag, FILE *fp);
int tbdetectSender(int tcpsock, int udpsock, struct sockaddr_in *from, double capacity, double sleepRes, unsigned int *result, unsigned int *minbktdepth, unsigned int *maxbktdepth, double *tbrate, unsigned int *abortflag, FILE *fp);
void printShaperResult(unsigned int tbresult, unsigned int tbmindepth, unsigned int tbmaxdepth, double tbrate, unsigned int tbabortflag, int dir, int (*eprintf)(char *,void  *), void *);

int mflowSender(int tcpsock, int udpsock, struct sockaddr_in *from, double capacity, double sleepRes, double *meascap, int lowprobe);
int mflowReceiver(int tcpsock, int udpsock, double *meascap, FILE *fp, int lowprobe);

#ifdef WIN32
int main123(void *edit, int (*eprintf)(char *,void  *), int (*eprogress)(int,void *), int (*estatus)(char *,void *));
int compd(const void *a, const void *b);
#endif

#define CHKRET(a) if(a != -1); \
	else return -1
#define CHKRETPTR(a) if(a != NULL); \
	else return -1

#endif

