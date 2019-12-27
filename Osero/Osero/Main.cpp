#include "Main.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <MMSystem.h>

/*	盤面の大きさ	*/
#define BOARD_SIZE 10

/*	マスの状態	*/
#define WALL -999		//<<盤外
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define BLACKCAN 3
#define WHITECAN 4

/* CPU用の初期化定数 */
#define INITSTATE -50

/* 一時停止 */
#define PAUSE getchar()

/* 盤面の状態 */
int g_CurrentBoard[BOARD_SIZE][BOARD_SIZE];		//<<現在の盤面
int g_Current2Board[BOARD_SIZE][BOARD_SIZE];	//<<２層目用

/* 盤面の保存用配列 */
int g_PreBoard[100][BOARD_SIZE][BOARD_SIZE];

/* 盤面の場所を文字で表す用 */
char g_PrintBord_s[BOARD_SIZE][BOARD_SIZE];	//<<名前を後で変更すること

/* 棋譜用配列 */
char g_White_kifu[30];
char g_Black_kifu[30];

/* 一手戻すようカウント変数 */
int g_PreCount = EMPTY;

/* 何回戻すか用変数 */
int g_UndoCount = EMPTY;

/* 盤の評価点数 */
int g_BoardScore[BOARD_SIZE][BOARD_SIZE];

/* ターン数 */
int g_Turn = EMPTY;

/* パスカウンター */
bool g_Path = false;

/* 現在のターンの色 */
int g_CurrentColor = EMPTY;

/* 音楽再生確認用変数 */
bool g_PlayMusic = false;

/* やり直し用関数 */
bool g_Continue = false;

/* ターンカウントのバックアップ用変数 */
int g_BackupCount = EMPTY;

/* 自動パス用変数 */
bool g_IsAutoPath = false;

/* どのタイミングで自動パスされたか保存する変数 */
bool g_PathSave[100] = { false };

/* CANが入っている場所を保存している変数 */
int g_WhiteCans[40][2];
int g_BlackCans[40][2];
int g_WhiteCans_2[40][2];	//<<２層目

/* CANの数 */
int g_BlkPos = EMPTY;

/* CPUの手札 */
int g_Ehand_x = EMPTY;
int g_Ehand_y = EMPTY;

/* 現在のターン数 */
int g_CurrentTurn = EMPTY;

/* 両方置けない時はその時点でゲーム終了変数 */
bool g_IsWhitePath = false;
bool g_IsBlackePath = false;
int g_GameEndCount = 0;	//<<両方の変数がtrueになったかどうか判定する用の変数
bool g_IsGameEnd = false;

/* 8方向 */
enum Direction
{
	Up,		//<<上
	Down,	//<<下
	Right,	//<<右
	Left,	//<<左
	Dia_up_right,	//<<右上
	Dia_down_right,	//<<右下
	Dia_up_left,		//<<左上
	Dia_down_left,	//<<左下
};

/* 盤面の状態 */
enum CpuLevel
{
	saisyo,		//<<序盤
	tyubann,	//<<中盤
	last		//<<終盤
};

/* WHITECAN関数の中が空かどうか */
bool IsWhite(int _pos)
{
	if (g_WhiteCans[_pos][0] == EMPTY)	return true;
	return false;
}

bool IsBlack(int _pos)
{
	if (g_BlackCans[_pos][0] == EMPTY)	return true;
	return false;
}

/* 文字変換関数 */
int Ctoi(char _c)
{
	if (_c >= '0' && _c <= '9')
	{
		return _c - '0';
	}

	return 0;
}

/* 入力された文字が数字かどうか判別する関数 */
bool IsNumber(char _moji)
{
	if ((_moji >= 'a'&& _moji <= 'z')
		|| (_moji >= 'A' && _moji <= 'Z')
		|| (_moji == '+'))
	{
		return false;
	}

	return true;
}

/* 現在の色を返す */
int GetCurrentColor()
{
	return g_Turn % 2 + 1;
}

/* 盤面の初期化 */
void Init_Board(int _bord[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		_bord[0][i] = _bord[i][0] = _bord[9][i] = _bord[i][9] = WALL;
	}

	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			_bord[i][j] = EMPTY;
		}
	}

	_bord[4][5] = _bord[5][4] = BLACK;
	_bord[4][4] = _bord[5][5] = WHITE;
}

/* 盤面の重み初期化 */
void Init_Score(int _bscore[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		_bscore[0][i] = _bscore[i][0] = _bscore[9][i] = _bscore[i][9] = WALL;
	}

	for (int t = 1; t <= 8; t++)
	{
		for (int g = 1; g <= 8; g++)
		{
			_bscore[t][g] = 0;
		}
	}

	_bscore[1][1] = _bscore[1][8] = _bscore[8][1] = _bscore[8][8] = 30;
	_bscore[1][2] = _bscore[2][1] = _bscore[1][7] = _bscore[2][8] = -12;
	_bscore[7][1] = _bscore[8][2] = _bscore[7][8] = _bscore[8][7] = -12;
	_bscore[2][2] = _bscore[2][7] = _bscore[7][2] = _bscore[7][7] = -15;
	_bscore[2][3] = _bscore[2][4] = _bscore[2][5] = _bscore[2][6] = -3;
	_bscore[3][2] = _bscore[4][2] = _bscore[5][2] = _bscore[6][2] = -3;
	_bscore[3][7] = _bscore[4][7] = _bscore[5][7] = _bscore[6][7] = -3;
	_bscore[7][3] = _bscore[7][4] = _bscore[7][5] = _bscore[7][6] = -3;
	_bscore[1][4] = _bscore[1][5] = _bscore[4][1] = _bscore[5][1] = -1;
	_bscore[8][4] = _bscore[8][5] = _bscore[4][8] = _bscore[5][8] = -1;

	for (int k = 3; k <= 6; k++)
	{
		for (int b = 3; b <= 6; b++)
		{
			_bscore[k][b] = -1;
		}
	}

	_bscore[3][3] = _bscore[3][6] = _bscore[6][3] = _bscore[6][6] = 0;

}

/* TODO 盤面を文字で場所が分かるように */
void Init_BordName(char _bord[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		_bord[0][i] = _bord[i][0] = _bord[9][i] = _bord[i][9] = 'abcd';
	}

	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			_bord[i][j] = EMPTY;
		}
	}
}

