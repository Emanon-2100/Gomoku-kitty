#include<stdio.h>
#include<easyx.h>// ͼ�ν���
#include<graphics.h>
#include<stdio.h>
#include<string.h>
#include<time.h>    //�������ѡ��
#include<cstring>


#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") // ���Ӷ�ý���




//�궨�� 15������
#define GRID_W 25      //���

//������ �к���
#define ROW 15
#define COL 15
#define SPACE (3 * GRID_W) //�м���Ͽո�//�ϱߺ���ߵļ��
#define MAX_HISTORY 225 // ���洢225����������̸�����//���ڻ���
#define BOARD_GRAD_SIZE 15

// ����ҳ�水ť�ߴ�
#define END_BUTTON_WIDTH 120
#define END_BUTTON_HEIGHT 30

// ����ҳ�水ťλ�ã����У�
#define END_BUTTON_X ((600 - END_BUTTON_WIDTH) / 2) // ���贰�ڿ��Ϊ600
#define END_BUTTON_Y 350 // ������Ҫ����Y��λ��

//С���Ȱ�ť
// 
// ����������ť�ĳߴ��λ��
#define SOUND_BUTTON_WIDTH 30
#define SOUND_BUTTON_HEIGHT 30
#define SOUND_BUTTON_MARGIN 10  // �봰�ڱ�Ե�ľ���
#define SOUND_BUTTON_X SOUND_BUTTON_MARGIN // ���½ǣ�X����Ϊ���Ե�ľ���
#define SOUND_BUTTON_Y (500 - SOUND_BUTTON_HEIGHT - SOUND_BUTTON_MARGIN) // Y���걣�ֲ��䣬λ�ڵײ�



// ���¿�ʼ��ť
#define RESTART_BUTTON_WIDTH 80
#define RESTART_BUTTON_HEIGHT 40
#define RESTART_BUTTON_X 500
#define RESTART_BUTTON_Y 100

// ���尴ť
#define UNDO_BUTTON_WIDTH 80
#define UNDO_BUTTON_HEIGHT 40
#define UNDO_BUTTON_X 500
#define UNDO_BUTTON_Y 160

// ������Ϸ��ť
#define ENDGAME_BUTTON_WIDTH 80
#define ENDGAME_BUTTON_HEIGHT 40
#define ENDGAME_BUTTON_X 500
#define ENDGAME_BUTTON_Y 220


// �浵��ť
#define SAVE_BUTTON_WIDTH 80
#define SAVE_BUTTON_HEIGHT 40
#define SAVE_BUTTON_X 500
#define SAVE_BUTTON_Y 280

// ������ť
#define LOAD_BUTTON_WIDTH 80
#define LOAD_BUTTON_HEIGHT 40
#define LOAD_BUTTON_X 500
#define LOAD_BUTTON_Y 340



// ȫ�ֱ����洢��ǰ���λ��
int currentMouseX = -1;
int currentMouseY = -1;


// ���������ť��ͼƬ����
IMAGE soundOnImg;
IMAGE soundOffImg;

// ����״̬������Ĭ��Ϊ����
bool soundOn = true;


typedef struct {
	int chessMap[BOARD_GRAD_SIZE][BOARD_GRAD_SIZE]; // ��ǰ����״̬
	int scoreMap[BOARD_GRAD_SIZE][BOARD_GRAD_SIZE]; // ����λ�õ�����
} ChessData;

struct Move {
	int row;
	int col;
	int player;
};

struct Move history[MAX_HISTORY]; // �洢ÿһ������ʷ��¼

int history_index = 0; // ��ǰ��ʷ��¼������

bool isAI = false;

//����ͼƬ 
IMAGE bk;		     //����ͼƬ

IMAGE kaishi;

//��ť
IMAGE renji;
IMAGE shuangren;
IMAGE tuichu;

IMAGE jieshu; // ����ҳ�汳��ͼƬ
IMAGE anniu;  // ��ťͼƬ

// ��ť����ͼƬ
IMAGE restartButtonImg;
IMAGE undoButtonImg;
IMAGE endgameButtonImg;
IMAGE savegameButtonImg;
IMAGE loadgameButtonImg;


//����һ���ж���Ӯ�ĺ���
bool judge(int r, int c);
//�������庯��
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

//ö����
//��������
enum Chess {

	Nome = 0,				//û������
	Black = 1,				//����
	White = -1				//����

};

typedef enum {
	STATE_INIT,     // ��ʼ��ҳ��
	STATE_MENU,     // ���˵�
	STATE_PLAYING,  // ��Ϸ������
	STATE_END       // ����ҳ��
} GameState;

GameState currentState = STATE_INIT;



//������װ��������
int map[ROW][COL];//װ�������ӵ�����




struct Pos
{
	//����λ��
	int row;
	int col;
	bool isShow;       //�Ƿ��¼��ɫ�Ŀ�
	int player;			//����


}pos = { -1,-1,false,Black };  //���ڴ���

