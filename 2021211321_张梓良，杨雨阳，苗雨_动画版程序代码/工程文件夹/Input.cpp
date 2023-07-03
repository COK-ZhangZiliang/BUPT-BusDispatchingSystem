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
extern struct SS stationState[10]; //����Ϣ���У��ṹ��000
extern struct initList* initHead;				  //ԭʼ״̬�������ͷָ�� 
struct initList* inintEmptyList()
{
	struct initList* head = (struct initList*)malloc(sizeof(struct initList));
	if (head != NULL)
	{
		head->next = NULL;
	}
	return head;
} //����ԭʼ����ͷָ��
void setParam() //�����ļ�
{
	FILE* in;
	errno_t err = fopen_s(&in, "./dict.dic", "r");
	while (!feof(in)) //δ�����ļ�ĩβʱ
	{
		int ch = fgetc(in);
		char str[5];
		if (ch == '#')
			while (fgetc(in) != '\n')
				;			//�����ƶ�ָ��ֱ�����ֻ��з�
		else if (ch == 'T') //ĳ����λ��T
			fscanf_s(in, "OTAL_STATION = %d", &totalStation);
		else if (ch == 'D') //ĳ����λ��D
			fscanf_s(in, "ISTANCE = %d", &Distance);
		else if (ch == 'S') //ĳ����λ��S
		{
			for (int i = 0; i < 10; i++)
				fgetc(in);
			//�ƶ������ݵ�
			fgets(str, 5, in);
			if (strcmp(str, "FCFS") == 0) //�����ȷ���
				strategy = 0;
			else if (strcmp(str, "SSTF") == 0) //���Ѱ��ʱ������
				strategy = 1;
			else if (strcmp(str, "SCAN") == 0) //˳�����
				strategy = 2;
		}
	}
	fclose(in);
}
void init() /*������״̬�����г�ʼ��*/
{
	bus.loc = 0;	//��������ʼλ��Ϊ0
	bus.state = 0; //��������ʼ״̬Ϊ����
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

	//��ʼ��վ����Ϣ
}
int Check1()
{
	struct initList* tmp = initHead;
	struct initList* cur = initHead->next;
	while (cur != NULL)
	{
		if (cur->type == retype && cur->station == station)
			return 0; //�����δ��ɵ��뵱ǰ��ͬ��Ŀ�� ����� ������
		cur = cur->next;
	}
	return 1; //���ظ� ���� 1
}
void Draw_out1()
{
	int _x = 820;//�����x���궼Ϊ950
	int _y = 50;//Time��һ�е�y����
	int _h = 60;//ÿ�����֮��ļ��

	char _time[10];
	sprintf_s(_time, "TIME:%d", sec); //��sec��intת��Ϊchar*
	setlinecolor(BLACK);//���ñ߿���ɫΪ��ɫ
	setlinestyle(PS_DASH, 2); //�������߿�
	setfillcolor(WHITE);
	setfillcolor(WHITE);
	setfillcolor(WHITE);
	setfillcolor(WHITE);
	rectangle(800, 20, 1180, 350); //�������߿�
	settextcolor(BLACK);//�������������ɫ
	settextstyle(25, 0, "����"); //��������
	outtextxy(_x, _y, _time);  //���time
	char _position[15];
	sprintf_s(_position, "position:%d", bus.loc); //��sec��intת��Ϊchar*
	settextcolor(BLACK);//�������������ɫ
	settextstyle(25, 0, "����"); //��������
	outtextxy(_x, _y + _h, _position); //���position
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
	}//��վ����Ϣ�ֱ�ת���������ַ�����
	settextcolor(BLACK);//�������������ɫ
	settextstyle(25, 0, "����"); //��������
	outtextxy(_x, _y + 2 * _h, _target); //���target
	outtextxy(_x, _y + 3 * _h, _clockwise); //���clockwise
	outtextxy(_x, _y + 4 * _h, _counterclockwise); //���counterclockwise
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
		goaltype = temp->type;       //��ȡgoal������
		goalstation = temp->station; //��ȡgoal��station
	//	printf("%doooooo\n", bus.state);
		while (((goalstation - 1) * Distance == bus.loc) && (bus.state == 0))
		{
			switch (goaltype) //�ı�վ������״̬
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
			goaltype = temp->type;       //��ȡgoal������
			goalstation = temp->station; //��ȡgoal��station
		}
		if (bus.state != 2)
		{
			//	printf("%d000000\n", bus.state);
			bus.state = 1;
			int Distance1, Distance2; //˳��ʱ��ľ���
			Distance1 = abs((goalstation - 1) * Distance - bus.loc);
			Distance2 = totalStation * Distance - Distance1;
			if (Distance1 < Distance2) //�Ƚ����ֲ�ͬ��ʽ����Ŀ������ľ����С
			{
				if (bus.loc <= (goalstation - 1) * Distance) //�жϸ÷�ʽ��˳��
					bus.direct = 0;
				else
					bus.direct = 1;
			}
			else if (Distance1 > Distance2)
			{
				if (bus.loc <= (goalstation - 1) * Distance) //�жϸ÷�ʽ��˳��
					bus.direct = 1;
				else
					bus.direct = 0;
			}
			else //���Distance1=Distance2 ѡ��˳ʱ�뷽��
			{
				bus.direct = 0;
			}
		}
	}
	else if (strategy == 1)
	{ //ֻ��*goaltype *goalstation Ϊ-1 �Ž���
		if (goalstation != -1 || goaltype != -1)
			return;
		//�ҵ���̵�Ŀ�� ��С����ǰλ�ô����������stationState�ҵ����
		int station = bus.loc / Distance; //��С����ǰλ��Ϊ���
		int cnt1 = 1;                      //������1
		int cnt2 = 1;                      //������2
		while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt2 <= totalStation - 1)
		{
			station += cnt1;
			if (station > totalStation - 1)
				station -= totalStation;
			else if (station < 0)
				station += totalStation;
			cnt1 = pow(-1, abs(cnt1)) * (abs(cnt1) + 1); //�ı�cnt1��ֵ:1 -2 3 -4 5...ʵ�����������Ѱ��Ҫ��
			cnt2++;
			//   printf("4cnt1%d\n",cnt1);
		}
		//  printf("cnt1%d\n",cnt1);
		//printf("bbbbbbbb%d\n",station);
		//����С��
		if (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0) //��Ŀ�� С������
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
		if (stationState[station + cnt1].target != 0 || stationState[station + cnt1].counterclockwise != 0 || stationState[station + cnt1].clockwise != 0) //�Ƚ϶ԳƵ���õ���ʱ���ϵ��Ⱥ�˳��
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
		if (Distance1 < Distance2) //�Ƚ����ֲ�ͬ��ʽ����Ŀ������ľ����С
		{
			if (bus.loc <= (goalstation - 1) * Distance) //�жϸ÷�ʽ��˳��
				bus.direct = 0;
			else
				bus.direct = 1;
		}
		else if (Distance1 > Distance2)
		{
			if (bus.loc <= (goalstation - 1) * Distance) //�жϸ÷�ʽ��˳��
				bus.direct = 1;
			else
				bus.direct = 0;
		}
		else //���Distance1=Distance2 ѡ��˳ʱ�뷽��
		{
			bus.direct = 0;
		}
	}
	else if (strategy == 2)
	{
		//ֻ��*goaltype *goalstation Ϊ-1 �Ž���
		if (goalstation != -1 || goaltype != -1)
			return;
		//�ҵ���̵�Ŀ�� ��С����ǰλ�ô����������stationState�ҵ����
		int station = bus.loc / Distance; //��С����ǰλ��Ϊ���
		int cnt = 1;                       //������ �ж�ѭ������
		if (bus.direct == 0)              //ԭʼ��������˳ʱ��
		{
			while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt <= totalStation - 1)
			{
				station++;
				if (station > totalStation - 1) // stationԽ�紦��
					station -= totalStation;
				cnt++;
			}
			if (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0) //��Ŀ�� С������
			{
				bus.state = 0;
				return;
			}
			if (cnt - 1 > totalStation / 2) //������������
			{
				bus.direct = 1; //��С�������Ϊ������
				cnt = 1;         //��ʼ����������ֵ
				station--;
				if (station < 0) // stationԽ�紦��
					station += totalStation;
				while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt <= totalStation - 1)
				{
					station--;
					if (station < 0) // stationԽ�紦��
						station += totalStation;
					cnt++;
				}
			}
		}
		else //ԭʼ��������ʱ��
		{
			while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt <= totalStation - 1)
			{
				station--;
				if (station < 0) // stationԽ�紦��
					station += totalStation;
				cnt++;
			}
			if (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0) //��Ŀ�� С������
			{
				bus.state = 0;
				return;
			}
			if (cnt - 1 > totalStation / 2) //������������
			{
				bus.direct = 0; //��С�������Ϊ������
				cnt = 1;         //��ʼ����������ֵ
				station++;
				if (station > totalStation - 1) // stationԽ�紦��
					station -= totalStation;
				while (stationState[station].target == 0 && stationState[station].counterclockwise == 0 && stationState[station].clockwise == 0 && cnt <= totalStation - 1)
				{
					station++;
					if (station > totalStation - 1) // stationԽ�紦��
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
	struct initList* p = (struct initList*)malloc(sizeof(struct initList)); //�����ڵ㣻
	if (p != NULL)
	{
		p->type = retype;	  //������Ϣ
		p->station = station; //������Ϣ
		p->next = NULL;
		struct initList* tmp = getEnd(); //���β�ڵ��ַ
		tmp->next = p;					  //����
	}
}
void input()
{
	int width = 1200;
	int height = 700;
	//rectangle(800, 350, 1180, 680); 
	int y1 = 400;//'T'��'C'��'U'��'E'��ť��y����Ϊ500
	int y_tip = 450;//��ʾ��y����
	int x1 = 840;//'T'��ť��x����
	int R = 30;//'T'��'C'��'U'��'E'��ť�İ뾶Ϊ50
	int L = 100;//'T'��'C'��'U'��'E'��ť�ļ��Ϊ150
	int y2 = 580;//���12345վ���y����Ϊ700
	int y3 = 630;//���678910վ���y����Ϊ800
	int x2 = 830;//վ�㰴ť��x����
	int r = 20;//���վ�㰴ť�İ뾶Ϊ30
	int l = 80; //���վ�㰴ť�ļ��Ϊ120
	ExMessage msg; //���������Ϣ�ṹ��
	while (true)
	{
		msg = getmessage(EM_MOUSE | EM_KEY); //��ȡ���̻������Ϣ
		if (msg.message == WM_LBUTTONDOWN) //��Ӧ������̲����¼�,���ҵ���ȡ��ϢΪ������ʱ.
		{
			int flag = 0;
			if (msg.x >= 75 && msg.x <= 135 && msg.y >= 20 && msg.y <= 74)
			{
				while (true)
				{
					if (music_flag == 1)
					{
						mciSendString("close ./bgm.mp3", NULL, 0, NULL);//ֹͣ��������
						music_flag = 0;
					}
					else if (music_flag == 0)
					{
						mciSendString("play ./bgm.mp3 repeat", NULL, 0, NULL);//������������
						music_flag = 1;
					}
					break;
				}
			}
			if (press_button(msg, x1, y1, R))  //target
			{
				//printf("button!\n");
				//code here ������ʾ��ʽ

				ExMessage msg1;
				while (true)
				{
					msg1 = getmessage(EM_MOUSE | EM_KEY); //��ȡ���̻������Ϣ
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
					msg1 = getmessage(EM_MOUSE | EM_KEY); //��ȡ���̻������Ϣ
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
					msg1 = getmessage(EM_MOUSE | EM_KEY); //��ȡ���̻������Ϣ
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
				if (strategy == 0) {//һ���� 
					if (Check1() == 1)
					{
						if (bus.loc == (station - 1) * Distance && bus.state == 0)
						{
							Draw_out1();
							continue;
						}
						insertEnd(); //�������֮ǰ���ظ����Ͳ���
						int p = station - 1;
						if (retype == 0) {
							stationState[p].target = 1;
							//printf("dddxxxxxx\n");
						}
						else if (retype == 1)
							stationState[p].clockwise = 1;
						else if (retype == 2)
							stationState[p].counterclockwise = 1; //���Ҹ������͸���վ����Ϣ		
					}//ָ��
					if (bus.state == 0) GetGoal1();
					//printf("%d\n", bus.state);
					cleardevice();
					Draw_out1();
				}
				else if (strategy == 1 || strategy == 2)
				{	// ����  ��ֵ���
					num++;
					if (bus.state == 0 && bus.loc == (station - 1) * Distance)
					{
						cleardevice();
						Draw_out1();
						continue;
					}
					//�ж��Ƿ��� 
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
						if (Distance1 < Distance2) //�Ƚ����ֲ�ͬ��ʽ����Ŀ������ľ����С
						{
							if (bus.loc <= (goalstation - 1) * Distance) //�жϸ÷�ʽ��˳��
								bus.direct = 0;
							else
								bus.direct = 1;
						}
						else if (Distance1 > Distance2)
						{
							if (bus.loc <= (goalstation - 1) * Distance) //�жϸ÷�ʽ��˳��
								bus.direct = 1;
							else
								bus.direct = 0;
						}
						else //���Distance1=Distance2 ѡ��˳ʱ�뷽��
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
						stationState[p].counterclockwise = 1; //���Ҹ������͸���վ����Ϣ
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