/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
// 각 영역의 좌상단 좌표
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
	display_resource(resource);      // 자원 상태
	display_map();                   // map은 전역 참조
	display_cursor(cursor);          // 커서
	display_status(cursor);          // cursor 인수 전달
	display_system_message();        // 시스템 메시지
	display_commands();              // 명령창
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

				// 각 요소 색상 설정
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

	// prev 위치 유효성 검사
	if (prev.row >= 0 && prev.row < MAP_HEIGHT && prev.column >= 0 && prev.column < MAP_WIDTH) {
		char ch = frontbuf[prev.row][prev.column];
		printc(padd(map_pos, prev), ch, COLOR_DEFAULT);
	}

	// curr 위치 유효성 검사
	if (curr.row >= 0 && curr.row < MAP_HEIGHT && curr.column >= 0 && curr.column < MAP_WIDTH) {
		char ch = frontbuf[curr.row][curr.column];
		printc(padd(map_pos, curr), ch, COLOR_CURSOR);
	}
}

void display_status(CURSOR cursor) {
	// 상태창 제목 출력
	gotoxy(status_pos);
	printf("상태창");

	// 상태창 내용 위치로 이동
	POSITION status_content_pos = { status_pos.row + 1, status_pos.column };
	gotoxy(status_content_pos);

	if (cursor.current.row >= 0 && cursor.current.row < MAP_HEIGHT &&
		cursor.current.column >= 0 && cursor.current.column < MAP_WIDTH) {

		if (cursor.isSelected) {
			char current_obj = map[0][cursor.current.row][cursor.current.column];


			if (current_obj == ' ') {
				printf("선택한 오브젝트: 사막 지형");
			}
			else {
				printf("선택한 오브젝트: %c", current_obj);
			}
		}
		else {
			printf("선택된 오브젝트가 없습니다.");
		}
	}
	else {
		printf("유효하지 않은 커서 위치");
	}
}

void display_system_message(void) {
	gotoxy(system_message_pos);
	printf("시스템 메시지");
	// 실제 시스템 메시지 추가 코드 작성하면 됨.
}

void display_commands(void) {
	gotoxy(command_window_pos);
	printf("명령창");
	// 실제 명령어 정보 추가 코드 작성하면 됨.
}
