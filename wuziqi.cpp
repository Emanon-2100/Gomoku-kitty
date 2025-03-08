#include<stdio.h>
#include<easyx.h>// 图形界面
#include<graphics.h>
#include<stdio.h>
#include<string.h>
#include<time.h>    //用于随机选择
#include<cstring>


#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") // 链接多媒体库




//宏定义 15个像素
#define GRID_W 25      //间隔

//画格子 行和列
#define ROW 15
#define COL 15
#define SPACE (3 * GRID_W) //中间加上空格//上边和左边的间距
#define MAX_HISTORY 225 // 最多存储225步（最大棋盘格数）//用于悔棋
#define BOARD_GRAD_SIZE 15

// 结束页面按钮尺寸
#define END_BUTTON_WIDTH 120
#define END_BUTTON_HEIGHT 30

// 结束页面按钮位置（居中）
#define END_BUTTON_X ((600 - END_BUTTON_WIDTH) / 2) // 假设窗口宽度为600
#define END_BUTTON_Y 350 // 根据需要调整Y轴位置

//小喇叭按钮
// 
// 定义声音按钮的尺寸和位置
#define SOUND_BUTTON_WIDTH 30
#define SOUND_BUTTON_HEIGHT 30
#define SOUND_BUTTON_MARGIN 10  // 离窗口边缘的距离
#define SOUND_BUTTON_X SOUND_BUTTON_MARGIN // 左下角，X坐标为左边缘的距离
#define SOUND_BUTTON_Y (500 - SOUND_BUTTON_HEIGHT - SOUND_BUTTON_MARGIN) // Y坐标保持不变，位于底部



// 重新开始按钮
#define RESTART_BUTTON_WIDTH 80
#define RESTART_BUTTON_HEIGHT 40
#define RESTART_BUTTON_X 500
#define RESTART_BUTTON_Y 100

// 悔棋按钮
#define UNDO_BUTTON_WIDTH 80
#define UNDO_BUTTON_HEIGHT 40
#define UNDO_BUTTON_X 500
#define UNDO_BUTTON_Y 160

// 结束游戏按钮
#define ENDGAME_BUTTON_WIDTH 80
#define ENDGAME_BUTTON_HEIGHT 40
#define ENDGAME_BUTTON_X 500
#define ENDGAME_BUTTON_Y 220


// 存档按钮
#define SAVE_BUTTON_WIDTH 80
#define SAVE_BUTTON_HEIGHT 40
#define SAVE_BUTTON_X 500
#define SAVE_BUTTON_Y 280

// 读档按钮
#define LOAD_BUTTON_WIDTH 80
#define LOAD_BUTTON_HEIGHT 40
#define LOAD_BUTTON_X 500
#define LOAD_BUTTON_Y 340



// 全局变量存储当前鼠标位置
int currentMouseX = -1;
int currentMouseY = -1;


// 添加声音按钮的图片变量
IMAGE soundOnImg;
IMAGE soundOffImg;

// 声音状态变量，默认为开启
bool soundOn = true;


typedef struct {
	int chessMap[BOARD_GRAD_SIZE][BOARD_GRAD_SIZE]; // 当前棋盘状态
	int scoreMap[BOARD_GRAD_SIZE][BOARD_GRAD_SIZE]; // 各个位置的评分
} ChessData;

struct Move {
	int row;
	int col;
	int player;
};

struct Move history[MAX_HISTORY]; // 存储每一步的历史记录

int history_index = 0; // 当前历史记录的索引

bool isAI = false;

//背景图片 
IMAGE bk;		     //背景图片

IMAGE kaishi;

//按钮
IMAGE renji;
IMAGE shuangren;
IMAGE tuichu;

IMAGE jieshu; // 结束页面背景图片
IMAGE anniu;  // 按钮图片

// 按钮背景图片
IMAGE restartButtonImg;
IMAGE undoButtonImg;
IMAGE endgameButtonImg;
IMAGE savegameButtonImg;
IMAGE loadgameButtonImg;


//声明一下判断输赢的函数
bool judge(int r, int c);
//声明悔棋函数
void undoMove();
void startGameTwoPlayer();
void startGameAI();
void mousePressMsg(ExMessage* msg);
void mouseMoveMsg(ExMessage* msg);
void  aiMove();
void aiDecision();
bool isForbiddenMove(int row, int col, int player);
void drawSoundButton(int mouseX, int mouseY);
void drawButtons(int mouseX, int mouseY);

//枚举类
//定义棋子
enum Chess {

	Nome = 0,				//没有棋子
	Black = 1,				//黑棋
	White = -1				//白棋

};

typedef enum {
	STATE_INIT,     // 初始化页面
	STATE_MENU,     // 主菜单
	STATE_PLAYING,  // 游戏进行中
	STATE_END       // 结束页面
} GameState;

GameState currentState = STATE_INIT;



//把棋子装进数组中
int map[ROW][COL];//装的是棋子的坐标




struct Pos
{
	//鼠标的位置
	int row;
	int col;
	bool isShow;       //是否记录蓝色的框
	int player;			//棋手


}pos = { -1,-1,false,Black };  //用于代替

//结束时的弹窗
bool endMessageShown = false;       // 标记是否已经显示过结束弹窗
char endMessage[100] = "";           // 存储结束消息



//用于实现透明化按钮
//用于处理透明背景的图片
// 载入PNG图并去透明部分
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}



