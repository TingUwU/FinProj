 
/*-----------------------------------------------
  名稱：串口通信
  網站：www.doflye.net
  編寫：shifang
  日期：2009.5
  修改：無
  內容：連接好串口或者usb轉串口至電腦，下載該程序，打開電源
        打開串口調試程序，將波特率設置為9600，無奇偶校驗
        晶振11.0592MHz，發送和接收使用的格式相同，如都使用
        字符型格式，按復位重啟程序，可以看到接收到 UART test，技術論壇：www.doflye.net 請在發送區輸入任意信
		然後在發送區發送任意信息，接收區返回同樣信息，表明串口收發無誤
------------------------------------------------*/

#include<reg52.h> //包含頭文件，一般情況不需要改動，頭文件包含特殊功能寄存器的定義 
#include <string.h>                       
#define MAX 10
#define DataPort P0

unsigned char buf[MAX];
unsigned int num[4];
unsigned char TempData[6];
unsigned char head = 0;

sbit LATCH1=P2^2;
sbit LATCH2=P2^3;

unsigned char code dofly_DuanMa[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x63, 0x39};
unsigned char code dofly_DuanMa_dp[10] = {0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef};
unsigned char code dofly_WeiMa[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};


/*------------------------------------------------
                   函數聲明
------------------------------------------------*/
void SendStr(unsigned char *s);

/*------------------------------------------------
                    串口初始化
------------------------------------------------*/
void InitUART  (void)
{

    SCON  = 0x50;		        // SCON: 模式 1, 8-bit UART, 使能接收  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit 重裝
    TH1   = 0xFD;               // TH1:  重裝值 9600 波特率 晶振 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 打開                         
    EA    = 1;                  //打開總中斷
   // ES    = 1;                  //打開串口中斷
}  

/*------------------------------------------------
 延時函數，含有輸入參數 unsigned int t，無返回值
 unsigned int 是定義無符號整形變量，其值的範圍是
 0~65535
------------------------------------------------*/

void Delay(unsigned int t)
{
	while(--t);
}

void Display(unsigned char FirstBit,unsigned char Num)
{
	unsigned char i;
	  
	for(i=0;i<Num;i++){ 
	  	DataPort=0;   //清空數據，防止有交替重影
      	LATCH1=1;     //段鎖存
      	LATCH1=0;

       	DataPort=dofly_WeiMa[i+FirstBit]; //取位碼 
       	LATCH2=1;     //位鎖存
       	LATCH2=0;

       	DataPort=TempData[i]; //取顯示數據，段碼
       	LATCH1=1;     //段鎖存
       	LATCH1=0;
		
	   	Delay(200); // 掃瞄間隙延時，時間太長會閃爍，太短會造成重影
	}
}

/*------------------------------------------------
                    主函數
------------------------------------------------*/
void main (void)
{
int i = 0;
InitUART();


SendStr("UART TEST");

ES    = 1;                  //打開串口中斷
while (1)                       
    {
 		if (head == 5)// xx.xx度
		{
			while(i < 5){// 不加有亂碼
				TempData[i] = dofly_DuanMa[0];
				i++;
			}

			buf[head] = '\0'; 
			head = 0;
			i = 0;

			num[0] = buf[0] - '0';
			num[1] = buf[1] - '0';
			num[2] = buf[3] - '0';
			num[3] = buf[4] - '0';
/*
			if ((strcmp(buf,"LEDON") == 0))	
				SendStr("REC_LED_ON");
			else if((strcmp(buf,"LEDOF") == 0))
				SendStr("REC_LED_OFF");	
*/
		}else if(head == 4){// xx.x0度
			while(i < 5){
				TempData[i] = dofly_DuanMa[0];
				i++;
			}

			buf[head] = '\0'; 
			head = 0;
			i = 0;

			num[0] = buf[0] - '0';
			num[1] = buf[1] - '0';
			num[2] = buf[3] - '0';
			num[3] = 0;
		}else if(head == 3){// xx.00度
			while(i < 5){
				TempData[i] = dofly_DuanMa[0];
				i++;
			}

			buf[head] = '\0'; 
			head = 0;
			i = 0;

			num[0] = buf[0] - '0';
			num[1] = buf[1] - '0';
			num[2] = 0;
			num[3] = 0;
		}
		TempData[0] = dofly_DuanMa[num[0]];
		TempData[1] = dofly_DuanMa_dp[num[1]];
		TempData[2] = dofly_DuanMa[num[2]];
		TempData[3] = dofly_DuanMa[num[3]];
		TempData[4] = dofly_DuanMa[10];
		TempData[5] = dofly_DuanMa[11];

		Display(2, 6);
    }
}

/*------------------------------------------------
                    發送一個字節
------------------------------------------------*/
void SendByte(unsigned char dat)
{
 SBUF = dat;
 while(!TI);
      TI = 0;
}
/*------------------------------------------------
                    發送一個字符串
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 while(*s!='\0')// \0 表示字符串結束標誌，通過檢測是否字符串末尾
  {
  SendByte(*s);
  s++;
  }
//  SendByte(0x0d);
//  SendByte(0x0a);
}
/*------------------------------------------------
                     串口中斷程序
------------------------------------------------*/
void UART_SER (void) interrupt 4 //串行中斷服務程序
{
    unsigned char Temp;          //定義臨時變量 
   
   if(RI)                        //判斷是接收中斷產生
     {
	  	RI=0;                      //標誌位清零
	  	Temp=SBUF;                 //讀入緩衝區的值
	//  P1=Temp;                   //把值輸出到P1口，用於觀察
    //  SBUF=Temp;                 //把接收到的值再發回電腦端
	  	buf[head] = Temp;
		head++;
		if (head == MAX) head = 0;
	 }
//   if(TI)                        //如果是發送標誌位，清零
//     TI=0;
} 


 