/* 盤面の表示 */
void Print_Board(int _bord[BOARD_SIZE][BOARD_SIZE])
{
	printf("\n\n\n");
	printf("    a b c d e f g h \n");
	printf("    1 2 3 4 5 6 7 8 \n");
	printf("   -----------------\n");

	for (int i = 1; i <= 8; i++)
	{
		printf("%d |", i);
		for (int j = 1; j <= 8; j++)
		{
			if (_bord[i][j] == BLACK)
			{
				printf("〇");
			}
			else if (_bord[i][j] == WHITE)
			{
				printf("●");
			}
			else if (_bord[i][j] == EMPTY)
			{
				printf("  ");
			}

			switch (GetCurrentColor())
			{
			case BLACK:
				if (_bord[i][j] == BLACKCAN)
				{
					printf("＋");
				}
				break;
			case WHITE:
				if (_bord[i][j] == WHITECAN)
				{
					printf("＋");
				}
				break;
			}
		}

		printf(" |\n");
	}

	printf("   -----------------\n");
}

/* ルールの表示 */
void PrintRule()
{
	/* TODO 最後に内容を確認するように */
	printf("\n		ルールの説明です! \n");
	printf(" 1 : 8×8 のゲーム盤上に，４個の石が初期状態としておいてある。\n");
	printf(" 2 : 交互に自分の色の石を置いていく。\n");
	printf(" 3 : 石をおける場所は，自分の色の石で相手の石が１個以上 \n");
	printf("続 : 挟める所である。挟む方向は，縦横斜めのどれでもよい。\n");
	printf(" 4 : このとき，挟まれた石は，自分の色の石となる。\n");
	printf(" 5 : 石をおける場所がない場合には，パスをする。\n");
	printf(" 6 : 石をおける場所がある場合には，パスはできない。\n");
	printf(" 7 : 両者とも石をおける場所がなくなった場合，ゲームは終了する。\n");
	printf(" 8 : ゲーム終了時の石の個数で勝敗を決める。\n");
	printf(" 9 : 石を置くときは座標を入力してもらいます。 \n");
	printf("10 : 例　 3、 5と入力すると、 \n");
	printf("続 : 左から３番目、上から５番目の場所に石を置く事が出来ます。\n");
	printf("11 : 他の石が置いていたり壁があると置く事が出来ません。\n");
	printf("12 : もし対戦時の入力を間違えたら場合は、2回目の入力の時に　\n");
	printf("続 : 2桁の数字を入力すると、最初から入力することが出来ます。\n");
	printf("13 : 音楽はー２を入力すると停止します。\n");
	printf("14 : 石を置くことが出来る場所には + が置いてあります。\n");
	printf("15 : 三ターン目以降から、u ボタンで一手戻す事が出来ます。\n");
	printf("頑張って！！！\n\n");
}

/* 裏返りがあるかどうかを調べる関数 */
bool IsReverseCheck(int _d, int _c, int _r, int _color, bool _p, int _bord[BOARD_SIZE][BOARD_SIZE])
{
	int Direction = _d, Column = _c, Row = _r, Color = _color;
	bool Plus = _p;

	int Clm_D = Column + 1, Clm_U = Column - 1;
	int Ro_R = Row + 1, Ro_L = Row - 1;

	switch (Direction)
	{
	case Up:
		if ((_bord[Clm_U][Row] == WALL)
			|| (_bord[Clm_U][Row] == EMPTY)
			|| (_bord[Clm_U][Row] == Color))
		{
			return false;
		}
		break;
	case Down:
		if ((_bord[Clm_D][Row] == WALL)
			|| (_bord[Clm_D][Row] == EMPTY)
			|| (_bord[Clm_D][Row] == Color))
		{
			return false;
		}
		break;
	case Right:
		if ((_bord[Column][Ro_R] == WALL)
			|| (_bord[Column][Ro_R] == EMPTY)
			|| (_bord[Column][Ro_R] == Color))
		{
			return false;
		}
		break;
	case Left:
		if ((_bord[Column][Ro_L] == WALL)
			|| (_bord[Column][Ro_L] == EMPTY)
			|| (_bord[Column][Ro_L] == Color))
		{
			return false;
		}
		break;
	case Dia_up_right:
		if ((_bord[Clm_U][Row + 1] == WALL)
			|| (_bord[Clm_U][Row + 1] == EMPTY)
			|| (_bord[Clm_U][Row + 1] == Color))
		{
			return false;
		}
		break;
	case Dia_down_right:
		if ((_bord[Column + 1][Row + 1] == WALL)
			|| (_bord[Column + 1][Row + 1] == EMPTY) 
			|| (_bord[Column + 1][Row + 1] == Color))
		{
			return false;
		}
		break;
	case Dia_up_left:
		if ((_bord[Clm_U][Row - 1] == WALL) 
			|| (_bord[Clm_U][Row - 1] == EMPTY)
			|| (_bord[Clm_U][Row - 1] == Color))
		{
			return false;
		}
		break;
	case Dia_down_left:
		if ((_bord[Column + 1][Row - 1] == WALL)
			|| (_bord[Column + 1][Row - 1] == EMPTY)
			|| (_bord[Column + 1][Row - 1] == Color))
		{
			return false;
		}
		break;
	}

	/* Plus変数がtrueなら,置いた先に別の色の石がある状態なので
	  その先を見て同じ色ならtrueを別の色ならばfalseを返す
	 */

	if (Plus)
	{
		switch (Direction)
		{
		case Up:
			for (int i = 2; i < 8; i++)
			{
				if ((g_CurrentBoard[Column - i][Row] == WALL)
					|| (g_CurrentBoard[Column - i][Row] == EMPTY))
				{
					return false;
				}
				else if (g_CurrentBoard[Column - i][Row] == Color)
				{
					return true;
				}
			}
			break;
		case Down:
			for (int i = 2; i < 8; i++)
			{
				if ((g_CurrentBoard[Column + i][Row] == WALL)
					|| (g_CurrentBoard[Column + i][Row] == EMPTY))
				{
					return false;
				}
				else if (g_CurrentBoard[Column + i][Row] == Color)
				{
					return true;
				}
			}
			break;
		case Right:
			for (int i = 2; i < 8; i++)
			{
				if ((g_CurrentBoard[Column][Row + i] == WALL) 
					|| (g_CurrentBoard[Column][Row + i] == EMPTY))
				{
					return false;
				}
				else if (g_CurrentBoard[Column][Row + i] == Color)
				{
					return true;
				}
			}
			break;
		case Left:
			for (int i = 2; i < 8; i++)
			{
				if ((g_CurrentBoard[Column][Row - i] == WALL)
					|| (g_CurrentBoard[Column][Row - i] == EMPTY))
				{
					return false;
				}
				else if (g_CurrentBoard[Column][Row - i] == Color)
				{
					return true;
				}
			}
			break;
		case Dia_up_right:
			for (int i = 2; i < 8; i++)
			{
				if ((g_CurrentBoard[Column - i][Row + i] == WALL)
					|| (g_CurrentBoard[Column - i][Row + i] == EMPTY))
				{
					return false;
				}
				else if (g_CurrentBoard[Column - i][Row + i] == Color)
				{
					return true;
				}
			}
			break;
		case Dia_down_right:
			for (int i = 2; i < 8; i++)
			{
				if ((g_CurrentBoard[Column + i][Row + i] == WALL) 
					|| (g_CurrentBoard[Column + i][Row + i] == EMPTY))
				{
					return false;
				}
				else if (g_CurrentBoard[Column + i][Row + i] == Color)
				{
					return true;
				}
			}
			break;
		case Dia_up_left:
			for (int i = 2; i < 8; i++)
			{
				if ((g_CurrentBoard[Column - i][Row - i] == WALL) 
					|| (g_CurrentBoard[Column - i][Row - i] == EMPTY))
				{
					return false;
				}
				else if (g_CurrentBoard[Column - i][Row - i] == Color)
				{
					return true;
				}
			}
			break;
		case Dia_down_left:
			for (int i = 2; i < 8; i++)
			{
				if ((g_CurrentBoard[Column + i][Row - i] == WALL)
					|| (g_CurrentBoard[Column + i][Row - i] == EMPTY))
				{
					return false;
				}
				else if (g_CurrentBoard[Column + i][Row - i] == Color)
				{
					return true;
				}
			}

			break;
		}
	}

	return true;
}