//����ʱ�ĵ���
bool endMessageShown = false;       // ����Ƿ��Ѿ���ʾ����������
char endMessage[100] = "";           // �洢������Ϣ



//����ʵ��͸������ť
//���ڴ���͸��������ͼƬ
// ����PNGͼ��ȥ͸������
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //xΪ����ͼƬ��X���꣬yΪY����
{

	// ������ʼ��
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
	int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
	int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
	int graphWidth = getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
	int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
	int dstX = 0;    //���Դ������صĽǱ�

	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
			int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //���Դ������صĽǱ�
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
			}
		}
	}
}



void loadResource() {
	//������Ϸ����
	initgraph(600, 500);
	//����ͼƬ
	loadimage(&kaishi, "resource/kaishi.jpg");
	loadimage(&renji, "resource/renji7.PNG");
	loadimage(&shuangren, "resource/shuangren7.PNG");
	loadimage(&tuichu, "resource/tuichu7.PNG");

	loadimage(&bk, "resource/bk.png");



	// ���ؽ���ҳ��Ͱ�ťͼƬ
	loadimage(&jieshu, "resource/jieshu.jpg");
	loadimage(&anniu, "resource/anniu.jpg");


	// ����������ťͼƬ
	loadimage(&soundOnImg, "resource/sound_on.png");
	loadimage(&soundOffImg, "resource/sound_off.png");

	// ���ذ�ť����ͼƬ
	loadimage(&restartButtonImg, "resource/restart_button.jpg"); // �ؿ���ťͼƬ
	loadimage(&undoButtonImg, "resource/undo_button.jpg");       // ���尴ťͼƬ
	loadimage(&endgameButtonImg, "resource/endgame_button.jpg"); // ������Ϸ��ťͼƬ




	//����
	//�������֣�ѭ������
	mciSendString("open \"resource/background.mp3\" type mpegvideo alias bgm", NULL, 0, NULL);
	mciSendString("play bgm repeat", NULL, 0, NULL);
}

//�����̲߳�����Ч
// ������Ч�����̺߳���
DWORD WINAPI playMoveSoundThread(LPVOID lpParam) {
	// �򿪲�����������Ч
	mciSendString("open \"resource/move.mp3\" type mpegvideo alias moveSound", NULL, 0, NULL);
	mciSendString("play moveSound from 0", NULL, 0, NULL);

	// ��ȡ������Ч��ʱ�����Ա���ȷ�ر�
	char lengthCmd[256];
	char lengthStr[32];

	// ʹ�� strcpy_s ���а�ȫ���ַ�������
	strcpy_s(lengthCmd, sizeof(lengthCmd), "status moveSound length");
	mciSendString(lengthCmd, lengthStr, sizeof(lengthStr), NULL);
	int length = atoi(lengthStr); // ʱ���Ժ���Ϊ��λ

	// �ȴ���Ч�������
	Sleep(length);

	// �ر���Ч
	mciSendString("close moveSound", NULL, 0, NULL);
	return 0;
}

// ����������Ч�ĺ���
void playMoveSound() {
	HANDLE hThread = CreateThread(
		NULL,                   // Ĭ�ϰ�ȫ����
		0,                      // Ĭ�϶�ջ��С
		playMoveSoundThread,    // �̺߳���
		NULL,                   // �������ݸ��̺߳���
		0,                      // Ĭ�ϴ�����־
		NULL                    // ����Ҫ�̱߳�ʶ��
	);

	if (hThread != NULL) {
		CloseHandle(hThread); // �ر��߳̾�������ȴ��߳̽���
	}
}


//���Ƴ�ʼ��ҳ��
// ���Ƴ�ʼ��ҳ��
void drawInitPage() {
	// ���Ƴ�ʼ��ҳ��ı���
	putimage(0, 0, &kaishi);

	// ���ư�ť
	drawAlpha(&shuangren, 20, 80);  // ˫�˶�ս��ť
	drawAlpha(&renji, 20, 200);    // �˻���ս��ť
	drawAlpha(&tuichu, 20, 320);     // �˳���ť

	// ����������ť
	drawSoundButton(currentMouseX, currentMouseY);
}


