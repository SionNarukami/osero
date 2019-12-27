#include "Main.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <MMSystem.h>

/*	�Ֆʂ̑傫��	*/
#define BOARD_SIZE 10

/*	�}�X�̏��	*/
#define WALL -999		//<<�ՊO
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define BLACKCAN 3
#define WHITECAN 4

/* CPU�p�̏������萔 */
#define INITSTATE -50

/* �ꎞ��~ */
#define PAUSE getchar()

/* �Ֆʂ̏�� */
int g_CurrentBoard[BOARD_SIZE][BOARD_SIZE];		//<<���݂̔Ֆ�
int g_Current2Board[BOARD_SIZE][BOARD_SIZE];	//<<�Q�w�ڗp

/* �Ֆʂ̕ۑ��p�z�� */
int g_PreBoard[100][BOARD_SIZE][BOARD_SIZE];

/* �Ֆʂ̏ꏊ�𕶎��ŕ\���p */
char g_PrintBord_s[BOARD_SIZE][BOARD_SIZE];	//<<���O����ŕύX���邱��

/* �����p�z�� */
char g_White_kifu[30];
char g_Black_kifu[30];

/* ���߂��悤�J�E���g�ϐ� */
int g_PreCount = EMPTY;

/* ����߂����p�ϐ� */
int g_UndoCount = EMPTY;

/* �Ղ̕]���_�� */
int g_BoardScore[BOARD_SIZE][BOARD_SIZE];

/* �^�[���� */
int g_Turn = EMPTY;

/* �p�X�J�E���^�[ */
bool g_Path = false;

/* ���݂̃^�[���̐F */
int g_CurrentColor = EMPTY;

/* ���y�Đ��m�F�p�ϐ� */
bool g_PlayMusic = false;

/* ��蒼���p�֐� */
bool g_Continue = false;

/* �^�[���J�E���g�̃o�b�N�A�b�v�p�ϐ� */
int g_BackupCount = EMPTY;

/* �����p�X�p�ϐ� */
bool g_IsAutoPath = false;

/* �ǂ̃^�C�~���O�Ŏ����p�X���ꂽ���ۑ�����ϐ� */
bool g_PathSave[100] = { false };

/* CAN�������Ă���ꏊ��ۑ����Ă���ϐ� */
int g_WhiteCans[40][2];
int g_BlackCans[40][2];
int g_WhiteCans_2[40][2];	//<<�Q�w��

/* CAN�̐� */
int g_BlkPos = EMPTY;

/* CPU�̎�D */
int g_Ehand_x = EMPTY;
int g_Ehand_y = EMPTY;

/* ���݂̃^�[���� */
int g_CurrentTurn = EMPTY;

/* �����u���Ȃ����͂��̎��_�ŃQ�[���I���ϐ� */
bool g_IsWhitePath = false;
bool g_IsBlackePath = false;
int g_GameEndCount = 0;	//<<�����̕ϐ���true�ɂȂ������ǂ������肷��p�̕ϐ�
bool g_IsGameEnd = false;

/* 8���� */
enum Direction
{
	Up,		//<<��
	Down,	//<<��
	Right,	//<<�E
	Left,	//<<��
	Dia_up_right,	//<<�E��
	Dia_down_right,	//<<�E��
	Dia_up_left,		//<<����
	Dia_down_left,	//<<����
};

/* �Ֆʂ̏�� */
enum CpuLevel
{
	saisyo,		//<<����
	tyubann,	//<<����
	last		//<<�I��
};

/* WHITECAN�֐��̒����󂩂ǂ��� */
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

/* �����ϊ��֐� */
int Ctoi(char _c)
{
	if (_c >= '0' && _c <= '9')
	{
		return _c - '0';
	}

	return 0;
}

/* ���͂��ꂽ�������������ǂ������ʂ���֐� */
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

/* ���݂̐F��Ԃ� */
int GetCurrentColor()
{
	return g_Turn % 2 + 1;
}

/* �Ֆʂ̏����� */
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

/* �Ֆʂ̏d�ݏ����� */
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

/* TODO �Ֆʂ𕶎��ŏꏊ��������悤�� */
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

