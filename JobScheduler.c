#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>
#include<dirent.h>

struct eyepee
{
	char addresses[30];
	int accepvalue;
	float load1,load5,load15;
	float effectiveLoad;
	int flag;
	int failflag;
	char processes[100][50];
	int noOfProcess;
}eyearr[20];

struct sockaddr_in sad,cad;
int leastaccep,tobeincreased;
float leastload;
int sockdes,len,value[5],vindex=0,tmpv=-1,i,j;
char buff[100],str[100],address[30];
pthread_t pth1,pth2[20],pth3,pth4,chk_redist,chk_bal,checkNodeThread;
int pthcnt=0;
int cnt,tmpcnt;
char tmp1[10],tmp2[10],tmp3[10],tmpaddr[30];
float tmpload1=0,tmpload5=0,tmpload15=0;
char processbuff[1000];int nop;char tmpp[50];

void *threadFuncb(void *val)
{
	int corrindex,i;
	int *vall=(int *)val;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	//if(eyearr[corrindex].failflag!=1)
	{
		while(1)
		{
			for(i=0;i<pthcnt;i++)
			{
				if(eyearr[i].accepvalue == *vall)
				{
					corrindex = i;
					break;
				}
			}
			if(eyearr[corrindex].failflag!=0)
			{
				//pthread_exit((void *)0);
				printf("\nthread of process %d killed!\n",corrindex);
				break;
			}	
			else if(read(*vall,buff,sizeof(buff)))
			{
				read(*vall,processbuff,sizeof(processbuff));
					printf("Process list!:\n%s\n",processbuff);
				tmpcnt=0;
				for(cnt=0;buff[cnt]!=' ';cnt++)
					tmp1[tmpcnt++] = buff[cnt];
				tmp1[tmpcnt]='\0';



			//	printf("\n\ntmp1:%s\n\n",tmp1);
				int convCount=0;
				//tmpload1 = strtod(tmp1,NULL);
				//printf("ch tmp1:%f\n",tmpload1);
				tmpload1 = 0;
				while(tmp1[convCount]!='.')
				{

					tmpload1 = tmpload1*10 + (tmp1[convCount++]-48);
				}
				float decimalTmpLoad1 = 0;
				convCount = strlen(tmp1)-1;
				while(tmp1[convCount]!='.')
				{
					decimalTmpLoad1 = decimalTmpLoad1/10 + (tmp1[convCount--]-48);
				}
				tmpload1 += (decimalTmpLoad1/10);
			//	printf("\n\ntmpload1:%f\n\n",tmpload1);


				tmpcnt=0;cnt++;
				while(buff[cnt]!=' ')
					tmp2[tmpcnt++] = buff[cnt++];
				tmp2[tmpcnt]='\0';





			//	printf("\n\ntmp2:%s\n\n",tmp2);
				//tmpload5 = strtof(tmp2,NULL);
				convCount=0;
				tmpload5 = 0;
				while(tmp2[convCount]!='.')
				{

					tmpload5 = tmpload5*10 + (tmp2[convCount++]-48);
				}
				
				float decimalTmpLoad5 = 0;
				convCount = strlen(tmp2)-1;
				while(tmp2[convCount]!='.')
				{
					decimalTmpLoad5 = decimalTmpLoad5/10 + (tmp2[convCount--]-48);
				}
				tmpload5 += (decimalTmpLoad5/10);
			//	printf("\n\ntmpload5:%f\n\n",tmpload5);



				tmpcnt=0;cnt++;
				while(buff[cnt]!='|')
					tmp3[tmpcnt++]=buff[cnt++];
				tmp3[tmpcnt]='\0';



		//		printf("\n\ntmp3:%s\n\n",tmp3);
				//tmpload15 = strtof(tmp3,NULL);
				convCount=0;
				tmpload15 = 0;
				while(tmp3[convCount]!='.')
				{

					tmpload15 = tmpload15*10 + (tmp3[convCount++]-48);
				}
				float decimalTmpLoad15 = 0;
				convCount = strlen(tmp3)-1;
				while(tmp3[convCount]!='.')
				{
					decimalTmpLoad15 = decimalTmpLoad15/10 + (tmp3[convCount--]-48);
				}
				tmpload15 += (decimalTmpLoad15/10);
		//		printf("\n\ntmpload15:%f\n\n",tmpload15);



				tmpcnt=0;cnt++;
				while(buff[cnt]!='\0')
					tmpaddr[tmpcnt++]=buff[cnt++];
				tmpaddr[tmpcnt]='\0';

				int z,y,x,w;
				for(z=0;z<pthcnt;z++)
				{
					if(!strcmp(tmpaddr,eyearr[z].addresses))
					{
						eyearr[z].load1 = tmpload1;
						eyearr[z].load5 = tmpload5;
						eyearr[z].load15 = tmpload15;
						eyearr[z].flag++;
						for(y=0;processbuff[y]!=' ';y++)
							tmpp[y] = processbuff[y];
						tmpp[y] = '\0';
						y++;
						nop = atoi(tmpp);

						printf("\n!!!!!!!!!!!!nop:%d\n!!!!!!!!!",nop);
						eyearr[z].noOfProcess=0;
						for(x=0;x<nop;x++)
						{
							w=0;
							while((processbuff[y]!=' ')&&(processbuff[y]!='\0'))
							{
								tmpp[w++] = processbuff[y++];
							}
							y++;
							tmpp[w] = '\0';
							printf("prcss %d:%s\n",x,tmpp);
							strcpy(eyearr[z].processes[eyearr[z].noOfProcess++],tmpp);
						}
						break;
					}
				}
				puts(buff);
				strcpy(buff,"\0");
			}
			
		}
	}
	return NULL;
}

