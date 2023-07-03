#include "Control.h"
#include "variable.h"
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
int isFINISH1()
{
	if (strategy == 0) //先来先服务无需考虑方向
	{
		if ((goalstation - 1) * Distance == bus.loc) //判断是否到达目标站点
			return 1;
		return 0;
	}
	else if (strategy == 1)
	{
		//printf("yyyyyyyyyy\n");
		if ((goalstation - 1) * Distance == bus.loc) return 1;	//如果bus到达goal直接返回1
		if (bus.loc % Distance == 0) {
			int station = (bus.loc / Distance);
			if (stationState[station].target == 1) return 1;
			if (((stationState[station].clockwise == 1) && (bus.direct == 0)) || ((stationState[station].counterclockwise == 1) && (bus.direct == 1))) return 1;
		}
		return 0;
		//如果bus.loc是Distance的整数倍 且所在站点的stationState有请求且方向相同就返回1
		//如果均不 返回0
	}
	else if (strategy == 2) {
		if ((goalstation - 1) * Distance == bus.loc) return 1;	//如果bus到达goal直接返回1
		if (bus.loc % Distance == 0) {
			int station = (bus.loc / Distance);
			int num = stationState[station].clockwise + stationState[station].target + stationState[station].counterclockwise;
			if (num != 0) return 1;
		}
		return 0;
	}
}
void finishGoal1()
{
	if (strategy == 0) // FCFS
	{
		struct initList* temp;
		temp = initHead->next;
		initHead->next = temp->next; //改变头指针的next的指向，使其指向下一个任务
		switch (goaltype)			 //改变站点请求状态
		{
		case 0:
			stationState[goalstation - 1].target = 0;
			break;
		case 1:
			stationState[goalstation - 1].clockwise = 0;
			break;
		case 2:
			stationState[goalstation - 1].counterclockwise = 0;
			break;
		default:
			break;
		}
		temp = initHead->next;
		while ((temp != NULL) && (temp->station == goalstation))
		{
			goaltype = temp->type;
			initHead->next = temp->next;
			temp = initHead->next;
			switch (goaltype) //改变站点请求状态
			{
			case 0:
				stationState[goalstation - 1].target = 0;
				break;
			case 1:
				stationState[goalstation - 1].clockwise = 0;
				break;
			case 2:
				stationState[goalstation - 1].counterclockwise = 0;
				break;
			default:
				break;
			}
		}
	}
	else if (strategy == 1) // SSTF
	{
		if ((goalstation - 1) * Distance == bus.loc) //目标站
		{
			stationState[goalstation - 1].target = 0;
			stationState[goalstation - 1].clockwise = 0;
			stationState[goalstation - 1].counterclockwise = 0; //目标站完成所有请求 
			stationState[goalstation - 1].target_num = max;
			stationState[goalstation - 1].clockwise_num = max;
			stationState[goalstation - 1].counterclockwise_num = max;
			pre = goalstation;
			goaltype = -1;
			goalstation = -1; //衔接getgoal 
		}
		else //顺便站
		{
			if (bus.direct == 0) //顺时针 
			{
				stationState[bus.loc / Distance].target = 0;
				stationState[bus.loc / Distance].clockwise = 0;
				stationState[bus.loc / Distance].target_num = max;
				stationState[bus.loc / Distance].clockwise_num = max;
			}
			else if (bus.direct == 1)  //逆时针
			{
				stationState[bus.loc / Distance].target = 0;
				stationState[bus.loc / Distance].counterclockwise = 0;
				stationState[bus.loc / Distance].target_num = max;
				stationState[bus.loc / Distance].counterclockwise_num = max;
			}
			bus.state = 1;
		} //顺便站改变站点信息，小车状态置为1
	}
	else if (strategy == 2) // SCAN
	{
		stationState[bus.loc / Distance].target = 0;
		stationState[bus.loc / Distance].clockwise = 0;
		stationState[bus.loc / Distance].counterclockwise = 0;
		stationState[bus.loc / Distance].target_num = max;
		stationState[bus.loc / Distance].clockwise_num = max;
		stationState[bus.loc / Distance].counterclockwise_num = max;
		//无论目标站还是顺便站都完成所有请求 
		if ((goalstation - 1) * Distance == bus.loc) //目标站
		{
			goaltype = -1;
			goalstation = -1; //衔接getgoal 
		}
		else //顺便站
			bus.state = 1;
	}//顺便站改变站点信息，小车状态置为1
}
void busRun()
{
	switch (bus.state)
	{ //分公交车状态讨论
	case 0:
		break; //如果空闲 什么都不做 退出
	case 1:
		//如果没完成 bus根据原来的方向边界行驶
		if (bus.direct == 0)
		{ //顺时针
			if (bus.loc + 1 <= Distance * totalStation - 1)
				bus.loc++;
			else
				bus.loc = 0; //边界处理
		}
		else
		{ //逆时针
			if (bus.loc - 1 >= 0)
				bus.loc--;
			else
				bus.loc = Distance * totalStation - 1; //边界处理
		}
		if (isFINISH1() == 1) {
			bus.state = 2; //如果到达了 bus的状态转为2（开始停靠状态）
			//printf("sssssss\n");
		}
		break;				//退出
	case 2:					// bus的状态为2：把bus的状态置为三
		bus.state = 3;
		break;
	default:
		break;
	}
}
void Draw_out2()
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
void GetGoal2()
{
	if (strategy == 0)
	{
		struct initList* temp;
		temp = initHead->next;
		if (temp == NULL)
		{
			//printf("%dttttt\n", bus.state);
			bus.state = 0;
			return;
		}
		goaltype = temp->type;       //获取goal的类型
		goalstation = temp->station; //获取goal的station
	//	printf("%doooooo\n", bus.state);
		while (((goalstation - 1) * Distance == bus.loc) && (bus.state == 0))
		{
			switch (goaltype) //改变站点请求状态
			{
			case 0:
				stationState[goalstation - 1].target = 0;
				break;
			case 1:
				stationState[goalstation - 1].clockwise = 0;
				break;
			case 2:
				stationState[goalstation - 1].counterclockwise = 0;
				break;
			default:
				break;
			}
			temp = temp->next;
			initHead->next = temp;
			if (temp == NULL)
			{
				//	printf("%d\n", bus.state);
				bus.state = 0;
				return;
			}
			goaltype = temp->type;       //获取goal的类型
			goalstation = temp->station; //获取goal的station
		}
		if (bus.state != 2)
		{
			//	printf("%d000000\n", bus.state);
			bus.state = 1;
			int Distance1, Distance2; //顺逆时针的距离
			Distance1 = abs((goalstation - 1) * Distance - bus.loc);
			Distance2 = totalStation * Distance - Distance1;
			if (Distance1 < Distance2) //比较两种不同方式到达目标所需的距离大小
			{
				if (bus.loc <= (goalstation - 1) * Distance) //判断该方式的顺逆
					bus.direct = 0;
				else
					bus.direct = 1;
			}
			else if (Distance1 > Distance2)
			{
				if (bus.loc <= (goalstation - 1) * Distance) //判断该方式的顺逆
					bus.direct = 1;
				else
					bus.direct = 0;
			}
			else //如果Distance1=Distance2 选择顺时针方向
			{
				bus.direct = 0;
			}
		}
	}
	else if (strategy == 1)
	{ //只有*goaltype *goalstation 为-1 才进入
		if (goalstation != -1 || goaltype != -1)
			return;
		//找到最短的目标 从小车当前位置从右往左访问stationState找到最短
		int station = bus.loc / Distance; //以小车当前位置为起点
		int cnt1 = 1;                      //计数器1
		int cnt2 = 1;                      //计数器2
		while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt2 <= totalStation - 1)
		{
			station += cnt1;
			if (station > totalStation - 1)
				station -= totalStation;
			else if (station < 0)
				station += totalStation;
			cnt1 = pow(-1, abs(cnt1)) * (abs(cnt1) + 1); //改变cnt1的值:1 -2 3 -4 5...实现先右再左的寻找要求
			cnt2++;
			//   printf("4cnt1%d\n",cnt1);
		}
		//  printf("cnt1%d\n",cnt1);
		//printf("bbbbbbbb%d\n",station);
		//调度小车
		if (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0) //无目标 小车空闲
		{
			bus.state = 0;
			return;
		}
		cnt1 = 2 * (bus.loc / Distance - station);
		if (station + cnt1 > totalStation - 1)
			cnt1 -= totalStation;
		else if (station + cnt1 < 0)
			cnt1 += totalStation;
		//  printf("cnt1%d\n",cnt1);
		if (stationState[station + cnt1].target != 0 || stationState[station + cnt1].counterclockwise != 0 || stationState[station + cnt1].clockwise != 0) //比较对称点与该点在时间上的先后顺序
		{
			//   printf("bbbbbbbb%d\n",station);
			int min1, min2;
			if (stationState[station].target_num <= stationState[station].counterclockwise_num)
				min1 = stationState[station].target_num;
			else
				min1 = stationState[station].counterclockwise_num;
			if (min1 > stationState[station].clockwise_num)
				min1 = stationState[station].clockwise_num;
			if (stationState[station + cnt1].target_num <= stationState[station + cnt1].counterclockwise_num)
				min2 = stationState[station + cnt1].target_num;
			else
				min2 = stationState[station + cnt1].counterclockwise_num;
			if (min2 > stationState[station + cnt1].clockwise_num)
				min2 = stationState[station + cnt1].clockwise_num;
			if (min2 < min1)
				station += cnt1;
		}
		bus.state = 1;
		// printf("bbbbbbbb%d\n",station);

		goalstation = station + 1;
		int Distance1, Distance2;
		Distance1 = abs((goalstation - 1) * Distance - bus.loc);
		Distance2 = totalStation * Distance - Distance1;
		if (Distance1 < Distance2) //比较两种不同方式到达目标所需的距离大小
		{
			if (bus.loc <= (goalstation - 1) * Distance) //判断该方式的顺逆
				bus.direct = 0;
			else
				bus.direct = 1;
		}
		else if (Distance1 > Distance2)
		{
			if (bus.loc <= (goalstation - 1) * Distance) //判断该方式的顺逆
				bus.direct = 1;
			else
				bus.direct = 0;
		}
		else //如果Distance1=Distance2 选择顺时针方向
		{
			bus.direct = 0;
		}
	}
	else if (strategy == 2)
	{
		//只有*goaltype *goalstation 为-1 才进入
		if (goalstation != -1 || goaltype != -1)
			return;
		//找到最短的目标 从小车当前位置从右往左访问stationState找到最短
		int station = bus.loc / Distance; //以小车当前位置为起点
		int cnt = 1;                       //计数器 判断循环次数
		if (bus.direct == 0)              //原始方向若是顺时针
		{
			while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt <= totalStation - 1)
			{
				station++;
				if (station > totalStation - 1) // station越界处理
					station -= totalStation;
				cnt++;
			}
			if (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0) //无目标 小车空闲
			{
				bus.state = 0;
				return;
			}
			if (cnt - 1 > totalStation / 2) //换方向重新找
			{
				bus.direct = 1; //将小车方向改为反方向
				cnt = 1;         //初始化计数器的值
				station--;
				if (station < 0) // station越界处理
					station += totalStation;
				while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt <= totalStation - 1)
				{
					station--;
					if (station < 0) // station越界处理
						station += totalStation;
					cnt++;
				}
			}
		}
		else //原始方向是逆时针
		{
			while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt <= totalStation - 1)
			{
				station--;
				if (station < 0) // station越界处理
					station += totalStation;
				cnt++;
			}
			if (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0) //无目标 小车空闲
			{
				bus.state = 0;
				return;
			}
			if (cnt - 1 > totalStation / 2) //换方向重新找
			{
				bus.direct = 0; //将小车方向改为反方向
				cnt = 1;         //初始化计数器的值
				station++;
				if (station > totalStation - 1) // station越界处理
					station -= totalStation;
				while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt <= totalStation - 1)
				{
					station++;
					if (station > totalStation - 1) // station越界处理
						station -= totalStation;
					cnt++;
				}
			}
		}
		bus.state = 1;
		goalstation = station + 1;
	}
}

void backRun() {
	while (End != 1)
	{
		Sleep(5000);
		if (End == 1) break;
		sec++;
		if (strategy == 1 || strategy == 2) {
			GetGoal2();
			if (bus.state == 0 && goalstation != -1)
				bus.state = 1;
			if (stationState[bus.loc / Distance].flag == 1) stationState[bus.loc / Distance].flag = 0;
		}
		busRun();
		if (bus.state == 3)
		{
			stationState[bus.loc / Distance].flag = 1;
			finishGoal1();
			GetGoal2();
		}
		if (End == 1) break;
		cleardevice();
		Draw_out2();
	}
}
int check()
{
	struct initList* tmp = initHead;
	struct initList* cur = initHead->next;
	while (cur != NULL)
	{
		if (cur->type == retype && cur->station == station)
			return 0; //如果有未完成的与当前相同的目标 则不入队 返回零
		cur = cur->next;
	}
	return 1; //无重复 返回 1
}