void loadResource() {
	//创建游戏界面
	initgraph(600, 500);
	//加载图片
	loadimage(&kaishi, "resource/kaishi.jpg");
	loadimage(&renji, "resource/renji7.PNG");
	loadimage(&shuangren, "resource/shuangren7.PNG");
	loadimage(&tuichu, "resource/tuichu7.PNG");

	loadimage(&bk, "resource/bk.png");



	// 加载结束页面和按钮图片
	loadimage(&jieshu, "resource/jieshu.jpg");
	loadimage(&anniu, "resource/anniu.jpg");


	// 加载声音按钮图片
	loadimage(&soundOnImg, "resource/sound_on.png");
	loadimage(&soundOffImg, "resource/sound_off.png");

	// 加载按钮背景图片
	loadimage(&restartButtonImg, "resource/restart_button.jpg"); // 重开按钮图片
	loadimage(&undoButtonImg, "resource/undo_button.jpg");       // 悔棋按钮图片
	loadimage(&endgameButtonImg, "resource/endgame_button.jpg"); // 结束游戏按钮图片




	//音乐
	//背景音乐，循环播放
	mciSendString("open \"resource/background.mp3\" type mpegvideo alias bgm", NULL, 0, NULL);
	mciSendString("play bgm repeat", NULL, 0, NULL);
}

//单独线程播放音效
// 下棋音效播放线程函数
DWORD WINAPI playMoveSoundThread(LPVOID lpParam) {
	// 打开并播放下棋音效
	mciSendString("open \"resource/move.mp3\" type mpegvideo alias moveSound", NULL, 0, NULL);
	mciSendString("play moveSound from 0", NULL, 0, NULL);

	// 获取下棋音效的时长，以便正确关闭
	char lengthCmd[256];
	char lengthStr[32];

	// 使用 strcpy_s 进行安全的字符串复制
	strcpy_s(lengthCmd, sizeof(lengthCmd), "status moveSound length");
	mciSendString(lengthCmd, lengthStr, sizeof(lengthStr), NULL);
	int length = atoi(lengthStr); // 时长以毫秒为单位

	// 等待音效播放完成
	Sleep(length);

	// 关闭音效
	mciSendString("close moveSound", NULL, 0, NULL);
	return 0;
}

// 播放下棋音效的函数
void playMoveSound() {
	HANDLE hThread = CreateThread(
		NULL,                   // 默认安全属性
		0,                      // 默认堆栈大小
		playMoveSoundThread,    // 线程函数
		NULL,                   // 参数传递给线程函数
		0,                      // 默认创建标志
		NULL                    // 不需要线程标识符
	);

	if (hThread != NULL) {
		CloseHandle(hThread); // 关闭线程句柄，不等待线程结束
	}
}


//绘制初始化页面
// 绘制初始化页面
void drawInitPage() {
	// 绘制初始化页面的背景
	putimage(0, 0, &kaishi);

	// 绘制按钮
	drawAlpha(&shuangren, 20, 80);  // 双人对战按钮
	drawAlpha(&renji, 20, 200);    // 人机对战按钮
	drawAlpha(&tuichu, 20, 320);     // 退出按钮

	// 绘制声音按钮
	drawSoundButton(currentMouseX, currentMouseY);
}


// 绘制游戏进行中的页面
// 绘制游戏进行中的页面
void drawGamePage() {
	// 输出背景图片
	putimage(0, 0, &bk);

	// 绘制棋盘,设置线的颜色
	setlinecolor(BLACK);

	// 线的样式
	setlinestyle(PS_SOLID, 2); // 设置线条样式为实线，宽度为2

	// 15条线
	for (int i = 0; i < 15; i++) {
		// 横线
		line(SPACE, i * GRID_W + SPACE, 14 * GRID_W + SPACE, i * GRID_W + SPACE);

		// 竖线
		line(i * GRID_W + SPACE, SPACE, i * GRID_W + SPACE, 14 * GRID_W + SPACE);
	}
	setlinestyle(PS_SOLID, 5); // 把线加粗

	// 绘制一个矩形
	rectangle(SPACE, SPACE, SPACE + GRID_W * 14, SPACE + GRID_W * 14);

	// 填充一个颜色
	setfillcolor(BLACK);

	// 绘制棋盘的五个黑点
	solidcircle(SPACE + 7 * GRID_W, SPACE + 7 * GRID_W, 5);			// 中心点
	solidcircle(SPACE + 3 * GRID_W, SPACE + 3 * GRID_W, 5);   // 左上
	solidcircle(SPACE + 11 * GRID_W, SPACE + 11 * GRID_W, 5);	// 右下
	solidcircle(SPACE + 3 * GRID_W, SPACE + 11 * GRID_W, 5);    // 左下
	solidcircle(SPACE + 11 * GRID_W, SPACE + 3 * GRID_W, 5);	// 右上

	// 绘制棋子
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			// 黑棋
			if (map[i][k] == Black) {
				setfillcolor(BLACK);
				solidcircle(k * GRID_W + SPACE, i * GRID_W + SPACE, 10);
			}
			// 白棋
			else if (map[i][k] == White) {
				setfillcolor(WHITE);
				solidcircle(k * GRID_W + SPACE, i * GRID_W + SPACE, 10);
			}
		}
	}

	// 绘制蓝色框框
	if (pos.isShow) {
		setlinecolor(BLUE); // 蓝色框
		rectangle(pos.col * GRID_W + SPACE - 12, pos.row * GRID_W + SPACE - 12,
			pos.col * GRID_W + SPACE + 12, pos.row * GRID_W + SPACE + 12);
	}

	// 绘制声音按钮
	drawSoundButton(currentMouseX, currentMouseY);

	// 绘制重新开始、悔棋和结束游戏按钮
	drawButtons(currentMouseX, currentMouseY);
}








