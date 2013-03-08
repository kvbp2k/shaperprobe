/*
* Packet replayer/cloner.
  * 
  * November 2008.
  *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#define __FAVOR_BSD /* For compilation in Linux.  */
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/select.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#else

#include <winsock2.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#endif

#include "tcpclient.h"
#include "tcpserver.h"
#include "packet.h"
#include "diffprobe.h"

#define PROBER_CONFIG "prober.conf"

/* Global paremeters from config.  */
unsigned int serverip = 0;
unsigned int clientip = 0;

unsigned int verbose = 0;


/* Utility functions.  */

char * ip2str(unsigned int ip)
{
  struct in_addr ia;

  ia.s_addr = ip;

  return inet_ntoa(ia);
}

unsigned int str2ip(char *ip)
{
  struct in_addr ia;
  unsigned long r;
  r = inet_addr(ip);
  ia.S_un.S_addr = r;
  if (r) return ntohl(ia.s_addr);
  return 0;
}

void die(char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(0);
}

int tryRandServers(unsigned long *serverList, int num_servers, int fileid)
{
	int tcpsock = -1, num = 0, i = 0;
	char *visited = (char*)malloc(num_servers*sizeof(char));
	memset(visited, 0, num_servers);
	while(1)
	{	
		int flag = 0;
		for(i = 0; i < num_servers; i++)
		{
			if(visited[i] == 0)
			{
				flag = 1;
				break;
			}
		}
		if(flag == 0)
		{
			printf("All servers are busy; please try in a few minutes.\n");
			free(visited);
			return -1;
		}
		num = rand()%num_servers;
		if(visited[num] == 1)
			continue;
		visited[num] = 1;
		tcpsock = connect2server(serverList[num], fileid);
		if(tcpsock == -1)	
			continue;
		break;
	}
	serverip = serverList[num];
	free(visited);
	return tcpsock;
}