void *threadFunca()
{
	while(1)
	{
		tmpv=accept(sockdes,(struct sockaddr*)&cad,&len);
		if(tmpv!=-1)
		{
			value[vindex++]=tmpv;
			read(value[vindex-1],address,sizeof(address));
			strcpy(eyearr[pthcnt].addresses,address);
			eyearr[pthcnt].accepvalue = value[vindex-1];
			eyearr[pthcnt].failflag = 0;
			int i,max=0;
			for(i=0;i<pthcnt;i++)
			{
				if(eyearr[i].flag > max)
				{
					max = eyearr[i].flag;
					eyearr[pthcnt].flag = max;
					
				}
			}
			eyearr[pthcnt].noOfProcess=0;
			printf("connection %d recognized..address is %s\n",vindex,address);
			pthread_create(&pth2[pthcnt++],NULL,threadFuncb,&value[vindex-1]);
			tmpv=-1;
		}
	}
}

void *threadfuncc()
{
	int cnt;
	while(1)
	{
		for(cnt=0;cnt<pthcnt;cnt++)
		{
			printf("client %d\n",(cnt+1));
			printf("Address:%s\n",eyearr[cnt].addresses);
			printf("AccepValue:%d\n",eyearr[cnt].accepvalue);
			printf("load1:%f\n",eyearr[cnt].load1);printf("\n\n");
		}
		sleep(10);
	}
}

void *checkNodeFailure()
{
	int i,q,cntx,cntxx;
	int max,nopc;
	char failstr[100];
	while(1)
	{
		max=0;nopc=0;
		for(i=0;i<pthcnt;i++)
		{
			if(eyearr[i].flag>max)
				max = eyearr[i].flag;
		}
		for(i=0;i<pthcnt;i++)
		{
			if((max - eyearr[i].flag) > 1)
			{
				printf("\nNode %d has failed !!!\n",i);
				eyearr[i].failflag = 1;
				for(cntx=0;cntx<eyearr[i].noOfProcess;cntx++)
				{
					
					printf("\nRestarting failed process...%d",eyearr[i].noOfProcess);
					strcpy(failstr,"FAIL ");
					leastaccep = eyearr[0].accepvalue;
					leastload = 10; 
					for(q=0;q<pthcnt;q++)
					{
						if((eyearr[q].effectiveLoad<leastload)&&(eyearr[q].failflag!=1))
						{
							leastload = eyearr[q].effectiveLoad;
							leastaccep = eyearr[q].accepvalue;
							tobeincreased = q;
						}
					}
					eyearr[tobeincreased].effectiveLoad +=  10.0;
					strcat(failstr,eyearr[i].processes[cntx]);
					write(eyearr[tobeincreased].accepvalue,failstr,sizeof(failstr));
				}
				eyearr[i].noOfProcess = 0;
				pthread_cancel(pth2[i]);
				for(cntxx=i;cntxx<(pthcnt-1);cntx++)
					eyearr[cntxx]=eyearr[cntxx+1];
				pthcnt--;
			}
		}
		sleep(5);
	}
}

