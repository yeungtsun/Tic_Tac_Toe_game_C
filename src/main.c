#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

typedef struct {
	int x;
	int y;
	int px;
	int py;
} in;

typedef struct {
	int who;
	int pos;
} val;

typedef struct {
	int row[3];
	int col[3];
	int slp[2];
} sc;

typedef struct {
	int row[3];
	int col[3];
	int slp[2];
} emp;

typedef struct {
	int turn;
	int p0;
	int p1;
	int winner;
	char p[2];
	in ins[9];
	val vals[3][3];
	sc scs[2];
	emp e;
	char b[11][17];
} game;

void init(game* g) {
	g->turn = 1;
	// winner = 1 means You, 2 means Com
	g->winner = 2;
	// init who play first
	srand(time(NULL));
	g->p0 = rand() % 2;
	g->p1 = g->p0 ? 0 : 1;
	// init player symbol
	g->p[0] = rand() % 2 ? 'O' : 'X';
	g->p[1] = g->p[0] == 'O' ? 'X' : 'O';

	int i = 0;
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			// set box values
			g->vals[y][x].pos = i + 1;
			g->vals[y][x].who = 2;
			// set input values
			g->ins[i].x = x;
			g->ins[i].y = y;
			i++;
		}

		// "sc" & "e" lend "y" save loop effort
		// init player 0 - row & col scores
		g->scs[0].row[y] = 0;
		g->scs[0].col[y] = 0;
		// init player 1 - row & col scores
		g->scs[1].row[y] = 0;
		g->scs[1].col[y] = 0;
		// init empty row & col statistic
		g->e.row[y] = 3;
		g->e.col[y] = 3;
	}

	// init player 0 - slope scores
	g->scs[0].slp[0] = 0;
	g->scs[0].slp[1] = 0;
	// init player 1 = slope scores
	g->scs[1].slp[0] = 0;
	g->scs[1].slp[1] = 0;
	// init empty slope scores
	g->e.slp[0] = 3;
	g->e.slp[1] = 3;

	// init board pixels
	for (int y = 0; y < 11; ++y) {
		for (int x = 0; x < 17; ++x) {
			// add col partitions
			if (x == 5 || x == 11)
				g->b[y][x] = '+';
			// add row partitions
			else if (y == 3 || y == 7)
				g->b[y][x] = '+';
			else
				g->b[y][x] = ' ';
		}
	}
	// init board values & inputs's board coords
	int j = 1;
	for (int y = 1; y < 11; y += 4) {
		for (int x = 2; x < 17; x += 6) {
			g->b[y][x] = j + '0';
			g->ins[j - 1].px = x;
			g->ins[j - 1].py = y;
			j++;
		}
	}
}

void update(game* g, int move, int player) {
	int i = move - 1;
	int x = g->ins[i].x;
	int y = g->ins[i].y;
	int px = g->ins[i].px;
	int py = g->ins[i].py;

	g->vals[y][x].who = player;
	g->vals[y][x].pos = player;

	g->scs[player].row[y]++;
	g->scs[player].col[x]++;
	g->e.row[y]--;
	g->e.col[x]--;

	g->b[py][px] = g->p[player];

	if (x == y) {
		g->scs[player].slp[0]++;
		g->e.slp[0]--;
		if (x == 1) {
			g->scs[player].slp[1]++;
			g->e.slp[1]--;
		}
	}

	if (abs(x - y) == 2) {
		g->scs[player].slp[1]++;
		g->e.slp[1]--;
	}
}

bool check_for_win(game* g, int player) {
	for (int i = 0; i < 3; ++i) {
		if (g->scs[player].row[i] == 3 || g->scs[player].col[i] == 3)
			return true;
	}
	if (g->scs[player].slp[0] == 3 || g->scs[player].slp[1] == 3)
		return true;
	return false;
}