// ������Ϸ�����е�ҳ��
// ������Ϸ�����е�ҳ��
void drawGamePage() {
	// �������ͼƬ
	putimage(0, 0, &bk);

	// ��������,�����ߵ���ɫ
	setlinecolor(BLACK);

	// �ߵ���ʽ
	setlinestyle(PS_SOLID, 2); // ����������ʽΪʵ�ߣ����Ϊ2

	// 15����
	for (int i = 0; i < 15; i++) {
		// ����
		line(SPACE, i * GRID_W + SPACE, 14 * GRID_W + SPACE, i * GRID_W + SPACE);

		// ����
		line(i * GRID_W + SPACE, SPACE, i * GRID_W + SPACE, 14 * GRID_W + SPACE);
	}
	setlinestyle(PS_SOLID, 5); // ���߼Ӵ�

	// ����һ������
	rectangle(SPACE, SPACE, SPACE + GRID_W * 14, SPACE + GRID_W * 14);

	// ���һ����ɫ
	setfillcolor(BLACK);

	// �������̵�����ڵ�
	solidcircle(SPACE + 7 * GRID_W, SPACE + 7 * GRID_W, 5);			// ���ĵ�
	solidcircle(SPACE + 3 * GRID_W, SPACE + 3 * GRID_W, 5);   // ����
	solidcircle(SPACE + 11 * GRID_W, SPACE + 11 * GRID_W, 5);	// ����
	solidcircle(SPACE + 3 * GRID_W, SPACE + 11 * GRID_W, 5);    // ����
	solidcircle(SPACE + 11 * GRID_W, SPACE + 3 * GRID_W, 5);	// ����

	// ��������
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			// ����
			if (map[i][k] == Black) {
				setfillcolor(BLACK);
				solidcircle(k * GRID_W + SPACE, i * GRID_W + SPACE, 10);
			}
			// ����
			else if (map[i][k] == White) {
				setfillcolor(WHITE);
				solidcircle(k * GRID_W + SPACE, i * GRID_W + SPACE, 10);
			}
		}
	}

	// ������ɫ���
	if (pos.isShow) {
		setlinecolor(BLUE); // ��ɫ��
		rectangle(pos.col * GRID_W + SPACE - 12, pos.row * GRID_W + SPACE - 12,
			pos.col * GRID_W + SPACE + 12, pos.row * GRID_W + SPACE + 12);
	}

	// ����������ť
	drawSoundButton(currentMouseX, currentMouseY);

	// �������¿�ʼ������ͽ�����Ϸ��ť
	drawButtons(currentMouseX, currentMouseY);
}








// �������¿�ʼ������ͽ�����Ϸ��ť
void drawButtons(int mouseX, int mouseY) {
	// ����������ʽ
	settextstyle(16, 0, "Arial"); // �����СΪ16��Arial����
	settextcolor(WHITE); // �ı���ɫΪ��ɫ

	// ���¿�ʼ��ť
	bool restartHover = false;
	if (mouseX >= RESTART_BUTTON_X && mouseX <= (RESTART_BUTTON_X + RESTART_BUTTON_WIDTH) &&
		mouseY >= RESTART_BUTTON_Y && mouseY <= (RESTART_BUTTON_Y + RESTART_BUTTON_HEIGHT)) {
		restartHover = true;
	}

	if (restartHover) {
		setlinecolor(RED); // �����߿���ɫ
		setlinestyle(PS_SOLID, 2);
		rectangle(RESTART_BUTTON_X - 2, RESTART_BUTTON_Y - 2,
			RESTART_BUTTON_X + RESTART_BUTTON_WIDTH + 2, RESTART_BUTTON_Y + RESTART_BUTTON_HEIGHT + 2);
	}

	// �������¿�ʼ��ť����ͼƬ
	putimage(RESTART_BUTTON_X, RESTART_BUTTON_Y, &restartButtonImg);

	// ���ơ��ؿ�����ť�ı�
	const char* restartText = "�ؿ�";
	int restartTextWidth = textwidth(restartText);
	int restartTextHeight = textheight(restartText);
	int restartTextX = RESTART_BUTTON_X + (RESTART_BUTTON_WIDTH - restartTextWidth) / 2;
	int restartTextY = RESTART_BUTTON_Y + (RESTART_BUTTON_HEIGHT - restartTextHeight) / 2;
	outtextxy(restartTextX, restartTextY, restartText);

	// ���尴ť
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

	// ���ƻ��尴ť����ͼƬ
	putimage(UNDO_BUTTON_X, UNDO_BUTTON_Y, &undoButtonImg);

	// ���ơ����塱��ť�ı�
	const char* undoText = "����";
	int undoTextWidth = textwidth(undoText);
	int undoTextHeight = textheight(undoText);
	int undoTextX = UNDO_BUTTON_X + (UNDO_BUTTON_WIDTH - undoTextWidth) / 2;
	int undoTextY = UNDO_BUTTON_Y + (UNDO_BUTTON_HEIGHT - undoTextHeight) / 2;
	outtextxy(undoTextX, undoTextY, undoText);

	// ������Ϸ��ť
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

	// ���ƽ�����Ϸ��ť����ͼƬ
	putimage(ENDGAME_BUTTON_X, ENDGAME_BUTTON_Y, &endgameButtonImg);

	// ���ơ���������ť�ı�
	const char* endgameText = "����";
	int endgameTextWidth = textwidth(endgameText);
	int endgameTextHeight = textheight(endgameText);
	int endgameTextX = ENDGAME_BUTTON_X + (ENDGAME_BUTTON_WIDTH - endgameTextWidth) / 2;
	int endgameTextY = ENDGAME_BUTTON_Y + (ENDGAME_BUTTON_HEIGHT - endgameTextHeight) / 2;
	outtextxy(endgameTextX, endgameTextY, endgameText);
}





