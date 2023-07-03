#pragma once
#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED
#include <easyx.h>
    void setParam();
    void init();
    struct initList* inintEmptyList();
	void input();
	bool press_button(ExMessage msg, int x, int y, int r); 	
	void insertEnd();
	struct initList* getEnd();
	int Check1();
	void Draw_out1();
	void GetGoal1();
#endif