void print_b(game* g) {
	system("cls");
	printf("\n\t\t\t\t\t    Welcome to Tic Tac Toe game!\n");
	printf("\n\t\t\t\t\t       You = %c     Com = %c\n", g->p[0], g->p[1]);
	if (g->p0)
		printf("\n\t\t\t\t\t\t     Com first!\n\n");
	else
		printf("\n\t\t\t\t\t\t     You first!\n\n");

	for (int y = 0; y < 11; ++y) {
		printf("\t\t\t\t\t\t ");
		for (int x = 0; x < 17; ++x) {
			printf("%c", g->b[y][x]);
		}
		printf("\n");
	}
}

bool is_occupy(game* g, int move) {
	int i = move - 1;
	if (move < 1 || move > 9) return true;
	int x = g->ins[i].x;
	int y = g->ins[i].y;
	int occupied = g->vals[y][x].who;
	if (occupied != 2) return true;
	return false;
}

int ai(game* g, int move) {
	for (int i = 0; i < 3; ++i) {
		// check win in rows
		if (g->scs[1].row[i] == 2 && g->e.row[i] == 1) {
			for (int x = 0; x < 3; ++x) {
				if (g->vals[i][x].who == 2)
					return g->vals[i][x].pos;
			}
		}
		// check win in columns
		if (g->scs[1].col[i] == 2 && g->e.col[i] == 1) {
			for (int y = 0; y < 3; ++y) {
				if (g->vals[y][i].who == 2)
					return g->vals[y][i].pos;
			}
		}
	}

	// check win in slopes
	if (g->scs[1].slp[0] == 2 && g->e.slp[0] == 1) {
		for (int i = 0; i < 3; ++i) {
			if (g->vals[i][i].who == 2)
				return g->vals[i][i].pos;
		}
	}
	if (g->scs[1].slp[1] == 2 && g->e.slp[1] == 1) {
		for (int i = 0, j = 2; i < 3; ++i, --j) {
			if (g->vals[i][j].who == 2)
				return g->vals[i][j].pos;
		}
	}

	for (int i = 0; i < 3; ++i) {
		// defense in rows
		if (g->scs[0].row[i] == 2 && g->e.row[i] == 1) {
			for (int x = 0; x < 3; ++x) {
				if (g->vals[i][x].who == 2)
					return g->vals[i][x].pos;
			}
		}
		// defense in columns
		if (g->scs[0].col[i] == 2 && g->e.col[i] == 1) {
			for (int y = 0; y < 3; ++y) {
				if (g->vals[y][i].who == 2)
					return g->vals[y][i].pos;
			}
		}
	}

	//defense in slopes
	if (g->scs[0].slp[0] == 2 && g->e.slp[0] == 1) {
		for (int i = 0; i < 3; ++i) {
			if (g->vals[i][i].who == 2)
				return g->vals[i][i].pos;
		}
	}
	if (g->scs[0].slp[1] == 2 && g->e.slp[1] == 1) {
		for (int i = 0, j = 2; i < 3; ++i, --j) {
			if (g->vals[i][j].who == 2)
				return g->vals[i][j].pos;
		}
	}

	// grow in rows
	for (int i = 0; i < 3; ++i) {
		if (g->scs[1].row[i] == 1 && g->e.row[i] == 2) {
			for (int x = 0; x < 3; ++x) {
				if (g->vals[i][x].who == 2)
					return g->vals[i][x].pos;
			}
		}
	}
	// grow in columns
	for (int i = 0; i < 3; ++i) {
		if (g->scs[1].col[i] == 1 && g->e.col[i] == 2) {
			for (int y = 0; y < 3; ++y) {
				if (g->vals[y][i].who == 2)
					return g->vals[y][i].pos;
			}
		}
	}

	// grow in slopes
	if (g->scs[1].slp[0] == 1 && g->e.slp[0] == 2) {
		for (int i = 0; i < 3; ++i) {
			if (g->vals[i][i].who == 2)
				return g->vals[i][i].pos;
		}
	}
	if (g->scs[1].slp[1] == 1 && g->e.slp[1] == 2) {
		for (int i = 0, j = 2; i < 3; ++i, --j) {
			if (g->vals[i][j].who == 2)
				return g->vals[i][j].pos;
		}
	}

	return move;
}

