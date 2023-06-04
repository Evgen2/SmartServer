/* SmartDevice.cpp */

#include "stdafx.h"
#include "SmartDevice.h"


/* 
rc = 
0x01 - нет связи с контроллером
0x12 - ошибка создания сокета
*/
int SmartDevice::InitUDP(int debug_mode)
{  int  rc;

    rc = lpc.createUDPconnection(ipaddr,base_port+1, 2000, 450,1);
    if(rc)
       return 0x12;
#if 0
/*******************************/
/*  рожаем мютексные семафоры  */
/*******************************/
    if(hmtx_LPC_io == NULL)
    {  hmtx_LPC_io = CreateMutex( 
          NULL,              // default security attributes
          FALSE,             // initially not owned
          SemNameD);             // unnamed mutex

		if (hmtx_LPC_io == NULL) 
	    {   printf("CreateMutex error: %d\n", GetLastError());
	        Beep(1000,1000);
	    }
	}
#else
/* мутексные семафоры для блокировки одновременного доступа из разных тредов не используем, т.к. один клиент - один тред */

#endif //0
   {   rc = lpc.InitClientConnection();
       if(rc == 0)
	   {//	type = TYPE_UDP;
//			strcpy(Name, client.IdName);
//			id = client.IdCode;
	   } else {
		   return 1;
	   }
	   Test(1);
   }

/******************************************/

  return 0;
}

int SmartDevice::Print(void)
{
	printf("%s\n", __FUNCTION__);
	printf("BiosCode %i\n",BiosCode);
    printf("Version %i\n",Version);
    printf("SubVersion %i\n",SubVersion);
    printf("BiosDate %s\n", BiosDate);
    printf("Variant %i\n", Variant);
    printf("online %i\n", online);
    printf("need_init %i\n", need_init);
    printf("connectionType %i\n", connectionType);
    printf("ipaddr %s\n", ipaddr);
    printf("base_port %i\n", base_port);
    printf("isInited %i\n", isInited);
    printf("sts %i\n", sts);
	return 0;
}

