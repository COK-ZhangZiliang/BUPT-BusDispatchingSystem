#include "variable.h"
#include "Draw_picture.h"
#include <iostream>
#include <windows.h>
#include <math.h>
#include <thread>
#include <easyx.h>
#include <math.h>
#define CLK 0
#define END 1
#define max 0x7fffffff 
#define _CRT_SECURE_NO_WARNINGS
#define PI 3.14159265359
extern int totalStation, strategy, Distance; // FCFS 0,SSTF=1,SCAN=2
extern int num;
extern int sec;
extern int goaltype;
extern int goalstation;
extern int pre;
extern int retype;
extern int station;
extern int End;
extern int music_flag;
extern struct Bus bus;
extern struct SS stationState[10]; //点信息队列（结构体000
extern struct initList* initHead;				  //原始状态请求队列头指针 
void button(int x, int y, int r, char ch)
{
	setbkmode(0); //设置背景为透明
	//设置按钮边框
	circle(x, y, r); //绘制按钮
	settextstyle(30, 20, "黑体"); //设置字体
	outtextxy(x - 10, y - 15, ch); //ch可为T、C、U、E
}
void draw_in(int y1, int x1, int R, int L, int y2, int y3, int x2, int r, int l)
{
	///setlinestyle(PS_DASH); //设置虚线框
	//rectangle(800, 350, 1180, 680); //绘制虚线框
	//setlinestyle(PS_SOLID, 5);
	setlinecolor(BLACK);//设置线框颜色为黑色
	setlinestyle(PS_DASH); //设置虚线框
	rectangle(800, 350, 1180, 680); //绘制虚线框
	setlinestyle(PS_SOLID, 5);
	settextcolor(BLACK);//设置按钮字体颜色
	//setlinecolor();
	//settextcolor();
	button(x1 + 0 * L, y1, R, 'T');
	//setlinecolor(); 
	//settextcolor(); 
	button(x1 + 1 * L, y1, R, 'C');
	//setlinecolor(); 
	//settextcolor(); 
	button(x1 + 2 * L, y1, R, 'U');
	//setlinecolor(); 
	//settextcolor(); 
	button(x1 + 3 * L, y1, R, 'E');//绘制不同指令的按钮.
	setlinestyle(PS_SOLID, 2);
	for (int i = 0; i < 4; i++)
	{
		button(x2 + i * l, y2, r, '1' + i);
		button(x2 + i * l, y3, r, '6' + i);
	}
	button(x2 + 4 * l, y2, r, '5');//绘制5站点按钮

	circle(x2 + 4 * l, y3, r); //绘制按钮
	settextstyle(30, 20, "黑体"); //设置字体
	outtextxy(x2 + 4 * l - 18, y3 - 16, "10");
	//单独绘制10站点按钮
	setlinecolor(WHITE); //还原边框颜色
	settextcolor(WHITE); //还原字体颜色
}
void draw_out()
{
	int _x = 820;//输出的x坐标都为950
	int _y = 50;//Time那一行的y坐标
	int _h = 60;//每个输出之间的间隔

	char _time[10];
	sprintf_s(_time, "TIME:%d", sec); //将sec从int转化为char*
	setlinecolor(BLACK);//设置边框颜色为黑色
	setlinestyle(PS_DASH, 2); //设置虚线框
	setfillcolor(WHITE);
	setfillcolor(WHITE);
	setfillcolor(WHITE);
	setfillcolor(WHITE);
	rectangle(800, 20, 1180, 350); //绘制虚线框
	settextcolor(BLACK);//设置输出字体颜色
	settextstyle(25, 0, "仿宋"); //设置字体
	outtextxy(_x, _y, _time);  //输出time
	char _position[15];
	sprintf_s(_position, "position:%d", bus.loc); //将sec从int转化为char*
	settextcolor(BLACK);//设置输出字体颜色
	settextstyle(25, 0, "仿宋"); //设置字体
	outtextxy(_x, _y + _h, _position); //输出position
	char _target[30] = "target:";
	char _clockwise[30] = "clockwise:";
	char _counterclockwise[30] = "counterclockwise:";
	for (int i = 0; i < totalStation; i++)
	{
		if (stationState[i].target)
			*(_target + 7 + i) = '1';
		else
			*(_target + 7 + i) = '0';
		if (stationState[i].clockwise)
			*(_clockwise + 10 + i) = '1';
		else
			*(_clockwise + 10 + i) = '0';
		if (stationState[i].counterclockwise)
			*(_counterclockwise + 17 + i) = '1';
		else
			*(_counterclockwise + 17 + i) = '0';
	}//将站点信息分别转化到三个字符串中
	settextcolor(BLACK);//设置输出字体颜色
	settextstyle(25, 0, "仿宋"); //设置字体
	outtextxy(_x, _y + 2 * _h, _target); //输出target
	outtextxy(_x, _y + 3 * _h, _clockwise); //输出clockwise
	outtextxy(_x, _y + 4 * _h, _counterclockwise); //输出counterclockwise
}
void Draw()
{
	int width = 1200;
	int height = 700;
	//rectangle(800, 350, 1180, 680); 
	int y1 = 400;//'T'、'C'、'U'、'E'按钮的y坐标为500
	int y_tip = 450;//提示的y坐标
	int x1 = 840;//'T'按钮的x坐标
	int R = 30;//'T'、'C'、'U'、'E'按钮的半径为50
	int L = 100;//'T'、'C'、'U'、'E'按钮的间隔为150
	int y2 = 580;//输出12345站点的y坐标为700
	int y3 = 630;//输出678910站点的y坐标为800
	int x2 = 830;//站点按钮的x坐标
	int r = 20;//输出站点按钮的半径为30
	int l = 80; //输出站点按钮的间隔为120
	//配置一堆参数
	BeginBatchDraw();
	setbkcolor(WHITE);
	cleardevice();
	while (End != 1)
	{
		//int _order = 0;// 1为target指令，2为clockwise指令，3为counterclockwise指令
		//int _station = -1;//站点请求
		ExMessage msg; //键盘鼠标信息结构体 //双缓冲绘图，绘画内容放在两个之间。 能防止“闪屏”
			// 设置背景 code here
		draw_in(y1, x1, R, L, y2, y3, x2, r, l);
		draw_out();
		outtextxy(860, y_tip, "先按上边的指令按钮");//提示
		outtextxy(860, y_tip + 50, "再按下边的站点按钮");//提示
	//	printf("ssssss\n");
	//	printf("ooooooo\n");
		setlinecolor(RGB(39, 255, 230));
		setlinestyle(PS_SOLID, 8);
		settextcolor(RGB(255, 0, 0));
		settextstyle(15, 12, "黑体");
		setbkmode(TRANSPARENT);
		circle(360, 360, 280);//画出圆形轨道
		int num;
		num = totalStation * Distance;//计算点的总数
		for (int i = 0; i < num; i++)
		{
			double x, y;
			double angle = i * 1.0 / num * 2 * PI;//计算坐标偏转角度
			char s[3];
			s[0] = '0' + (i / Distance + 1) / 10;
			s[1] = '0' + (i / Distance + 1) % 10;
			s[2] = '\0';
			x = 360 - 280 * cos(angle);//点的横坐标
			y = 360 - 280 * sin(angle);//点的纵坐标；
			if (i % Distance == 0)//判断是否为站点，若是使用不同的颜色，不同的大小
			{
				if (i == bus.loc || (i == 0 && bus.loc == num))//判断公交车是否到达该点，若是闪烁该点
					setfillcolor(RGB(255, 145, 203));
				else
					setfillcolor(LIGHTGREEN);
				solidcircle(x, y, 16);
				outtextxy(x - 11, y - 7, s);
			}
			else
			{
				if (i == bus.loc || (i == 0 && bus.loc == num))//判断公交车是否到达该点，若是闪烁该点
					setfillcolor(RGB(255, 145, 203));
				else
					setfillcolor(RGB(0, 182, 255));
				solidcircle(x, y, 8);
			}
		}
		IMAGE img1, img2;//原图片  旋转后的图片
		int num1 = totalStation * Distance;//计算点的总数
		double rotate_angle = -(2 * PI / num1) * bus.loc;//计算图片旋转角度
		double angle = bus.loc * 1.0 / num1 * 2 * PI;//计算坐标偏转角度
		int x, y;//公交车的坐标
		x = 360 - 315 * cos(angle) - 22.5;//公交车的横坐标
		y = 360 - 315 * sin(angle) - 14;//公交车的纵坐标；
		loadimage(&img1, "./bus1.jpg");
		loadimage(&img2, "./bus2.jpg");
		putimage(x, y, &img2, SRCAND);
		putimage(x, y, &img1, SRCPAINT);
		IMAGE img;
		loadimage(&img, "./bupt.jpg");
		putimage(211, 210, &img, SRCCOPY);
		IMAGE button;
		loadimage(&button, "./button.png");
		putimage(75, 20, &button, SRCCOPY);//图片 60*54
		setbkmode(TRANSPARENT);
		settextstyle(20, 0, "宋体");
		settextcolor(BLACK);
		char text[] = "点击以播放/关闭音乐";
		int tx = 75 - (textwidth(text) - 60) / 2;
		int ty = 80;
		outtextxy(tx, ty, text);
		FlushBatchDraw();
		//	printf("ttttttt\n");
	}

}