// ���ƽ���ҳ��ĺ���
void drawEndPage() {
	// ����
	cleardevice();

	// ���ƽ���ҳ�汳��
	putimage(0, 0, &jieshu);

	// ���Ʒ������˵���ť
	putimage(END_BUTTON_X, END_BUTTON_Y, &anniu);

	// ��Ӱ�ť�ı������У�
	settextcolor(WHITE);
	settextstyle(16, 0, "Arial"); // ���������С����Ӧ��ť�߶�

	// ��ȡ�ı��Ŀ�Ⱥ͸߶�
	const char* buttonText = "�������˵�";
	int text_width = textwidth(buttonText);
	int text_height = textheight(buttonText);

	// �����ı�����ʼλ���Ծ�����ʾ
	int text_x = END_BUTTON_X + (END_BUTTON_WIDTH - text_width) / 2 - 15;//΢��һ�£�¶������
	int text_y = END_BUTTON_Y + (END_BUTTON_HEIGHT - text_height) / 2;

	outtextxy(text_x, text_y, buttonText);

	// ����������ť
	drawSoundButton(currentMouseX, currentMouseY);
}


// ����������ť�������ͣЧ��
void drawSoundButton(int mouseX, int mouseY) {
	bool isHover = false;
	if (mouseX >= SOUND_BUTTON_X && mouseX <= (SOUND_BUTTON_X + SOUND_BUTTON_WIDTH) &&
		mouseY >= SOUND_BUTTON_Y && mouseY <= (SOUND_BUTTON_Y + SOUND_BUTTON_HEIGHT)) {
		isHover = true;
	}

	if (isHover) {
		setlinecolor(RED); // �����߿���ɫ
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




// �������ҳ��ĵ���¼�
bool handleEndPageClick(ExMessage* msg) {
	if (msg->message == WM_LBUTTONDOWN) {
		// ������Ƿ��ڰ�ť������
		if (msg->x >= END_BUTTON_X && msg->x <= (END_BUTTON_X + END_BUTTON_WIDTH) &&
			msg->y >= END_BUTTON_Y && msg->y <= (END_BUTTON_Y + END_BUTTON_HEIGHT)) {
			// �������˵�
			currentState = STATE_MENU;

			// �������̺���ʷ��¼
			memset(map, 0, sizeof(map));
			history_index = 0;
			pos.player = Black;

			// ���ý�����Ϣ״̬
			endMessageShown = false;
			endMessage[0] = '\0'; // �����Ϣ����

			return true;
		}
	}
	return false;
}

// �������˵��ĵ���¼�
void handleMenuClick(ExMessage* msg) {
	if (msg->message == WM_LBUTTONDOWN) {
		// ˫�˶�ս��ť����
		// ����ʵ�ʰ�ťͼƬ�Ĵ�С�������귶Χ
		// ������谴ťͼƬ���Ϊ180���߶�Ϊ50
		if (msg->x >= 20 && msg->x <= 20 + 180) {
			if (msg->y >= 80 && msg->y <= 80 + 120) {
				// ����˫�˶�սģʽ
				isAI = false;
				currentState = STATE_PLAYING;
				// �������̺���ʷ��¼
				memset(map, 0, sizeof(map));
				history_index = 0;
				pos.player = Black;
				return;
			}
			if (msg->y >= 200 && msg->y <= 200 + 120) {
				// �����˻���սģʽ
				isAI = true;
				currentState = STATE_PLAYING;
				// �������̺���ʷ��¼
				memset(map, 0, sizeof(map));
				history_index = 0;
				pos.player = Black;
				return;
			}
			if (msg->y >= 320 && msg->y <= 320 + 120) {
				// �˳���Ϸ
				exit(0);
			}
		}
	}
}




// ��ʼ��Ϸ�ĺ���

// ˫�˶�ս
void startGameTwoPlayer() {
	memset(map, 0, sizeof(map));  // �������
	pos.player = Black;           // Ĭ�Ϻ�������
	history_index = 0;            // ������ʷ��¼
	currentState = STATE_PLAYING; // ���õ�ǰ״̬Ϊ��Ϸ������
}

// �˻���ս
void startGameAI() {
	memset(map, 0, sizeof(map));  // �������
	pos.player = Black;           // Ĭ�Ϻ�������
	history_index = 0;            // ������ʷ��¼
	currentState = STATE_PLAYING; // ���õ�ǰ״̬Ϊ��Ϸ������
}






// ����ƶ�
void mouseMoveMsg(ExMessage* msg) { // ������Ϣ
	// ����ȫ�����λ��
	currentMouseX = msg->x;
	currentMouseY = msg->y;

	// ����λ�� �����±�
	pos.isShow = false;

	// ����ÿ������
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			// ������������
			int gridx = k * GRID_W + SPACE;
			int gridy = i * GRID_W + SPACE;
			if (abs(msg->x - gridx) <= 12 && abs(msg->y - gridy) <= 12) {
				// �жϣ�������������ķ�Χ֮��
				pos.isShow = true;
				pos.row = i;
				pos.col = k;
			}
		}
	}
}