// 绘制重新开始、悔棋和结束游戏按钮
void drawButtons(int mouseX, int mouseY) {
	// 设置字体样式
	settextstyle(16, 0, "Arial"); // 字体大小为16，Arial字体
	settextcolor(WHITE); // 文本颜色为白色

	// 重新开始按钮
	bool restartHover = false;
	if (mouseX >= RESTART_BUTTON_X && mouseX <= (RESTART_BUTTON_X + RESTART_BUTTON_WIDTH) &&
		mouseY >= RESTART_BUTTON_Y && mouseY <= (RESTART_BUTTON_Y + RESTART_BUTTON_HEIGHT)) {
		restartHover = true;
	}

	if (restartHover) {
		setlinecolor(RED); // 高亮边框颜色
		setlinestyle(PS_SOLID, 2);
		rectangle(RESTART_BUTTON_X - 2, RESTART_BUTTON_Y - 2,
			RESTART_BUTTON_X + RESTART_BUTTON_WIDTH + 2, RESTART_BUTTON_Y + RESTART_BUTTON_HEIGHT + 2);
	}

	// 绘制重新开始按钮背景图片
	putimage(RESTART_BUTTON_X, RESTART_BUTTON_Y, &restartButtonImg);

	// 绘制“重开”按钮文本
	const char* restartText = "重开";
	int restartTextWidth = textwidth(restartText);
	int restartTextHeight = textheight(restartText);
	int restartTextX = RESTART_BUTTON_X + (RESTART_BUTTON_WIDTH - restartTextWidth) / 2;
	int restartTextY = RESTART_BUTTON_Y + (RESTART_BUTTON_HEIGHT - restartTextHeight) / 2;
	outtextxy(restartTextX, restartTextY, restartText);

	// 悔棋按钮
	bool undoHover = false;
	if (mouseX >= UNDO_BUTTON_X && mouseX <= (UNDO_BUTTON_X + UNDO_BUTTON_WIDTH) &&
		mouseY >= UNDO_BUTTON_Y && mouseY <= (UNDO_BUTTON_Y + UNDO_BUTTON_HEIGHT)) {
		undoHover = true;
	}

	if (undoHover) {
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 2);
		rectangle(UNDO_BUTTON_X - 2, UNDO_BUTTON_Y - 2,
			UNDO_BUTTON_X + UNDO_BUTTON_WIDTH + 2, UNDO_BUTTON_Y + UNDO_BUTTON_HEIGHT + 2);
	}

	// 绘制悔棋按钮背景图片
	putimage(UNDO_BUTTON_X, UNDO_BUTTON_Y, &undoButtonImg);

	// 绘制“悔棋”按钮文本
	const char* undoText = "悔棋";
	int undoTextWidth = textwidth(undoText);
	int undoTextHeight = textheight(undoText);
	int undoTextX = UNDO_BUTTON_X + (UNDO_BUTTON_WIDTH - undoTextWidth) / 2;
	int undoTextY = UNDO_BUTTON_Y + (UNDO_BUTTON_HEIGHT - undoTextHeight) / 2;
	outtextxy(undoTextX, undoTextY, undoText);

	// 结束游戏按钮
	bool endgameHover = false;
	if (mouseX >= ENDGAME_BUTTON_X && mouseX <= (ENDGAME_BUTTON_X + ENDGAME_BUTTON_WIDTH) &&
		mouseY >= ENDGAME_BUTTON_Y && mouseY <= (ENDGAME_BUTTON_Y + ENDGAME_BUTTON_HEIGHT)) {
		endgameHover = true;
	}

	if (endgameHover) {
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 2);
		rectangle(ENDGAME_BUTTON_X - 2, ENDGAME_BUTTON_Y - 2,
			ENDGAME_BUTTON_X + ENDGAME_BUTTON_WIDTH + 2, ENDGAME_BUTTON_Y + ENDGAME_BUTTON_HEIGHT + 2);
	}

	// 绘制结束游戏按钮背景图片
	putimage(ENDGAME_BUTTON_X, ENDGAME_BUTTON_Y, &endgameButtonImg);

	// 绘制“结束”按钮文本
	const char* endgameText = "结束";
	int endgameTextWidth = textwidth(endgameText);
	int endgameTextHeight = textheight(endgameText);
	int endgameTextX = ENDGAME_BUTTON_X + (ENDGAME_BUTTON_WIDTH - endgameTextWidth) / 2;
	int endgameTextY = ENDGAME_BUTTON_Y + (ENDGAME_BUTTON_HEIGHT - endgameTextHeight) / 2;
	outtextxy(endgameTextX, endgameTextY, endgameText);
}





