#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>

#define FRAME_X 5
#define FRAME_Y 5
#define WIDTH 20
#define HEIGHT 20

const int SNAKE_INIT_LENGTH = 3;
const int SNAKE_INIT_SPEED = 400;
const int SNAKE_SPEED_INCREMENT = 50;
const int SNAKE_SPEED_INTERVAL = 3;

int i, j, a[2];

// 定义颜色
enum Color {
	COLOR_DEFAULT = 7,
	COLOR_COVER_TEXT = 14,
	COLOR_BORDER = 15,
	COLOR_SNAKE_HEAD = 10,
	COLOR_SNAKE_BODY = 2,
	COLOR_FOOD = 12,
	COLOR_INFO_TEXT = 11
};

// 设置控制台文字颜色
void setConsoleColor(HANDLE hout, int color) {
	SetConsoleTextAttribute(hout, color);
}

// 将光标移动到指定位置
void gotoxy(HANDLE hout, int x, int y) {
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hout, pos);
}

// 游戏封面
void cover(HANDLE hout) {
	setConsoleColor(hout, COLOR_COVER_TEXT);
	gotoxy(hout, FRAME_X + WIDTH, FRAME_Y);
	printf("欢迎使用贪吃蛇游戏1.9");
	gotoxy(hout, FRAME_X + WIDTH, FRAME_Y + 5);
	printf("开始游戏前请关闭中文输入法");
	gotoxy(hout, FRAME_X + WIDTH * 2, FRAME_Y + 20);
	printf("游戏制作者：MA");
	gotoxy(hout, FRAME_X + WIDTH * 2, FRAME_Y + 22);
	printf("制作时间：2023年6月");
	
	// ASCII Art for Snake
	gotoxy(hout, FRAME_X + WIDTH, FRAME_Y + 10);
	printf("        __\n");
	gotoxy(hout, FRAME_X + WIDTH, FRAME_Y + 11);
	printf("       /  \\\n");
	gotoxy(hout, FRAME_X + WIDTH, FRAME_Y + 12);
	printf("      /    \\\n");
	gotoxy(hout, FRAME_X + WIDTH, FRAME_Y + 13);
	printf("     /      \\\n");
	gotoxy(hout, FRAME_X + WIDTH, FRAME_Y + 14);
	printf("    /________\\\n");
	
	getchar();
	setConsoleColor(hout, COLOR_DEFAULT);
	system("cls");
}

// 定义蛇的结构体
struct Snake {
	int x[100];
	int y[100];
	int speed;
	int length;
	int count;
};

// 定义食物的结构体
struct Food {
	int x;
	int y;
};

// 制作框架
void makeframe(struct Snake snake) {
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	setConsoleColor(hout, COLOR_BORDER);
	gotoxy(hout, FRAME_X + WIDTH * 2 + 5, FRAME_Y);
	printf("   贪吃蛇游戏");
	gotoxy(hout, FRAME_X + WIDTH * 2 + 5, FRAME_Y + 3);
	printf("使用方向键或wasd移动");
	gotoxy(hout, FRAME_X + WIDTH * 2 + 5, FRAME_Y + 5);
	printf("长按方向键可加速");
	gotoxy(hout, FRAME_X + WIDTH * 2 + 5, FRAME_Y + 7);
	printf("按任意键暂停,方向键继续");
	
	for (i = 0; i < WIDTH * 2 + 1; i++) {
		gotoxy(hout, FRAME_X + i, FRAME_Y);
		printf("*");
	}
	
	for (i = 0; i < WIDTH * 2 + 2; i++) {
		gotoxy(hout, FRAME_X + i, FRAME_Y + HEIGHT);
		printf("*");
	}
	
	for (i = 0; i < HEIGHT; i++) {
		gotoxy(hout, FRAME_X, FRAME_Y + i);
		printf("*");
	}
	
	for (i = 0; i < HEIGHT; i++) {
		gotoxy(hout, FRAME_X + WIDTH * 2 + 1, FRAME_Y + i);
		printf("*");
	}
	setConsoleColor(hout, COLOR_DEFAULT);
}

// 游戏信息
void infor(HANDLE hout, struct Snake* snake) {
	setConsoleColor(hout, COLOR_INFO_TEXT);
	gotoxy(hout, FRAME_X + WIDTH * 2 + 5, FRAME_Y + 15);
	printf("当前速度: %d", -snake->speed + 500);
	gotoxy(hout, FRAME_X + WIDTH * 2 + 5, FRAME_Y + 17);
	printf("当前得分: %d", snake->count);
	gotoxy(hout, FRAME_X + WIDTH * 2 + 5, FRAME_Y + 19);
	printf("当前长度: %d", snake->length);
	setConsoleColor(hout, COLOR_DEFAULT);
}

// 初始化蛇
void initsnake(struct Snake* snake) {
	snake->x[0] = FRAME_X + 2;
	snake->y[0] = FRAME_Y + HEIGHT / 2;
	snake->count = 0;
	snake->length = SNAKE_INIT_LENGTH;
	snake->speed = SNAKE_INIT_SPEED;
	for (i = 1; i < snake->length; i++) {
		snake->x[i] = snake->x[i - 1] + 1;
		snake->y[i] = snake->y[i - 1];
	}
}