// ���������
void mousePressMsg(ExMessage* msg) {
	// �ж�����Ƿ���
	if (msg->message == WM_LBUTTONDOWN) {
		// ������¿�ʼ��ť����
		if (msg->x >= RESTART_BUTTON_X && msg->x <= (RESTART_BUTTON_X + RESTART_BUTTON_WIDTH) &&
			msg->y >= RESTART_BUTTON_Y && msg->y <= (RESTART_BUTTON_Y + RESTART_BUTTON_HEIGHT)) {
			// ���¿�ʼ��Ϸ
			memset(map, 0, sizeof(map));  // �������
			history_index = 0;            // ������ʷ��¼
			pos.player = Black;           // Ĭ�Ϻ�������
			currentState = STATE_PLAYING; // ��������Ϸ������
		}
		// �����尴ť����
		else if (msg->x >= UNDO_BUTTON_X && msg->x <= (UNDO_BUTTON_X + UNDO_BUTTON_WIDTH) &&
			msg->y >= UNDO_BUTTON_Y && msg->y <= (UNDO_BUTTON_Y + UNDO_BUTTON_HEIGHT)) {
			// ���û��幦��
			undoMove();
		}
		// ��������Ϸ��ť����
		else if (msg->x >= ENDGAME_BUTTON_X && msg->x <= (ENDGAME_BUTTON_X + ENDGAME_BUTTON_WIDTH) &&
			msg->y >= ENDGAME_BUTTON_Y && msg->y <= (ENDGAME_BUTTON_Y + ENDGAME_BUTTON_HEIGHT)) {
			// ������Ϸ���������˵�
			currentState = STATE_MENU;

			// �������̺���ʷ��¼
			memset(map, 0, sizeof(map));
			history_index = 0;
			pos.player = Black;

			// ���ý�����Ϣ״̬
			endMessageShown = false;
			endMessage[0] = '\0'; // �����Ϣ����
		}
		else {
			// ���������¼�
			if (pos.isShow && map[pos.row][pos.col] == Nome) { // ȷ����ǰλ����Ч��û������
				map[pos.row][pos.col] = pos.player; // ������������

				// ���浱ǰ�岽
				if (history_index < MAX_HISTORY) {
					history[history_index].row = pos.row;
					history[history_index].col = pos.col;
					history[history_index].player = pos.player;
					history_index++; // ������ʷ����
				}

				// ����������Ч
				playMoveSound();

				// �ж��Ƿ�����һ�ʤ���γɽ���
				bool hasFive = judge(pos.row, pos.col);
				bool isForbidden = false;
				if (pos.player == Black) {
					isForbidden = isForbiddenMove(pos.row, pos.col, Black);
				}

				if (hasFive && !isForbidden) {
					if (pos.player == Black) {
						strcpy_s(endMessage, sizeof(endMessage), "����Ӯ�ˣ�");
					}
					else {
						strcpy_s(endMessage, sizeof(endMessage), "����Ӯ�ˣ�");
					}
					currentState = STATE_END;
				}
				else if (isForbidden && !hasFive) {
					strcpy_s(endMessage, sizeof(endMessage), "�����γɽ��֣����帺��");
					currentState = STATE_END;
				}
				else if (hasFive && isForbidden) {
					strcpy_s(endMessage, sizeof(endMessage), "��Ӯ�ˣ��������ȣ�");
					currentState = STATE_END;
				}

				// �л����
				pos.player = (pos.player == Black ? White : Black);

				// ������˻���ս���ֵ�AI���壬����AI����
				if (isAI && currentState == STATE_PLAYING && pos.player == White) {
					aiDecision();
				}
			}
		}
	}
}




