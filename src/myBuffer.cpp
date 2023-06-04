/* mybuffer.cpp */
#include "stdafx.h"
#include <stdlib.h>
#include <string.h>

#include "mybuffer.hpp"

void QueryCom(void);

/* отдаем под myBuffer lb байт по адресу pb */
void myBuffer::Init(char *pb, int Lb)
{   pbuf = pb;
    Lbuf = Lb;
    ibuf = ifree = 0;	 
}

/* отдаем под myBuffer lb байт по адресу pb, запоминаем длину элемента  _Litem */
void myBuffer2::Init(void *pb, int Lb, int _Litem)
{   pbuf = (char *)pb;
    Litem	= _Litem;
    Lbuf = Lb/Litem*Litem; // Lbuf выравнивается на длину элемента
    ibuf = ifree = 0;
}

/* добавить в myBuffer Lb байт с адреса pb */
int myBuffer::Add(char *pb, int Lb)
{ 
	if(GetFree() <= Lb)
		  return 0;

	if((ifree + Lb) < Lbuf)
	{	memcpy((void *)&pbuf[ifree],(void *)pb ,Lb);
		ifree += Lb;
	} else {
		int k0, k1;
		k0 = ifree + Lb -	Lbuf;
		k1 = Lb - k0;
		memcpy((void *)&pbuf[ifree],(void *)pb, k1 );
		if(k0)
		   memcpy((void *)&pbuf[0], (void *)&pb[k1], k0);
		ifree = k0;
		
	}
	return 1;
}


/* добавить в myBuffer один байт */
int myBuffer::Add(char byte)
{ 
	if(GetFree() <= 1)
		  return 0;
  pbuf[ifree] = byte;
	if((ifree + 1) < Lbuf)
	{	ifree++;
	} else {
		ifree = 0;
	}
	return 1;
}

int myBufferCmd::Add(unsigned char byte)
{   int rc;
    rc = myBuffer::Add(byte);
	if(rc == 1)
	{   if(byte == 0x0a) 
				nCmd++;
	}
/*
	if(fp)
	{    fprintf(fp,"%i %i %i %i %i %x\n", raz, ibuf,ibuf2,ifree, nCmd, (unsigned char)byte);
		 fflush(fp);
		 raz++;
	}
*/
	return rc;
}
int myBufferCmd::Add(unsigned char *ptr, int nb)
{  int i, rc;
   for(i=0;i<nb;i++)
   {  rc = Add(ptr[i]);
      if(rc == 0)
		  break;
   }
   return rc;
}

int myBufferCmd::GetCmd(unsigned char *ptr, int &nb)
{  int i, l, lc, rc;
   unsigned char ch;
   if(nCmd <= 0)
	   return 0;
   l = GetLbuf();
   StartRead();
   lc = 0;
   for(i=0; i<l; i++)
   {  rc = Read((char *)&ch);
      if(rc == 0)
	  {  ptr[i] = ch;		  
		if(ch == 0x0a)
		{	lc = i+1; 
			break;
		}
	  }
   }
   EndRead();
   if(lc == 0)
   {  printf("Err in %s\n", __FUNCTION__);
      return 0;
   }
   ptr[lc] = 0;
   nb = lc;
   nCmd--;
   /*
   {
static int start = 1;
     if(start)
	 {   start = 0;
	     fp = fopen("mbcmd.log", "w");
	 }
     l = GetLbuf();
     fprintf(fp,"%i %i %i %i %i\n", raz, ibuf,ibuf2,ifree, nCmd);
		 raz++;
	 fprintf(fp,"l=%i, nb=%i\n",  l,nb);

	 for(i=0;i<nb;i++)
		 fprintf(fp,"%02x ",((unsigned char)ptr[i]));
	 fprintf(fp,"\n");
   l = GetLbuf();
   StartRead();
   for(i=0; i<l; i++)
   {  rc = Read((char *)&ch);
	  fprintf(fp,"%02x ",ch);
   }
//   EndRead();
	 fprintf(fp,"\n");
	 fflush(fp);
   }
   */
   return 1;
}

/* добавить в myBuffer2 один элемент */
int myBuffer2::Add(void *ptr)
{  int is_wrap = 0, l;
	 l = GetFree();
	if(l < Litem)
		  return 0;
	if(l == Litem) 
		 is_wrap = 1;
	
	memcpy((void *)&pbuf[ifree], ptr, Litem);
	if((ifree + Litem) < Lbuf)
	{	ifree +=Litem;
	} else {
		ifree = 0;
	}
	if(is_wrap)
	{ if((ibuf + Litem) < Lbuf)
		{	ibuf +=Litem;
		} else {
			ibuf = 0;
		}
	}
	return 1;
}

/* Вернуть элемент буфера и освободить место */
int myBuffer::Get(void)
{   int rc; 
	if(GetLbuf() <= 0)
		  return -1;
	 rc = pbuf[ibuf];
   ibuf++;	
    if(ibuf == Lbuf)
			     ibuf = 0;
	return rc;
}

/* Вернуть элемент буфера и освободить место */
int myBuffer2::Get(void *ptr)
{   
	if(GetLbuf() <= 0)
		  return -1;
   memcpy(ptr,(void *)&pbuf[ibuf], Litem);
   ibuf += Litem;	
    if(ibuf == Lbuf)
			     ibuf = 0;
	return 0;
}

