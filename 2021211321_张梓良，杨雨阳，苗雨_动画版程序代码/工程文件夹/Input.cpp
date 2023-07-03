#include "Input.h"
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
using namespace std;
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
struct initList* inintEmptyList()
{
	struct initList* head = (struct initList*)malloc(sizeof(struct initList));
	if (head != NULL)
	{
		head->next = NULL;
	}
	return head;
} //创建原始对列头指针
void setParam() //配置文件
{
	FILE* in;
	errno_t err = fopen_s(&in, "./dict.dic", "r");
	while (!feof(in)) //未读到文件末尾时
	{
		int ch = fgetc(in);
		char str[5];
		if (ch == '#')
			while (fgetc(in) != '\n')
				;			//连续移动指针直到出现换行符
		else if (ch == 'T') //某行首位是T
			fscanf_s(in, "OTAL_STATION = %d", &totalStation);
		else if (ch == 'D') //某行首位是D
			fscanf_s(in, "ISTANCE = %d", &Distance);
		else if (ch == 'S') //某行首位是S
		{
			for (int i = 0; i < 10; i++)
				fgetc(in);
			//移动到数据点
			fgets(str, 5, in);
			if (strcmp(str, "FCFS") == 0) //先来先服务
				strategy = 0;
			else if (strcmp(str, "SSTF") == 0) //最短寻找时间优先
				strategy = 1;
			else if (strcmp(str, "SCAN") == 0) //顺便服务
				strategy = 2;
		}
	}
	fclose(in);
}
void init() /*公交车状态、队列初始化*/
{
	bus.loc = 0;	//公交车初始位置为0
	bus.state = 0; //公交车初始状态为空闲
	for (int i = 0; i < totalStation; i++)
	{
		stationState[i].flag = 0;
		stationState[i].counterclockwise = 0;
		stationState[i].clockwise = 0;
		stationState[i].target = 0;
		stationState[i].counterclockwise_num = max;
		stationState[i].clockwise_num = max;
		stationState[i].target_num = max;
	}

	//初始化站点信息
}
int Check1()
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
void Draw_out1()
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
void GetGoal1()
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
struct initList* getEnd()
{
	struct initList* tmp = initHead;
	struct initList* cur = initHead->next;
	while (cur != NULL)
	{
		tmp = cur;
		cur = cur->next;
	}
	return tmp;
}
void insertEnd()
{
	struct initList* p = (struct initList*)malloc(sizeof(struct initList)); //创建节点；
	if (p != NULL)
	{
		p->type = retype;	  //复制信息
		p->station = station; //复制信息
		p->next = NULL;
		struct initList* tmp = getEnd(); //获得尾节点地址
		tmp->next = p;					  //插入
	}
}
void input()
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
	ExMessage msg; //键盘鼠标信息结构体
	while (true)
	{
		msg = getmessage(EM_MOUSE | EM_KEY); //获取键盘或鼠标信息
		if (msg.message == WM_LBUTTONDOWN) //响应鼠标或键盘操作事件,并且当获取信息为鼠标左键时.
		{
			int flag = 0;
			if (msg.x >= 75 && msg.x <= 135 && msg.y >= 20 && msg.y <= 74)
			{
				while (true)
				{
					if (music_flag == 1)
					{
						mciSendString("close ./bgm.mp3", NULL, 0, NULL);//停止播放音乐
						music_flag = 0;
					}
					else if (music_flag == 0)
					{
						mciSendString("play ./bgm.mp3 repeat", NULL, 0, NULL);//继续播放音乐
						music_flag = 1;
					}
					break;
				}
			}
			if (press_button(msg, x1, y1, R))  //target
			{
				//printf("button!\n");
				//code here 设置提示格式

				ExMessage msg1;
				while (true)
				{
					msg1 = getmessage(EM_MOUSE | EM_KEY); //获取键盘或鼠标信息
					if (msg1.message == WM_LBUTTONDOWN)
					{
						//printf("yes!\n");
						for (int i = 0; i < 5; i++)
						{
							if (press_button(msg1, x2 + i * l, y2, r))
							{
								//cleardevice();
								//printf("haha\n");
								retype = 0;
								station = i + 1;
								flag = 1;
							}

							if (press_button(msg1, x2 + i * l, y3, r))
							{
								//cleardevice();
								retype = 0;
								station = i + 5 + 1;
								flag = 1;
							}
						}
						break;
					}
				}
			}
			if (press_button(msg, x1 + 1 * L, y1, R))  //clockwise
			{
				ExMessage msg1;
				while (true)
				{
					msg1 = getmessage(EM_MOUSE | EM_KEY); //获取键盘或鼠标信息
					if (msg1.message == WM_LBUTTONDOWN)
					{
						for (int i = 0; i < 5; i++)
						{
							if (press_button(msg1, x2 + i * l, y2, r))
							{
								//cleardevice();
								retype = 1;
								station = i + 1;
								flag = 1;
							}

							if (press_button(msg1, x2 + i * l, y3, r))
							{
								//cleardevice();
								retype = 1;
								station = i + 1 + 5;
								flag = 1;
							}
						}
						break;
					}
				}
			}
			if (press_button(msg, x1 + 2 * L, y1, R))  //counterclockwise
			{
				ExMessage msg1;
				while (true)
				{
					msg1 = getmessage(EM_MOUSE | EM_KEY); //获取键盘或鼠标信息
					if (msg1.message == WM_LBUTTONDOWN)
					{
						for (int i = 0; i < 5; i++)
						{
							if (press_button(msg1, x2 + i * l, y2, r))
							{
								//cleardevice();
								retype = 2;
								station = i + 1;
								flag = 1;
							}

							if (press_button(msg1, x2 + i * l, y3, r))
							{
								//cleardevice();
								retype = 2;
								station = i + 1 + 5;
								flag = 1;
							}
						}
						break;
					}
				}
			}
			if (press_button(msg, x1 + 3 * L, y1, R)) //end
			{
				End = 1;
				break;
			}
			if (flag == 1)
			{
				if (strategy == 0) {//一策略 
					if (Check1() == 1)
					{
						if (bus.loc == (station - 1) * Distance && bus.state == 0)
						{
							Draw_out1();
							continue;
						}
						insertEnd(); //如果不和之前的重复，就插入
						int p = station - 1;
						if (retype == 0) {
							stationState[p].target = 1;
							//printf("dddxxxxxx\n");
						}
						else if (retype == 1)
							stationState[p].clockwise = 1;
						else if (retype == 2)
							stationState[p].counterclockwise = 1; //并且根据类型更改站点信息		
					}//指令
					if (bus.state == 0) GetGoal1();
					//printf("%d\n", bus.state);
					cleardevice();
					Draw_out1();
				}
				else if (strategy == 1 || strategy == 2)
				{	// 查重  赋值编号
					num++;
					if (bus.state == 0 && bus.loc == (station - 1) * Distance)
					{
						cleardevice();
						Draw_out1();
						continue;
					}
					//判断是否重 
					if ((stationState[bus.loc / Distance].flag == 1) && (bus.loc == (station - 1) * Distance)) {
						if (strategy == 1) {
							if ((pre - 1) * Distance == bus.loc) {
								cleardevice();
								Draw_out1();
								continue;
							}
							else {
								if (retype == 0) {
									cleardevice();
									Draw_out1();
									continue;
								}
								else if (bus.direct + 1 == retype) {
									cleardevice();
									Draw_out1();
									continue;
								}
							}

						}
						if (strategy == 2) {
							cleardevice();
							Draw_out1();
							continue;
						}
					}
					if (bus.state == 0 && goalstation == -1) {
						goaltype = retype;
						goalstation = station;
						int Distance1, Distance2;
						Distance1 = abs((goalstation - 1) * Distance - bus.loc);
						Distance2 = bus.loc + (totalStation - goalstation + 1) * Distance <= totalStation * Distance - bus.loc + (goalstation - 1) * Distance ? bus.loc + (totalStation - goalstation + 1) * Distance : totalStation * Distance - bus.loc + (goalstation - 1) * Distance;
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
						//printf("%d %d\n",*goaltype,*goalstation);
					}
					int p = station - 1;
					if (retype == 0 && stationState[p].target == 0) {
						stationState[p].target = 1;
						stationState[p].target_num = num;
					}
					else if (retype == 1 && stationState[p].clockwise == 0) {
						stationState[p].clockwise = 1;
						stationState[p].clockwise_num = num;
					}
					else if (retype == 2 && stationState[p].counterclockwise == 0) {
						stationState[p].counterclockwise = 1; //并且根据类型更改站点信息
						stationState[p].counterclockwise_num = num;
					}
					//	printf("%d\n",*num);
					cleardevice();
					Draw_out1();
					continue;
				}
				flag = 0;
			}

		}
		FlushBatchDraw();
	}
}
bool press_button(ExMessage msg, int x, int y, int r)
{
	double dis = (msg.x - x) * (msg.x - x) + (msg.y - y) * (msg.y - y);
	if (dis <= r * r)
		return 1;
	else
		return 0;
}