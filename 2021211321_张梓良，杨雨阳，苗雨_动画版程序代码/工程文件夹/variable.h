#pragma once
struct Bus
{
	int loc;
	int state;	// 0 1 2  ���� ���� ͣ��
	int direct; //������Ԥ��������2Ҳ������ͣ��ʱԭ���� 0˳ʱ�� 1��ʱ��
				//	int y;//������Ԥ��
}; //������״̬�ṹ��
struct SS
{
	int flag;
	int counterclockwise;
	int clockwise;
	int target;
	int counterclockwise_num;
	int clockwise_num;
	int target_num;
}; //վ����Ϣ
struct initList
{
	int type; // �������� 0 1 2  target clockwise counterclockwise
	int station;
	struct initList* next;
}; //������Ϣ����ڵ�
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
extern struct SS stationState[10]; //����Ϣ���У��ṹ��
extern struct initList* initHead;	//ԭʼ״̬�������ͷָ�� 
extern int music_flag;