/* �Ֆʂ̕\�� */
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
				printf("�Z");
			}
			else if (_bord[i][j] == WHITE)
			{
				printf("��");
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
					printf("�{");
				}
				break;
			case WHITE:
				if (_bord[i][j] == WHITECAN)
				{
					printf("�{");
				}
				break;
			}
		}

		printf(" |\n");
	}

	printf("   -----------------\n");
}

/* ���[���̕\�� */
void PrintRule()
{
	/* TODO �Ō�ɓ��e���m�F����悤�� */
	printf("\n		���[���̐����ł�! \n");
	printf(" 1 : 8�~8 �̃Q�[���Տ�ɁC�S�̐΂�������ԂƂ��Ă����Ă���B\n");
	printf(" 2 : ���݂Ɏ����̐F�̐΂�u���Ă����B\n");
	printf(" 3 : �΂�������ꏊ�́C�����̐F�̐΂ő���̐΂��P�ȏ� \n");
	printf("�� : ���߂鏊�ł���B���ޕ����́C�c���΂߂̂ǂ�ł��悢�B\n");
	printf(" 4 : ���̂Ƃ��C���܂ꂽ�΂́C�����̐F�̐΂ƂȂ�B\n");
	printf(" 5 : �΂�������ꏊ���Ȃ��ꍇ�ɂ́C�p�X������B\n");
	printf(" 6 : �΂�������ꏊ������ꍇ�ɂ́C�p�X�͂ł��Ȃ��B\n");
	printf(" 7 : ���҂Ƃ��΂�������ꏊ���Ȃ��Ȃ����ꍇ�C�Q�[���͏I������B\n");
	printf(" 8 : �Q�[���I�����̐΂̌��ŏ��s�����߂�B\n");
	printf(" 9 : �΂�u���Ƃ��͍��W����͂��Ă��炢�܂��B \n");
	printf("10 : ��@ 3�A 5�Ɠ��͂���ƁA \n");
	printf("�� : ������R�ԖځA�ォ��T�Ԗڂ̏ꏊ�ɐ΂�u�������o���܂��B\n");
	printf("11 : ���̐΂��u���Ă�����ǂ�����ƒu�������o���܂���B\n");
	printf("12 : �����ΐ펞�̓��͂��ԈႦ����ꍇ�́A2��ڂ̓��͂̎��Ɂ@\n");
	printf("�� : 2���̐�������͂���ƁA�ŏ�������͂��邱�Ƃ��o���܂��B\n");
	printf("13 : ���y�́[�Q����͂���ƒ�~���܂��B\n");
	printf("14 : �΂�u�����Ƃ��o����ꏊ�ɂ� + ���u���Ă���܂��B\n");
	printf("15 : �O�^�[���ڈȍ~����Au �{�^���ň��߂������o���܂��B\n");
	printf("�撣���āI�I�I\n\n");
}

