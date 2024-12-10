//리포지토리 연결 확인용 주석

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
int last_key_time[4] = { 0 };                // 방향키 더블클릭을 위한 상태 추적

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
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
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
	// layer 0에 지형 생성
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

		// 아트레이디스 - 플레이어(좌측 하단)
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

		// 하코넨(AI)- 우측 상단
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

		// 중간 지역 - 샌드웜 및 바위
		map[1][6][10] = SAND_WORM;
		map[1][8][13] = SAND_WORM;

		// 2x2 바위
		map[0][5][24] = ROCK;
		map[0][5][25] = ROCK;
		map[0][6][24] = ROCK;
		map[0][6][25] = ROCK;

		map[0][10][32] = ROCK;
		map[0][10][33] = ROCK;
		map[0][11][32] = ROCK;
		map[0][11][33] = ROCK;

		// 바위 1X1
		map[0][12][10] = ROCK;
		map[0][4][47] = ROCK;
		map[0][11][49] = ROCK;

	}

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

// 유효 위치 확인 함수
bool is_position_valid(POSITION pos) {
	return pos.row >= 0 && pos.row < MAP_HEIGHT &&
		pos.column >= 0 && pos.column < MAP_WIDTH;
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(cursor.current, dir);  // 새 위치 계산

	// 방향키 더블클릭 시 거리 조정(500ms 이내에 두 번 누르면 한 칸 더 이동)
	int current_time = sys_clock;
	if (current_time - last_key_time[dir] < 500) {
		new_pos = pmove(new_pos, dir);
	}
	last_key_time[dir] = current_time;

	// 유효한 위치인지 검사
	if (is_position_valid(new_pos)) {
		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
	else {
		// 유효X -> 커서를 초기 위치
		cursor.current = curr;  // 현재 위치 유지
	}
}

void game_loop(void) {
	while (1) {
		KEY key = get_key();

		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else if (key == KEY_SELECT) {  // 스페이스바로 오브젝트 선택
			cursor.isSelected = true;
			display_status(cursor);
		}
		else if (key == KEY_CANCEL) {  // ESC로 선택 취소
			cursor.isSelected = false;
			display_cursor(cursor);
			display_status(cursor);
			Sleep(TICK);
			sys_clock += TICK;
		}

		// 선택 해제 상태를 유지하지 않으려면 아래 코드 삭제 가능
		cursor.isSelected = false;
		display_status(cursor);
	}
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		//
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}