//���ֺ���
// �������ֺ���
void calculateScore(ChessData* data)
{
	if (!data) return;

	// ͳ����һ��ߵ������ɵ���
	int personNum = 0; // ��������ӵĸ���
	int botNum = 0;     // AI�����ӵĸ���
	int emptyNum = 0;   // ������հ�λ�ĸ���

	// �����������
	memset(data->scoreMap, 0, sizeof(data->scoreMap));

	for (int row = 0; row < BOARD_GRAD_SIZE; row++) {
		for (int col = 0; col < BOARD_GRAD_SIZE; col++) {
			// ֻ�Կհ׵����
			if (data->chessMap[row][col] == Nome) {
				// �����ĸ�����ˮƽ����ֱ�����Խ��ߡ����Խ���
				int directs[4][2] = {
					{1, 0},  // ˮƽ
					{1, 1},  // ���Խ���
					{0, 1},  // ��ֱ
					{-1, 1}  // ���Խ���
				};

				for (int k = 0; k < 4; k++) {
					int x = directs[k][0];
					int y = directs[k][1];

					// ����
					personNum = 0;
					botNum = 0;
					emptyNum = 0;

					// �Ժ������֣���ң�
					for (int i = 1; i <= 4; i++) {
						int newRow = row + i * y;
						int newCol = col + i * x;
						if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Black) { // ��ң����壩����
							personNum++;
						}
						else if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Nome) { // �հ�λ
							emptyNum++;
							break; // �����հ�λ�ã���ֹͣ�÷�������
						}
						else { // ���߽������AI����
							break;
						}
					}

					// �����������
					for (int i = 1; i <= 4; i++) {
						int newRow = row - i * y;
						int newCol = col - i * x;
						if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Black) { // ��ҵ���
							personNum++;
						}
						else if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Nome) { // �հ�λ
							emptyNum++;
							break;
						}
						else { // ���߽磬������AI�Լ�������
							break;
						}
					}

					// ����������ӵ������Ϳհ�λ������������
					if (personNum == 1) {                      // ɱ��
						data->scoreMap[row][col] += 10;
					}
					else if (personNum == 2) {                 // ɱ��
						if (emptyNum == 1)
							data->scoreMap[row][col] += 30;
						else if (emptyNum == 2)
							data->scoreMap[row][col] += 40;
					}
					else if (personNum == 3) {                 // ɱ��
						if (emptyNum == 1)
							data->scoreMap[row][col] += 60;
						else if (emptyNum == 2)
							data->scoreMap[row][col] += 200;
					}
					else if (personNum >= 4) {                 // ɱ��
						data->scoreMap[row][col] += 20000;
					}

					// ���ÿհ�λ��
					emptyNum = 0;

					// �԰������֣�AI��
					for (int i = 1; i <= 4; i++) {
						int newRow = row + i * y;
						int newCol = col + i * x;
						if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == White) { // AI����
							botNum++;
						}
						else if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Nome) { // �հ�λ
							emptyNum++;
							break; // �����հ�λ�ã���ֹͣ�÷�������
						}
						else { // ���߽��������ҵ���
							break;
						}
					}

					// �����������
					for (int i = 1; i <= 4; i++) {
						int newRow = row - i * y;
						int newCol = col - i * x;
						if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == White) { // AI����
							botNum++;
						}
						else if (newRow >= 0 && newRow < BOARD_GRAD_SIZE &&
							newCol >= 0 && newCol < BOARD_GRAD_SIZE &&
							data->chessMap[newRow][newCol] == Nome) { // �հ�λ
							emptyNum++;
							break;
						}
						else { // ���߽�
							break;
						}
					}

					// ����AI���ӵ������Ϳհ�λ������������
					if (botNum == 0) {                      // ��ͨ����
						data->scoreMap[row][col] += 5;
					}
					else if (botNum == 1) {                 // ���
						data->scoreMap[row][col] += 10;
					}
					else if (botNum == 2) {
						if (emptyNum == 1)                // ����
							data->scoreMap[row][col] += 25;
						else if (emptyNum == 2)           // ����
							data->scoreMap[row][col] += 50;
					}
					else if (botNum == 3) {
						if (emptyNum == 1)                // ����
							data->scoreMap[row][col] += 55;
						else if (emptyNum == 2)           // ����
							data->scoreMap[row][col] += 300;
					}
					else if (botNum >= 4) {                 // ����
						data->scoreMap[row][col] += 30000;
					}
				}
			}
		}
	}
}




//AI���ߺ���
// AI���ߺ���
void aiDecision()
{
	ChessData data;
	// ����ǰ����״̬���Ƶ� ChessData �ṹ����
	for (int row = 0; row < BOARD_GRAD_SIZE; row++) {
		for (int col = 0; col < BOARD_GRAD_SIZE; col++) {
			data.chessMap[row][col] = map[row][col];
		}
	}

	// ��������
	calculateScore(&data);

	// �ҵ��������
	int maxScore = INT_MIN;
	for (int row = 0; row < BOARD_GRAD_SIZE; row++) {
		for (int col = 0; col < BOARD_GRAD_SIZE; col++) {
			if (data.chessMap[row][col] == Nome && data.scoreMap[row][col] > maxScore) {
				maxScore = data.scoreMap[row][col];
			}
		}
	}

	// �ռ�����������ֵĵ�
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
		// ���ѡ��һ��������ӵ�
		srand(time(NULL));
		int chosenIndex = rand() % bestMoveCount;
		int chosenRow = bestMoves[chosenIndex].row;
		int chosenCol = bestMoves[chosenIndex].col;

		// AI����
		map[chosenRow][chosenCol] = White; // AI�ǰ���
		history[history_index].row = chosenRow;
		history[history_index].col = chosenCol;
		history[history_index].player = White;
		history_index++;

		//����������Ч
		playMoveSound();

		// ����AI����
		drawGamePage();

		// �ж�AI�Ƿ��ʤ
		if (judge(chosenRow, chosenCol)) {
			if (map[chosenRow][chosenCol] == White) {
				strcpy_s(endMessage, sizeof(endMessage), "AIӮ�ˣ�");
			}
			else {
				strcpy_s(endMessage, sizeof(endMessage), "��Ӯ�ˣ�");
			}
			currentState = STATE_END;
			return;
		}



		// �л������
		pos.player = Black;
	}
}


