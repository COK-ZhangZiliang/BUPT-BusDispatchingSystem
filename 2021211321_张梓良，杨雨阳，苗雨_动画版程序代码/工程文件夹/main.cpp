#define _CRT_SECURE_NO_WARNINGS
#define PI 3.14159265359
#include <iostream>
#include <windows.h>
#include <math.h>
#include <thread>
#include <easyx.h>
#include <math.h>
#include<mmsystem.h>  
#pragma comment(lib,"winmm.lib")
#define CLK 0
#define END 1
#define max 0x7fffffff 
#include "variable.h"
#include "Input.h"
#include "Control.h"
#include "Done.h"
#include "Draw_picture.h"
using namespace std;
int totalStation = 5, strategy = 0, Distance = 2; // FCFS 0,SSTF=1,SCAN=2
int num = 0;
int sec = 0;
int goaltype = -1;
int goalstation = -1;
int pre;
int retype = -1;
int station;
int End = 0;
int music_flag = 0;
struct Bus bus;
struct SS stationState[10]; //点信息队列（结构体000
struct initList* initHead = NULL;				  //原始状态请求队列头指针 																																																																			//查重 函数
int main(void)
{
	initHead = inintEmptyList();
	setParam();
	init();
	initgraph(1200, 700);
	mciSendString("open ./bgm.mp3", NULL, 0, NULL);//打开音乐
	thread test1(input);
	thread test2(backRun);
	thread test3(Draw);
	test1.join();
	test2.join();
	test3.join();
	printf("end\n");
	return 0;
}