int SmartDevice::Test(int mode) /* тест обмена  */
{   int i,l,rc;
    int raz0, raz, is_err,nErr=0, numBytesNE=8, numMsg=0;
    int t0, dt, indPrev=0, lostOld=0, once=1, bytes=0;
    int n_err[8],  Nraz00=0xffff, Nraz01 = 0x1f,  Nraz00_incr=  0x100,  raz_offline=0, raz_reboot = 0;
    int MsgOut_cmdOld=0;
struct Msg1 MsgIn, MsgOut;


    if(mode == 1) { Nraz00=0x2; Nraz01 = 4; }
    for(i=0;i<8;i++) n_err[i] = 0;
    t0 = clock();
    MsgIn.cmd0 = 0x22;   // кроме 0xfe
  for(raz0 = 1; raz0 < Nraz00;  raz0+= Nraz00_incr)   //если надо повысить тщательность - ставь меньший инкремент
  {   dt = clock()-t0;
      if(mode == 0 && dt > 0)
      {   printf("%i %.1f kb/sec\r", bytes, double(bytes)/double(dt) ); fflush(stdout);
      }
     if(nErr > 100)
                break;
	 if(nErr == 0 && dt > 1000)
		 break;
	 else
		if(dt > 5000)
			  break;
     for(raz = 0; raz < Nraz01; raz++)
     {
        MsgIn.cmd0 = lpc.indcmd & 0x3f;
        MsgIn.cmd = raz0|0x8000;//тест
        MsgIn.ind = lpc.indcmd;
        lpc.indcmd = (lpc.indcmd+1)&0xffff;

        is_err = 0;
//        for(i=0; i<60; i++) MsgOut.Buf[i]=i;
//        for(i=0; i<60; i++) MsgIn.Buf[i]=(i+raz)%256;
         /* сбрасываем биты по маске и устанавливаем биты по маске */
        l = sizeof(MsgIn);

		if(connectionType == CONNECT_TCP)		rc = client.SendAndConfirm((char *)&MsgIn, l, (char *)&MsgOut, l, 1);
		else 
			if(connectionType == CONNECT_UDP)	rc =    lpc.SendAndConfirm((char *)&MsgIn, l, (char *)&MsgOut, l);
		else
		{
            printf("Error: connectionType=%i at %s\n",connectionType, __FUNCTION__);
			exit(1);
		}
        if(rc == 1)
        {   online = 0;
            raz_offline++;
            printf("контроллер raz_offline %i\n",raz_offline);
            continue;
        }
        numMsg++;
        bytes += l;
        if(online == 0)
             need_init = 1;
        online = 1;
        if(raz == 0)
        { // lostOld = *((int *)&MsgOut.Buf[28]);
           indPrev =  MsgOut.ind;
          continue;
        }
        sts = *((short int *)&MsgOut.Buf[44]);
//проверка

        if (MsgIn.cmd0 != MsgOut.cmd0)
        {  if(MsgOut.cmd0 & 0x80)
           {  raz_reboot++;
              printf("контроллер raz_reboot %i\n",raz_reboot);
              indPrev =  MsgOut.ind;
              continue;

           } else {
             printf("MsgOut.cmd0 %x  MsgIn.cmd0 %x !!!\n",MsgOut.cmd0,MsgIn.cmd0);
             is_err = 1;
             nErr++;
           }
        }

        if(MsgOut.cmd !=  MsgIn.cmd)
        {     printf("MsgOut.cmd0 %x  MsgIn.cmd0 %x !!!\n",MsgOut.cmd0,MsgIn.cmd0);

              printf("MsgOut.cmd %x  MsgIn.cmd %x (rc=%x, raz=%i, raz0=%i) ",MsgOut.cmd,MsgIn.cmd, rc, raz, raz0);
              printf("+%x %x %x %x\n", MsgIn.ind, MsgOut.ind, indPrev, *((int *)&MsgOut.Buf[28]));

              if(MsgOut_cmdOld+1 == MsgOut.cmd)
              {    printf("Drives LPC Controller is not progammed ?!\n");
              }
		if(connectionType == CONNECT_TCP) rc = client.SendAndConfirm((char *)&MsgIn, l, (char *)&MsgOut, l,1);
		else       rc =    lpc.SendAndConfirm((char *)&MsgIn, l, (char *)&MsgOut, l);

              printf("MsgOut.cmd0 %x  MsgIn.cmd0 %x !!!\n",MsgOut.cmd0,MsgIn.cmd0);
              printf("MsgOut.cmd %x  MsgIn.cmd %x (rc=%x, raz=%i, raz0=%i) ",MsgOut.cmd,MsgIn.cmd, rc, raz, raz0);
              printf("+%x %x %x %x\n", MsgIn.ind, MsgOut.ind, indPrev, *((int *)&MsgOut.Buf[28]));
             is_err = 1;
             nErr++;
        }
        else if(sts) {
             printf("sts %x !!!\n",sts);
             nErr++;
             is_err = 1;
        }
        else
        {  int iserr=0;
           for(i=1; i < 8; i++)
           {  if(MsgOut.Buf[64-8] != MsgOut.Buf[64-8+i])
              {   if(numBytesNE > i) numBytesNE = i;
                  n_err[i]++;
                  if(i < 4 || iserr) //пока не заморачиваемся на 4 последних байта
                  {
                       printf("[%i]=%x ", 60-8+i+4, MsgOut.Buf[60-8+i]);
                       iserr++;
                       nErr++;
                       is_err = 1;
                  }
              }
           }
           if(iserr) printf(" Должно быть:%x\n", MsgOut.Buf[64-8]);

        if (MsgIn.ind != (short int)(MsgOut.ind+1))
        {  if(MsgIn.ind == MsgOut.ind) ; // наша посылка была повторена - nop
           else
           {  printf("+%x %x %x %x\n", MsgIn.ind, MsgOut.ind, indPrev, *((int *)&MsgOut.Buf[24]));
              is_err = 1;
              nErr++;
           }
        }
        if(!once)
        {   if(lostOld  != *((int *)&MsgOut.Buf[26])  )
            {     printf("!!!!!! %x %x\n", *((int *)&MsgOut.Buf[26]), lostOld );
                  nErr++;
                  is_err = 1;
            }
        }  else once = 0;
/*
           printf("%i \r", raz); fflush(stdout);
           for(i=0; i < (10-1); i++)
           { t0 = *((int *)&MsgOut.Buf[4+i*4]);
             t1 = *((int *)&MsgOut.Buf[4+i*4+4]);
             if(t1 == 0) break;
             printf("%i\n", t1-t0);
           }
*/

        }
        MsgOut_cmdOld = MsgOut.cmd;
        if(!is_err)
        {   BiosCode = *((short int *)&MsgOut.Buf[4]);
            Version  = *((short int *)&MsgOut.Buf[6]);
            SubVersion = *((short int *)&MsgOut.Buf[8]);
            memcpy(BiosDate,((void *)&MsgOut.Buf[30]),12);
//            buf_size_Max = *((short int *)&MsgOut.Buf[10]);
//            lTemLogElSize = *((short int *)&MsgOut.Buf[12]);
            num_lost = *((int *)&MsgOut.Buf[26]);
            Variant = *((int *)&MsgOut.Buf[50]);
        }

        lostOld = *((int *)&MsgOut.Buf[26]);
        indPrev =  MsgOut.ind;

     }
  }
 if(mode == 0)
 {
  if(nErr == 0 && numBytesNE != 8)
  {  printf("Напоминалка: Согласно теста последние %i байт в буфере ненадежные!!!\n", 8-numBytesNE);
     for(i=0; i<8; i++)
     {  if(n_err[i])
          printf("[%i]=%i %2.f%%\n",i, n_err[i], n_err[i]/double(numMsg)*100.);
     }
     printf("Всего пакетов: %i\n",numMsg);
  }
 }
  return nErr;

}

