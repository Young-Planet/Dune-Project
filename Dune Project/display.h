/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112

// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
// display.h�� �߰�
#define COLOR_PLAYER_UNIT 9   // �÷��̾�(��Ʈ���̵�) ����: �Ķ���
#define COLOR_AI_UNIT 12      // AI(���ڳ�) ����: ������
#define COLOR_SAND_WORM 6          // ��: Ȳ���
#define COLOR_SPICE 14        // �����̽�: ��Ȳ��
#define COLOR_PLATE 8         // ����: ������
#define COLOR_ROCK 7          // ��Ÿ ����(ȸ��)

void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

void display_map(void);
void display_status(CURSOR cursor);
void display_system_message(void);
void display_commands(void);


#endif