/**************************************************/
/* Вернуть элемент буфера и и не освободить место */
/* начать чтение */
void myBuffer::StartRead(void)
{    ibuf2 = ibuf;
}
/* закончить чтение и освободить место в буфере */
void myBuffer::EndRead(void)
{
	 ibuf = ibuf2;
}

int myBuffer::GetUnread(void)
{   int l, l2;
/*******************/
//QueryCom(); //PC
     l = ifree - ibuf;
	 if(l < 0) l += Lbuf;	// l = длина буфера
	 if(l <= 0)
		 return 0; // буфер пустой
//  ibuf...................ifree
//  ....|...ibuf2.........|.....
     l2 = ibuf2 - ibuf;
	 if(l2 < 0) l2 += Lbuf;	// l2 = длина прочитанной части буфера
	 if(l2 >= l)
		 return 0; // буфер прочитан
	 return l - l2;
}

/* Собственно Вернуть элемент буфера и не освободить место */
int myBuffer::Read(char *el)
{   int  l, l2; 
/*******************/
     l = ifree - ibuf;
	 if(l < 0) l += Lbuf;	// l = длина буфера
	 if(l <= 0)
		 return -1; // буфер пустой
//  ibuf...................ifree
//  ....|...ibuf2.........|.....
     l2 = ibuf2 - ibuf;
	 if(l2 < 0) l2 += Lbuf;	// l2 = длина прочитанной части буфера
	 if(l2 >= l)
		 return -2; // буфер прочитан

/******************/
	*el = pbuf[ibuf2];
    ibuf2++;	
    if(ibuf2 == Lbuf)
			     ibuf2 = 0;
	return 0;
}

/* Сколько свободно в буфере */
int myBuffer::GetFree(void)
{ 
	return Lbuf - GetLbuf();
}

/* Длина занятого буфера в char */
int myBuffer::GetLbuf(void)
{  int l;
// QueryCom(); //PC
     l = ifree - ibuf;
	 if(l < 0) l += Lbuf;	
	return l;
}
/* Длина занятого буфера в элементах */
int myBuffer2::GetLbuf(void)
{  int l;
     l = ifree - ibuf;
	 if(l < 0) l += Lbuf;	
	return l/Litem;
}

#if 0 //poka	

/* Сколько свободно в элементах в буфере */
int myBuffer::GetFree2(void)
{  int l;
     l = ifree - ibuf;
	 if(l < 0) l += Lbuf;
  l = Lbuf - l;
	return l/Litem;
}

#endif //0

/****************************************************/

/*  класс для буфера команд в формате 0xfe 0x00 2байта команда 2 байта длина данных+6  ...данные */
/*
class myBufferCmd2:public myBuffer
{
public:
 volatile int nCmd; // Число команд в буфере. команда - последовательность байтов до 0x0a 
   int raz;
   myBufferCmd(void)
   {  nCmd = 0;
	  raz = 0;
   };
   int Add(unsigned char byte);
   int Add(unsigned char *ptr, int nb);
   int AddCmd(unsigned char *prt);
   int GetCmd(unsigned char *prt, int &nb);
};
*/

int myBufferCmd2::Add(unsigned char *ptr, int nb)
{  int i, rc;
   if(GetFree() <= nb)
		  return -1;

   for(i=0;i<nb;i++)
   {  rc = Add(ptr[i]);
      if(rc == 0)
	  {  return i +1;
	  }
   }
   return nb;
}

int myBufferCmd2::Add(unsigned char byte)
{   int rc;
unsigned char bf[4];
unsigned short int lc;
	if(il == 0)
	{   if(byte == 0xfe)  //признак команды байт 0
		{	rc = myBuffer::Add(byte);
	        il++;
		}
	} else if(il == 1) {  //признак команды байт 1
		if(byte == 0x00)
		{	rc = myBuffer::Add(byte);
	        il++;
		} else { // сбой
			Reset();
			return -1;
		}
	} else if(il < 5) {  //номер команды - байт 2, 3;  длина команды байт 4, 5
		 rc = myBuffer::Add(byte);
	     il++;
	} else if(il == 5) { //длина команды байт 4, 5
		 bf[0] = pbuf[4];
         bf[1] = byte;
		 lc = *((unsigned short int *)&bf[0]);
		 lcmd = lc;
		 if(lcmd > Lbuf)
		 {  Reset();
			return -2;
		 }
		 rc = myBuffer::Add(byte);
	     il++;
		 nCmd++;
	} else if(il < lcmd) {
		rc = myBuffer::Add(byte);
	    il++;
		if(il == lcmd)
			return 0;
	}

	return 1;
}

int myBufferCmd2::GetCmd(unsigned char *ptr, int &nb)
{  int i, l, rc;
   unsigned char ch;
   if(nCmd <= 0)
	   return 0;
   l = GetLbuf();
   StartRead();
   if(l != lcmd)
	   return -1;
   for(i=0; i<l; i++)
   {  rc = Read((char *)&ch);
      if(rc == 0)
	  {  ptr[i] = ch;		  
	  }
   }
   EndRead();

   ptr[i] = 0;
   nb = l;
   nCmd--;
   Reset();
   return 1;
}

