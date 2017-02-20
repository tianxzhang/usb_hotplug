#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>

#define IPSTR "12.12.12.12"
#define PORT 80
#define BUFSIZE 4096

char *device_file="/sys/devices/platform/ehci-platform/usb1";
//char *device_file="/sys/devices/pci0000:00/0000:00:14.0/usb1";
char jsonstr[BUFSIZE];
int jsoncount=0;
char* IpAddress="10.185.32.199";
int networkok=1;
int port=80;

int count(char * str,const char * delim){
    int num = 0;
    while( NULL != ( str = strstr(str,delim) ) ){
        str ++;
        num ++;
    }
    return num;
}

char ** split(char * str, const char * del){
    int num = count(str,del);
    char ** arr = ( char ** ) malloc( sizeof(char*) * ( num +1));
    char ** result = arr;
   
    char * s = strtok(str,del);
   
    while( s != NULL){
        *arr++ = s;
        s = strtok(NULL,del);

    }
    return result;
}

int geturl(char* url)
{
  printf("\nsend data----------------------\n");
  int cfd;
  struct sockaddr_in cadd;
  struct hostent *pURL = NULL;
  char myurl[BUFSIZE];
  char *pHost = 0;
  char host[BUFSIZE],GET[BUFSIZE];
  char request[BUFSIZE];
  static char text[BUFSIZE];
  int i,j;


  memset(myurl,0,BUFSIZE);
  memset(host,0,BUFSIZE);
  memset(GET,0,BUFSIZE);
  strcpy(myurl,url);
  for(pHost = myurl;*pHost != '/' && *pHost != '\0';++pHost);

  if((int)(pHost-myurl) == strlen(myurl))
  {
          strcpy(GET,"/");
  }
  else
  {
          strcpy(GET,pHost);
  }
  *pHost = '\0';
  strcpy(host,myurl);
  if(-1 == (cfd = socket(AF_INET,SOCK_STREAM,0)))
  {
          printf("create socket failed of client!\n");
          exit(-1);
  }

  pURL = gethostbyname(host);

  bzero(&cadd,sizeof(struct sockaddr_in));
  cadd.sin_family = AF_INET;
  cadd.sin_addr.s_addr = *((unsigned long*)pURL->h_addr_list[0]);
  cadd.sin_port = htons(port);
  memset(request,0,BUFSIZE);
  strcat(request,"GET ");
  strcat(request,GET);
  strcat(request," HTTP/1.1\r\n");
  strcat(request,"HOST: ");
  strcat(request,host);
  strcat(request,"\r\n");
  strcat(request,"Cache-Control: no-cache\r\n\r\n");
  int cc;
  if(-1 == (cc = connect(cfd,(struct sockaddr*)&cadd,(socklen_t)sizeof(cadd))))
  {
          printf("connect failed of client!\n");
          exit(1);
  }
  //printf("connect success!\n");

  int cs;
  printf("\n=============request:%s-----------\n",GET);
  if(-1 == (cs = send(cfd,request,strlen(request),0)))
  {
          printf("向服务器发送请求的request失败!\n");
          exit(1);
  }
  printf("发送成功,发送的字节数:%d\n",cs);

  //客户端接收服务器的返回信息
  ///memset(text,0,BUFSIZE);
  /*int cr;
  if(-1 == (cr = recv(cfd,text,BUFSIZE,0)))
  {
          printf("recieve failed!\n");
          exit(1);
  }
  else
  {
          //printf("receive succecc!\n");
  }*/
  close(cfd);
  return 0;
} 