// 绘制结束页面的函数
void drawEndPage() {
	// 清屏
	cleardevice();

	// 绘制结束页面背景
	putimage(0, 0, &jieshu);

	// 绘制返回主菜单按钮
	putimage(END_BUTTON_X, END_BUTTON_Y, &anniu);

	// 添加按钮文本（居中）
	settextcolor(WHITE);
	settextstyle(16, 0, "Arial"); // 调整字体大小以适应按钮高度

	// 获取文本的宽度和高度
	const char* buttonText = "返回主菜单";
	int text_width = textwidth(buttonText);
	int text_height = textheight(buttonText);

	// 计算文本的起始位置以居中显示
	int text_x = END_BUTTON_X + (END_BUTTON_WIDTH - text_width) / 2 - 15;//微调一下，露出星星
	int text_y = END_BUTTON_Y + (END_BUTTON_HEIGHT - text_height) / 2;

	outtextxy(text_x, text_y, buttonText);

	// 绘制声音按钮
	drawSoundButton(currentMouseX, currentMouseY);
}


// 绘制声音按钮并添加悬停效果
void drawSoundButton(int mouseX, int mouseY) {
	bool isHover = false;
	if (mouseX >= SOUND_BUTTON_X && mouseX <= (SOUND_BUTTON_X + SOUND_BUTTON_WIDTH) &&
		mouseY >= SOUND_BUTTON_Y && mouseY <= (SOUND_BUTTON_Y + SOUND_BUTTON_HEIGHT)) {
		isHover = true;
	}

	if (isHover) {
		setlinecolor(RED); // 高亮边框颜色
		setlinestyle(PS_SOLID, 2);
		rectangle(SOUND_BUTTON_X - 2, SOUND_BUTTON_Y - 2,
			SOUND_BUTTON_X + SOUND_BUTTON_WIDTH + 2, SOUND_BUTTON_Y + SOUND_BUTTON_HEIGHT + 2);
	}

	if (soundOn) {
		drawAlpha(&soundOnImg, SOUND_BUTTON_X, SOUND_BUTTON_Y);
	}
	else {
		drawAlpha(&soundOffImg, SOUND_BUTTON_X, SOUND_BUTTON_Y);
	}
}




// 处理结束页面的点击事件
bool handleEndPageClick(ExMessage* msg) {
	if (msg->message == WM_LBUTTONDOWN) {
		// 检查点击是否在按钮区域内
		if (msg->x >= END_BUTTON_X && msg->x <= (END_BUTTON_X + END_BUTTON_WIDTH) &&
			msg->y >= END_BUTTON_Y && msg->y <= (END_BUTTON_Y + END_BUTTON_HEIGHT)) {
			// 返回主菜单
			currentState = STATE_MENU;

			// 重置棋盘和历史记录
			memset(map, 0, sizeof(map));
			history_index = 0;
			pos.player = Black;

			// 重置结束消息状态
			endMessageShown = false;
			endMessage[0] = '\0'; // 清空消息内容

			return true;
		}
	}
	return false;
}

// 处理主菜单的点击事件
void handleMenuClick(ExMessage* msg) {
	if (msg->message == WM_LBUTTONDOWN) {
		// 双人对战按钮区域
		// 根据实际按钮图片的大小调整坐标范围
		// 这里假设按钮图片宽度为180，高度为50
		if (msg->x >= 20 && msg->x <= 20 + 180) {
			if (msg->y >= 80 && msg->y <= 80 + 120) {
				// 进入双人对战模式
				isAI = false;
				currentState = STATE_PLAYING;
				// 重置棋盘和历史记录
				memset(map, 0, sizeof(map));
				history_index = 0;
				pos.player = Black;
				return;
			}
			if (msg->y >= 200 && msg->y <= 200 + 120) {
				// 进入人机对战模式
				isAI = true;
				currentState = STATE_PLAYING;
				// 重置棋盘和历史记录
				memset(map, 0, sizeof(map));
				history_index = 0;
				pos.player = Black;
				return;
			}
			if (msg->y >= 320 && msg->y <= 320 + 120) {
				// 退出游戏
				exit(0);
			}
		}
	}
}




// 开始游戏的函数

// 双人对战
void startGameTwoPlayer() {
	memset(map, 0, sizeof(map));  // 清空棋盘
	pos.player = Black;           // 默认黑棋先行
	history_index = 0;            // 重置历史记录
	currentState = STATE_PLAYING; // 设置当前状态为游戏进行中
}

// 人机对战
void startGameAI() {
	memset(map, 0, sizeof(map));  // 清空棋盘
	pos.player = Black;           // 默认黑棋先行
	history_index = 0;            // 重置历史记录
	currentState = STATE_PLAYING; // 设置当前状态为游戏进行中
}






// 鼠标移动
void mouseMoveMsg(ExMessage* msg) { // 传入消息
	// 更新全局鼠标位置
	currentMouseX = msg->x;
	currentMouseY = msg->y;

	// 鼠标的位置 鼠标的下标
	pos.isShow = false;

	// 遍历每个格子
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			// 格子左上坐标
			int gridx = k * GRID_W + SPACE;
			int gridy = i * GRID_W + SPACE;
			if (abs(msg->x - gridx) <= 12 && abs(msg->y - gridy) <= 12) {
				// 判断：在棋盘里，在鼠标的范围之内
				pos.isShow = true;
				pos.row = i;
				pos.col = k;
			}
		}
	}
}

