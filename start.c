#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>

struct sockaddr_in sad,cad;
int sockdes,val,value;
char buff[100],str[100],buff1[100];
pthread_t restartThread,restartThreadRem;
char processIDs[100][50];
int noOfProcess;
int OK;
char message[1000];

void *periodicCheckpoint()
{
	char str[100];
	FILE *procs,*fp;
	char processID[100],context[100],check[400];
	int noOfProcess,loop;
	char numberOfProcess[10];
	while(1)
	{
		
		OK=0;
		procs = popen("ps -e -o pid,cmd","r");
		noOfProcess=0;
		fscanf(procs,"%s",processID);
		fgets(str,100,procs);
		while(!feof(procs))
		{
			
			fscanf(procs,"%s",processID);
			fgets(str,100,procs);
			if(strstr(str,"a.out")!=NULL)
			{
				sprintf(str,"cr_checkpoint --save-all %s",processID);
				strcpy(check,"\0");
				fp=popen(str,"r");
				fgets(check,100,fp);
				fclose(fp);
				
				if(strstr(check,"Checkpoint")!=NULL)
				{
					printf("\nThe selected file could not be checkpointed");
				}
				else
				{
					printf("\nPeriodic Checkpoint successful\n");
					sprintf(context,"chmod 777 context.%s",processID);
					system(context);
					sprintf(context,"mv context.%s /home/cse4119/context_files/bal/context.%s",processID,processID);
					system(context);

					sprintf(context,"/home/cse4119/context_files/bal/context.%s",processID);
					strcpy(processIDs[noOfProcess],context);
					noOfProcess++;
				}
			}
					
		}
		
		
		sprintf(numberOfProcess,"%d ",noOfProcess);
		strcpy(message,numberOfProcess);
		for(loop=0;loop<noOfProcess;loop++)
		{
			strcat(message,processIDs[loop]);
			strcat(message," ");
		}
		OK = 1;
		sleep(15);
	}

}

void *restartFunc(void *Val)
{
	
	int i;
	char *dummy = (char *)Val;
	char restart[100],rstart[100],remove[100];
	sprintf(remove,"rm %s",dummy);
	strcpy(restart,"cr_restart --no-restore-pid ");



	if(strstr(buff,"FAIL")!=NULL)
	{
		for(i=5;i<=strlen(buff);i++)
		{
			rstart[i-5]=buff[i];
		}
		strcat(restart,rstart);
		system(restart);
		//system(remove);

	}
	else
	{
		strcat(restart,buff);
		system(restart);
		//system(remove);
	}
}

void *restartFuncDel(void *string)
{
	char *rmvstr = (char *)string;
	char remove[100];
	sprintf(remove,"rm %s",rmvstr);
	printf("\n\nabout to remove %s\n\n",rmvstr);
	sleep(5);
	system(remove);
}