int selectServer(int fileid)
{
#define MAXDATASIZE 25
	char *selectorList[NUM_SELECT_SERVERS] = {"64.9.225.142","64.9.225.153","64.9.225.166"};
	int visited[NUM_SELECT_SERVERS], size = 0;
	int num, ctr_code, sockfd, numbytes, num_servers, tcpsock;
	char buf[MAXDATASIZE], ctr_buff[8];
	struct sockaddr_in their_addr;
	char hostname[128];
	unsigned long *serverlist;

	srand((unsigned int)time(NULL));
	memset(visited, 0, NUM_SELECT_SERVERS*sizeof(int));
	memset(hostname, 0, 128);
	while(1)
	{
		int flag = 0;
		int i = 0;
		for(i = 0; i < NUM_SELECT_SERVERS; i++)
		{
			if(visited[i]==0)
			{
				flag = 1;
				break;
			}
		}
		if(!flag)
		{
			MessageBox(NULL, "All servers are busy. Please try again later.", "Connect", MB_ICONERROR|MB_OK);
			return -1;
		}
		num = rand()%NUM_SELECT_SERVERS;
		if(visited[num] == 1)
			continue;
		visited[num] = 1;
		strcpy(hostname,selectorList[num]);
		if((sockfd = (int)socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror("socket");
			continue;
		}
		their_addr.sin_family = AF_INET;
		their_addr.sin_port = htons(SELECTPORT);
		their_addr.sin_addr.s_addr = htonl(str2ip(hostname));
		memset((char *)&(their_addr.sin_zero), 0, sizeof(their_addr.sin_zero));
		if(connect(sockfd,(struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
		{
			perror("connect");
			continue;
		}
		if((numbytes=recv(sockfd, ctr_buff, sizeof(unsigned int), 0)) == -1)
		{
			perror("recv");
			closesocket(sockfd);
			return -1;
		}
		memcpy(&ctr_code, ctr_buff, sizeof(unsigned int));
		num_servers = ntohl(ctr_code);
		serverlist = malloc(num_servers*sizeof(unsigned long));
		for(i = 0; i < num_servers; i++)
		{
			memset(buf, 0, MAXDATASIZE);
			if((numbytes=recv(sockfd, ctr_buff, sizeof(unsigned int), 0)) == -1)
			{
				closesocket(sockfd);
				return -1;
			}
			memcpy(&ctr_code, ctr_buff, sizeof(unsigned int));
			size = ntohl(ctr_code);
			if((numbytes=recv(sockfd, buf, size, 0)) == -1)
			{
				perror("recv");
				closesocket(sockfd);
				return -1;
			}
			serverlist[i] = htonl(str2ip(buf));
		}
		closesocket(sockfd);
		break;
	}
	tcpsock = tryRandServers(serverlist, num_servers, fileid);
	free(serverlist);
	return tcpsock;
}


int prober_config_load(char *tracefile, int *fileid)
{
  serverip = htonl(str2ip("143.215.129.100")); //38.102.0.111
  //serverip = htonl(str2ip("38.102.0.111")); //38.102.0.111
  return 0;
}

int sendData(int tcpsock, char *filename)
{
	prcvdata pkt;
	struct _stat infobuf;
	int ret = 0, len = 0, bytesleft = 0;
	char *buf = NULL;
	FILE *fp;

	if(_stat(filename, &infobuf) == -1)
	{
		perror("error: file");
		return -1;
	}
	len = infobuf.st_size;

	//printf("\nsending measurement data to server."); fflush(stdout);
	pkt.header.ptype = P_RECVDATA;
	pkt.header.length = 0;
	pkt.datalength = htonl(len);
	ret = writewrapper(tcpsock, (char *)&pkt, sizeof(struct _rcvdata));
	if(ret == -1)
	{
		fprintf(stderr, "CLI: error sending data to serv: %d\n", tcpsock);
		closesocket(tcpsock);
		return -1;
	}

	buf = (char *)malloc(len*sizeof(char));
	fp = fopen(filename, "r");
	ret = (int)fread((void *)buf, sizeof(char), len, fp);
	fclose(fp);

	bytesleft = len;
	while(bytesleft > 0)
	{
		int tosend = (bytesleft > 1400) ? 1400 : bytesleft;
		//ret = writewrapper(tcpsock, (char *)buf, len);
		ret = writewrapper(tcpsock, (char *)buf+(len-bytesleft), tosend);
		if(ret == -1)
		{
			fprintf(stderr, "CLI: error sending data to serv: %d\n", tcpsock);
			perror("");
			closesocket(tcpsock);
			free(buf);
			return -1;
		}
		bytesleft -= ret;
	}

	//printf(".done.\n");
	free(buf);
	return 0;
}


int main123(void *edit, int (*eprintf)(char *,void  *), int (*eprogress)(int,void *), int (*estatus)(char *,void *))
{
  int tcpsock = 0;
  int udpsock = 0;
  struct sockaddr_in from;
  double capacityup = 0, capacitydown = 0;
  double measupcap = 0, measdowncap = 0;
  unsigned int tbresult = 0, tbmindepth = 0, tbmaxdepth = 0, tbabortflag = 0;
  double tbrate = 0, truecapup = 0, truecapdown = 0;
  double sleepRes = 1;
  char filename[256], tracefile[256];
  int fileid = -1;
  struct in_addr sin_addr;
  struct timeval tv;
  FILE *fp;
  WSADATA wsa;
  extern LARGE_INTEGER freq;
  extern HANDLE hTimer;
  char str[256];
  extern double TB_RATE_AVG_INTERVAL;

  TB_RATE_AVG_INTERVAL = 0.3;

  WSAStartup(MAKEWORD(2,2), &wsa);
  QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
  hTimer = CreateWaitableTimer(NULL, TRUE, "WaitableTimer");
  if(hTimer == NULL)
  {
	  return 0;
  }

  eprintf("DiffProbe release. January 2012. Build 1008.\r\n", edit);
  eprintf("Shaper Detection Module.\r\n\r\n", edit);

  memset(tracefile, 0, 256);
  CHKRET(prober_config_load(tracefile, &fileid));

  sleepRes = prober_sleep_resolution();

  //tcpsock = connect2server(serverip, fileid);
  tcpsock = selectServer(fileid);
  if(tcpsock <= 0) eprintf("Server busy. Please try again later.\r\n", edit);
  CHKRET(tcpsock);

  memset(&from, 0, sizeof(from));
  from.sin_family      = PF_INET;
  from.sin_port        = htons(SERV_PORT_UDP);
  from.sin_addr.s_addr = serverip;

  gettimeofday(&tv, NULL);
  sin_addr.s_addr = serverip;
  memset(filename, 0, 256);
  sprintf(filename, "%s_%d.txt", inet_ntoa(sin_addr), (int)tv.tv_sec);
  fp = fopen(filename, "w");
  fprintf(fp, "sleep time resolution: %.2f ms.\r\n\0", sleepRes*1000);

  udpsock = udpclient(serverip, SERV_PORT_UDP);
  CHKRET(udpsock);
  sin_addr.s_addr = serverip;
  sprintf(str, "Connected to server %s.\r\n\0", inet_ntoa(sin_addr));
  eprintf(str, edit);

  eprintf("\r\nEstimating capacity:\r\n", edit);
  capacityup = estimateCapacity(tcpsock, udpsock, &from, edit, estatus);
  CHKRET(capacityup);
  CHKRET(sendCapEst(tcpsock));
  capacitydown = capacityEstimation(tcpsock, udpsock, &from, fp, edit, estatus);
  CHKRET(capacitydown);
  eprogress(5, edit);

  mflowSender(tcpsock, udpsock, &from,
	(capacityup > 200000) ? 195000 : capacityup, sleepRes, &measupcap, 0);
  mflowReceiver(tcpsock, udpsock, &measdowncap, fp, 0);
  //XXX: the meas code needs trains and lower cpu
  //following two lines for 802.11a/b/g/n links
  if(capacityup < 80000) capacityup = measupcap;
  if(capacitydown < 80000) capacitydown = measdowncap;
  if(capacityup > 200000)
  {
	  sprintf(str, "Upstream: greater than 200 Mbps.\r\n");
	  capacityup = 195000;
  }
  else
  {
	  sprintf(str, "Upstream: %d Kbps.\r\n", (int)capacityup);
	  //capacityup *= 0.95;
  }
  truecapup = capacityup;
  eprintf(str, edit);
  if(capacitydown > 200000)
  {
	  sprintf(str, "Downstream: greater than 200 Mbps.\r\n");
	  capacitydown = 195000;
  }
  else
  {
	  sprintf(str, "Downstream: %d Kbps.\r\n", (int)capacitydown);
	  //capacitydown *= 0.95;
  }
  eprintf(str, edit);
  truecapdown = capacitydown;
  eprogress(10, edit);

  sprintf(str, "\r\nThe measurement will take upto %.1f minutes. Please wait.\r\n",
	0.5*ceil(2*(
	(2 * (60)  // probing + low-rate
	+ (40560 + 3.5 * capacitydown * (60) ) * 8 / (1000*capacityup))/60)));  // to upload file 
  eprintf(str, edit);

  sprintf(str, "\r\nChecking for traffic shapers:\r\n\r\n"); eprintf(str, edit); estatus("Measuring upload..", edit);
  mflowSender(tcpsock, udpsock, &from, -1, sleepRes, NULL, 1);
  CHKRET(tbdetectSender(tcpsock, udpsock, &from, capacityup, sleepRes, 
		  &tbresult, &tbmindepth, &tbmaxdepth, &tbrate, &tbabortflag, fp));
  if(tbresult == 1) truecapup = tbrate;
  eprogress(30, edit);
  //mflowSender(tcpsock, udpsock, &from, (tbresult == 1) ? tbrate : capacityup/2.0, sleepRes);
  printShaperResult(tbresult, tbmindepth, tbmaxdepth, tbrate, tbabortflag, 0, eprintf, edit);
  eprogress(50, edit);

  estatus("Measuring download..", edit);
  mflowReceiver(tcpsock, udpsock, NULL, fp, 1);
  CHKRET(tbdetectReceiver(tcpsock, udpsock, capacitydown, sleepRes,
		  &tbresult, &tbmindepth, &tbmaxdepth, &tbrate, &tbabortflag, fp));
  if(tbresult == 1) truecapdown = tbrate;
  eprogress(60, edit);
  //mflowReceiver(tcpsock, udpsock, fp);
  fclose(fp);
  eprogress(75, edit);
  sendData(tcpsock, filename);
  printShaperResult(tbresult, tbmindepth, tbmaxdepth, tbrate, tbabortflag, 1, eprintf, edit);
  estatus("", edit); eprogress(100, edit);

  closesocket(udpsock);
  closesocket(tcpsock);
  _unlink(filename);

  eprintf("\r\nFor more information, visit: http://www.cc.gatech.edu/~partha/diffprobe\r\n", edit);

  return(0);
}