// 鼠标点击处理
void mousePressMsg(ExMessage* msg) {
	// 判断左键是否点击
	if (msg->message == WM_LBUTTONDOWN) {
		// 检查重新开始按钮区域
		if (msg->x >= RESTART_BUTTON_X && msg->x <= (RESTART_BUTTON_X + RESTART_BUTTON_WIDTH) &&
			msg->y >= RESTART_BUTTON_Y && msg->y <= (RESTART_BUTTON_Y + RESTART_BUTTON_HEIGHT)) {
			// 重新开始游戏
			memset(map, 0, sizeof(map));  // 清空棋盘
			history_index = 0;            // 重置历史记录
			pos.player = Black;           // 默认黑棋先行
			currentState = STATE_PLAYING; // 保持在游戏进行中
		}
		// 检查悔棋按钮区域
		else if (msg->x >= UNDO_BUTTON_X && msg->x <= (UNDO_BUTTON_X + UNDO_BUTTON_WIDTH) &&
			msg->y >= UNDO_BUTTON_Y && msg->y <= (UNDO_BUTTON_Y + UNDO_BUTTON_HEIGHT)) {
			// 调用悔棋功能
			undoMove();
		}
		// 检查结束游戏按钮区域
		else if (msg->x >= ENDGAME_BUTTON_X && msg->x <= (ENDGAME_BUTTON_X + ENDGAME_BUTTON_WIDTH) &&
			msg->y >= ENDGAME_BUTTON_Y && msg->y <= (ENDGAME_BUTTON_Y + ENDGAME_BUTTON_HEIGHT)) {
			// 结束游戏并返回主菜单
			currentState = STATE_MENU;

			// 重置棋盘和历史记录
			memset(map, 0, sizeof(map));
			history_index = 0;
			pos.player = Black;

			// 重置结束消息状态
			endMessageShown = false;
			endMessage[0] = '\0'; // 清空消息内容
		}
		else {
			// 处理下棋事件
			if (pos.isShow && map[pos.row][pos.col] == Nome) { // 确保当前位置有效且没有棋子
				map[pos.row][pos.col] = pos.player; // 在棋盘上落子

				// 保存当前棋步
				if (history_index < MAX_HISTORY) {
					history[history_index].row = pos.row;
					history[history_index].col = pos.col;
					history[history_index].player = pos.player;
					history_index++; // 增加历史索引
				}

				// 播放下棋音效
				playMoveSound();

				// 判断是否有玩家获胜或形成禁手
				bool hasFive = judge(pos.row, pos.col);
				bool isForbidden = false;
				if (pos.player == Black) {
					isForbidden = isForbiddenMove(pos.row, pos.col, Black);
				}

				if (hasFive && !isForbidden) {
					if (pos.player == Black) {
						strcpy_s(endMessage, sizeof(endMessage), "黑棋赢了！");
					}
					else {
						strcpy_s(endMessage, sizeof(endMessage), "白棋赢了！");
					}
					currentState = STATE_END;
				}
				else if (isForbidden && !hasFive) {
					strcpy_s(endMessage, sizeof(endMessage), "黑棋形成禁手，黑棋负！");
					currentState = STATE_END;
				}
				else if (hasFive && isForbidden) {
					strcpy_s(endMessage, sizeof(endMessage), "你赢了！五连优先！");
					currentState = STATE_END;
				}

				// 切换玩家
				pos.player = (pos.player == Black ? White : Black);

				// 如果是人机对战且轮到AI下棋，调用AI决策
				if (isAI && currentState == STATE_PLAYING && pos.player == White) {
					aiDecision();
				}
			}
		}
	}
}