/* 石を裏返す */
void ReverseStone(int _c, int _r, int _color, int _bord[BOARD_SIZE][BOARD_SIZE])
{
	int Color = _color, Column = _c, Row = _r, Count = 0;
	if ((Column == 0) && (Row == 0)) return;  //<<白がパスの場合は何もせずに返す

	/* 現在の色を貰っている */
	int NowCans = GetCurrentColor() + 2;			
	if ((NowCans == 1) || (NowCans == 2))
	{
		NowCans += 2;
	}

	/* trueで返ってきた方向を保存して,下の処理でその方向を見に行く */
	bool direction[8] = { false };
	for (int d = 0; d < 8; d++)
	{
		direction[d] = IsReverseCheck(d, Column, Row, Color, false, _bord);
	}

	for (int d = 0; d < 8; d++)
	{
		switch (d)
		{
		case Up:
			if (direction[Up] == true)
			{
				for (int j = 1; j < 8; j++)
				{
					/* 置いた石の上に壁か空白ならその時点でストップ */
					if ((_bord[Column - j][Row] == WALL)
						|| (_bord[Column - j][Row] == EMPTY)
						|| (_bord[Column - j][Row] == NowCans))
					{
						Count = 0;
						break;
					}
					/* 上が置いた石と同じ色なら何も無し */
					else if (_bord[Column - j][Row] == Color)
					{
						if (!Count == 0)
						{
							/* ひっくり返す処理 */
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column - k][Row] = Color;
							}
						}
						Count = 0;
						break;
					}
					/* 別の色の石なら最後に一気に裏返す用のカウント進める */
					else if (_bord[Column - j][Row] != Color)
					{
						Count++;
					}
				}
			}
			break;
		case Down:
			if (direction[Down] == true)
			{
				for (int j = 1; j < 8; j++)
				{
					if ((_bord[Column + j][Row] == WALL) 
						|| (_bord[Column + j][Row] == EMPTY)
						|| (_bord[Column + j][Row] == NowCans))
					{
						Count = 0;
						break;
					}
					else if (_bord[Column + j][Row] == Color)
					{
						if (!Count == 0)
						{
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column + k][Row] = Color;
							}
						}
						Count = 0;
						break;
					}
					else if (_bord[Column + j][Row] != Color)
					{
						Count++;
					}
				}
			}
			break;
		case Right:
			if (direction[Right] == true)
			{
				for (int j = 1; j < 8; j++)
				{
					if ((_bord[Column][Row + j] == WALL) 
						|| (_bord[Column][Row + j] == EMPTY)
						|| (_bord[Column][Row + j] == NowCans))
					{
						Count = 0;
						break;
					}
					else if (_bord[Column][Row + j] == Color)
					{
						if (!Count == 0)
						{
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column][Row + k] = Color;
							}
						}
						Count = 0;
						break;
					}
					else if (_bord[Column][Row + j] != Color)
					{
						Count++;
					}
				}
			}
			break;
		case Left:
			if (direction[Left] == true)
			{
				for (int j = 1; j < 8; j++)
				{
					if ((_bord[Column][Row - j] == WALL) 
						|| (_bord[Column][Row - j] == EMPTY)
						|| (_bord[Column][Row - j] == NowCans))
					{
						Count = 0;
						break;
					}
					else if (_bord[Column][Row - j] == Color)
					{
						if (!Count == 0)
						{
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column][Row - k] = Color;
							}
						}
						Count = 0;
						break;
					}
					else if (_bord[Column][Row - j] != Color)
					{
						Count++;
					}
				}
			}
			break;
		case Dia_up_right:
			if (direction[Dia_up_right] == true)
			{
				for (int j = 1; j < 8; j++)
				{
					if ((_bord[Column - j][Row + j] == WALL)
						|| (_bord[Column - j][Row + j] == EMPTY)
						|| (_bord[Column - j][Row + j] == NowCans))
					{
						Count = 0;
						break;
					}
					else if (_bord[Column - j][Row + j] == Color)
					{
						if (!Count == 0)
						{
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column - k][Row + k] = Color;
							}
						}
						Count = 0;
						break;
					}
					else if (_bord[Column - j][Row + j] != Color)
					{
						Count++;
					}
				}
			}
			break;
		case Dia_down_right:
			if (direction[Dia_down_right] == true)
			{
				for (int j = 1; j < 8; j++)
				{
					if ((_bord[Column + j][Row + j] == WALL)
						|| (_bord[Column + j][Row + j] == EMPTY)
						|| (_bord[Column + j][Row + j] == NowCans))
					{
						Count = 0;
						break;
					}
					else if (_bord[Column + j][Row + j] == Color)
					{
						if (!Count == 0)
						{
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column + k][Row + k] = Color;
							}
						}
						Count = 0;
						break;
					}
					else if (_bord[Column + j][Row + j] != Color)
					{
						Count++;
					}
				}
			}
			break;
		case Dia_up_left:
			if (direction[Dia_up_left] == true)
			{
				for (int j = 1; j < 8; j++)
				{
					if ((_bord[Column - j][Row - j] == WALL)
						|| (_bord[Column - j][Row - j] == EMPTY)
						|| (_bord[Column - j][Row - j] == NowCans))
					{
						Count = 0;
						break;
					}
					else if (_bord[Column - j][Row - j] == Color)
					{
						if (!Count == 0)
						{
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column - k][Row - k] = Color;
							}
						}
						Count = 0;
						break;
					}
					else if (_bord[Column - j][Row - j] != Color)
					{
						Count++;
					}
				}
			}
			break;
		case Dia_down_left:
			if (direction[Dia_down_left] == true)
			{
				for (int j = 1; j < 8; j++)
				{
					if ((_bord[Column + j][Row - j] == WALL)
						|| (_bord[Column + j][Row - j] == EMPTY) 
						|| (_bord[Column + j][Row - j] == NowCans))
					{
						Count = 0;
						break;
					}
					else if (_bord[Column + j][Row - j] == Color)
					{
						if (!Count == 0)
						{
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column + k][Row - k] = Color;
							}
						}
						Count = 0;
						break;
					}
					else if (_bord[Column + j][Row - j] != Color)
					{
						Count++;
					}
				}
			}

			break;
		}
	}

	int kk = 0;
}