int referinfo(char* rootDirPath,char* deviceNumber)
{
  FILE* devnumFileInfo;
  char buffers[200]={0};
  DIR *rootDir;
  struct dirent *rootDirInfo;
  char dirname[256];
  char subDirPath[200];
  char fileContent[255];
  char deviceNOInDevnum[3];
  char devnumPath[255];
  

  //printf("%s--rootDirPath\n",rootDirPath);
  strcat(buffers,deviceNumber);
  //printf("%s--buffers\n",buffers);
  bzero(subDirPath,sizeof(subDirPath));
  if((rootDir=opendir(rootDirPath)) == NULL)
    printf("can't open %s\n",rootDirPath);
  while((rootDirInfo=readdir(rootDir)) != NULL)
  {
    if(rootDirInfo->d_type == 4 && strcmp(rootDirInfo->d_name,".") && strcmp(rootDirInfo->d_name,".."))//It is a directory
    {
      sprintf(subDirPath,"%s/%s",rootDirPath,rootDirInfo->d_name);
      //printf("%s--Sub directory path\n",subDirPath);
      //return 0;
      //referinfo(filegetcwd,buffers);

      bzero(devnumPath,sizeof(devnumPath));

      //printf("%s\n",path);
      strcat(devnumPath,subDirPath);
      strcat(devnumPath,"/devnum");
      if(!access(devnumPath,0))
      {
        devnumFileInfo=fopen(devnumPath,"r");
        fscanf(devnumFileInfo,"%s",fileContent);
        fclose(devnumFileInfo);
        //printf("%ld\n",strlen(buff));
        //printf("%s\n",buff);
        bzero(deviceNOInDevnum,sizeof(deviceNOInDevnum[3]));
        if(strlen(fileContent) == 1)
        {
          strcat(deviceNOInDevnum,"00");
          strcat(deviceNOInDevnum,fileContent);
        }
        else if(strlen(fileContent) == 2)
        {
          strcat(deviceNOInDevnum,"0");
          strcat(deviceNOInDevnum,fileContent);
        }
        else
	{
	  strcat(deviceNOInDevnum,fileContent);
	}
        /*printf("%s\n",ch);*/
        if(strstr(deviceNumber,deviceNOInDevnum))
        {
	  //printf("\n");
	  //printf("Find the Motorola Radio Directory:\n%s",subDirPath);
 	  //printf("\n");
          getradioinfo(subDirPath);
        }
        else
        {
          referinfo(subDirPath,deviceNumber);
	}
      }
      else
      {
          referinfo(subDirPath,deviceNumber);
      }
    }
  }
  return 0;
}

int scan()
{
  printf("scan start!\n");
  //DIR *dp;
  //struct dirent *dirp;
  //char dirname[256];
  FILE* fp;
  char buffer[200];
  //char path[255];
  //char buff[255];
  //char ch[3];  
  //char devnum[255];
  char scaninfostr[1000]={0};
  strcat(scaninfostr,IpAddress);
  //char scaninfostr[1000]="/RMBoxService/RadioAttachedNotify?deviceJasonString=[";
  strcat(scaninfostr,"/RMBoxService/RadioAttachedNotify?deviceJasonString=[");
  
  jsoncount=0;
  memset(buffer,0,200);
  fp=popen("lsusb | grep \"Motorola CGISS\" | awk '{printf $4}'","r");
  fgets(buffer,sizeof(buffer),fp);
  //puts(buffer);
  pclose(fp);
  if(access(device_file,0))
  {
    puts("folder not found!");
    return 0;
  }
  referinfo(device_file,buffer);  
  /*if((dp=opendir(device_file)) == NULL)
    printf("can't open %s\n",device_file);
  while((dirp=readdir(dp)) != NULL)
  {
    if(dirp->d_type == 4 && dirp->d_name != "..")
    {
      bzero(path,sizeof(path));
      bzero(devnum,sizeof(devnum));
      strcat(path,device_file);
      strcat(path,dirp->d_name);
      //printf("%s\n",path);
      strcat(devnum,path);
      strcat(devnum,"/devnum");
      if(!access(devnum,0))
      {
        fp=fopen(devnum,"r");
        fscanf(fp,"%s",buff);
 	fclose(fp);
 	//printf("%ld\n",strlen(buff));
        //printf("%s\n",buff);
	bzero(ch,sizeof(ch[3]));
 	if(strlen(buff) == 1)
	{
	  strcat(ch,"00");
 	  strcat(ch,buff);
 	}
 	if(strlen(buff) == 2)
	{
	  strcat(ch,"0");
	  strcat(ch,buff);
	}
  	printf("%s\n",ch);
        if(strstr(buffer,ch))
	{
	  getradioinfo(path);
	} 
  	
      }
            
      //printf("%s\n",path);
      
    }
  }*/
  jsonstr[strlen(jsonstr)-1]=0;
  printf("%s------jsonstr\n",jsonstr);
  printf("total %i items\n",jsoncount);
  strcat(scaninfostr,jsonstr);
  strcat(scaninfostr,"]");
  if(networkok)
    geturl(scaninfostr);
  bzero(jsonstr,sizeof(jsonstr));
  return 0;
}

