#include "variable.h"
#include "Done.h"
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
extern struct SS stationState[10]; //����Ϣ���У��ṹ��000
extern struct initList* initHead;				  //ԭʼ״̬�������ͷָ�� 
void getGoal()
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
int isFINISH()
{
	if (strategy == 0) //�����ȷ������迼�Ƿ���
	{
		if ((goalstation - 1) * Distance == bus.loc) //�ж��Ƿ񵽴�Ŀ��վ��
			return 1;
		return 0;
	}
	else if (strategy == 1)
	{
		//printf("yyyyyyyyyy\n");
		if ((goalstation - 1) * Distance == bus.loc) return 1;	//���bus����goalֱ�ӷ���1
		if (bus.loc % Distance == 0) {
			int station = (bus.loc / Distance);
			if (stationState[station].target == 1) return 1;
			if (((stationState[station].clockwise == 1) && (bus.direct == 0)) || ((stationState[station].counterclockwise == 1) && (bus.direct == 1))) return 1;
		}
		return 0;
		//���bus.loc��Distance�������� ������վ���stationState�������ҷ�����ͬ�ͷ���1
		//������� ����0
	}
	else if (strategy == 2) {
		if ((goalstation - 1) * Distance == bus.loc) return 1;	//���bus����goalֱ�ӷ���1
		if (bus.loc % Distance == 0) {
			int station = (bus.loc / Distance);
			int num = stationState[station].clockwise + stationState[station].target + stationState[station].counterclockwise;
			if (num != 0) return 1;
		}
		return 0;
	}
}
void finishGoal()
{
	if (strategy == 0) // FCFS
	{
		struct initList* temp;
		temp = initHead->next;
		initHead->next = temp->next; //�ı�ͷָ���next��ָ��ʹ��ָ����һ������
		switch (goaltype)			 //�ı�վ������״̬
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
		}
	}
	else if (strategy == 1) // SSTF
	{
		if ((goalstation - 1) * Distance == bus.loc) //Ŀ��վ
		{
			stationState[goalstation - 1].target = 0;
			stationState[goalstation - 1].clockwise = 0;
			stationState[goalstation - 1].counterclockwise = 0; //Ŀ��վ����������� 
			stationState[goalstation - 1].target_num = max;
			stationState[goalstation - 1].clockwise_num = max;
			stationState[goalstation - 1].counterclockwise_num = max;
			pre = goalstation;
			goaltype = -1;
			goalstation = -1; //�ν�getgoal 
		}
		else //˳��վ
		{
			if (bus.direct == 0) //˳ʱ�� 
			{
				stationState[bus.loc / Distance].target = 0;
				stationState[bus.loc / Distance].clockwise = 0;
				stationState[bus.loc / Distance].target_num = max;
				stationState[bus.loc / Distance].clockwise_num = max;
			}
			else if (bus.direct == 1)  //��ʱ��
			{
				stationState[bus.loc / Distance].target = 0;
				stationState[bus.loc / Distance].counterclockwise = 0;
				stationState[bus.loc / Distance].target_num = max;
				stationState[bus.loc / Distance].counterclockwise_num = max;
			}
			bus.state = 1;
		} //˳��վ�ı�վ����Ϣ��С��״̬��Ϊ1
	}
	else if (strategy == 2) // SCAN
	{
		stationState[bus.loc / Distance].target = 0;
		stationState[bus.loc / Distance].clockwise = 0;
		stationState[bus.loc / Distance].counterclockwise = 0;
		stationState[bus.loc / Distance].target_num = max;
		stationState[bus.loc / Distance].clockwise_num = max;
		stationState[bus.loc / Distance].counterclockwise_num = max;
		//����Ŀ��վ����˳��վ������������� 
		if ((goalstation - 1) * Distance == bus.loc) //Ŀ��վ
		{
			goaltype = -1;
			goalstation = -1; //�ν�getgoal 
		}
		else //˳��վ
			bus.state = 1;
	}//˳��վ�ı�վ����Ϣ��С��״̬��Ϊ1
}