//评分函数
// 计算评分函数
void calculateScore(ChessData* data)
{
	if (!data) return;

	// 统计玩家或者电脑连成的子
	int personNum = 0; // 玩家连成子的个数
	int botNum = 0;     // AI连成子的个数
	int emptyNum = 0;   // 各方向空白位的个数

	// 清空评分数组
	memset(data->scoreMap, 0, sizeof(data->scoreMap));

	for (int row = 0; row < BOARD_GRAD_SIZE; row++) {
		for (int col = 0; col < BOARD_GRAD_SIZE; col++) {
			// 只对空白点计算
			if (data->chessMap[row][col] == Nome) {
				// 定义四个方向：水平、垂直、主对角线、副对角线
				int directs[4][2] = {
					{1, 0},  // 水平
					{1, 1},  // 主对角线
					{0, 1},  // 垂直
					{-1, 1}  // 副对角线
				};

				for (int k = 0; k < 4; k++) {
					int x = directs[k][0];
					int y = directs[k][1];

					// 重置
					personNum = 0;
					botNum = 0;
					emptyNum = 0;

					// 对黑棋评分（玩家）
					for (int i = 1; i <= 4; i++) {
						int newRow = row + i * y;
						int newCol = col + i * x;
						if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Black) { // 玩家（黑棋）的子
							personNum++;
						}
						else if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Nome) { // 空白位
							emptyNum++;
							break; // 遇到空白位置，则停止该方向搜索
						}
						else { // 出边界或遇到AI的子
							break;
						}
					}

					// 往这个方向反向
					for (int i = 1; i <= 4; i++) {
						int newRow = row - i * y;
						int newCol = col - i * x;
						if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Black) { // 玩家的子
							personNum++;
						}
						else if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Nome) { // 空白位
							emptyNum++;
							break;
						}
						else { // 出边界，或者有AI自己的棋子
							break;
						}
					}

					// 根据玩家连子的数量和空白位数，更新评分
					if (personNum == 1) {                      // 杀二
						data->scoreMap[row][col] += 10;
					}
					else if (personNum == 2) {                 // 杀三
						if (emptyNum == 1)
							data->scoreMap[row][col] += 30;
						else if (emptyNum == 2)
							data->scoreMap[row][col] += 40;
					}
					else if (personNum == 3) {                 // 杀四
						if (emptyNum == 1)
							data->scoreMap[row][col] += 60;
						else if (emptyNum == 2)
							data->scoreMap[row][col] += 200;
					}
					else if (personNum >= 4) {                 // 杀五
						data->scoreMap[row][col] += 20000;
					}

					// 重置空白位数
					emptyNum = 0;

					// 对白棋评分（AI）
					for (int i = 1; i <= 4; i++) {
						int newRow = row + i * y;
						int newCol = col + i * x;
						if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == White) { // AI的子
							botNum++;
						}
						else if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Nome) { // 空白位
							emptyNum++;
							break; // 遇到空白位置，则停止该方向搜索
						}
						else { // 出边界或遇到玩家的子
							break;
						}
					}

					// 往这个方向反向
					for (int i = 1; i <= 4; i++) {
						int newRow = row - i * y;
						int newCol = col - i * x;
						if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == White) { // AI的子
							botNum++;
						}
						else if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Nome) { // 空白位
							emptyNum++;
							break;
						}
						else { // 出边界
							break;
						}
					}

					// 根据AI连子的数量和空白位数，更新评分
					if (botNum == 0) {                      // 普通下子
						data->scoreMap[row][col] += 5;
					}
					else if (botNum == 1) {                 // 活二
						data->scoreMap[row][col] += 10;
					}
					else if (botNum == 2) {
						if (emptyNum == 1)                // 死三
							data->scoreMap[row][col] += 25;
						else if (emptyNum == 2)           // 活三
							data->scoreMap[row][col] += 50;
					}
					else if (botNum == 3) {
						if (emptyNum == 1)                // 死四
							data->scoreMap[row][col] += 55;
						else if (emptyNum == 2)           // 活四
							data->scoreMap[row][col] += 300;
					}
					else if (botNum >= 4) {                 // 活五
						data->scoreMap[row][col] += 30000;
					}
				}
			}
		}
	}
}




//AI决策函数
// AI决策函数
void aiDecision()
{
	ChessData data;
	// 将当前棋盘状态复制到 ChessData 结构体中
	for (int row = 0; row < BOARD_GRAD_SIZE; row++) {
		for (int col = 0; col < BOARD_GRAD_SIZE; col++) {
			data.chessMap[row][col] = map[row][col];
		}
	}

	// 计算评分
	calculateScore(&data);

	// 找到最高评分
	int maxScore = INT_MIN;
	for (int row = 0; row < BOARD_GRAD_SIZE; row++) {
		for (int col = 0; col < BOARD_GRAD_SIZE; col++) {
			if (data.chessMap[row][col] == Nome && data.scoreMap[row][col] > maxScore) {
				maxScore = data.scoreMap[row][col];
			}
		}
	}

	// 收集所有最高评分的点
	typedef struct {
		int row;
		int col;
	} MovePoint;

	MovePoint bestMoves[BOARD_GRAD_SIZE * BOARD_GRAD_SIZE];
	int bestMoveCount = 0;

	for (int row = 0; row < BOARD_GRAD_SIZE; row++) {
		for (int col = 0; col < BOARD_GRAD_SIZE; col++) {
			if (data.chessMap[row][col] == Nome && data.scoreMap[row][col] == maxScore) {
				bestMoves[bestMoveCount].row = row;
				bestMoves[bestMoveCount].col = col;
				bestMoveCount++;
			}
		}
	}

	if (bestMoveCount > 0) {
		// 随机选择一个最佳落子点
		srand(time(NULL));
		int chosenIndex = rand() % bestMoveCount;
		int chosenRow = bestMoves[chosenIndex].row;
		int chosenCol = bestMoves[chosenIndex].col;

		// AI下子
		map[chosenRow][chosenCol] = White; // AI是白棋
		history[history_index].row = chosenRow;
		history[history_index].col = chosenCol;
		history[history_index].player = White;
		history_index++;

		//播放下棋音效
		playMoveSound();

		// 绘制AI落子
		drawGamePage();

		// 判断AI是否获胜
		if (judge(chosenRow, chosenCol)) {
			if (map[chosenRow][chosenCol] == White) {
				strcpy_s(endMessage, sizeof(endMessage), "AI赢了！");
			}
			else {
				strcpy_s(endMessage, sizeof(endMessage), "你赢了！");
			}
			currentState = STATE_END;
			return;
		}



		// 切换回玩家
		pos.player = Black;
	}
}


/*    随机落子函数
//AI下棋
void aiMove() {
	// 随机选择一个空的格子
	int row, col;
	do {
		row = rand() % ROW;
		col = rand() % COL;
	} while (map[row][col] != Nome);  // 找到一个空的位置

	map[row][col] = White;  // AI下棋为白棋

	// 判断是否有玩家获胜
	if (judge(row, col)) {
		MessageBox(GetHWnd(), "AI赢了", "Game Over", MB_OK);
		exit(0);
	}

	// 切换玩家
	pos.player = Black;  // 轮到玩家下棋
}
*/

