/* SmartHosts.hpp */
#ifndef ROZETKA_H
#define ROZETKA_H

#define TYPE_NONE 0
#define TYPE_UDP  1
#define TYPE_TCP  2

#define MAX_HOSTS 512

class host_property
{
 public:
    char IPaddr[20];
    char Name[256];
    int type; 
	int id;
	int inum;
    host_property(void)
    {  strcpy(IPaddr,"127.0.0.1");
       strcpy(Name, "myself");
       type = TYPE_NONE;
	   id = 0;
	   inum = 0;
    } 
};


class my_hosts
{
 public:
   int n;
//   HANDLE hmtx; /* мутексный семафор для блокировки одновременного доступа из разных тредов */
   int hostport; // 6769
   host_property host[MAX_HOSTS];
   my_hosts(void)
   {  n = 0;
#if 0
      hmtx = CreateMutex( 
          NULL,              // default security attributes
          FALSE,             // initially not owned
          NULL);           // unnamed mutex

		if (hmtx == NULL) 
	    {   printf("CreateMutex error: %d\n", GetLastError());
	    }
#endif //0
		hostport = 6769; //0x1a70+1
   }
   ~my_hosts(void)
   {
#if 0
	   if (hmtx) 
			CloseHandle(hmtx);
#endif //0
   }

   int AddHost(char *_Ip, char *_Name, int id, int inum, int type)
   {   int  i,l, is=0, in;
	   if(n >= MAX_HOSTS)
			return -1;
#if 0
	   rc = WaitForSingleObject( 
               hmtx,    // handle to mutex
               INFINITE);  // no time-out interval
#endif //0
		in = n;
		if(n > 0)
		{	for(i=0; i<n; i++)
			{	if(!strcmp(_Ip, host[i].IPaddr))
				{	is = 1;
					in = i;
			       break;
				}
			}
		}

       strcpy(host[in].IPaddr,_Ip);
	   if(_Name)
       {   if(strchr(_Name, ' ')) /* заменяем пробелы на  _ */
		   {   l = strlen(_Name);
		       for(i=0; i<l; i++)
			   {   if(_Name[i] == ' ') _Name[i] = '_';
			   }
		   }
			strcpy(host[in].Name,_Name);
	   }  else {
		   host[in].Name[0] = 0;
	   }
	   host[in].id = id;
	   host[in].inum = inum;
	   host[in].type = type;
	   if(!is)
			n++;
#if 0
      ReleaseMutex(hmtx);
#endif
	   return 0;
   }
   int Write(char *name);
   int Read(char *name);
   int RenameToBak(char *name);
};


#endif //ROZETKA_H