/* ���Ԃ肪���邩�ǂ����𒲂ׂ�֐� */
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

	/* Plus�ϐ���true�Ȃ�,�u������ɕʂ̐F�̐΂������ԂȂ̂�
	  ���̐�����ē����F�Ȃ�true��ʂ̐F�Ȃ��false��Ԃ�
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

/* �΂𗠕Ԃ� */
void ReverseStone(int _c, int _r, int _color, int _bord[BOARD_SIZE][BOARD_SIZE])
{
	int Color = _color, Column = _c, Row = _r, Count = 0;
	if ((Column == 0) && (Row == 0)) return;  //<<�����p�X�̏ꍇ�͉��������ɕԂ�

	/* ���݂̐F�����Ă��� */
	int NowCans = GetCurrentColor() + 2;			
	if ((NowCans == 1) || (NowCans == 2))
	{
		NowCans += 2;
	}

	/* true�ŕԂ��Ă���������ۑ�����,���̏����ł��̕��������ɍs�� */
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
					/* �u�����΂̏�ɕǂ��󔒂Ȃ炻�̎��_�ŃX�g�b�v */
					if ((_bord[Column - j][Row] == WALL)
						|| (_bord[Column - j][Row] == EMPTY)
						|| (_bord[Column - j][Row] == NowCans))
					{
						Count = 0;
						break;
					}
					/* �オ�u�����΂Ɠ����F�Ȃ牽������ */
					else if (_bord[Column - j][Row] == Color)
					{
						if (!Count == 0)
						{
							/* �Ђ�����Ԃ����� */
							for (int k = 1; k < j + 1; k++)
							{
								_bord[Column - k][Row] = Color;
							}
						}
						Count = 0;
						break;
					}
					/* �ʂ̐F�̐΂Ȃ�Ō�Ɉ�C�ɗ��Ԃ��p�̃J�E���g�i�߂� */
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

/*  SerchStone�֐��p ��悪�Ⴄ�F�̏ꍇ�̂�true��Ԃ� */
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

/* �΂��ǂ��̏ꏊ�ɒu���邩���ׂ�֐� */
void SerchStone(int _bord[BOARD_SIZE][BOARD_SIZE], int _color)
{
	int MyColor = _color;
	bool DirectionCount[8] = { false };
	int AutoPathCount = 0;			//<<�@�����p�X�p�ϐ�

	for (int i = 1; i < 9; i++)
	{
		for (int j = 1; j < 9; j++)
		{
			/* �����őS�������ɍs����悤�ɂ��Ă��� */
			for (int q = 0; q < 8; q++)
			{
				DirectionCount[q] = false;
			}

			/* �����̐F�̐΂�����,���̏ꏊ����������Ă��� */
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
								/* ���������ɂ͓��ȏ�͌��Ȃ��Ă������͂� */
								if (!DirectionCount[Up])
								{
									if (_bord[i - k][j] == MyColor)	break;

									/* �ǂ�Can�Ȃ�Break*/
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
											/* �u����ꏊ�̍��W��ۑ����Ă��� */
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

/* ���y�Đ� */
/* ���y���s�]�Ȃ̂ł�����������܂� */
//void MusicPlay()
//{
//	if (!PlaySound(TEXT("music\\last-war.wav"), NULL, SND_FILENAME 
//		| SND_ASYNC 
//		| SND_LOOP))
//	{
//		printf("�Đ��ł��܂���B");
//		g_PlayMusic = false;
//		return;
//	}
//
//	g_PlayMusic = true;
//}

/* ���y��~ */
//void MusicStop()
//{
//	PlaySound(NULL, NULL, 0);
//	printf("���y�������܂��B\n");
//	g_PlayMusic = false;
//}

/* �����̏������y�ѐ����̕ϊ� */
int IsTranceform(char _c)
{
	/* ����񂯂��0�p */
	if (_c == '0')
	{
		return -5;
	}
	/* ���y�̒�~ */
	if (_c == '-')
	{
		//MusicStop();
		return 0;
	}

	/* ���߂� */
	if (_c == 'u')
	{
		g_UndoCount++;
		return -4;
	}

	/* ���͂��ꂽ���̂������Ȃ�ϊ����Ă��� */
	if (IsNumber(_c))
	{
		return Ctoi(_c);
	}

	return 0;
}

/* �I������ */
bool IsFinish()
{
	/* �u������������ԂȂ�I�� */
	if (g_IsGameEnd) return true;

	/* �S�����܂��Ă�����I�� */
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

/* �΂̐��𐔂��� */
void StoneCount()
{
	int W = 0, B = 0;	//<< �΂̐�

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

	printf("���̐΂̐���%d�ł��B\n", B);
	printf("���̐΂̐���%d�ł��B\n", W);

	if (B < W)
	{
		printf("���̏����ł��B\n");
	}
	else if (B == W)
	{
		printf("���_�Ȃ̂ŁA���������ł��B\n");
	}
	else
	{
		printf("���̏����ł��B\n");
		printf("���߂łƂ��������܂�!!\n");
	}
}

/* �u���邩�ǂ����̔���֐� */
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

/* ���݂ǂ���̃^�[���Ȃ̂��̐錾�֐� */
void PrintCurrentTurn()
{
	g_CurrentColor = GetCurrentColor();

	switch (g_CurrentColor)
	{
	case BLACK:
		printf("���̃^�[���ł��B\n");
		break;
	case WHITE:
		printf("���̃^�[���ł��B\n");
		break;
	}
}

/* �Ֆʂ̏�Ԃ����߂��֐� */
void BordUndo()
{
	/* �Q�^�[���ڈȍ~���炶�ᖳ���Ɩ߂����͏o���Ȃ� */
	if (g_CurrentTurn < 3)
	{
		printf("�߂����Ƃ��o���܂���B\n");
		return;
	}

	/* ����O�ɖ߂��Ă��� */
	g_CurrentTurn -= 2;
	g_BackupCount -= 2;

	/* �O��̃^�[���Ɏ����p�X������Ă����炻�̈���O�ɖ߂� */
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

	/* �R�s�[�����Ă��� */
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			g_CurrentBoard[i][j] = g_PreBoard[g_CurrentTurn][i][j];
		}
	}

	printf("���߂��܂����B\n");

	/* �߂����̂ł��̑O�̔Ֆʂ�\�����Ȃ���΂����Ȃ� */
	SerchStone(g_CurrentBoard, GetCurrentColor());
	Print_Board(g_CurrentBoard);
}

/* CPU�� */
void PlotCPU()
{
	/* �܂��łĂ���Ⴄ */
	/* WHITECAN�������Ă�ꏊ��ϐ��ŕۑ����Ă��� */
	int Vir_x = 0, Vir_y = 0;
	int Px = 0, Py = 0;
	int Score = INITSTATE;
	int PScore = INITSTATE;

	/* Can�̐� */
	int WhiteCans1 = 0;		//<< WhiteCan�̐�(�P�w��)
	int WhiteCans2 = 0;		//<< WhiteCan�̐�( 2�w��)
	int BlackCans = 0;		//<< BlackCan�̐�(�P�w��)
	//���̎�
	int Black_Hand_x = 0;
	int Black_Hand_y = 0;
	//���̕]���l
	int Bscore = INITSTATE;
	int Bpscore = INITSTATE;
	//�Q�w�ڂ̔��̎�
	int White_hand_x = 0;
	int White_hand_y = 0;
	//�Q�w�ڗp�̕]���l
	int Score_2 = 0;
	int PScore_2 = INITSTATE;
	//�_����ۑ����Ă����z��
	int ScoreSave[50] = { INITSTATE };
	/* �z��Q�Ɨp�ϐ� */
	int W_pos = 0;
	int W_pos2 = 0;
	int B_pos = 0;
	int Roop = 0;

	/* ������ */
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

	/* �L�����̐��𐔂��Ă��� */
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

	/* �u���Ƃ��낪�Ȃ��ꍇ */
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

		/* �p�X */
		if (Roop == 3)
		{
			printf("�u����Ƃ��낪����܂���.................\n");
			g_Ehand_x = 0;
			g_Ehand_y = 0;
			g_Path = true;
			g_IsWhitePath = true;
			return;
		}

		g_CurrentTurn = g_BackupCount;
		Roop++;
	}

	/*�@�P�w�ڂ�WhiteCan�̐������񂷁@*/
	for (int wcans = 0; wcans < WhiteCans1; wcans++)
	{
		/* ���z�Ֆʂ�����đ�����Ă��� */
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

		/* WHITECAN�̏����� */
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

		/* BLACKCAN�����Ă��� */
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

		/* ����̒u���ꏊ������Ȃ��ꍇ */
		if (B_pos == 0)
		{
			wcans++;
			ScoreSave[wcans] = Score;
			continue;
		}

		/* ��ԕ]���̗ǂ��ꏊ�ɐ΂����Ă��� */
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

			/* ���ɉ��������Ă��Ȃ�������ł��؂� */
			if (IsBlack(i + 1)) break;
		}

		/* ���z�Ֆʂɑ�����Ђ�����Ԃ��Ă��� */
		g_Current2Board[Black_Hand_x][Black_Hand_y] = BLACK;
		ReverseStone(Black_Hand_x, Black_Hand_y, BLACK, g_Current2Board);

		/* BLACKCAN�̏����� */
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

		/* �P�w�ڂ�WHITECAN�̃R�s�[�������� */
		for (int i = 0; i < 40; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				g_WhiteCans_2[i][0] = g_WhiteCans[i][0];
				g_WhiteCans_2[i][1] = g_WhiteCans[i][1];
			}
		}

		/* �o�O�p */
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

		/* �Q�w�ڂ�WHITECAN������ */
		SerchStone(g_Current2Board, WHITE);
		/* �����Ă��� */
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

		/* ���ۂɓ����킯�ł͖����̂ŃX�R�A�̌v�Z�����ł���΂��� */
		for (int i = 0; i < WhiteCans2; i++)
		{

			Vir_x = g_WhiteCans[i][0];
			Vir_y = g_WhiteCans[i][1];

			PScore_2 = g_BoardScore[Vir_x][Vir_y];
			if (Score_2 <= PScore_2)
			{
				Score_2 = PScore_2;
			}

			/* ���ɉ��������Ă��Ȃ�������ł��؂� */
			if (IsWhite(i + 1)) break;
		}

		/* ���ɖ߂� */
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

		/* ������ */
		g_BlkPos = 0;

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				g_Current2Board[i][j] = g_CurrentBoard[i][j];
			}
		}

		//�_����ۑ����Ă����z��
		ScoreSave[wcans] = Score_2 + Score;
		/* ������ */
		W_pos2 = 0;
		B_pos = 0;
		BlackCans = 0;
		Roop = 0;
	}

	int scoresin = INITSTATE;
	/* �ŏI�I�ȓ_���̗D������� */
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

