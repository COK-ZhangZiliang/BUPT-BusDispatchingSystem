#pragma once
struct Bus
{
	int loc;
	int state;	// 0 1 2  空闲 运行 停靠
	int direct; //动画版预留；策略2也需区分停靠时原方向 0顺时针 1逆时针
				//	int y;//动画版预留
}; //公交车状态结构体
struct SS
{
	int flag;
	int counterclockwise;
	int clockwise;
	int target;
	int counterclockwise_num;
	int clockwise_num;
	int target_num;
}; //站点信息
struct initList
{
	int type; // 三个类型 0 1 2  target clockwise counterclockwise
	int station;
	struct initList* next;
}; //请求信息链表节点
extern int totalStation, strategy, Distance; // FCFS 0,SSTF=1,SCAN=2
extern int num;
extern int sec;
extern int goaltype;
extern int goalstation;
extern int pre;
extern int retype;
extern int station;
extern int End;
extern struct Bus bus;
extern struct SS stationState[10]; //点信息队列（结构体
extern struct initList* initHead;	//原始状态请求队列头指针 
extern int music_flag;