/*    ������Ӻ���
//AI����
void aiMove() {
	// ���ѡ��һ���յĸ���
	int row, col;
	do {
		row = rand() % ROW;
		col = rand() % COL;
	} while (map[row][col] != Nome);  // �ҵ�һ���յ�λ��

	map[row][col] = White;  // AI����Ϊ����

	// �ж��Ƿ�����һ�ʤ
	if (judge(row, col)) {
		MessageBox(GetHWnd(), "AIӮ��", "Game Over", MB_OK);
		exit(0);
	}

	// �л����
	pos.player = Black;  // �ֵ��������
}
*/

bool judge(int r, int c) {
	int who = map[r][c]; // ��ǰ���ӵ����

	// �����ĸ�����ˮƽ����ֱ�����Խ��ߡ����Խ���
	int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };

	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];

		// ���㵱ǰ�����ϵ�������������
		int count = 1; // ��ǰ����������һ��

		// ����������
		for (int step = 1; step < 6; step++) { // ����������
			int newRow = r + step * dr;
			int newCol = c + step * dc;
			if (newRow >= 0 && newRow < ROW && newCol >= 0 && newCol < COL && map[newRow][newCol] == who) {
				count++;
			}
			else {
				break;
			}
		}

		// �򷴷�����
		for (int step = 1; step < 6; step++) { // ����������
			int newRow = r - step * dr;
			int newCol = c - step * dc;
			if (newRow >= 0 && newRow < ROW && newCol >= 0 && newCol < COL && map[newRow][newCol] == who) {
				count++;
			}
			else {
				break;
			}
		}

		// �ж��Ƿ��γɳ�������������ࣩ
		if (count >= 6) {
			if (who == Black) {
				// �����γɳ������ж�����Ϊ��
				strcpy_s(endMessage, sizeof(endMessage), "�����γɳ��������帺��");
				currentState = STATE_END;
				return false;
			}
			else {
				// �����γɳ������ж�����ΪӮ
				strcpy_s(endMessage, sizeof(endMessage), "�����γɳ���������Ӯ�ˣ�");
				currentState = STATE_END;
				return true;
			}
		}

		// �ж��Ƿ��γ�����
		if (count == 5) {
			if (who == Black) {
				// �����γ��������ж�����ΪӮ
				strcpy_s(endMessage, sizeof(endMessage), "����Ӯ�ˣ�");
				currentState = STATE_END;
				return true;
			}
			else {
				// �����γ��������ж�����ΪӮ
				strcpy_s(endMessage, sizeof(endMessage), "����Ӯ�ˣ�");
				currentState = STATE_END;
				return true;
			}
		}
	}

	// ���û���ҵ������������򷵻�false
	return false;
}


// ����
void undoMove() {
	if (history_index > 0) {
		// �����AIģʽ��ÿ�γ��������壨��Һ�AI��һ�֣�
		if (isAI) {
			// ����AI�����һ��
			history_index--;
			if (history_index >= 0) {
				int ai_row = history[history_index].row;
				int ai_col = history[history_index].col;
				map[ai_row][ai_col] = Nome;
				// �л������
				pos.player = (pos.player == Black) ? White : Black;
			}

			// ������ҵ����һ��
			if (history_index > 0) {
				history_index--;
				int player_row = history[history_index].row;
				int player_col = history[history_index].col;
				map[player_row][player_col] = Nome;
				// �л���AI
				pos.player = (pos.player == Black) ? White : Black;
			}
		}
		else {
			// ��AIģʽ��ֻ����һ����
			history_index--;
			int last_row = history[history_index].row;
			int last_col = history[history_index].col;
			map[last_row][last_col] = Nome;
			// �л����
			pos.player = (pos.player == Black) ? White : Black;
		}

		// ���ý�����Ϣ״̬�����֮ǰ����Ϸ����״̬��
		if (currentState == STATE_END) {
			currentState = STATE_PLAYING;
			endMessageShown = false;
			endMessage[0] = '\0'; // �����Ϣ����
		}
	}
}


//*****************************************************����************************************
// �������ض�������������ͬ���ӵ�����
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