bool judge(int r, int c) {
	int who = map[r][c]; // 当前落子的玩家

	// 定义四个方向：水平、垂直、主对角线、副对角线
	int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };

	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];

		// 计算当前方向上的连续棋子数量
		int count = 1; // 当前棋子算作第一个

		// 向正方向检查
		for (int step = 1; step < 6; step++) { // 检查最多六连
			int newRow = r + step * dr;
			int newCol = c + step * dc;
			if (newRow >= 0 && newRow < ROW && newCol >= 0 && newCol < COL && map[newRow][newCol] == who) {
				count++;
			}
			else {
				break;
			}
		}

		// 向反方向检查
		for (int step = 1; step < 6; step++) { // 检查最多六连
			int newRow = r - step * dr;
			int newCol = c - step * dc;
			if (newRow >= 0 && newRow < ROW && newCol >= 0 && newCol < COL && map[newRow][newCol] == who) {
				count++;
			}
			else {
				break;
			}
		}

		// 判断是否形成长连（六连或更多）
		if (count >= 6) {
			if (who == Black) {
				// 黑棋形成长连，判定黑棋为输
				strcpy_s(endMessage, sizeof(endMessage), "黑棋形成长连，黑棋负！");
				currentState = STATE_END;
				return false;
			}
			else {
				// 白棋形成长连，判定白棋为赢
				strcpy_s(endMessage, sizeof(endMessage), "白棋形成长连，白棋赢了！");
				currentState = STATE_END;
				return true;
			}
		}

		// 判断是否形成五连
		if (count == 5) {
			if (who == Black) {
				// 黑棋形成五连，判定黑棋为赢
				strcpy_s(endMessage, sizeof(endMessage), "黑棋赢了！");
				currentState = STATE_END;
				return true;
			}
			else {
				// 白棋形成五连，判定白棋为赢
				strcpy_s(endMessage, sizeof(endMessage), "白棋赢了！");
				currentState = STATE_END;
				return true;
			}
		}
	}

	// 如果没有找到五连或长连，则返回false
	return false;
}


// 悔棋
void undoMove() {
	if (history_index > 0) {
		// 如果是AI模式，每次撤销两步棋（玩家和AI各一手）
		if (isAI) {
			// 撤销AI的最后一手
			history_index--;
			if (history_index >= 0) {
				int ai_row = history[history_index].row;
				int ai_col = history[history_index].col;
				map[ai_row][ai_col] = Nome;
				// 切换回玩家
				pos.player = (pos.player == Black) ? White : Black;
			}

			// 撤销玩家的最后一手
			if (history_index > 0) {
				history_index--;
				int player_row = history[history_index].row;
				int player_col = history[history_index].col;
				map[player_row][player_col] = Nome;
				// 切换回AI
				pos.player = (pos.player == Black) ? White : Black;
			}
		}
		else {
			// 非AI模式，只撤销一手棋
			history_index--;
			int last_row = history[history_index].row;
			int last_col = history[history_index].col;
			map[last_row][last_col] = Nome;
			// 切换玩家
			pos.player = (pos.player == Black) ? White : Black;
		}

		// 重置结束消息状态（如果之前是游戏结束状态）
		if (currentState == STATE_END) {
			currentState = STATE_PLAYING;
			endMessageShown = false;
			endMessage[0] = '\0'; // 清空消息内容
		}
	}
}


//*****************************************************禁手************************************
// 计算在特定方向上连续相同棋子的数量
int countConsecutive(int row, int col, int deltaRow, int deltaCol, int player) {
	int count = 0;
	row += deltaRow;
	col += deltaCol;
	while (row >= 0 && row < ROW && col >= 0 && col < COL && map[row][col] == player) {
		count++;
		row += deltaRow;
		col += deltaCol;
	}
	return count;
}

// 检测是否形成活三
int countAliveThree(int row, int col, int player) {
	int aliveThree = 0;
	// 四个方向：水平、垂直、主对角线、副对角线
	int directions[4][2] = { {1,0}, {0,1}, {1,1}, {1,-1} };
	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];
		// 检查当前方向是否有活三
		int count1 = countConsecutive(row, col, dr, dc, player);
		int count2 = countConsecutive(row, col, -dr, -dc, player);
		if (count1 + count2 + 1 == 3) { // 包含当前下的棋子
			// 检查两端是否为空
			int end1_r = row + (count1 + 1) * dr;
			int end1_c = col + (count1 + 1) * dc;
			int end2_r = row - (count2 + 1) * dr;
			int end2_c = col - (count2 + 1) * dc;
			bool openEnds = false;
			if (end1_r >= 0 && end1_r < ROW && end1_c >= 0 && end1_c < COL && map[end1_r][end1_c] == Nome) {
				openEnds = true;
			}
			if (end2_r >= 0 && end2_r < ROW && end2_c >= 0 && end2_c < COL && map[end2_r][end2_c] == Nome) {
				openEnds = true;
			}
			if (openEnds) aliveThree++;
		}
	}
	return aliveThree;
}