void *threadfunc()
{
	int cnt,cnt1;
	char str[100];
	char copy[100],tmpstr[100],tmpfinal[100];
	int z;int i;
	char restartFile[100],rstart[100];
	int pos;
	char processID[100],check[400],context[100];
	FILE *fp;
	while(1)
	{
		if(read(sockdes,buff,sizeof(buff)))
		{
			puts(buff);
			if(!strcmp(buff,"CP"))
			{
				FILE *procs;
				procs = popen("ps -e -o pid,cmd","r");
				fscanf(procs,"%s",processID);
				fgets(str,100,procs);

				while(!feof(procs))
				{
					fscanf(procs,"%s",processID);
					fgets(str,100,procs);

					
					if(strstr(str,"a.out")!=NULL)
					{
						sprintf(str,"cr_checkpoint --save-all --kill %s",processID);
						strcpy(tmpstr,"/home/cse4119/context_files/bal/context.");
						strcat(tmpstr,processID);
						printf("\nHere\n");
						for(cnt=0;strcmp(tmpstr,processIDs[cnt])!=0;cnt++)
						{
						//	printf("\n%s --> %s",processIDs[cnt],tmpstr);
						}
						printf("\nHere\n");
						for(cnt1=(cnt+1);cnt1<noOfProcess;cnt1++)
							strcpy(processIDs[cnt1-1],processIDs[cnt1]);
						noOfProcess--;
						strcpy(check,"\0");
						fp=popen(str,"r");
						fgets(check,100,fp);
						fclose(fp);
						
						if(strstr(check,"Checkpoint")!=NULL)
						{
							printf("\nThe selected file could not be checkpointed");
						}
						else
						{
							printf("\nForced Checkpoint due to overload successful\n");
							sprintf(context,"chmod 777 context.%s",processID);
							system(context);
							sprintf(context,"mv context.%s /home/cse4119/context_files/redist/context.%s",processID,processID);
							system(context);
							break;
							
						}
					}
					
				}
				
			}

			else if(strstr(buff,"FAIL")!=NULL)
			{
				for(z=strlen(buff);buff[z]!='/';z--);
					z++;
				pos=0;
				for(;z<=strlen(buff);z++)
					restartFile[pos++]=buff[z];


				printf("\nTo be restarted: %s",restartFile);

				if(strstr(buff,"FAIL")!=NULL)
				{
					for(i=5;i<=strlen(buff);i++)
					{
						rstart[i-5]=buff[i];
					}
				}
							
			//	sprintf(copy,"mv %s %s",rstart,restartFile);
			//	system(copy);
			//	sleep(3);
				printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&\n&&&&&&&&&&&&&&&&&&&&\nRESTART\n&&&&&&&&&&&&&&&&&&&&&&&&&&&\n&&&&&&&&&&&&&&&&&&&&\n");
				pthread_create(&restartThread,NULL,restartFunc,buff);
				pthread_create(&restartThreadRem,NULL,restartFuncDel,rstart);
			}
			else if(strstr(buff,"context")!=NULL)
			{
				
				sprintf(copy,"mv /home/cse4119/context_files/redist/%s %s",buff,buff);
				system(copy);
				sprintf(tmpfinal,"/home/cse4119/context_files/redist/%s",buff);
			//	printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&\n&&&&&&&&&&&&&&&&&&&&\nRESTART\n&&&&&&&&&&&&&&&&&&&&&&&&&&&\n&&&&&&&&&&&&&&&&&&&&\n");
				pthread_create(&restartThread,NULL,restartFunc,buff);
				//pthread_create(&restartThreadRem,NULL,restartFuncDel,tmpfinal);
			}
		}
	}
}

main(int argc,char **argv)
{
	pthread_t pth1;
	pthread_t periodicCheckpointThread;
	FILE *up,*addr;
	char temp[20],address[30],string[100],load1[10],load5[10],load15[10];
	strcpy(buff1,"aftersleep");
	sockdes=socket(AF_INET,SOCK_STREAM,0);
	bzero(&sad,sizeof(sad));
	sad.sin_family=AF_INET;
        sad.sin_port=htons(4023);
        sad.sin_addr.s_addr=inet_addr(argv[1]);

	addr = popen("/sbin/ifconfig -a | grep Bcast","r");
	fscanf(addr,"%s",temp);
	fscanf(addr,"%s",temp);
	int cnt=0,addrcnt=0;
	while(temp[cnt]!=':')//printf("\n --> %d",noOfProcess);
		cnt++;
	cnt++;
	while(temp[cnt]!='\0')
		address[addrcnt++]=temp[cnt++];
	address[addrcnt]='\0';
//	printf("\n%s\n",address);
	pclose(addr);

	val=connect(sockdes,(struct sockaddr*)&sad,sizeof(sad));
	strcpy(address,"START ");
	strcat(address,argv[2]);

	write(sockdes,address,sizeof(address));
	
	
	close(sockdes);
}