// ����Ƿ��γɻ���
int countAliveThree(int row, int col, int player) {
	int aliveThree = 0;
	// �ĸ�����ˮƽ����ֱ�����Խ��ߡ����Խ���
	int directions[4][2] = { {1,0}, {0,1}, {1,1}, {1,-1} };
	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];
		// ��鵱ǰ�����Ƿ��л���
		int count1 = countConsecutive(row, col, dr, dc, player);
		int count2 = countConsecutive(row, col, -dr, -dc, player);
		if (count1 + count2 + 1 == 3) { // ������ǰ�µ�����
			// ��������Ƿ�Ϊ��
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

// ����Ƿ��γɻ���
int countAliveFour(int row, int col, int player) {
	int aliveFour = 0;
	// �ĸ�����ˮƽ����ֱ�����Խ��ߡ����Խ���
	int directions[4][2] = { {1,0}, {0,1}, {1,1}, {1,-1} };
	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];
		// ��鵱ǰ�����Ƿ��л���
		int count1 = countConsecutive(row, col, dr, dc, player);
		int count2 = countConsecutive(row, col, -dr, -dc, player);
		if (count1 + count2 + 1 == 4) { // ������ǰ�µ�����
			// ��������Ƿ�Ϊ��
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

// ����Ƿ��γɳ�������������ࣩ
bool isLongConnect(int row, int col, int player) {
	// �ĸ�����ˮƽ����ֱ�����Խ��ߡ����Խ���
	int directions[4][2] = { {1,0}, {0,1}, {1,1}, {1,-1} };
	for (int i = 0; i < 4; i++) {
		int dr = directions[i][0];
		int dc = directions[i][1];
		int count = 1; // ������ǰ�µ�����
		count += countConsecutive(row, col, dr, dc, player);
		count += countConsecutive(row, col, -dr, -dc, player);
		if (count >= 6) return true;
	}
	return false;
}

// �����ֹ���
bool isForbiddenMove(int row, int col, int player) {
	if (player != Black) return false; // ֻ�к����н��ֹ���

	int aliveThree = countAliveThree(row, col, player);
	int aliveFour = countAliveFour(row, col, player);
	bool longConnect = isLongConnect(row, col, player);

	// �������֣��γ���������
	if (aliveThree >= 2) return true;

	// ���Ľ��֣��γ���������
	if (aliveFour >= 2) return true;

	// �������֣��γ����������
	if (longConnect) return true;

	// �������֣��γ�һ�����ĺ�һ������
	if (aliveFour >= 1 && aliveThree >= 1) return true;

	return false;
}
//********************************************����***************************************
// ������ť�����ȴ����
bool soundButtonClicked = false;
DWORD lastClickTime = 0;

// ����������ť�ĵ���¼�
void handleSoundButtonClick(ExMessage* msg) {
	// ��ȡ��ǰʱ��
	DWORD currentTime = GetTickCount();

	// ������ȴʱ��Ϊ200����
	if (currentTime - lastClickTime < 200) {
		return; // ��ȴʱ���ں��Ե��
	}

	// ������Ƿ���������ť������
	if (msg->x >= SOUND_BUTTON_X && msg->x <= (SOUND_BUTTON_X + SOUND_BUTTON_WIDTH) &&
		msg->y >= SOUND_BUTTON_Y && msg->y <= (SOUND_BUTTON_Y + SOUND_BUTTON_HEIGHT)) {
		// �л�����״̬
		soundOn = !soundOn;

		if (soundOn) {
			// ��������
			mciSendString("play bgm repeat", NULL, 0, NULL);
		}
		else {
			// �ر�����
			mciSendString("stop bgm", NULL, 0, NULL);
		}

		// �����ϴε��ʱ��
		lastClickTime = currentTime;
	}
}


int main() {
	// ������Դ����ʼ��ҳ������̱�����
	loadResource();

	// ��ʼ��״̬Ϊ���˵�
	currentState = STATE_MENU;

	// ��ʼ���������
	srand(time(NULL));

	// ��ѭ��
	while (true) {
		ExMessage msg;
		bool hasMessage = peekmessage(&msg, EX_MOUSE);

		BeginBatchDraw();

		// ���ݵ�ǰ״̬������Ӧ��ҳ��
		if (currentState == STATE_MENU) {
			drawInitPage();
		}
		else if (currentState == STATE_PLAYING) {
			drawGamePage();  // �������̺�����
		}
		else if (currentState == STATE_END) {
			drawEndPage();
		}

		EndBatchDraw();

		// ��ʾ������Ϣ��
		if (currentState == STATE_END && !endMessageShown) {
			MessageBoxA(GetHWnd(), endMessage, "��Ϸ����", MB_OK);
			endMessageShown = true;
		}

		if (hasMessage) {
			if (msg.message == WM_LBUTTONDOWN) {
				// �ȼ���Ƿ�����������ť����
				if (msg.x >= SOUND_BUTTON_X && msg.x <= (SOUND_BUTTON_X + SOUND_BUTTON_WIDTH) &&
					msg.y >= SOUND_BUTTON_Y && msg.y <= (SOUND_BUTTON_Y + SOUND_BUTTON_HEIGHT)) {
					handleSoundButtonClick(&msg);
				}
				else {
					// ����������ť���
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
				// Ҳ����������״̬�´�������ƶ��Ը���ȫ�����λ��
				currentMouseX = msg.x;
				currentMouseY = msg.y;
			}
		}

		// ������˻���ս���ֵ�AI���壬����AI����
		if (currentState == STATE_PLAYING && isAI && pos.player == White) {
			aiDecision();
		}

		Sleep(10); // ����CPUռ��
	}

	// ���ᵽ������
	getchar(); // ���ִ���
	return 0;
}