/* 1�w�ڂ̂� */
void PlotCPU_new()
{
	/* �܂��łĂ���Ⴄ */
	/* WHITECAN�������Ă�ꏊ��ϐ��ŕۑ����Ă��� */
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

		/* ���ɉ��������Ă��Ȃ�������ł��؂� */
		if (IsWhite(i + 1)) break;
	}
}

/* �΂�u�� */
void PutStone()
{
	int C = 0, R = 0, CurrentColor = 0, Countinue_Count = 0,P = 0;
	char Ro;

	if (g_PreCount == 2)
	{
		printf("���߂������o����p�ɂȂ�܂����B\n");
	}

	do
	{
		if (Countinue_Count >= 1)
		{
			printf("\n\n\n���̏ꏊ�ɂ́A�u�������o���܂���B\n");
			printf("�ʂ̏ꏊ�ɒu���Ă��������B\n\n");
			Print_Board(g_CurrentBoard);
			printf("�s(��)����͂��Ă�������\n");
		}
		/* �G���[���� */
		if (g_CurrentColor == 0)
		{
			printf("�o�O����!\n");
			g_CurrentColor = WHITE;
			CurrentColor = WHITE;
		}

		/* CPU�� */
		if (g_CurrentColor == WHITE)
		{
			CurrentColor = WHITE;
			PlotCPU();
			/* �u���Ƃ��낪������ */
			if ((g_Ehand_x == 0) || (g_Ehand_y == 0))
			{
				printf("�p�X�����܂����B\n");
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
			/* �p�X�����Ă����� */
			if ((C == 0) || (R == 0))
			{
				g_Path = true;
			}
		}
		else if (g_CurrentColor == BLACK)
		{
			/* x���W */

			if (P == 0)
			{
				printf("�s(��)����͂��Ă�������\n");
				printf("�����ȊO�͓��͂��Ȃ��ł��������I�I�I\n");
				if (g_PlayMusic) printf("���y�� �[ ����͂���ƒ�~���܂��B\n");
			}

			/* �����ɂ��u���ꏊ��������� */
			if (g_IsAutoPath)
			{
				printf("�����ɂ��u���ꏊ������܂���B\n");
				g_PathSave[g_Turn] = true;
				g_IsBlackePath = true;
				R = -3;
			}
			else
			{
				scanf_s("%c", &Ro);
				if (!(R = IsTranceform(Ro)))
				{
					printf("��������͂��Ă��������B\n\n");
					P++;
					continue;
				}
			}

			if (R > 8)
			{
				printf("�P�`�W�܂ł̐�������͂��Ă��������B\n\n");
				P++;
				continue;
			}

			/* �p�X */
			if (R == -3)
			{
				printf("�p�X�����܂����B\n");
				Countinue_Count++;
				g_Path = true;
				break;
			}

			/* ���߂� */
			if (R == -4)
			{
				BordUndo();
				continue;
			}

			/* y���W */
			printf("��(�c)����͂��Ă�������\n");
			printf("�ԈႦ����������͂��Ă��܂����ꍇ�A�Q���̐�������͂���ƍŏ�������͂ł��܂��B\n");
			if (g_PlayMusic) printf("���y�� �[ ����͂���ƒ�~���܂��B\n");

			scanf_s("%d", &C);
			if (C == -2)
			{
				//MusicStop();
				continue;
			}

			if (C > 8)
			{
				printf("�P�`�W�܂ł̐�������͂��Ă��������B\n\n");
				continue;
			}
		}

		/* �F�����Ă��� */
		if (!g_Path)
		{
			CurrentColor = GetCurrentColor();
		}

		printf("%d %d", R, C);

		/* �p�X */
		if ((CurrentColor == WHITE) || (g_Path == true))
		{
			break;
		}

		/* ���̏ꏊ�ɂ����邩�ǂ����̔��� */
		if (!Can_put_stones(R, C, CurrentColor))
		{
			Countinue_Count++;
			continue;
		}

		break;
	} while (1);

	/* �p�X���Ă�����A���̏����͂��Ȃ��B */
	if (!g_Path)
	{
		g_CurrentBoard[C][R] = CurrentColor;
	}

	/* �c�A���A */
	ReverseStone(C, R, CurrentColor, g_CurrentBoard);

}

/* TODO ������\������֐� */
/* BoardSave�̎��ɕۑ��͂���Ă��܂��Ă��� */
void PrintKifu()
{

}

/* �^�[���I�����ɔՖʂ̏�Ԃ�߂��֐� */
void TurnEnd()
{
	g_BlkPos = 0;
	/* ������ */
	for (int i = 0; i <= 20; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			g_WhiteCans[i][j] = EMPTY;
			g_BlackCans[i][j] = EMPTY;
		}
	}

	/* CAN�̏����� */
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

	/* �����p�X�̏ꍇ�Q�[���I�� */
	if ((g_IsBlackePath == true) && (g_IsWhitePath == true))
	{
		printf("�����łĂ�ꏊ�������̂ŃQ�[�����I�����܂��B\n");
		g_IsGameEnd = true;
	}
	/* �ǂ��炩���I�[�g�p�X�ɂȂ�����J�E���g�J�n */
	if ((g_IsBlackePath == true) || (g_IsWhitePath == true))
	{
		g_GameEndCount++;
	}
	/* �Е���true�ň��������̂ŗ���false�� */
	if (g_GameEndCount == 2)
	{
		g_IsBlackePath = false;
		g_IsWhitePath = false;
	}

	/* �ϐ��̏������y�эX�V */
	g_CurrentColor = 0;
	g_Path = false;
	g_IsAutoPath = false;
	g_UndoCount = 0;
	g_PreCount++;
	g_Ehand_x = INITSTATE;
	g_Ehand_x = INITSTATE;
	g_Turn++;		

}

/* �Ֆʂ̏���ۑ����Ă��� */
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

/* �Q�[���̏������֐� */
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

/* �Q�[�����[�v�֐� */
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
	printf("\n�S�^�[������%d�ł��B\n", g_Turn);
	PAUSE;
}

/* �Q�[���̏I���ɌĂԊ֐� */
void GameEnd()
{
	//MusicStop();
	printf("�Q�[���̏��s�����܂����B\n");
	printf("\nEnter�L�[�������ĉ������B\n");
	PAUSE;
	StoneCount();

	int r = 0;	//<<�@���g���C�p�ϐ�
	printf("�Q�[�����������V�т܂����H\n");
	printf("�͂��@�F�@�P�A�������@�F�@�Q\n");
	printf("���͂��Ă�������\n");
	scanf_s("%d", &r);
	if (r == 1) g_Continue = true;
	if (g_Continue)
	{
		printf("�Q�[���𑱂��܂�\n");
	}
	else
	{
		printf("�Q�[�����I�����܂�!\n");
	}

	system("cls");
}

/* ���C���֐� */
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