/* lIn - длина исходящего сообщения, lOut - длина ответа */

int SmartDevice::GetSts0(int cmd, struct Msg1  *MsgIn, int lIn, struct Msg1  *MsgOut, int lOut)
{  int  l, rc;

    lpc.indcmd = (lpc.indcmd+1)&0xffff;
    num_send++;

    MsgIn->cmd0 = 0x22;
    MsgIn->cmd = cmd;
    MsgIn->ind = lpc.indcmd;
    l = sizeof(*MsgIn);

	if(connectionType == CONNECT_TCP) rc = client.SendAndConfirm((char *)MsgIn, lIn, (char *)MsgOut, lOut,1);
	else       rc = lpc.SendAndConfirm((char *)MsgIn, lIn, (char *)MsgOut, lOut);

	if(rc == 1 || rc == 2)
    {   online = 0;
        return rc;
     } else
          online = 1;
    if(MsgOut->cmd0 != MsgIn->cmd0)
    {
        if(MsgOut->cmd0 & 0x8000 && (MsgIn->cmd0 == (MsgOut->cmd0&0xff)) )
                        return 0x12;
         return 0x10;
    }
    if(MsgOut->cmd != MsgIn->cmd)
    {
		if(connectionType == CONNECT_TCP) rc = client.SendAndConfirm((char *)MsgIn, lIn, (char *)MsgOut, lOut, 1);
		else       rc = lpc.SendAndConfirm((char *)MsgIn, lIn, (char *)MsgOut, lOut);
        if(rc == 1)
        {   online = 0;
          return rc;
        } else
          online = 1;
    }

    if(MsgOut->cmd != MsgIn->cmd)
          return 0x11;
    return 0;
}

int SmartTerm::WriteSts(FILE *fp)
{  int i;
	fprintf(fp,"MAC=");
	for(i=0; i<6; i++)
		fprintf(fp,"%02x ", mac[i]);
	fprintf(fp,"\n");
	fprintf(fp,"ns=%d\n", ns);
	return 0;
}

int	SmartTerm::ReadSts(FILE *fp)
{	char str[256], *pstr, *pstr1;
    unsigned int mac0[6]; 
	int i, rc;
    pstr = fgets(str,256, fp);
	if(pstr == NULL)
		return -1;
	pstr1 = strstr(str,"=");
	if(pstr1 == NULL)
		return -2;
	pstr = pstr1 +1;
	rc = sscanf(pstr,"%02x %02x %02x %02x %02x %02x", &mac0[0],&mac0[1],&mac0[2],&mac0[3],&mac0[4],&mac0[5]);
	for(i=0; i<6; i++) mac[i] = mac0[i];
    pstr = fgets(str,256, fp);
	if(pstr == NULL)
		return -1;
	pstr1 = strstr(str,"=");
	if(pstr1 == NULL)
		return -2;
	pstr = pstr1 +1;

	rc = sscanf(pstr,"%d", &ns);

	return 0;
}

