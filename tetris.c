#include "tetris.h"

int block[BLOCK_HEIGHT][BLOCK_WIDTH];
int stage[FIELD_HEIGHT][FIELD_WIDTH];
int field[FIELD_HEIGHT][FIELD_WIDTH];

/*最初は四角いブロックのみ*/
/*
int blocks[BLOCK_HEIGHT][BLOCK_WIDTH] = {
	{0,0,0,0},
	{0,1,1,0},
	{0,1,1,0},
	{0,0,0,0}
};
*/

/*ブロックの種類を増やす*/
int blocks[24][16] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
	{0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,1,0,0,0,1,0,0,1,0,0,0,1,1,0},
	{0,0,1,0,0,1,1,0,0,1,1,0,0,1,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,1,0,0,1,1,0,0,0,1,0,0,1,0,0},
	{0,1,0,0,0,0,1,0,0,1,1,0,0,1,1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,0,1,0,0,1,1,0,0,0,0,0,1,1,0},
	{0,1,1,1,0,0,1,0,0,1,1,1,0,0,1,0},
	{0,0,0,0,0,0,1,1,0,1,0,1,0,1,1,0},

	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0},
	{0,1,1,0,1,1,1,0,1,1,0,0,1,1,1,0},
	{0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0},

	{0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
	{0,1,0,0,1,1,1,1,0,1,0,0,1,1,1,1},
	{0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
	{0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0}
};

/*現在のブロックの種類を表す変数*/
int block_id;

/*ブロックの横位置*/
int block_x;

/*ブロックの縦位置*/
int block_y;

/*当たり判定*/
int collision_flag;

/*my_make_block()」にブロックを生成するかどうかのフラグ*/
int make_block_flag;

/*ゲームオーバーしたかどうかの判定*/
int gameover_flag;

/*グローバル変数に揃った列の場所を入れる為の変数*/
int clear_line_point[20];

/*クリア状態かどうかのフラグ*/
int clear_flag;

int clear_count;

/*グローバル変数として回転状態を表す変数*/
int turn_point;

/*回転後のブロックを表す*/
int turn_block[BLOCK_HEIGHT][BLOCK_WIDTH];

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
	collision_flag = 0;
	gameover_flag = 0;
	make_block_flag = 1;
	clear_flag = 0;
	block_id = 0;
	clear_count = 0;
	srand((unsigned int)time(NULL));
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

void my_make_field2(){
	int i,j;

	for(i=0;i<FIELD_HEIGHT;i++){
		for(j=0;j<FIELD_WIDTH;j++){
			field[i][j] = stage[i][j];	
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
	if(make_block_flag == 0){
		block_y++;
	}
}

/*四日目　当たり判定*/
/*今回は横に移動するのと壁の当たり判定を確認する所までやっていきたいと思います*/
/*
キー入力
まずは横に動かすためのキー入力の簡単なプログラムを作ってみます。
実行すると入力待ちの無限ループになりますので矢印キーの左か右を押してください！
*/

/*①ブロックの左側の当たり判定を調べる「my_collision_left()」*/
void my_collision_left()
{
	int x,y;

	collision_flag = 0;
	for(y=0; y<BLOCK_HEIGHT;y++)
	{
		for(x=0;x<BLOCK_WIDTH;x++)
		{
			if (block[y][x] != 0)
			{
				if(stage[block_y + y][block_x + (x - 1)] != 0)
				{
					collision_flag = 1;
				}
			}
		}
	}
}

void my_collision_right()
{
	int x,y;

	collision_flag = 0;
	for(y=0; y<BLOCK_HEIGHT;y++)
	{
		for(x=0;x<BLOCK_WIDTH;x++)
		{
			if (block[y][x] != 0)
			{
				if(stage[block_y + y][block_x + (x + 1)] != 0)
				{
					collision_flag = 1;
				}
			}
		}
	}
}

/*キー入力に当たり判定を組み込む*/
void my_get_key(){
	switch(getche())
   {
    case 0x1b:
    switch(getche())
    {
			case 0x5b:
			switch(getche()){
			        case 0x41: printf("↑\n"); break;
		            case 0x42: printf("↓\n"); break;
		            case 0x44: 
						my_collision_left();
						if(collision_flag == 0){
							block_x--;
						}
						break;
		            case 0x43: 
						my_collision_right();
						if(collision_flag == 0){
							block_x++;
						}
						break;
			}
    }
    break;
   }
}


/*五日目 当たり判定*/

/*底辺の当たり判定*/
void my_collision_bottom()
{
	int x,y;

	collision_flag = 0;
	for(y=0; y<BLOCK_HEIGHT;y++)
	{
		for(x=0;x<BLOCK_WIDTH;x++)
		{
			if (block[y][x] != 0)
			{
				if(stage[block_y + (y + 1)][block_x + x] != 0)
				{
					collision_flag = 1;
				}
			}
		}
	}
}
/*ブロック再生成*/
/*今回からブロックが固定された後にブロックが再生成されるのでその処理を加えたいと思います。*/
/*
void my_make_block()
{
	int x,y;

	if(make_block_flag == 1)
	{
		for(y=0;y<BLOCK_HEIGHT;y++)
		{
			for(x=0;x<BLOCK_WIDTH;x++)
			{
				block[y][x] = blocks[y][x];
			}
		}
		make_block_flag = 0;
	}
}
*/

/*ブロック固定の手順*/
/*
１・底辺の当たり判定を調べる
２・当たり判定があれば現在の「block」の状態を「stage」に保存する
３・ブロックの座標を元に戻す
*/
/*blockをstageに保存する*/
void my_save_block(){
	int x,y;

	for(y=0;y<BLOCK_HEIGHT;y++){
		for(x=0;x<BLOCK_WIDTH;x++){
			stage[block_y + y][block_x + x] += block[y][x];
		}
	}
}

/*ブロックの座標を元に戻す*/
void my_init_var2(){
	block_x = 7;
	block_y = 0;
	make_block_flag = 1;
}

void my_search_line()
{
	int i,j;

	for(i=0;i<FIELD_HEIGHT - 3;i++){
		clear_line_point[i] = 0;
	}

	for(i=0;i<FIELD_HEIGHT - 3;i++){
		for(j=3;j<FIELD_WIDTH - 3;j++){
			if(stage[i][j] == 0){
				clear_line_point[i] = 1;
				break;
			}
		}
	}

	for(i=0;i<FIELD_HEIGHT - 3;i++){
		if(clear_line_point[i] == 0){
			clear_flag = 1;
			break;
		}
	}
}

/*ブロックの固定*/
void my_fix_block(){
	int x,y;

	my_collision_bottom();

	if(collision_flag != 0){
		my_save_block();
		my_search_line();
		if (clear_flag == 0){
			my_init_var2();
		}
	}
}

/*ゲームオーバを作成*/
/*ブロックパズルにおいてのゲームオーバーの条件はブロック再生成時に
そこにすでにあるブロックと重なってしまった場合なので、
まずは重なった場合の当たり判定用関数を作ります。*/
void my_collision_center(){
	int x,y;

	collision_flag = 0;
	for(y=0;y<BLOCK_HEIGHT;y++){
		for(x=0;x<BLOCK_WIDTH;x++){
			if(block[y][x] != 0){
				if(stage[block_y + y][block_x + x] != 0){
					collision_flag = 1;
				}
			}
		}
	}
}
void my_gameover(){
	my_collision_center();

	if(collision_flag != 0){
		gameover_flag = 1;
	}
}

/*六日目*/
/*ブロック消去*/
/*今回からこのブロックの元の２次元配列からランダムにブロックを取り出してみたいと思います。*/
void my_make_block()
{
	int x,y;

	if(make_block_flag == 1)
	{
		block_id = (rand() % 6);
		for(y=0;y<BLOCK_HEIGHT;y++)
		{
			for(x=0;x<BLOCK_WIDTH;x++)
			{
				block[y][x] = blocks[(block_id * BLOCK_HEIGHT) + y][x];
			}
		}
		make_block_flag = 0;
	}
}

/*消去する列を探す*/

/*クリアして表示*/
/*まずは揃った列の部分をクリアして表示したいので、その列を空白を表す「0」に置き換えます。*/
void my_clear_line()
{
	int i,j;
	int remain_line_point[20] = {0};
	int remain_line_index = 0;
	/*clear_line_point[i] == 0」の場所がクリアすべき列になりますね。そこを空白を表す「0」に全て置き換えます。*/
	if(clear_count < 2){
		for(i=0;i<FIELD_HEIGHT - 3;i++){
			if(clear_line_point[i] == 0){
				for(j=3;j<FIELD_WIDTH - 3;j++){
					stage[i][j] = 0;
				}
			}
		}
		clear_count++;
	}
	else{
		for(i=FIELD_HEIGHT - 4;i >= 0;i--){
			if(clear_line_point[i] != 0){
				remain_line_point[remain_line_index] = i;
				remain_line_index++;
			}
		}
		remain_line_index = 0;
		for(i=FIELD_HEIGHT - 4;i >= 0;i--){
			for(j=3;j<FIELD_WIDTH - 3;j++){
				stage[i][j] = stage[remain_line_point[remain_line_index]][j];
			}
			remain_line_index++;
		}
		clear_flag = 0;
		clear_count = 0;
		my_init_var2();
	}
}

void my_timer(){
	int x,y;
	for(y=0;y<10000;y++){
		for(x=0;x<10000;x++){
		}
	}
}

void my_collision_turn(){
	int x,y;

	collision_flag = 0;

	for(y=0;y<BLOCK_HEIGHT;y++){
		for(x=0;x<BLOCK_WIDTH;x++){
			if(turn_block[y][x] != 0){
				if(stage[block_y + y][block_x + x] != 0){
					collision_flag = 1;
				}
			}
		}
	}
}

void my_turn_right(){
	int x,y;

	turn_point++;
	
	for(y=0;y<BLOCK_HEIGHT;y++){
		for(x=0;x<BLOCK_WIDTH;x++){
			turn_block[y][x] = 
				blocks[(block_id * BLOCK_HEIGHT) + y][(turn_point % 4 * BLOCK_WIDTH) + x];
		}
	}

	my_collision_turn();

	if(collision_flag == 0){
		for(y=0;y<BLOCK_HEIGHT;y++){
			for(x=0;x<BLOCK_WIDTH;x++){
				block[y][x] = turn_block[y][x];
			}
		}
	}
	else{
		turn_point--;
	}
}



int main(){

	my_init_var();

	while(gameover_flag == 0){
		my_clear_field();

		if(clear_flag == 0){
			my_make_block();
			my_gameover();
			my_get_key();
			my_make_field();
			my_fix_block();
			my_fall_block();
		}
		else{
			my_clear_line();
			my_make_field2();
		}
		my_draw_field();
		my_timer();
	}
	printf("gameover\n");		
	return 0;
}