/*  SerchStone関数用 一つ先が違う色の場合のみtrueを返す */
bool IsAnotherColor(int _d, int _c, int _r, int _color, int _bord[BOARD_SIZE][BOARD_SIZE])
{
	int Direction = _d, Column = _c, Row = _r, Color = _color;
	int Cancolor = Color + 2;
	int Clm_D = Column + 1, Clm_U = Column - 1;
	int Ro_R = Row + 1, Ro_L = Row - 1;

	switch (Direction)
	{
	case Up:
		if ((_bord[Clm_U][Row] == WALL) 
			|| (_bord[Clm_U][Row] == EMPTY)
			|| (_bord[Clm_U][Row] == Color)
			|| (_bord[Clm_U][Row] == Cancolor))
		{
			return false;
		}
		break;
	case Down:
		if ((_bord[Clm_D][Row] == WALL) 
			|| (_bord[Clm_D][Row] == EMPTY)
			|| (_bord[Clm_D][Row] == Color)
			|| (_bord[Clm_D][Row] == Cancolor))
		{
			return false;
		}
		break;
	case Right:
		if ((_bord[Column][Ro_R] == WALL)
			|| (_bord[Column][Ro_R] == EMPTY)
			|| (_bord[Column][Ro_R] == Color)
			|| (_bord[Column][Ro_R] == Cancolor))
		{
			return false;
		}
		break;
	case Left:
		if ((_bord[Column][Ro_L] == WALL)
			|| (_bord[Column][Ro_L] == EMPTY)
			|| (_bord[Column][Ro_L] == Color)
			|| (_bord[Column][Ro_L] == Cancolor))
		{
			return false;
		}
		break;
	case Dia_up_right:
		if ((_bord[Clm_U][Ro_R] == WALL)
			|| (_bord[Clm_U][Ro_R] == EMPTY)
			|| (_bord[Clm_U][Ro_R] == Color)
			|| (_bord[Clm_U][Ro_R] == Cancolor))
		{
			return false;
		}
		break;
	case Dia_down_right:
		if ((_bord[Clm_D][Ro_R] == WALL)
			|| (_bord[Clm_D][Ro_R] == EMPTY)
			|| (_bord[Clm_D][Ro_R] == Color)
			|| (_bord[Clm_D][Ro_R] == Cancolor))
		{
			return false;
		}
		break;
	case Dia_up_left:
		if ((_bord[Clm_U][Ro_L] == WALL)
			|| (_bord[Clm_U][Ro_L] == EMPTY)
			|| (_bord[Clm_U][Ro_L] == Color)
			|| (_bord[Clm_U][Ro_L] == Cancolor))
		{
			return false;
		}
		break;
	case Dia_down_left:
		if ((_bord[Clm_D][Ro_L] == WALL)
			|| (_bord[Clm_D][Ro_L] == EMPTY)
			|| (_bord[Clm_D][Ro_L] == Color)
			|| (_bord[Clm_D][Ro_L] == Cancolor))
		{
			return false;
		}
		break;
	}

	return true;
}