char* getusbnuminfo(char* netDirPath)
{
  DIR *rootDir;
  struct dirent *rootDirInfo;
 
  if((rootDir=opendir(netDirPath)) == NULL)
    printf("can't open %s\n",netDirPath);
  
  while((rootDirInfo=readdir(rootDir)) != NULL)
  {
    //printf("File Name: %s\n",rootDirInfo->d_name);
    if(rootDirInfo->d_type==4 && (rootDirInfo->d_name)[0]=='u' && (rootDirInfo->d_name)[1]=='s'&&(rootDirInfo->d_name)[2]=='b')
    {
      //printf("Find the usb path: %s",rootDirInfo->d_name);
      return rootDirInfo->d_name;
    }
    
  }
  return NULL;
}

int getradioinfo(char* path)
{
  //printf("radioinfo\n");
  FILE *fp;
  char buffer[2048];
  int n = strlen(path);
  char infopath[n+1];
  char infopathcp[n+1];
  int i=0;
  char* usbnum;
  
  for(i;i<n;i++)
  {
    infopath[i]=*(path++);
  }
  
  char *del="/";
  char **res;
  int num;
  char infopath_net[100];
  char infopath_net1[100];
  bzero(infopath_net,sizeof(infopath_net));
  bzero(infopath_net1,sizeof(infopath_net1));
  bzero(infopathcp,sizeof(infopathcp));
  char arg[1024]="./info.sh ";
  strcpy(infopath_net,infopath);
  strcpy(infopathcp,infopath);
  num=count(infopath,del);
  res=split(infopath,del);
  //printf("%d---num\n",num);
  strcat(infopath_net,"/");
  strcat(infopath_net,*(res+(num-1)));
  //strcpy(infopath_net1,infopath_net);
  //strcat(infopath_net,":1.0/net/usb0");
  //strcat(infopath_net1,":1.0/net/usb1");
  strcat(infopath_net,":1.0/net/");
  if(!access(infopath_net,0))
  { 
    if((usbnum=getusbnuminfo(infopath_net)) != NULL)
    {
      strcat(infopath_net,usbnum);
      //printf("Valid usb path: %s\n",infopath);
    }
    //bzero(infopath_net,sizeof(infopath_net));
    //strcpy(infopath_net,infopath_net1);
  }
  //printf("%s----infopath_net\n",infopath_net);
  strcat(arg,infopathcp);
  strcat(arg," ");
  strcat(arg,infopath_net);
  printf("%s---arg\n",arg);
  bzero(buffer,sizeof(buffer));
  fp=popen(arg,"r");
  fgets(buffer,sizeof(buffer),fp);
  //printf("%s\n",buffer);
  //printf("%ld-----strlen(buffer)\n",strlen(buffer));
  buffer[strlen(buffer)-1]=0;
  //printf("%s--buffer\n",buffer);
  strcat(buffer,*(res+num-1));
  strcat(buffer,"\"},");
  jsoncount += 1;
  strcat(jsonstr,buffer);
  bzero(buffer,sizeof(buffer));
  pclose(fp);
  //printf("radioninfoend\n");
  return 0;
}

