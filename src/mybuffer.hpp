/* mybuffer.hpp */
#ifndef MYBUFFER
#define MYBUFFER

class myBuffer
{
 public:
   char *pbuf; // указатель на буфер
   int Lbuf;  // длина буфера в байтах
   int ibuf;  // индекс начала занятой области 
   int ibuf2; // индекс начала прочитанной области 
   int ifree; // индекс начала свободной области 
//   int Litem; // длина элемента в int, 0=переменная длина элемента

   myBuffer(void)
   {  pbuf = NULL;
      Lbuf = 0;
      ibuf = ibuf2 = ifree = 0;
//      Litem = 0;
   };
    void Init(char *pb, int Lb);
	  int Add(char byte);
    int Add(char *pb, int Lb);
//    int Add2(int *pb, int Lb);
    int Get(void);
    void StartRead(void);
    int GetUnread(void);
    int Read(char *el);
    void EndRead(void);
    int GetFree(void);
    int GetLbuf(void);
};

class myBuffer2:myBuffer
{
 public:
   int Litem; // длина элемента в int, 0=переменная длина элемента

   myBuffer2(void)
   {  Litem = 0;
   };
   void Init(void *pb, int Lb, int _Litem);
	 int Add(void *prt);
   int Get(void *prt);
   int GetLbuf(void);
};

class myBufferCmd:public myBuffer
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

/*  класс для буфера команд в формате 0xfe 0x00 2байта команда 2 байта длина данных+6  ...данные */
class myBufferCmd2:public myBuffer
{
public:
 volatile int nCmd; // Число команд в буфере. команда - последовательность байтов в формате выше
	int il; // текущее количество полученных байт 
	int lcmd;// длина команды, не меньше 6
	int raz;
   myBufferCmd2(void)
   {  nCmd = 0;
	  raz = 0;
	  il = lcmd = 0;
   };
	void Reset(void)
	{ il = lcmd = nCmd = 0;
      ibuf = ibuf2 = ifree = 0;
	};
   int Add(unsigned char byte);
   int Add(unsigned char *ptr, int nb);
   int AddCmd(unsigned char *prt);
   int GetCmd(unsigned char *prt, int &nb);
};

#endif // MYBUFFER