/* 石をどこの場所に置けるか調べる関数 */
void SerchStone(int _bord[BOARD_SIZE][BOARD_SIZE], int _color)
{
	int MyColor = _color;
	bool DirectionCount[8] = { false };
	int AutoPathCount = 0;			//<<　自動パス用変数

	for (int i = 1; i < 9; i++)
	{
		for (int j = 1; j < 9; j++)
		{
			/* ここで全方向見に行けるようにしている */
			for (int q = 0; q < 8; q++)
			{
				DirectionCount[q] = false;
			}

			/* 自分の色の石を見つけ,その場所から回りを見ていく */
			if (_bord[i][j] == MyColor)
			{
				for (int d = 0; d < 8; d++)
				{
					switch (d)
					{
					case Up:
						if (IsAnotherColor(d, i, j, MyColor, _bord))
						{
							for (int k = 1; k < 8; k++)
							{
								/* 同じ方向には二回以上は見なくてもいいはず */
								if (!DirectionCount[Up])
								{
									if (_bord[i - k][j] == MyColor)	break;

									/* 壁かCanならBreak*/
									if ((_bord[i - k][j] == BLACKCAN) 
										|| (_bord[i - k][j] == WHITECAN)
										|| (_bord[i - k][j] == WALL))
									{
										break;
									}
									else if (_bord[i - k][j] == EMPTY)
									{
										if (MyColor == BLACK)
										{
											_bord[i - k][j] = BLACKCAN;
											AutoPathCount++;
											if (!IsBlack(g_BlkPos)) g_BlkPos++;
											/* 置ける場所の座標を保存している */
											g_BlackCans[g_BlkPos][0] = i - k;
											g_BlackCans[g_BlkPos][1] = j;
											g_BlkPos++;
										}
										else if (MyColor == WHITE)
										{
											_bord[i - k][j] = WHITECAN;
											AutoPathCount++;
										}

										DirectionCount[Up] = true;
										break;
									}
								}
							}
						}
						break;
					case Down:
						if (IsAnotherColor(d, i, j, MyColor, _bord))
						{
							for (int k = 1; k < 8; k++)
							{
								if (!DirectionCount[Down])
								{
									if (_bord[i + k][j] == MyColor)	break;

									if ((_bord[i + k][j] == BLACKCAN) 
										|| (_bord[i + k][j] == WHITECAN)
										|| (_bord[i + k][j] == WALL))
									{
										break;
									}
									else if (_bord[i + k][j] == EMPTY)
									{
										if (MyColor == BLACK)
										{
											_bord[i + k][j] = BLACKCAN;
											AutoPathCount++;
											if (!IsBlack(g_BlkPos)) g_BlkPos++;
											g_BlackCans[g_BlkPos][0] = i + k;
											g_BlackCans[g_BlkPos][1] = j;
											g_BlkPos++;
										}
										else if (MyColor == WHITE)
										{
											_bord[i + k][j] = WHITECAN;
											AutoPathCount++;
										}

										DirectionCount[Down] = true;
										break;
									}
								}
							}
						}
						break;
					case Right:
						if (IsAnotherColor(d, i, j, MyColor, _bord))
						{
							for (int k = 1; k < 8; k++)
							{
								if (!DirectionCount[Right])
								{
									if (_bord[i][j + k] == MyColor)	break;

									if ((_bord[i][j + k] == BLACKCAN) 
										|| (_bord[i][j + k] == WHITECAN)
										|| (_bord[i][j + k] == WALL))
									{
										break;
									}
									else if (_bord[i][j + k] == EMPTY)
									{
										if (MyColor == BLACK)
										{
											_bord[i][j + k] = BLACKCAN;
											AutoPathCount++;
											if (!IsBlack(g_BlkPos)) g_BlkPos++;
											g_BlackCans[g_BlkPos][0] = i;
											g_BlackCans[g_BlkPos][1] = j + k;
											g_BlkPos++;
										}
										else if (MyColor == WHITE)
										{
											_bord[i][j + k] = WHITECAN;
											AutoPathCount++;
										}

										DirectionCount[Right] = true;
										break;
									}
								}
							}
						}
						break;
					case Left:
						if (IsAnotherColor(d, i, j, MyColor, _bord))
						{
							for (int k = 1; k < 8; k++)
							{
								if (!DirectionCount[Left])
								{
									if (_bord[i][j - k] == MyColor)	break;

									if ((_bord[i][j - k] == BLACKCAN) 
										|| (_bord[i][j - k] == WHITECAN)
										|| (_bord[i][j - k] == WALL))
									{
										break;
									}
									else if (_bord[i][j - k] == EMPTY)
									{
										if (MyColor == BLACK)
										{
											_bord[i][j - k] = BLACKCAN;
											AutoPathCount++;
											if (!IsBlack(g_BlkPos)) g_BlkPos++;
											g_BlackCans[g_BlkPos][0] = i;
											g_BlackCans[g_BlkPos][1] = j - k;
											g_BlkPos++;
										}
										else if (MyColor == WHITE)
										{
											_bord[i][j - k] = WHITECAN;
											AutoPathCount++;
										}

										DirectionCount[Left] = true;
										break;
									}
								}
							}
						}
						break;
					case Dia_up_right:
						if (IsAnotherColor(d, i, j, MyColor, _bord))
						{
							for (int k = 1; k < 8; k++)
							{
								if (!DirectionCount[Dia_up_right])
								{
									if (_bord[i - k][j + k] == MyColor)	break;

									if ((_bord[i - k][j + k] == BLACKCAN)
										|| (_bord[i - k][j + k] == WHITECAN)
										|| (_bord[i - k][j + k] == WALL))
									{
										break;
									}
									else if (_bord[i - k][j + k] == EMPTY)
									{
										if (MyColor == BLACK)
										{
											_bord[i - k][j + k] = BLACKCAN;
											AutoPathCount++;
											if (!IsBlack(g_BlkPos)) g_BlkPos++;
											g_BlackCans[g_BlkPos][0] = i - k;
											g_BlackCans[g_BlkPos][1] = j + k;
											g_BlkPos++;
										}
										else if (MyColor == WHITE)
										{
											_bord[i - k][j + k] = WHITECAN;
											AutoPathCount++;
										}

										DirectionCount[Dia_up_right] = true;
										break;
									}
								}
							}
						}
						break;
					case Dia_down_right:
						if (IsAnotherColor(d, i, j, MyColor, _bord))
						{
							for (int k = 1; k < 8; k++)
							{
								if (!DirectionCount[Dia_down_right])
								{
									if (_bord[i + k][j + k] == MyColor)	break;

									if ((_bord[i + k][j + k] == BLACKCAN) 
										|| (_bord[i + k][j + k] == WHITECAN)
										|| (_bord[i + k][j + k] == WALL))
									{
										break;
									}
									else if (_bord[i + k][j + k] == EMPTY)
									{
										if (MyColor == BLACK)
										{
											_bord[i + k][j + k] = BLACKCAN;
											AutoPathCount++;
											if (!IsBlack(g_BlkPos)) g_BlkPos++;
											g_BlackCans[g_BlkPos][0] = i + k;
											g_BlackCans[g_BlkPos][1] = j + k;
											g_BlkPos++;
										}
										else if (MyColor == WHITE)
										{
											_bord[i + k][j + k] = WHITECAN;
											AutoPathCount++;
										}

										DirectionCount[Dia_down_right] = true;
										break;
									}
								}
							}
						}
						break;
					case Dia_up_left:
						if (IsAnotherColor(d, i, j, MyColor, _bord))
						{
							for (int k = 1; k < 8; k++)
							{
								if (!DirectionCount[Dia_up_left])
								{
									if (_bord[i - k][j - k] == MyColor)	break;

									if ((_bord[i - k][j - k] == BLACKCAN) 
										|| (_bord[i - k][j - k] == WHITECAN)
										|| (_bord[i - k][j - k] == WALL))
									{
										break;
									}
									else if (_bord[i - k][j - k] == EMPTY)
									{
										if (MyColor == BLACK)
										{
											_bord[i - k][j - k] = BLACKCAN;
											AutoPathCount++;
											if (!IsBlack(g_BlkPos)) g_BlkPos++;
											g_BlackCans[g_BlkPos][0] = i - k;
											g_BlackCans[g_BlkPos][1] = j - k;
											g_BlkPos++;
										}
										else if (MyColor == WHITE)
										{
											_bord[i - k][j - k] = WHITECAN;
											AutoPathCount++;
										}

										DirectionCount[Dia_up_left] = true;
										break;
									}
								}
							}
						}
						break;
					case Dia_down_left:
						if (IsAnotherColor(d, i, j, MyColor, _bord))
						{
							for (int k = 1; k < 8; k++)
							{
								if (!DirectionCount[Dia_down_left])
								{
									if (_bord[i + k][j - k] == MyColor)	break;

									if ((_bord[i + k][j - k] == BLACKCAN) 
										|| (_bord[i + k][j - k] == WHITECAN)
										|| (_bord[i + k][j - k] == WALL))
									{
										break;
									}
									else if (_bord[i + k][j - k] == EMPTY)
									{
										if (MyColor == BLACK)
										{
											_bord[i + k][j - k] = BLACKCAN;
											AutoPathCount++;
											if (!IsBlack(g_BlkPos)) g_BlkPos++;
											g_BlackCans[g_BlkPos][0] = i + k;
											g_BlackCans[g_BlkPos][1] = j - k;
											g_BlkPos++;
										}
										else if (MyColor == WHITE)
										{
											_bord[i + k][j - k] = WHITECAN;
											AutoPathCount++;
										}

										DirectionCount[Dia_down_left] = true;
										break;
									}
								}
							}
						}
						break;
					}
				}
			}
		}
	}

	if (AutoPathCount == 0)	g_IsAutoPath = true;
}