void MonitorRadioConnection()
{
  printf("Monitoring...");
  int sockfd;
  struct sockaddr_nl sa;
  int len;
  char connectionEventContent[4096];
  struct iovec iov;
  struct msghdr msg;
  int i;
  char *str;
  char pid[5];
  char vid[5];
  char Devnum[5];
  char Devpath[100];
  int num;
  char **res;
  char jsonstrs[200];
  char addstr[110]="/sys";
  char addstr_net[1024]="/sys";
  int charnum;
  FILE *fp;
  char arg[1024]="./info.sh ";
  char buffer[1024];
  char addinfostr[200]={0};
  char removeinfostr[200]={0};
  strcat(addinfostr,IpAddress);
  strcat(removeinfostr,IpAddress);
  //char addinfostr[200]="10.185.32.199/RMBoxService/RadioAttachedNotify?deviceJasonString=[";
  //char removeinfostr[200]="10.185.32.199/RMBoxService/RadioDettachedNotify?deviceJasonString=[";
  strcat(addinfostr,"/RMBoxService/RadioAttachedNotify?deviceJasonString=[");
  strcat(removeinfostr,"/RMBoxService/RadioDettachedNotify?deviceJasonString=[");
  char* usbnum;
 
  /*Create socket to monitor device usb attach and dettach event */
  memset(&sa,0,sizeof(sa));
  bzero(Devnum,sizeof(Devnum));
  bzero(Devpath,sizeof(Devpath));
  sa.nl_family=AF_NETLINK;
  sa.nl_groups=NETLINK_KOBJECT_UEVENT;
  sa.nl_pid = 0;
  memset(&msg,0,sizeof(msg));
  iov.iov_base=(void *)connectionEventContent;
  iov.iov_len=sizeof(connectionEventContent);
  msg.msg_name=(void *)&sa;
  msg.msg_namelen=sizeof(sa);
  msg.msg_iov=&iov;
  msg.msg_iovlen=1;
  
  sockfd=socket(AF_NETLINK,SOCK_RAW,NETLINK_KOBJECT_UEVENT);
  if(sockfd==-1)
      printf("socket creating failed:%s\n",strerror(errno));
  if(bind(sockfd,(struct sockaddr *)&sa,sizeof(sa))==-1)
      printf("bind error:%s\n",strerror(errno));
 
  while(len=recvmsg(sockfd,&msg,0))
  {
  /*if(len<0)
      printf("receive error\n");
  else if(len<32||len>sizeof(connectionEventContent))
      printf("invalid message");
  else if(len>274&&len<300)//Receive valid message
      break;*/
  if(connectionEventContent[strlen(connectionEventContent)-1]== '0' && connectionEventContent[strlen(connectionEventContent)-2]== '.' && connectionEventContent[strlen(connectionEventContent)-3]== '1' &&connectionEventContent[strlen(connectionEventContent)-4]== ':')
  {
    for(i=0;i<len;i++)
      if(*(connectionEventContent+i)=='\0')
          connectionEventContent[i]='\n';
    if(str=strstr(connectionEventContent,"PRODUCT"))
      break;    
  } 
  }

  //printf("connetionInfo : %s",connectionEventContent);
  
  //Analyze the content to get PID,VID

  for(i=0;i<strlen(str);i++)
      if(*(str+i)=='\n')
       *(str+i)='\0';
  for(i=12;i<16;i++)
  {
    pid[i-12]=*(str+i);
  }
  for(i=8;i<11;i++)
  {
    vid[i-7]=*(str+i);
  }
  vid[0]='0';
  for(i=0;i<len;i++)
      if(*(connectionEventContent+i)=='\n')
          connectionEventContent[i]='\0';
  strcpy(Devpath,connectionEventContent);
  num=count(Devpath,"/");
  //printf("%s-----Devpath\n",Devpath);
  res=split(Devpath,"/");
  if(strstr(connectionEventContent,"add@"))
  {
    printf("\n\nadd----add----add------add-------add---add-----add\n\n");
    charnum=0;
    for(i=strlen(connectionEventContent)-1;connectionEventContent[i] != '/';i--)
    {
      charnum++;
    }
    charnum=strlen(connectionEventContent)-charnum;
    for(i=4;i<charnum;i++)
    {
      addstr[i]=connectionEventContent[i];      
    }

    //printf("First Arg: %s\n",addstr);

    
    for(i=4;i<strlen(connectionEventContent);i++)
    {
      addstr_net[i]=connectionEventContent[i];      
    }   
    
    //printf("orginal value:%s\n", addstr_net);
    char usbAddress[512];
    strcpy(usbAddress, addstr_net);
    strcat(usbAddress,"/net/");  

    sleep(3);
    if((usbnum=getusbnuminfo(usbAddress)) != NULL)
    {
      strcat(usbAddress,usbnum);
      //printf("Valid usb path: %s\n",usbAddress);
      strcpy(addstr_net,usbAddress);
    }
    else
    {  
	printf("Can not find any usb path");
        return;       
    }

    //printf("addstr_net: %s\n",addstr_net);

    strcat(arg,addstr);
    strcat(arg," ");
    strcat(arg,addstr_net);  
    //printf("%s----arg\n",arg); 
    fp=popen(arg,"r");
    fgets(buffer,sizeof(buffer),fp);
    buffer[strlen(buffer)-1]=0;
    strcat(buffer,*(res+num-1));
    strcat(buffer,"\"}");
    strcpy(jsonstrs,buffer);
    printf("%s-----jsonstrs\n",jsonstrs);
    pclose(fp);
    strcat(addinfostr,jsonstrs);
    strcat(addinfostr,"]");
    if(networkok)
      geturl(addinfostr);
    memset(jsonstrs,0,sizeof(jsonstrs));
    memset(connectionEventContent,0,sizeof(connectionEventContent));
    memset(addstr,0,sizeof(addstr));
    bzero(addstr_net,sizeof(addstr_net));
    bzero(connectionEventContent,sizeof(connectionEventContent));
  }
  else if(strstr(connectionEventContent,"remove@"))
  {
    /*for(i=0;i<len;i++)
       if(*(buf+i)=='\n')
           buf[i]='\0';*/ 
    printf("\n\nremove----remove----remove----remove-----remove---\n\n");
  
    strcpy(Devpath,connectionEventContent);
    num=count(Devpath,"/");
    //printf("%s-----Devpath\n",Devpath);
    res=split(Devpath,"/");
    strcat(Devnum,*(res+num-1));
    //printf("Vid:%s---Pid:%s----RadioId:%s_%s\n",vid,pid,pid,Devnum);
    strcat(jsonstrs,"{\"IPAddress\":\"\",\"VID\":\"");
    strcat(jsonstrs,vid);
    strcat(jsonstrs,"\",\"PID\":\"");
    strcat(jsonstrs,pid);
    strcat(jsonstrs,"\",\"RadioID\":\"");
    strcat(jsonstrs,pid);
    strcat(jsonstrs,"_");
    strcat(jsonstrs,Devnum);
    strcat(jsonstrs,"\"}");
    printf("%s----jsonstrs\n",jsonstrs);
    printf("----------------RadioId:%s_%s\n",pid,Devnum);
    strcat(removeinfostr,jsonstrs);
    strcat(removeinfostr,"]");
    if(networkok)
{
      printf("\n\n-----------removeinfostr--%s---------------\n\n",removeinfostr);
      geturl(removeinfostr);
      
   }
    memset(jsonstrs,0,sizeof(jsonstrs));  
  }
  /*printf("received %d bytes\n%s\n",len,buf);*/
}

