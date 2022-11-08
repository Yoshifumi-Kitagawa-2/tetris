#include <stdio.h>
#include <stdlib.h>
#include "tetris.h"

int block[BLOCK_HEIGHT][BLOCK_WIDTH];
int stage[FIELD_HEIGHT][FIELD_WIDTH];
int field[FIELD_HEIGHT][FIELD_WIDTH];

/*最初は四角いブロックのみ*/
int blocks[BLOCK_HEIGHT][BLOCK_WIDTH] = {
	{0,0,0,0},
	{0,1,1,0},
	{0,1,1,0},
	{0,0,0,0}
};

/*ブロックの横位置*/
int block_x;

/*ブロックの縦位置*/
int block_y;

/*まずは変数などを初期化*/
void my_init_var(){
    int i,j;

    for (i=0; i < FIELD_HEIGHT; i++){
        for(j=0; j < FIELD_WIDTH; j++){
            stage[i][0] = 9;
			stage[i][1] = 9;
			stage[i][2] = 9;
			stage[20][j] = 9;
			stage[21][j] = 9;
			stage[22][j] = 9;
			stage[i][15] = 9;
			stage[i][16] = 9;
			stage[i][17] = 9;
        }
    }
    block_x = 7;
	block_y = 0;
}

/*ブロックを登録*/
void my_make_block(){
    int x,y;
    for(y=0;y<BLOCK_HEIGHT;y++){
		for(x=0;x<BLOCK_WIDTH;x++){
			block[y][x] = blocks[y][x];
		}
	}
}

/*重ね合わせる*/
/*「field[23][18]」に「stage[23][18]」と「block[4][4]」を重ね合わせていきます。*/
void my_make_field(){
	int i,j,x,y;

	for(i=0;i<FIELD_HEIGHT;i++){
		for(j=0;j<FIELD_WIDTH;j++){
			field[i][j] = stage[i][j];	
		}
	}

	for(y=0;y<BLOCK_HEIGHT;y++){
		for(x=0;x<BLOCK_WIDTH;x++){
			field[block_y + y][block_x + x] += block[y][x];
		}
	}	
}

/*画面表示*/
/*では先ほどの「field[23][18]」を使って画面を描いていきます。*/
void my_draw_field(){
	int i,j;

	for(i=0;i<FIELD_HEIGHT - 2;i++){
		for(j=2;j<FIELD_WIDTH - 2;j++){
			if(field[i][j] == 9)printf("■");
			else if(field[i][j] == 1)printf("□");
			else printf(" ");
		}
		printf("\n");
	}
}

/*三日目　落下*/
/*fieldを消去*/
/*そのままでは「field[23][18]」に前のデータが残ったままになってしまうので
念のため毎回その内容を消去します。*/

void my_clear_field(){
    int i,j;

    for(i=0;i<FIELD_HEIGHT;i++){
        for(j=0;j<FIELD_WIDTH;j++){
            field[i][j] = 0;
        }
    }
}

void my_fall_block(){
    block_y++;
}

int main(){
	int stop = 0;

	my_init_var();

	while(1){
		my_clear_field();
		my_make_block();
		my_make_field();
		my_draw_field();
		my_fall_block();

		if(block_y > 17)break;
	}
	return 0;
}