/* 音楽再生 */
/* 音楽が不評なのでいったん消します */
//void MusicPlay()
//{
//	if (!PlaySound(TEXT("music\\last-war.wav"), NULL, SND_FILENAME 
//		| SND_ASYNC 
//		| SND_LOOP))
//	{
//		printf("再生できません。");
//		g_PlayMusic = false;
//		return;
//	}
//
//	g_PlayMusic = true;
//}

/* 音楽停止 */
//void MusicStop()
//{
//	PlaySound(NULL, NULL, 0);
//	printf("音楽を消します。\n");
//	g_PlayMusic = false;
//}

/* 文字の条件式及び数字の変換 */
int IsTranceform(char _c)
{
	/* じゃんけんの0用 */
	if (_c == '0')
	{
		return -5;
	}
	/* 音楽の停止 */
	if (_c == '-')
	{
		//MusicStop();
		return 0;
	}

	/* 一手戻す */
	if (_c == 'u')
	{
		g_UndoCount++;
		return -4;
	}

	/* 入力されたものが数字なら変換している */
	if (IsNumber(_c))
	{
		return Ctoi(_c);
	}

	return 0;
}

/* 終了判定 */
bool IsFinish()
{
	/* 置く所が無い状態なら終了 */
	if (g_IsGameEnd) return true;

	/* 全部埋まっていたら終了 */
	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			if ((g_CurrentBoard[i][j] == EMPTY) 
				|| (g_CurrentBoard[i][j] == WHITECAN)
				|| (g_CurrentBoard[i][j] == BLACKCAN))
			{
				return false;
			}
		}
	}

	return true;
}

/* 石の数を数える */
void StoneCount()
{
	int W = 0, B = 0;	//<< 石の数

	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			if (g_CurrentBoard[i][j] == 1)
			{
				B++;
			}
			else if (g_CurrentBoard[i][j] == 2)
			{
				W++;
			}
		}
	}

	printf("黒の石の数は%dです。\n", B);
	printf("白の石の数は%dです。\n", W);

	if (B < W)
	{
		printf("白の勝ちです。\n");
	}
	else if (B == W)
	{
		printf("同点なので、引き分けです。\n");
	}
	else
	{
		printf("黒の勝ちです。\n");
		printf("おめでとうございます!!\n");
	}
}

/* 置けるかどうかの判定関数 */
bool Can_put_stones(int _r, int _c, int _color)
{
	int R = _r, C = _c, Color = _color;
	int CanColor = Color + 2;

	if (g_CurrentBoard[C][R] != CanColor)
	{
		return false;
	}

	return true;
}

/* 現在どちらのターンなのかの宣言関数 */
void PrintCurrentTurn()
{
	g_CurrentColor = GetCurrentColor();

	switch (g_CurrentColor)
	{
	case BLACK:
		printf("黒のターンです。\n");
		break;
	case WHITE:
		printf("白のターンです。\n");
		break;
	}
}

/* 盤面の状態を一手戻す関数 */
void BordUndo()
{
	/* ２ターン目以降からじゃ無いと戻す事は出来ない */
	if (g_CurrentTurn < 3)
	{
		printf("戻すことが出来ません。\n");
		return;
	}

	/* 一周前に戻している */
	g_CurrentTurn -= 2;
	g_BackupCount -= 2;

	/* 前回のターンに自動パスをされていたらその一周前に戻る */
	for (int f = 0; f < 10; f++)
	{
		if (g_PathSave[g_CurrentTurn])
		{
			g_CurrentTurn -= 2;
		}
		else
		{
			break;
		}
	}

	/* コピーをしている */
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			g_CurrentBoard[i][j] = g_PreBoard[g_CurrentTurn][i][j];
		}
	}

	printf("一手戻しました。\n");

	/* 戻したのでその前の盤面を表示しなければいけない */
	SerchStone(g_CurrentBoard, GetCurrentColor());
	Print_Board(g_CurrentBoard);
}

