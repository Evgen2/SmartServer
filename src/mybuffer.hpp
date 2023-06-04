/* mybuffer.hpp */
#ifndef MYBUFFER
#define MYBUFFER

class myBuffer
{
 public:
   char *pbuf; // ��������� �� �����
   int Lbuf;  // ����� ������ � ������
   int ibuf;  // ������ ������ ������� ������� 
   int ibuf2; // ������ ������ ����������� ������� 
   int ifree; // ������ ������ ��������� ������� 
//   int Litem; // ����� �������� � int, 0=���������� ����� ��������

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
   int Litem; // ����� �������� � int, 0=���������� ����� ��������

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
 volatile int nCmd; // ����� ������ � ������. ������� - ������������������ ������ �� 0x0a 
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

/*  ����� ��� ������ ������ � ������� 0xfe 0x00 2����� ������� 2 ����� ����� ������+6  ...������ */
class myBufferCmd2:public myBuffer
{
public:
 volatile int nCmd; // ����� ������ � ������. ������� - ������������������ ������ � ������� ����
	int il; // ������� ���������� ���������� ���� 
	int lcmd;// ����� �������, �� ������ 6
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