// 检测是否形成活四
int countAliveFour(int row, int col, int player) {
	int aliveFour = 0;
	// 四个方向：水平、垂直、主对角线、副对角线
	int directions[4][2] = { {1,0}, {0,1}, {1,1}, {1,-1} };
	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];
		// 检查当前方向是否有活四
		int count1 = countConsecutive(row, col, dr, dc, player);
		int count2 = countConsecutive(row, col, -dr, -dc, player);
		if (count1 + count2 + 1 == 4) { // 包含当前下的棋子
			// 检查两端是否为空
			int end1_r = row + (count1 + 1) * dr;
			int end1_c = col + (count1 + 1) * dc;
			int end2_r = row - (count2 + 1) * dr;
			int end2_c = col - (count2 + 1) * dc;
			bool openEnds = false;
			if (end1_r >= 0 && end1_r < ROW && end1_c >= 0 && end1_c < COL && map[end1_r][end1_c] == Nome) {
				openEnds = true;
			}
			if (end2_r >= 0 && end2_r < ROW && end2_c >= 0 && end2_c < COL && map[end2_r][end2_c] == Nome) {
				openEnds = true;
			}
			if (openEnds) aliveFour++;
		}
	}
	return aliveFour;
}

// 检测是否形成长连（六连或更多）
bool isLongConnect(int row, int col, int player) {
	// 四个方向：水平、垂直、主对角线、副对角线
	int directions[4][2] = { {1,0}, {0,1}, {1,1}, {1,-1} };
	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];
		int count = 1; // 包含当前下的棋子
		count += countConsecutive(row, col, dr, dc, player);
		count += countConsecutive(row, col, -dr, -dc, player);
		if (count >= 6) return true;
	}
	return false;
}

// 检测禁手规则
bool isForbiddenMove(int row, int col, int player) {
	if (player != Black) return false; // 只有黑棋有禁手规则

	int aliveThree = countAliveThree(row, col, player);
	int aliveFour = countAliveFour(row, col, player);
	bool longConnect = isLongConnect(row, col, player);

	// 三三禁手：形成两个活三
	if (aliveThree >= 2) return true;

	// 四四禁手：形成两个活四
	if (aliveFour >= 2) return true;

	// 长连禁手：形成六连或更多
	if (longConnect) return true;

	// 四三禁手：形成一个活四和一个活三
	if (aliveFour >= 1 && aliveThree >= 1) return true;

	return false;
}
//********************************************禁手***************************************
// 声音按钮点击冷却机制
bool soundButtonClicked = false;
DWORD lastClickTime = 0;

// 处理声音按钮的点击事件
void handleSoundButtonClick(ExMessage* msg) {
	// 获取当前时间
	DWORD currentTime = GetTickCount();

	// 设置冷却时间为200毫秒
	if (currentTime - lastClickTime < 200) {
		return; // 冷却时间内忽略点击
	}

	// 检查点击是否在声音按钮区域内
	if (msg->x >= SOUND_BUTTON_X && msg->x <= (SOUND_BUTTON_X + SOUND_BUTTON_WIDTH) &&
		msg->y >= SOUND_BUTTON_Y && msg->y <= (SOUND_BUTTON_Y + SOUND_BUTTON_HEIGHT)) {
		// 切换声音状态
		soundOn = !soundOn;

		if (soundOn) {
			// 开启音乐
			mciSendString("play bgm repeat", NULL, 0, NULL);
		}
		else {
			// 关闭音乐
			mciSendString("stop bgm", NULL, 0, NULL);
		}

		// 更新上次点击时间
		lastClickTime = currentTime;
	}
}


int main() {
	// 加载资源（初始化页面和棋盘背景）
	loadResource();

	// 初始化状态为主菜单
	currentState = STATE_MENU;

	// 初始化随机种子
	srand(time(NULL));

	// 主循环
	while (true) {
		ExMessage msg;
		bool hasMessage = peekmessage(&msg, EX_MOUSE);

		BeginBatchDraw();

		// 根据当前状态绘制相应的页面
		if (currentState == STATE_MENU) {
			drawInitPage();
		}
		else if (currentState == STATE_PLAYING) {
			drawGamePage();  // 绘制棋盘和棋子
		}
		else if (currentState == STATE_END) {
			drawEndPage();
		}

		EndBatchDraw();

		// 显示结束消息框
		if (currentState == STATE_END && !endMessageShown) {
			MessageBoxA(GetHWnd(), endMessage, "游戏结束", MB_OK);
			endMessageShown = true;
		}

		if (hasMessage) {
			if (msg.message == WM_LBUTTONDOWN) {
				// 先检查是否点击在声音按钮区域
				if (msg.x >= SOUND_BUTTON_X && msg.x <= (SOUND_BUTTON_X + SOUND_BUTTON_WIDTH) &&
					msg.y >= SOUND_BUTTON_Y && msg.y <= (SOUND_BUTTON_Y + SOUND_BUTTON_HEIGHT)) {
					handleSoundButtonClick(&msg);
				}
				else {
					// 处理其他按钮点击
					if (currentState == STATE_MENU) {
						handleMenuClick(&msg);
					}
					else if (currentState == STATE_PLAYING) {
						mousePressMsg(&msg);
					}
					else if (currentState == STATE_END) {
						handleEndPageClick(&msg);
					}
				}
			}
			else if (msg.message == WM_MOUSEMOVE) {
				if (currentState == STATE_PLAYING) {
					mouseMoveMsg(&msg);
				}
				// 也可以在其他状态下处理鼠标移动以更新全局鼠标位置
				currentMouseX = msg.x;
				currentMouseY = msg.y;
			}
		}

		// 如果是人机对战且轮到AI下棋，调用AI决策
		if (currentState == STATE_PLAYING && isAI && pos.player == White) {
			aiDecision();
		}

		Sleep(10); // 减少CPU占用
	}

	// 不会到达这里
	getchar(); // 保持窗口
	return 0;
}