/* CPU戦 */
void PlotCPU()
{
	/* まず打てる手を貰う */
	/* WHITECANが入ってる場所を変数で保存しておく */
	int Vir_x = 0, Vir_y = 0;
	int Px = 0, Py = 0;
	int Score = INITSTATE;
	int PScore = INITSTATE;

	/* Canの数 */
	int WhiteCans1 = 0;		//<< WhiteCanの数(１層目)
	int WhiteCans2 = 0;		//<< WhiteCanの数( 2層目)
	int BlackCans = 0;		//<< BlackCanの数(１層目)
	//黒の手
	int Black_Hand_x = 0;
	int Black_Hand_y = 0;
	//黒の評価値
	int Bscore = INITSTATE;
	int Bpscore = INITSTATE;
	//２層目の白の手
	int White_hand_x = 0;
	int White_hand_y = 0;
	//２層目用の評価値
	int Score_2 = 0;
	int PScore_2 = INITSTATE;
	//点数を保存しておく配列
	int ScoreSave[50] = { INITSTATE };
	/* 配列参照用変数 */
	int W_pos = 0;
	int W_pos2 = 0;
	int B_pos = 0;
	int Roop = 0;

	/* 初期化 */
	for (int i = 0; i < 50; i++)
	{
		ScoreSave[i] = INITSTATE;
	}

	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			g_WhiteCans[i][j] = EMPTY;
		}
	}

	/* キャンの数を数えている */
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			g_Current2Board[i][j] = g_CurrentBoard[i][j];
			if (g_Current2Board[i][j] == 4)
			{
				WhiteCans1++;
				g_WhiteCans[W_pos][0] = i;
				g_WhiteCans[W_pos][1] = j;
				W_pos++;
			}
		}
	}

	/* 置くところがない場合 */
	while (WhiteCans1 == 0)
	{
		g_Ehand_x = 0;
		g_Ehand_y = 0;

		for (int i = 0; i < 40; i++)
		{
			for (int j = 0; j <= 2; j++)
			{
				g_WhiteCans[i][j] = EMPTY;
			}
		}
		W_pos = 0;
		SerchStone(g_Current2Board, WHITE);
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (g_Current2Board[i][j] == 4)
				{
					WhiteCans1++;
					g_WhiteCans[W_pos][0] = i;
					g_WhiteCans[W_pos][1] = j;
					W_pos++;
				}
			}
		}

		/* パス */
		if (Roop == 3)
		{
			printf("置けるところがありません.................\n");
			g_Ehand_x = 0;
			g_Ehand_y = 0;
			g_Path = true;
			g_IsWhitePath = true;
			return;
		}

		g_CurrentTurn = g_BackupCount;
		Roop++;
	}

	/*　１層目のWhiteCanの数だけ回す　*/
	for (int wcans = 0; wcans < WhiteCans1; wcans++)
	{
		/* 仮想盤面を作って代入していく */
		for (int k = 0; k < WhiteCans1; k++)
		{
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					if (g_Current2Board[i][j] == 4)
					{
						g_WhiteCans[k][0] = i;
						g_WhiteCans[k][1] = j;
						k++;
					}
				}
			}
		}

		Vir_x = g_WhiteCans[wcans][0];
		Vir_y = g_WhiteCans[wcans][1];

		g_Current2Board[Vir_x][Vir_y] = WHITE;

		Score = g_BoardScore[Vir_x][Vir_y];

		ReverseStone(Vir_x, Vir_y, WHITE, g_Current2Board);

		/* WHITECANの初期化 */
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (g_Current2Board[i][j] == WHITECAN)
				{
					g_Current2Board[i][j] = EMPTY;
				}
			}
		}

		/* BLACKCANを入れている */
		SerchStone(g_Current2Board, BLACK);
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (g_Current2Board[i][j] == 3)
				{
					BlackCans++;
					g_BlackCans[B_pos][0] = i;
					g_BlackCans[B_pos][1] = j;
					B_pos++;
				}
			}
		}

		/* 相手の置く場所が一つもない場合 */
		if (B_pos == 0)
		{
			wcans++;
			ScoreSave[wcans] = Score;
			continue;
		}

		/* 一番評価の良い場所に石を入れている */
		for (int i = 0; i < BlackCans; i++)
		{
			Vir_x = g_BlackCans[i][0];
			Vir_y = g_BlackCans[i][1];

			Bpscore = g_BoardScore[Vir_x][Vir_y];
			if (Bpscore >= Bscore)
			{
				Black_Hand_x = Vir_x;
				Black_Hand_y = Vir_y;
				Bscore = Bpscore;
			}

			/* 一個先に何も入っていなかったら打ち切り */
			if (IsBlack(i + 1)) break;
		}

		/* 仮想盤面に代入しひっくり返している */
		g_Current2Board[Black_Hand_x][Black_Hand_y] = BLACK;
		ReverseStone(Black_Hand_x, Black_Hand_y, BLACK, g_Current2Board);

		/* BLACKCANの初期化 */
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (g_Current2Board[i][j] == BLACKCAN)
				{
					g_Current2Board[i][j] = EMPTY;
				}
			}
		}

		/* １層目のWHITECANのコピー＆初期化 */
		for (int i = 0; i < 40; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				g_WhiteCans_2[i][0] = g_WhiteCans[i][0];
				g_WhiteCans_2[i][1] = g_WhiteCans[i][1];
			}
		}

		/* バグ用 */
		if (g_CurrentTurn == 0)
		{
			g_CurrentTurn = g_BackupCount;
		}

		for (int i = 0; i < 40; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				g_WhiteCans[i][j] = EMPTY;
			}
		}

		/* ２層目のWHITECANを入れる */
		SerchStone(g_Current2Board, WHITE);
		/* 数えている */
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (g_Current2Board[i][j] == 4)
				{
					WhiteCans2++;
					g_WhiteCans[W_pos2][0] = i;
					g_WhiteCans[W_pos2][1] = j;
					W_pos2++;
				}
			}
		}

		/* 実際に入れるわけでは無いのでスコアの計算さえできればいい */
		for (int i = 0; i < WhiteCans2; i++)
		{

			Vir_x = g_WhiteCans[i][0];
			Vir_y = g_WhiteCans[i][1];

			PScore_2 = g_BoardScore[Vir_x][Vir_y];
			if (Score_2 <= PScore_2)
			{
				Score_2 = PScore_2;
			}

			/* 一個先に何も入っていなかったら打ち切り */
			if (IsWhite(i + 1)) break;
		}

		/* 元に戻す */
		for (int i = 0; i < 40; i++)
		{
			for (int j = 0; j <= 2; j++)
			{
				g_WhiteCans[i][0] = g_WhiteCans_2[i][0];
				g_WhiteCans[i][1] = g_WhiteCans_2[i][1];
			}
		}

		for (int i = 0; i <= 40; i++)
		{
			for (int j = 0; j <= 2; j++)
			{
				g_WhiteCans_2[i][j] = EMPTY;
			}
		}

		/* 初期化 */
		g_BlkPos = 0;

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				g_Current2Board[i][j] = g_CurrentBoard[i][j];
			}
		}

		//点数を保存しておく配列
		ScoreSave[wcans] = Score_2 + Score;
		/* 初期化 */
		W_pos2 = 0;
		B_pos = 0;
		BlackCans = 0;
		Roop = 0;
	}

	int scoresin = INITSTATE;
	/* 最終的な点数の優劣をつける */
	for (int w = 0; w < WhiteCans1; w++)
	{
		if (scoresin <= ScoreSave[w])
		{
			scoresin = ScoreSave[w];
			g_Ehand_x = g_WhiteCans[w][0];
			g_Ehand_y = g_WhiteCans[w][1];
		}
	}
}

/* 1層目のみ */
void PlotCPU_new()
{
	/* まず打てる手を貰う */
	/* WHITECANが入ってる場所を変数で保存しておく */
	int Vir_x = 0, Vir_y = 0;
	int Score = INITSTATE;
	int PScore = INITSTATE;

	for (int i = 0; i <= 20; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			Vir_x = g_WhiteCans[i][0];
			Vir_y = g_WhiteCans[i][1];

			PScore = g_BoardScore[Vir_x][Vir_y];

			if (PScore >= Score)
			{
				g_Ehand_x = Vir_x;
				g_Ehand_y = Vir_y;
				Score = PScore;
			}
			else if (PScore == Score)
			{
				g_Ehand_x = Vir_x;
				g_Ehand_y = Vir_y;
				Score = PScore;
			}
		}

		/* 一個先に何も入っていなかったら打ち切り */
		if (IsWhite(i + 1)) break;
	}
}

