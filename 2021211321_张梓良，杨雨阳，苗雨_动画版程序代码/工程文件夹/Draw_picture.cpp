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
extern struct SS stationState[10]; //����Ϣ���У��ṹ��000
extern struct initList* initHead;				  //ԭʼ״̬�������ͷָ�� 
void button(int x, int y, int r, char ch)
{
	setbkmode(0); //���ñ���Ϊ͸��
	//���ð�ť�߿�
	circle(x, y, r); //���ư�ť
	settextstyle(30, 20, "����"); //��������
	outtextxy(x - 10, y - 15, ch); //ch��ΪT��C��U��E
}
void draw_in(int y1, int x1, int R, int L, int y2, int y3, int x2, int r, int l)
{
	///setlinestyle(PS_DASH); //�������߿�
	//rectangle(800, 350, 1180, 680); //�������߿�
	//setlinestyle(PS_SOLID, 5);
	setlinecolor(BLACK);//�����߿���ɫΪ��ɫ
	setlinestyle(PS_DASH); //�������߿�
	rectangle(800, 350, 1180, 680); //�������߿�
	setlinestyle(PS_SOLID, 5);
	settextcolor(BLACK);//���ð�ť������ɫ
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
	button(x1 + 3 * L, y1, R, 'E');//���Ʋ�ָͬ��İ�ť.
	setlinestyle(PS_SOLID, 2);
	for (int i = 0; i < 4; i++)
	{
		button(x2 + i * l, y2, r, '1' + i);
		button(x2 + i * l, y3, r, '6' + i);
	}
	button(x2 + 4 * l, y2, r, '5');//����5վ�㰴ť

	circle(x2 + 4 * l, y3, r); //���ư�ť
	settextstyle(30, 20, "����"); //��������
	outtextxy(x2 + 4 * l - 18, y3 - 16, "10");
	//��������10վ�㰴ť
	setlinecolor(WHITE); //��ԭ�߿���ɫ
	settextcolor(WHITE); //��ԭ������ɫ
}
void draw_out()
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
void Draw()
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
	//����һ�Ѳ���
	BeginBatchDraw();
	setbkcolor(WHITE);
	cleardevice();
	while (End != 1)
	{
		//int _order = 0;// 1Ϊtargetָ�2Ϊclockwiseָ�3Ϊcounterclockwiseָ��
		//int _station = -1;//վ������
		ExMessage msg; //���������Ϣ�ṹ�� //˫�����ͼ���滭���ݷ�������֮�䡣 �ܷ�ֹ��������
			// ���ñ��� code here
		draw_in(y1, x1, R, L, y2, y3, x2, r, l);
		draw_out();
		outtextxy(860, y_tip, "�Ȱ��ϱߵ�ָ�ť");//��ʾ
		outtextxy(860, y_tip + 50, "�ٰ��±ߵ�վ�㰴ť");//��ʾ
	//	printf("ssssss\n");
	//	printf("ooooooo\n");
		setlinecolor(RGB(39, 255, 230));
		setlinestyle(PS_SOLID, 8);
		settextcolor(RGB(255, 0, 0));
		settextstyle(15, 12, "����");
		setbkmode(TRANSPARENT);
		circle(360, 360, 280);//����Բ�ι��
		int num;
		num = totalStation * Distance;//����������
		for (int i = 0; i < num; i++)
		{
			double x, y;
			double angle = i * 1.0 / num * 2 * PI;//��������ƫת�Ƕ�
			char s[3];
			s[0] = '0' + (i / Distance + 1) / 10;
			s[1] = '0' + (i / Distance + 1) % 10;
			s[2] = '\0';
			x = 360 - 280 * cos(angle);//��ĺ�����
			y = 360 - 280 * sin(angle);//��������ꣻ
			if (i % Distance == 0)//�ж��Ƿ�Ϊվ�㣬����ʹ�ò�ͬ����ɫ����ͬ�Ĵ�С
			{
				if (i == bus.loc || (i == 0 && bus.loc == num))//�жϹ������Ƿ񵽴�õ㣬������˸�õ�
					setfillcolor(RGB(255, 145, 203));
				else
					setfillcolor(LIGHTGREEN);
				solidcircle(x, y, 16);
				outtextxy(x - 11, y - 7, s);
			}
			else
			{
				if (i == bus.loc || (i == 0 && bus.loc == num))//�жϹ������Ƿ񵽴�õ㣬������˸�õ�
					setfillcolor(RGB(255, 145, 203));
				else
					setfillcolor(RGB(0, 182, 255));
				solidcircle(x, y, 8);
			}
		}
		IMAGE img1, img2;//ԭͼƬ  ��ת���ͼƬ
		int num1 = totalStation * Distance;//����������
		double rotate_angle = -(2 * PI / num1) * bus.loc;//����ͼƬ��ת�Ƕ�
		double angle = bus.loc * 1.0 / num1 * 2 * PI;//��������ƫת�Ƕ�
		int x, y;//������������
		x = 360 - 315 * cos(angle) - 22.5;//�������ĺ�����
		y = 360 - 315 * sin(angle) - 14;//�������������ꣻ
		loadimage(&img1, "./bus1.jpg");
		loadimage(&img2, "./bus2.jpg");
		putimage(x, y, &img2, SRCAND);
		putimage(x, y, &img1, SRCPAINT);
		IMAGE img;
		loadimage(&img, "./bupt.jpg");
		putimage(211, 210, &img, SRCCOPY);
		IMAGE button;
		loadimage(&button, "./button.png");
		putimage(75, 20, &button, SRCCOPY);//ͼƬ 60*54
		setbkmode(TRANSPARENT);
		settextstyle(20, 0, "����");
		settextcolor(BLACK);
		char text[] = "����Բ���/�ر�����";
		int tx = 75 - (textwidth(text) - 60) / 2;
		int ty = 80;
		outtextxy(tx, ty, text);
		FlushBatchDraw();
		//	printf("ttttttt\n");
	}

}


