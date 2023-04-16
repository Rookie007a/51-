#include <reg51.h>
#define TIMER0_INITIAL_VALUE 0xB2 // (256-50) = 206，定时器 0 在溢出前的初值

static unsigned char nixie_tub_tab[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
sbit P3_0=P3^0;
int smog_alarm = 3;
unsigned int timer0_count = 0; // 用于计数的全局变量
unsigned int ggb;
unsigned int ppl;
unsigned int opo;

void init_timer0()
{
	TMOD |= 0x01;						// 设置定时器 0 为模式 1，16 位定时器
	TH0 = TIMER0_INITIAL_VALUE / 0x100; // 设置定时器 0 的高 8 位初值
	TL0 = TIMER0_INITIAL_VALUE % 0x100; // 设置定时器 0 的低 8 位初值
	ET0 = 1;							// 允许定时器 0 中断
	TR0 = 1;							// 启动定时器 0
}

void delay(unsigned int i)
{
  unsigned int j;
  for(; i > 0; i--)
    for(j = 0; j < 333; j++) {;}
}

void main()
{
	init_timer0(); 
	P0  = 0x00;
	P2  = 0xFF;
	P1 = 0X00;
	EA  = 1;    // 全局中断使能
	EX0 = 1;	// 允许外部中断 0
	IT0 = 1;	// 设置外部中断 0 为下降沿触发
	EX1 = 1;	// 允许外部中断 1
	IT1 = 1;	// 设置外部中断 1 为下降沿触发
	IP	= 0;
	while(1)
	{
		if(P3_0==0&smog_alarm==0){
			P1 = nixie_tub_tab[opo];
		}
		if(P3_0==0&smog_alarm==3){
			P1 = 0X40;
		}
		delay(20);
	}
}

void int0(void) interrupt 0
{
	P2 = 0xFF;
	P0 = 0X00;
	smog_alarm = 0;
}

void int1(void) interrupt 2
{
	smog_alarm = 1;
	ppl = 1;
}

void timer0(void) interrupt 1
{
	timer0_count++;						// 每次定时器 0 溢出，计数器加 1
	if(timer0_count>9)
	{
		ggb++;
		timer0_count = 0;
		if(ggb>9){
			ggb = 0;}
	}
	
	if(ppl)
	{
		opo = ggb;
		ppl = 0;
	}
	if(smog_alarm==1)
	{
		P2 = ~P2;
		P0 = nixie_tub_tab[opo];
	}
	
	TH0 = TIMER0_INITIAL_VALUE / 0x100; // 设置定时器 0 的高 8 位初值
	TL0 = TIMER0_INITIAL_VALUE % 0x100; // 设置定时器 0 的低 8 位初值
}