/* 石を置く */
void PutStone()
{
	int C = 0, R = 0, CurrentColor = 0, Countinue_Count = 0,P = 0;
	char Ro;

	if (g_PreCount == 2)
	{
		printf("一手戻す事が出来る用になりました。\n");
	}

	do
	{
		if (Countinue_Count >= 1)
		{
			printf("\n\n\nその場所には、置く事が出来ません。\n");
			printf("別の場所に置いてください。\n\n");
			Print_Board(g_CurrentBoard);
			printf("行(横)を入力してください\n");
		}
		/* エラー処理 */
		if (g_CurrentColor == 0)
		{
			printf("バグ発生!\n");
			g_CurrentColor = WHITE;
			CurrentColor = WHITE;
		}

		/* CPU戦 */
		if (g_CurrentColor == WHITE)
		{
			CurrentColor = WHITE;
			PlotCPU();
			/* 置くところが無い状況 */
			if ((g_Ehand_x == 0) || (g_Ehand_y == 0))
			{
				printf("パスをしました。\n");
				break;			
			}
			if ((g_Ehand_x == INITSTATE) || (g_Ehand_y == INITSTATE))
			{
				PlotCPU();
			}
			C = g_Ehand_x;
			R = g_Ehand_y;
			if (CurrentColor == WHITE)
			{
				g_Path = false;
			}
			/* パスをしている状態 */
			if ((C == 0) || (R == 0))
			{
				g_Path = true;
			}
		}
		else if (g_CurrentColor == BLACK)
		{
			/* x座標 */

			if (P == 0)
			{
				printf("行(横)を入力してください\n");
				printf("数字以外は入力しないでください！！！\n");
				if (g_PlayMusic) printf("音楽は ー を入力すると停止します。\n");
			}

			/* 何処にも置く場所が無い状態 */
			if (g_IsAutoPath)
			{
				printf("何処にも置く場所がありません。\n");
				g_PathSave[g_Turn] = true;
				g_IsBlackePath = true;
				R = -3;
			}
			else
			{
				scanf_s("%c", &Ro);
				if (!(R = IsTranceform(Ro)))
				{
					printf("数字を入力してください。\n\n");
					P++;
					continue;
				}
			}

			if (R > 8)
			{
				printf("１～８までの数字を入力してください。\n\n");
				P++;
				continue;
			}

			/* パス */
			if (R == -3)
			{
				printf("パスをしました。\n");
				Countinue_Count++;
				g_Path = true;
				break;
			}

			/* 一手戻す */
			if (R == -4)
			{
				BordUndo();
				continue;
			}

			/* y座標 */
			printf("列(縦)を入力してください\n");
			printf("間違えた数字を入力してしまった場合、２桁の数字を入力すると最初から入力できます。\n");
			if (g_PlayMusic) printf("音楽は ー を入力すると停止します。\n");

			scanf_s("%d", &C);
			if (C == -2)
			{
				//MusicStop();
				continue;
			}

			if (C > 8)
			{
				printf("１～８までの数字を入力してください。\n\n");
				continue;
			}
		}

		/* 色を入れている */
		if (!g_Path)
		{
			CurrentColor = GetCurrentColor();
		}

		printf("%d %d", R, C);

		/* パス */
		if ((CurrentColor == WHITE) || (g_Path == true))
		{
			break;
		}

		/* その場所におけるかどうかの判定 */
		if (!Can_put_stones(R, C, CurrentColor))
		{
			Countinue_Count++;
			continue;
		}

		break;
	} while (1);

	/* パスしていたら、中の処理はしない。 */
	if (!g_Path)
	{
		g_CurrentBoard[C][R] = CurrentColor;
	}

	/* 縦、横、 */
	ReverseStone(C, R, CurrentColor, g_CurrentBoard);

}

/* TODO 棋譜を表示する関数 */
/* BoardSaveの時に保存はやってしまっておく */
void PrintKifu()
{

}

/* ターン終了時に盤面の状態を戻す関数 */
void TurnEnd()
{
	g_BlkPos = 0;
	/* 初期化 */
	for (int i = 0; i <= 20; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			g_WhiteCans[i][j] = EMPTY;
			g_BlackCans[i][j] = EMPTY;
		}
	}

	/* CANの初期化 */
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if ((g_CurrentBoard[i][j] == BLACKCAN)
				|| (g_CurrentBoard[i][j] == WHITECAN))
			{
				g_CurrentBoard[i][j] = EMPTY;
			}
		}
	}

	/* 両方パスの場合ゲーム終了 */
	if ((g_IsBlackePath == true) && (g_IsWhitePath == true))
	{
		printf("両方打てる場所が無いのでゲームを終了します。\n");
		g_IsGameEnd = true;
	}
	/* どちらかがオートパスになったらカウント開始 */
	if ((g_IsBlackePath == true) || (g_IsWhitePath == true))
	{
		g_GameEndCount++;
	}
	/* 片方がtrueで一周回ったので両方falseに */
	if (g_GameEndCount == 2)
	{
		g_IsBlackePath = false;
		g_IsWhitePath = false;
	}

	/* 変数の初期化及び更新 */
	g_CurrentColor = 0;
	g_Path = false;
	g_IsAutoPath = false;
	g_UndoCount = 0;
	g_PreCount++;
	g_Ehand_x = INITSTATE;
	g_Ehand_x = INITSTATE;
	g_Turn++;		

}

/* 盤面の情報を保存している */
void BoardSave()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			g_PreBoard[g_CurrentTurn][i][j] = g_CurrentBoard[i][j];
		}
	}
}

/* ゲームの初期化関数 */
void GameInit()
{
	g_Turn = 0;
	g_BackupCount = 0;
	g_Continue = false;
	//MusicPlay();
	PrintRule();
	Init_Board(g_CurrentBoard);
	Init_Score(g_BoardScore);
}

/* ゲームループ関数 */
void GameLoop()
{
	while (1)
	{
		g_CurrentTurn++;
		g_BackupCount++;
		g_CurrentTurn = g_BackupCount;
		BoardSave();
		SerchStone(g_CurrentBoard, GetCurrentColor());
		Print_Board(g_CurrentBoard);
		PrintCurrentTurn();
		PutStone();
		TurnEnd();
		if (IsFinish()) break;
	}
	Print_Board(g_CurrentBoard);
	printf("\n全ターン数は%dです。\n", g_Turn);
	PAUSE;
}

/* ゲームの終わりに呼ぶ関数 */
void GameEnd()
{
	//MusicStop();
	printf("ゲームの勝敗がつきました。\n");
	printf("\nEnterキーを押して下さい。\n");
	PAUSE;
	StoneCount();

	int r = 0;	//<<　リトライ用変数
	printf("ゲームをもう一回遊びますか？\n");
	printf("はい　：　１、いいえ　：　２\n");
	printf("入力してください\n");
	scanf_s("%d", &r);
	if (r == 1) g_Continue = true;
	if (g_Continue)
	{
		printf("ゲームを続けます\n");
	}
	else
	{
		printf("ゲームを終了します!\n");
	}

	system("cls");
}

/* メイン関数 */
void main()
{
	do
	{
		GameInit();
		GameLoop();
		PAUSE;
		GameEnd();
	} while (g_Continue);

	PAUSE;
	//PAUSE;
}