int ai_move(game* g) {
	srand(time(NULL));
	int move = rand() % 9 + 1;
	while (is_occupy(g, move)) {
		move = rand() % 9 + 1;
	}
	move = ai(g, move);
	if (is_occupy(g, move)) {
		printf("AI Error Occur!\n");
		Sleep(60000);
	}
	return move;
}

bool game_turn(game* g, int player) {
	int move = 0;
	if (player == 0) {
		printf("\n\t\t\t\t\t    Please enter the number: ");
		scanf_s("%d", &move);
		while (is_occupy(g, move)) {
			printf("\n\t\t\t\t\t    Invalid move, please enter the number again: ");
			scanf_s("%d", &move);
		}
	}
	if (player == 1) {
		move = ai_move(g);
	}
	update(g, move, player);
	print_b(g);

	if (check_for_win(g, player)) {
		g->winner = player;
		if (player == 0)
			printf("\n\t\t\t\t\t     Congratulation! You Win!\n");
		if (player == 1)
			printf("\n\t\t\t\t\t     You Lose!\n");
		return true;
	}
	g->turn++;
	if (g->turn > 9) {
		printf("\n\t\t\t\t\t     Draw Game!\n");
		return true;
	}
	return false;
}

void print_v(game* g) {
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			printf("%d", g->vals[y][x].pos);
		}
		printf("\n");
	}
}

void print_s(game* g) {
	for (int i = 0; i < 3; ++i) {
		printf("p0's row %d: %d\n", i, g->scs[0].row[i]);
	}
	for (int i = 0; i < 3; ++i) {
		printf("p0's col %d: %d\n", i, g->scs[0].col[i]);
	}
	printf("p0's slope %d: %d\n", 0, g->scs[0].slp[0]);
	printf("p0's slope %d: %d\n", 1, g->scs[0].slp[1]);
}

void write_to_file(game* g) {
	FILE* fp = NULL;
	fopen_s(&fp, "result.txt", "a+");
	if (!fp) exit(-1);

	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	int round = 1;

	// initialize result.txt
	if (!len) {
		fprintf(fp, "\n\t\t\t\t\t     Tic Tac Toe game result");
		fprintf(fp, "\n\n\t\t\t\t\tRound\tWinner\tDate and Time\n");
	}
	else {
		// set round and get round num by counting how many new line
		fseek(fp, 0, SEEK_SET);
		int nl = 0;
		int ch = 0;
		while ((ch = fgetc(fp)) != EOF) {
			if (ch == '\n') nl++;
		}
		round = nl - 3;
	}

	// set winner
	char winner[10];
	if (g->winner == 2) strcpy_s(winner, 10, "Draw");
	if (g->winner == 0) strcpy_s(winner, 10, "You");
	if (g->winner == 1) strcpy_s(winner, 10, "Com");

	// set date and time
	time_t t = time(NULL);
	struct tm ts;
	char date[80];
	localtime_s(&ts, &t);
	strftime(date, sizeof(date), "%a %Y-%m-%d %H:%M:%S", &ts);

	// insert record to result.txt
	fprintf(fp, "\n\t\t\t\t\t%d\t%s\t%s", round, winner, date);

	fclose(fp);
}

void print_r() {
	int ch = 0;
	FILE* fp = NULL;
	fopen_s(&fp, "result.txt", "r");
	if (!fp) exit(-1);
	while ((ch = fgetc(fp)) != EOF) {
		putchar(ch);
	}
	fclose(fp);
}

int main() {
	char again = 'y';
	while (again == 'y') {
		game* g = malloc(sizeof(game));
		if (!g) exit(-1);

		init(g);
		print_b(g);

		while (1) {
			if (game_turn(g, g->p0)) break;
			if (game_turn(g, g->p1)) break;
		}

		write_to_file(g);

		print_r();

		printf("\n\n\t\t\t\t    Would you like to play again? Yes(y) / No(n): ");
		scanf_s(" %c", &again, 1);

		free(g);
	}
	return 0;
}