int timer(int seconds)
{
  while(1)
  {
    scan();
    sleep(seconds);
  }
}

void* Monitoring()
{
  while(1)
  {
    MonitorRadioConnection();
  }
  return NULL;
}

int CheckFileExists(char* filePath)
{
  int retryCount = 5;
  while(retryCount >=0)
  {
   if(access(filePath,F_OK)==0)
    {
      printf("File exists\n");
      return 0;
    }
  else
    {
      retryCount -= 1;
      printf("%d---retrycout\n",retryCount);
      usleep(500*1000);
    }
  }
  return -1;
}

/*enum RunningMode
{
  Scan = 0x1,
  Monotor = 0x2,
  Both = 0x3,
};*/
 
int main(int argc,char *argv[])
{
  pthread_t t0;
  char ch;
  int platform=0;
  int timeinterval=600;
  int mode=3;
  networkok=1;
  
  while((ch=getopt(argc,argv,"p:t:a:e:n:o:")) != EOF)
    switch(ch) {
      case 'p':
        platform=atoi(optarg);
        break;
      case 't':
        timeinterval=atoi(optarg);
        break;
      case 'a':
        IpAddress=optarg;
        break;
      case 'e':
        networkok=atoi(optarg);
        break;
      case 'n':
        mode=atoi(optarg);
        break;
      case 'o':
        port=atoi(optarg);
        break;
      default:
        fprintf(stderr,"Unknown option: '%s'\n",optarg);
        return 1;
     }
  argc -= optind;
  argv += optind;
       
  if(platform==1)//PC
    device_file="/sys/devices/pci0000:00/0000:00:14.0/usb1";
  else if(platform!=0)
    printf("platform unsupport!!!\n");

  printf("Platform:%i\n",platform);
  printf("Time interval:%i\n",timeinterval);
  printf("IP: %s\n",IpAddress);
  printf("Port:%i\n",port);
  printf("Network:%i\n",networkok);
  printf("WorkMode:%i\n\n",mode);
  if(mode == 1)//Scan
  {    
    timer(timeinterval);
  }

  else if(mode == 2)//Monitor
  {
    if(pthread_create(&t0,NULL,Monitoring,NULL) == -1)
       printf("can't create thread t0\n");

  } 
  else
  {
    if(pthread_create(&t0,NULL,Monitoring,NULL) == -1)
       printf("can't create thread t0\n");
    timer(timeinterval);
  }
  
  return 0;
}