// 打印蛇
void printsnake(HANDLE hout, struct Snake* snake) {
	for (i = 0; i < snake->length; i++) {
		gotoxy(hout, snake->x[i], snake->y[i]);
		if (i == snake->length - 1) {
			setConsoleColor(hout, COLOR_SNAKE_HEAD);
			printf("O"); // 蛇头
		} else if (i == 0) {
			setConsoleColor(hout, COLOR_SNAKE_BODY);
			printf("o"); // 蛇尾
		} else {
			setConsoleColor(hout, COLOR_SNAKE_BODY);
			printf("*"); // 蛇身
		}
	}
	setConsoleColor(hout, COLOR_DEFAULT);
}

// 移动蛇
void movesnake(HANDLE hout, struct Snake* snake) {
	gotoxy(hout, snake->x[0], snake->y[0]);
	printf(" ");
	for (i = 1; i < snake->length; i++) {
		snake->x[i - 1] = snake->x[i];
		snake->y[i - 1] = snake->y[i];
	}
}

// 打印食物
void printfood(HANDLE hout, struct Snake* snake, struct Food* food) {
	srand((unsigned)time(NULL));
	while (1) {
		food->x = rand() % (WIDTH - 2) + 1;
		food->y = rand() % (HEIGHT - 2) + 1;
		if (food->x == 0 && food->y == 0)
			continue;
		food->x = 2 * food->x + FRAME_X;
		food->y += FRAME_Y;
		for (i = 0; i < snake->length; i++) {
			if (food->x == snake->x[i] && food->y == snake->y[i])
				break;
		}
		if (i == snake->length) {
			setConsoleColor(hout, COLOR_FOOD);
			gotoxy(hout, food->x, food->y);
			printf("X"); // 食物
			setConsoleColor(hout, COLOR_DEFAULT);
			break;
		}
	}
}

// 吃食物
void eatfood(HANDLE hout, struct Snake* snake, struct Food* food) {
	if (snake->x[snake->length - 1] == food->x && snake->y[snake->length - 1] == food->y) {
		snake->length++;
		for (i = snake->length - 1; i > 0; i--) {
			snake->x[i] = snake->x[i - 1];
			snake->y[i] = snake->y[i - 1];
		}
		snake->x[0] = a[0];
		snake->y[0] = a[1];
		printfood(hout, snake, food);
		snake->count++;
		if (snake->count % SNAKE_SPEED_INTERVAL == 0)
			snake->speed -= SNAKE_SPEED_INCREMENT;
	}
}

// 死亡判断
int ifdead(struct Snake* snake) {
	if (snake->x[snake->length - 1] == FRAME_X || 
		snake->x[snake->length - 1] == FRAME_X + WIDTH * 2 ||
		snake->y[snake->length - 1] == FRAME_Y ||
		snake->y[snake->length - 1] == FRAME_Y + HEIGHT)
		return 0;
	for (i = 0; i < snake->length - 1; i++)
		if (snake->x[snake->length - 1] == snake->x[i] && snake->y[snake->length - 1] == snake->y[i])
			return 0;
	return 1;
}

// 开始游戏
int main() {
	unsigned char ch = 77;
	bool gameOver = false;
	bool accelerated = false;
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	cover(hout);
	struct Snake s, *snake = &s;
	struct Food f, *food = &f;
	makeframe(*snake);
	initsnake(snake);
	printfood(hout, snake, food);
	Sleep(500);
	
	while (!gameOver) {
		infor(hout, snake);
		a[0] = snake->x[0];
		a[1] = snake->y[0];
		accelerated = false;
		if (kbhit()) {
			ch = getch();
			if (kbhit()) {
				Sleep(20);
				accelerated = true;
			}
		}
		switch (ch) {
		case 'W':
		case 'w':
		case 72:
			movesnake(hout, snake);
			snake->y[snake->length - 1] -= 1;
			break;
		case 'S':
		case 's':
		case 80:
			movesnake(hout, snake);
			snake->y[snake->length - 1] += 1;
			break;
		case 'A':
		case 'a':
		case 75:
			movesnake(hout, snake);
			snake->x[snake->length - 1] -= 2;
			break;
		case 'D':
		case 'd':
		case 77:
			movesnake(hout, snake);
			snake->x[snake->length - 1] += 2;
			break;
		case 27:
			gameOver = true;
			break;
		}
		
		eatfood(hout, snake, food);
		printsnake(hout, snake);
		if (ifdead(snake) == 0 || snake->speed == 0) {
			gameOver = true;
		}
		
		if (!accelerated)
			Sleep(snake->speed);
		else
			Sleep(20);
	}
	
	setConsoleColor(hout, COLOR_INFO_TEXT);
	gotoxy(hout, FRAME_X + WIDTH / 2, FRAME_Y - 2);
	if (snake->speed == 0)
		printf("恭喜你通关了！！!");
	else
		printf("糟糕 T_T");
	setConsoleColor(hout, COLOR_DEFAULT);
	
	Sleep(3000);
	system("cls");
	if (snake->speed == 0) {
		setConsoleColor(hout, COLOR_INFO_TEXT);
		printf("\n\n\n\n\n\t\t\t哈哈，你赢了\n\n\t\t\t你得到了满分：24\n\n\n");
	} else {
		setConsoleColor(hout, COLOR_INFO_TEXT);
		printf("\n\n\n\n\n\t\t\t哈哈，你输了\n\n\t\t\t你的最终得分是：%d\n\n\n", snake->count);
	}
	setConsoleColor(hout, COLOR_DEFAULT);
	Sleep(3000);
	return 0;
}
