/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»�� ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
// �� ������ �»�� ��ǥ
const POSITION status_pos = { 1, MAP_WIDTH + 5 };
const POSITION system_message_pos = { MAP_HEIGHT + 2, 0 };
const POSITION command_window_pos = { MAP_HEIGHT + 2, MAP_WIDTH + 5 };

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
extern char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH];
void display_cursor(CURSOR cursor);

void display(
	RESOURCE resource,
	CURSOR cursor)
{
	display_resource(resource);      // �ڿ� ����
	display_map();                   // map�� ���� ����
	display_cursor(cursor);          // Ŀ��
	display_status(cursor);          // cursor �μ� ����
	display_system_message();        // �ý��� �޽���
	display_commands();              // ���â
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(void) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				char ch = backbuf[i][j];
				int color = COLOR_DEFAULT;

				// �� ��� ���� ����
				switch (ch) {
				case BASE:
					if ((i <= 2 && j >= MAP_WIDTH - 3)) {
						color = COLOR_AI_UNIT;
					}
					else {
						color = COLOR_PLAYER_UNIT;
					}
					break;
				case HARVESTER:
					if ((i == 3 && j >= MAP_WIDTH - 3) || (i == 3 && j >= MAP_WIDTH - 3)) {
						color = COLOR_AI_UNIT;
					}
					else {
						color = COLOR_PLAYER_UNIT;
					}
					break;
				case SAND_WORM: color = COLOR_SAND_WORM; break;
				case SPICE: color = COLOR_SPICE; break;
				case PLATE: color = COLOR_PLATE; break;
				case ROCK: color = COLOR_ROCK; break;
				default: color = COLOR_DEFAULT; break;
				}

				printc(padd(map_pos, pos), ch, color);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// prev ��ġ ��ȿ�� �˻�
	if (prev.row >= 0 && prev.row < MAP_HEIGHT && prev.column >= 0 && prev.column < MAP_WIDTH) {
		char ch = frontbuf[prev.row][prev.column];
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT);
	}

	// curr ��ġ ��ȿ�� �˻�
	if (curr.row >= 0 && curr.row < MAP_HEIGHT && curr.column >= 0 && curr.column < MAP_WIDTH) {
		char ch = frontbuf[curr.row][curr.column];
		printc(padd(map_pos, curr), ch, COLOR_CURSOR);
	}
}

void display_status(CURSOR cursor) {
	// ����â ���� ���
	gotoxy(status_pos);
	printf("����â");

	// ����â ���� ��ġ�� �̵�
	POSITION status_content_pos = { status_pos.row + 1, status_pos.column };
	gotoxy(status_content_pos);

	if (cursor.current.row >= 0 && cursor.current.row < MAP_HEIGHT &&
		cursor.current.column >= 0 && cursor.current.column < MAP_WIDTH) {

		if (cursor.isSelected) {
			char current_obj = map[0][cursor.current.row][cursor.current.column];


			if (current_obj == ' ') {
				printf("������ ������Ʈ: �縷 ����");
			}
			else {
				printf("������ ������Ʈ: %c", current_obj);
			}
		}
		else {
			printf("���õ� ������Ʈ�� �����ϴ�.");
		}
	}
	else {
		printf("��ȿ���� ���� Ŀ�� ��ġ");
	}
}

void display_system_message(void) {
	gotoxy(system_message_pos);
	printf("�ý��� �޽���");
	// ���� �ý��� �޽��� �߰� �ڵ� �ۼ��ϸ� ��.
}

void display_commands(void) {
	gotoxy(command_window_pos);
	printf("���â");
	// ���� ��ɾ� ���� �߰� �ڵ� �ۼ��ϸ� ��.
}
