//�������丮 ���� Ȯ�ο� �ּ�

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { {1, 1}, {1, 1}, false };
int last_key_time[4] = { 0 };                // ����Ű ����Ŭ���� ���� ���� ����

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.move_period = 300,
	.next_move_time = 300
};

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	while (1) {
		// loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ��
		KEY key = get_key();

		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// ����Ű ���� �Է�
			switch (key) {
			case k_quit: outro();
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// ���� ������Ʈ ����
		sample_obj_move();

		// ȭ�� ���
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DDD    U   U   N   N  EEEEE\n");
	printf("D  D   U   U   NN  N  E\n");
	printf("D   D  U   U   N N N  EEEEE\n");
	printf("D  D   U   U   N  NN  E\n");
	printf("DDD     UUU    N   N  EEEEE\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0�� ���� ����
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}

		// ��Ʈ���̵� - �÷��̾�(���� �ϴ�)
		map[0][MAP_HEIGHT - 3][1] = BASE;
		map[0][MAP_HEIGHT - 3][2] = BASE;
		map[0][MAP_HEIGHT - 2][1] = BASE;
		map[0][MAP_HEIGHT - 2][2] = BASE;
		map[0][MAP_HEIGHT - 4][1] = HARVESTER;
		map[0][MAP_HEIGHT - 6][1] = SPICE;
		map[0][MAP_HEIGHT - 3][3] = PLATE;
		map[0][MAP_HEIGHT - 3][4] = PLATE;
		map[0][MAP_HEIGHT - 2][3] = PLATE;
		map[0][MAP_HEIGHT - 2][4] = PLATE;

		// ���ڳ�(AI)- ���� ���
		map[0][1][MAP_WIDTH - 3] = BASE;
		map[0][1][MAP_WIDTH - 2] = BASE;
		map[0][2][MAP_WIDTH - 3] = BASE;
		map[0][2][MAP_WIDTH - 2] = BASE;
		map[0][3][MAP_WIDTH - 2] = HARVESTER;
		map[0][5][MAP_WIDTH - 2] = SPICE;
		map[0][1][MAP_WIDTH - 5] = PLATE;
		map[0][1][MAP_WIDTH - 4] = PLATE;
		map[0][2][MAP_WIDTH - 5] = PLATE;
		map[0][2][MAP_WIDTH - 4] = PLATE;

		// �߰� ���� - ����� �� ����
		map[1][6][10] = SAND_WORM;
		map[1][8][13] = SAND_WORM;

		// 2x2 ����
		map[0][5][24] = ROCK;
		map[0][5][25] = ROCK;
		map[0][6][24] = ROCK;
		map[0][6][25] = ROCK;

		map[0][10][32] = ROCK;
		map[0][10][33] = ROCK;
		map[0][11][32] = ROCK;
		map[0][11][33] = ROCK;

		// ���� 1X1
		map[0][12][10] = ROCK;
		map[0][4][47] = ROCK;
		map[0][11][49] = ROCK;

	}

	// layer 1(map[1])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

// ��ȿ ��ġ Ȯ�� �Լ�
bool is_position_valid(POSITION pos) {
	return pos.row >= 0 && pos.row < MAP_HEIGHT &&
		pos.column >= 0 && pos.column < MAP_WIDTH;
}

// (�����ϴٸ�) ������ �������� Ŀ�� �̵�
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(cursor.current, dir);  // �� ��ġ ���

	// ����Ű ����Ŭ�� �� �Ÿ� ����(500ms �̳��� �� �� ������ �� ĭ �� �̵�)
	int current_time = sys_clock;
	if (current_time - last_key_time[dir] < 500) {
		new_pos = pmove(new_pos, dir);
	}
	last_key_time[dir] = current_time;

	// ��ȿ�� ��ġ���� �˻�
	if (is_position_valid(new_pos)) {
		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
	else {
		// ��ȿX -> Ŀ���� �ʱ� ��ġ
		cursor.current = curr;  // ���� ��ġ ����
	}
}

void game_loop(void) {
	while (1) {
		KEY key = get_key();

		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else if (key == KEY_SELECT) {  // �����̽��ٷ� ������Ʈ ����
			cursor.isSelected = true;
			display_status(cursor);
		}
		else if (key == KEY_CANCEL) {  // ESC�� ���� ���
			cursor.isSelected = false;
			display_cursor(cursor);
			display_status(cursor);
			Sleep(TICK);
			sys_clock += TICK;
		}

		// ���� ���� ���¸� �������� �������� �Ʒ� �ڵ� ���� ����
		cursor.isSelected = false;
		display_status(cursor);
	}
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright�� ������ ����
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // ���ڸ�
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		//
		return;
	}

	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}