void *threadfunchk()
{
	while(1)
	{
		int i;
		for(i=0;i<pthcnt;i++)
		{
			eyearr[i].effectiveLoad = 50*eyearr[i].load1 + 30*eyearr[i].load5 + 20*eyearr[i].load15;
		}
		int avg=0;
		for(i=0;i<pthcnt;i++)
		{
			avg += eyearr[i].effectiveLoad;
		}
		if(pthcnt>0)
		{
			avg = avg/pthcnt;
			char cp[3];
			strcpy(cp,"CP");
			for(i=0;i<pthcnt;i++)
			{	
				if((eyearr[i].effectiveLoad - avg > 2))//&&(eyearr[i].failflag!=0))
				{
					write(eyearr[i].accepvalue,cp,sizeof(cp));
				}
			}
		}
		sleep(15);
	}
}

void *threadredist()
{
	DIR *dp;
	struct dirent *ep = NULL;
	int i;
	char tobesent[100];
	while(1)
	{
		strcpy(tobesent,"\0");
		dp = opendir("/home/cse4119/context_files/redist/");
		if(dp!=NULL)
		{
			while(ep = readdir(dp))
			{
				if(strstr(ep->d_name,"context")!=NULL)
				{
					//printf("\nDirectory file begin!\n");
					puts(ep->d_name);
					printf("\nDir file: %s\n",ep->d_name);
					//printf("\nDirectory file end!\n");
					leastaccep = eyearr[0].accepvalue;
					leastload = 10; 
					for(i=0;i<pthcnt;i++)
					{
						if((eyearr[i].effectiveLoad<leastload)&&(eyearr[i].failflag!=1))
						{
							leastload = eyearr[i].effectiveLoad;
							leastaccep = eyearr[i].accepvalue;
							tobeincreased = i;
						}
					}
					eyearr[tobeincreased].effectiveLoad +=  10.0;
					//strcpy(tobesent,"cr_restart ");
					strcpy(tobesent,ep->d_name);
					write(leastaccep,tobesent,sizeof(tobesent));
					strcpy(ep->d_name,"\0");
				}
			}
			(void)closedir(dp);
			ep=NULL;
			strcpy(tobesent,"\0");
			sleep(5);
		}
		else
			perror("couldnt open directory");
	}
}

main()
{
	int dum,max;
	fflush(stdin);
	//max = atoi(argv[1]);
	sockdes=socket(AF_INET,SOCK_STREAM,0);
	bzero(&sad,sizeof(sad));
	sad.sin_family=AF_INET;
	sad.sin_port=htons(4023);
	sad.sin_addr.s_addr=INADDR_ANY;
	bind(sockdes,(struct sockaddr*)&sad,sizeof(sad));
	listen(sockdes,5);
	len=sizeof(cad);
	pthread_create(&checkNodeThread,NULL,checkNodeFailure,NULL);
	pthread_create(&pth1,NULL,threadFunca,NULL);
	//pthread_create(&pth3,NULL,threadfuncc,NULL);
	pthread_create(&pth4,NULL,threadfunchk,NULL);
	pthread_create(&chk_redist,NULL,threadredist,NULL);
	while(1)
	{
	}
	close(sockdes);
	for(i=0;i<vindex;i++)
		close(value[i]);
}
