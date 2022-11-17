#include <stdio.h>
#include <stdlib.h>
#include "conio.h"
#include <windows.h>
#include "tetris.h"

int horizon = 0; // [X座業] ←→キーで操作可能
int fall = 0;    // [Y座標] ループ毎に加算
int stopflg = 0; // 落下中ブロックの停止フラグ
int dlskflg = 0; // "↓"入力でブロックを早く落下させるためのフラグ
int key;         // キー入力コードを代入する変数
int field[F_HEIGHT][F_WIDTH]; // 全体のフィールド
int block[B_HEIGHT][B_WIDTH]; // 落下させるブロック
int* pfield = &field[0][0]; // フィールド全体のアドレスを格納
int* pnwfield[NO_WALL_NUM]; // 壁を除いたフィールドの各マスのアドレスを格納

struct BLOCK* pst;

struct BLOCK blocks[] = {
    {
        {STICK},
        {"stick"},
        {{0,0,0,0},
         {1,1,1,1},
         {0,0,0,0},
         {0,0,0,0}}
    },
    {
        {SQUARE},
        {"square"},
        {{0,0,0,0},
         {0,1,1,0},
         {0,1,1,0},
         {0,0,0,0}}
    },
    {
        {MOUNT},
        {"mount"},
        {{0,0,0,0},
         {0,1,0,0},
         {1,1,1,0},
         {0,0,0,0}}
    },
    {
        {LWORD1},
        {"lword1"},
        {{0,0,0,0},
         {0,0,1,0},
         {1,1,1,0},
         {0,0,0,0}}
    },
    {
        {LWORD2},
        {"lword2"},
        {{0,0,0,0},
         {1,0,0,0},
         {1,1,1,0},
         {0,0,0,0}}
    },
    {
        {TWIST1},
        {"twist1"},
        {{0,0,0,0},
         {1,1,0,0},
         {0,1,1,0},
         {0,0,0,0}}
    },
    {
        {TWIST2},
        {"twist2"},
        {{0,0,0,0},
         {0,1,1,0},
         {1,1,0,0},
         {0,0,0,0}}
    }
};

int savef[F_HEIGHT][F_WIDTH] = {
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};
int* psavef = &savef[0][0]; // フィールド保存用テーブルのアドレスを格納

//関数ポインタ(引数：なし　戻り値：なし)
void (*pfunc[]) () = { make_wall,
                       block_plus_field,
                       draw_field,
                       init_field,
                       save_field,
                       stop_block,
                       erase_block,
                       control_block,
                       fall_block_at_once,
                       finish_game
};


void main()
{
    int loopcount = 0; //main関数内のfor文がループした回数をカウント(一定回数のループでブロックを下にスライド)

    // 範囲乱数公式：最小値 + (int)( rand() * (最大値 - 最小値 + 1.0) / (1.0 + RAND_MAX) )
    pst = &blocks[(int)((double)rand() * (sizeof(blocks) / sizeof(struct BLOCK)) / (1.0 + RAND_MAX))];

    int x, y;
    int z = 0;

    // 左右及び下の壁を除く、フィールド各マスのアドレスリストを作成
    for (y = 0; y < F_HEIGHT - 1; y++) // 下の壁を除くために"-1"
    {
        for (x = 1; x < F_WIDTH - 1; x++) // 左右の壁を除くために"-1"
        {
            pnwfield[z] = &field[y][x];
            z++;
        }
    }

    for (;;)
    {
        dlskflg = 0; // ブロックを早く落下させるフラグをループ毎に初期化
        loopcount++;

        pfunc[CONTROL_BLOCK](); // ブロックを操作する（左右上下、回転）

        if (strcmp(get_block_landing_state(), "block_landed") == 0)
        {
            pfunc[STOP_BLOCK](); //ブロックが下に着地したかどうかの判定を実施
            pfunc[SAVE_FIELD](); //現在のフィールドの状態を保存
        }
        if ((loopcount == FALL_LOOP_NUM) || dlskflg)// 一定時間経過 or ↓キー押下で、ブロックを１マス下に移動
        {
            fall++;
            loopcount = 0;
            dlskflg = 0;
        }
        pfunc[MAKE_WALL](); //関数ポインタ・←↓→の壁の生成
        pfunc[INIT_FIELD]();       // フィールドの初期化（消去）
        pfunc[BLOCK_PLUS_FIELD](); // (このループの中で、3→1になっている)生成したブロックに壁を足し合わせ
        pfunc[DRAW_FIELD]();       // 12×21の壁を生成
    }
}

void control_block() // ブロックを操作する（左右上下、回転）
{
    int i;

    if (_kbhit()) //何かしらのキー入力を検知
    {
        key = _getch(); //キー入力コードを取得

        switch (key)
        {
            case PIVOT_CW:
            case PIVOT_CCW:

                roll(blocks, key); //ブロックの回転(ブロック、回転方向

                pfunc[INIT_FIELD]();
                pfunc[BLOCK_PLUS_FIELD]();

                for (i = 0; i < 3; i++)
                {
                    if (strcmp(get_field_state_when_roll(), "block_overlap_on_right") == 0)
                    {
                        slide_block("left");

                        pfunc[INIT_FIELD]();
                        pfunc[MAKE_WALL]();
                        pfunc[BLOCK_PLUS_FIELD]();

                        if (strcmp(get_field_state_when_roll(), "block_overlap_on_left") == 0)
                        {
                            if (key == PIVOT_CW)
                            {
                                roll(blocks, PIVOT_CCW);
                            }
                            else
                            {
                                roll(blocks, PIVOT_CW);
                            }
                        }
                    }
                    else if (strcmp(get_field_state_when_roll(), "block_overlap_on_left") == 0)
                    {
                        slide_block("right");

                        pfunc[INIT_FIELD]();
                        pfunc[MAKE_WALL]();
                        pfunc[BLOCK_PLUS_FIELD]();

                        if (strcmp(get_field_state_when_roll(), "block_overlap_on_right") == 0)
                        {
                            if (key == PIVOT_CCW)
                            {
                                roll(blocks, PIVOT_CW);
                            }
                            else
                            {
                                roll(blocks, PIVOT_CCW);
                            }
                        }
                    }
                }
                break;

            case BUTTON_ARROW://入力キーが矢印キーのどれかの場合
            {
                key = _getch();//矢印キー入力はコードが次に来る

                switch (key)
                {
                case BUTTON_LEFT:
                    if (strcmp(get_field_state_when_slide(BUTTON_LEFT), "block_on_left_block") != 0)
                    {
                        slide_block("left");
                    }
                    break;

                case BUTTON_RIGHT:
                    if (strcmp(get_field_state_when_slide(BUTTON_RIGHT), "block_on_right_block") != 0)
                    {
                        slide_block("right");
                    }
                    break;

                case BUTTON_DOWN: // "↓"(下)を入力
                    dlskflg = 1;
                    break;

                case BUTTON_UP: // "↑"(上)を入力
                    pfunc[FALL_BLOCK_AT_ONCE]();
                    break;

                default:
                    break;
                }
            }
        }
    }
}

char* get_field_state_when_slide(int direction) // ブロックが移動しようとしている方向に、他のブロックや壁がないか確認
{
    int x, y, *tgt1, *tgt2;

    // ブロック＋上下左右1マス分の状態を精査
    for (y = -1; y < B_HEIGHT + 1; y++)
    {
        for (x = -1; x < B_WIDTH + 1; x++)
        {
            // tgt1は一番左上から精査、tgt2はtgt1の右隣
            tgt1 = &field[x + fall][y + horizon + INITX];
            tgt2 = tgt1 + 1;

            switch (*tgt1 + *tgt2)
            {
                case BLOCK_ON_BLOCK:
                case BLOCK_ON_FIELD_WALL:

                    if (direction == BUTTON_RIGHT)
                    {
                        if (*tgt1 == BLOCK_WHEN_FALLING && (*tgt2 == BLOCK_AFTER_LANDING || *tgt2 == FIELD_WALL))
                        {
                            return "block_on_right_block";
                        }
                    }
                    else if (direction == BUTTON_LEFT)
                    {
                        if (*tgt2 == BLOCK_WHEN_FALLING && (*tgt1 == BLOCK_AFTER_LANDING || *tgt1 == FIELD_WALL))
                        {
                            return "block_on_left_block";
                        }
                    }
                default:
                    break;
            }
        }
    }
    return "block_on_nothing";
}


char* get_field_state_when_roll(void) // 回転したブロックが、他のブロックや壁と重なってないか確認
{
    int x, y, * tgt1, * tgt2;

    // ブロック＋上下左右1マス分の状態を精査
    for (x = -1; x < B_HEIGHT + 1; x++)
    {
        for (y = -1; y < B_WIDTH + 1; y++)
        {
            // tgt1は一番左上から精査、tgt2はtgt1の右隣
            tgt1 = &field[x + fall][y + horizon + INITX];
            tgt2 = tgt1 + 1;

            if (*tgt1 == BLOCK_WHEN_FALLING && (*tgt2 == BLOCK_OVERLAP_WITH_BLOCK || *tgt2 == BLOCK_OVERLAP_WITH_FIELD))
            {
                return "block_overlap_on_right";
            }
            else if (*tgt2 == BLOCK_WHEN_FALLING && (*tgt1 == BLOCK_OVERLAP_WITH_BLOCK || *tgt1 == BLOCK_OVERLAP_WITH_FIELD))
            {
                return "block_overlap_on_left";
            }
        }
    }
    return "block_on_nothing";
}

void make_wall() //フィールドの壁を生成
{
    int x, y;

    for (y = 1; y < F_HEIGHT; y++)
    {
        for (x = 0; x < F_WIDTH; x++)
        {
            field[y][0]            = FIELD_WALL; // 左の壁
            field[y][F_WIDTH - 1]  = FIELD_WALL; // 右の壁
            field[F_HEIGHT - 1][x] = FIELD_WALL; // 下の壁
        }
    }
}

void block_plus_field() //フィールドとブロックを足し合わせる
{
    int x, y;

    for (x = 0; x < F_NUM; x++)
    {
        *(pfield + x) += *(psavef + x); //現在のフィールド + 保存したフィールド
    }

    // 落下中のブロックをフィールドに足し合わせる
    for (y = 0; y < B_HEIGHT; y++)
    {
        for (x = 0; x < B_WIDTH; x++)
        {
            {
                field[y + fall][x + horizon + INITX] += pst->shape[y][x];
            }
        }
    }
}

void save_field() // フィールドの状態を保存
{
    int i;

    stopflg = 0;

    for (i = 0; i < F_NUM; i++)
    {
        // 左右下の壁以外の情報を保存
        if (*(pfield + i) == FIELD_WALL)
        {
            *(psavef + i) = 0;
        }
        else
        {
            *(psavef + i) = *(pfield + i);
        }
    }
}

void draw_field() //フィールド上の壁やブロックを認識しやすい記号に置換
{
    int x, y;

    system("cls"); // コンソールの内容をクリア

    for (y = 0; y < F_HEIGHT; y++)
    {
        for (x = 0; x < F_WIDTH; x++)
        {
            switch (field[y][x])
            {
                case BLOCK_WHEN_FALLING:  // 落下中のブロック
                case BLOCK_AFTER_LANDING: // 落下後のブロック
                    printf("□");
                    break;

                case FIELD_WALL:  // 左右下の壁
                    printf("■");
                    break;

                default:
                    printf("　");
                    break;
            }
        }
        printf("\n");
    }
}

void init_field() // フィールドを初期化
{
    int i;

    for (i = 0; i < NO_WALL_NUM; i++)
    {
        *pnwfield[i] = 0;
    }
}

void roll(struct BLOCK* hblock, int direction) // 落下中のブロックを回転
{
    int x, y;

    int preblock[B_HEIGHT][B_WIDTH]; // 回転前のブロック状態を保存するための配列

    for (y = 0; y < B_HEIGHT; y++)
    {
        for (x = 0; x < B_WIDTH; x++)
        {
            preblock[y][x] = pst->shape[y][x]; //回転前のブロック状態を保存
        }
    }

    if (direction == PIVOT_CW) //ブロックを右回りに回転
    {
        for (y = 0; y < B_HEIGHT; y++)
        {
            for (x = 0; x < B_WIDTH; x++)
            {
                pst->shape[y][x] = preblock[3-x][y];
            }
        }
    }
    else //ブロックを左回りに回転
    {
        for (y = 0; y < B_HEIGHT; y++)
        {
            for (x = 0; x < B_WIDTH; x++)
            {
                pst->shape[y][x] = preblock[x][3-y];
            }
        }
    }
}

void fall_block_at_once(void) // "↑"キー押下時、ブロックを一気に下に落とす
{
    int i;

    for (i = 0; i < F_HEIGHT; i++)
    {
        fall++;
        pfunc[INIT_FIELD]();       // フィールドの初期化（消去）
        pfunc[BLOCK_PLUS_FIELD](); // (このループの中で、3→1になっている)生成したブロックに壁を足し合わせ

        if (strcmp(get_block_landing_state(), "block_landed") == 0)
        {
            pfunc[STOP_BLOCK]();
            return;
        }
    }
}

char* get_block_landing_state() // ブロックが着地したかどうか判定
{
    int i;

    for (i = 0; i < F_NUM; i++)
    {
        // 落下中のブロックがフィールドの壁もしくは落下済ブロックの上に着地した時
        if ((*(pfield + i) == BLOCK_WHEN_FALLING) && (*(pfield + F_WIDTH + i) == BLOCK_AFTER_LANDING || *(pfield + F_WIDTH + i) == FIELD_WALL))
        {
            return "block_landed";
        }
    }
    return "block_not_landed";
}

void stop_block() // 着地したブロックの動きを止める
{
    int i;

    pfunc[MAKE_WALL](); //関数ポインタ・←↓→の壁の生成
    pfunc[INIT_FIELD]();
    pfunc[BLOCK_PLUS_FIELD]();
    pfunc[DRAW_FIELD]();       // 12×21の壁を生成

    int waitms = 150; // ブロックが着地してから左右に動かせる時間の猶予
    Sleep(waitms);

    // ブロックが着地してから、ブロックを少しだけ左右に動かすための猶予を与える
    if (_kbhit()) //何かしらのキー入力を検知
    {
        key = _getch(); //キー入力コードを取得

        switch (key)
        {
            case BUTTON_ARROW://入力キーが矢印キーのどれかの場合
            {
                key = _getch();//矢印キー入力はコードが次に来る

                switch (key)
                {
                case BUTTON_LEFT:
                    if (strcmp(get_field_state_when_slide(BUTTON_LEFT), "block_on_left_block") != 0)
                    {
                        slide_block("left");
                    }
                    break;

                case BUTTON_RIGHT:
                    if (strcmp(get_field_state_when_slide(BUTTON_RIGHT), "block_on_right_block") != 0)
                    {
                        slide_block("right");
                    }
                    break;
                }
            }
        }
    }
    pfunc[INIT_FIELD]();
    pfunc[MAKE_WALL]();
    pfunc[BLOCK_PLUS_FIELD]();

    stopflg = 1;

    for (i = 0; i < NO_WALL_NUM; i++)
    {
        if (*pnwfield[i] == BLOCK_WHEN_FALLING)
        {
            *pnwfield[i] = BLOCK_AFTER_LANDING;
        }
    }
    erase_block(); // ブロックを削除
    finish_game(); // 一番上の行にブロックがある場合はゲームオーバー

    fall = 0;    // ブロック縦位置を初期化
    horizon = 0; // ブロック横位置を初期化
    pst = &blocks[(int)((double)rand() * (sizeof(blocks) / sizeof(struct BLOCK)) / (1.0 + RAND_MAX))];
}


void erase_block()  // ブロックで埋まった行を削除する
{
    int x, y, column, lowest, saveflg = 1;
    int dltrownum = 0; // 削除した行数の合計

    for (y = F_HEIGHT - 2; y > 0; y--) //行の精査・下から
    {
        column = 0; //ある行の数字合計数の初期化

        for (x = 1; x < F_WIDTH - 1; x++) //列の精査
        {
            column += field[y][x];

            if (column == (BLOCK_AFTER_LANDING * (F_WIDTH - 2))) //一つの行に全てブロックがある場合
            {
                column  = 0; // 合計数をリセット
                saveflg = 0; //ブロックを消去する場合、セーブはしない
                //erasenum ++; //消去する行の数をカウントする

                lowest = F_HEIGHT - 1 - y; //ブロックを最後に消した行の高さを格納する

                for (x = 1; x < F_WIDTH - 1; x++)
                {
                    field[y][x] = 0; //消去する行を"0"で初期化する  
                }
                dltrownum++;
            }
        }   
    }
    fill_erased_row(dltrownum);
    pfunc[SAVE_FIELD]();
}

void fill_erased_row(int dltnum) // ブロック消去時に全て空欄の行ができた場合、下に詰める
{
    int x, y, xx, i;
    int nowrow = 0;
    int continurow;

    x = F_HEIGHT - 2; // 精査はフィールドの一番下から開始

    while(dltnum)
    {
        continurow = 0;

        if (!get_row_sum(x)) // 空欄の行があるか精査
        {
            continurow = 1;

            // 空欄の行がある場合、何行続くか精査（最大で４行）
            for (i = 1; i <= 3; i++)
            {
                if (!get_row_sum(x - i))
                {
                    continurow++;
                }
                else // 空欄の行が連続しなくなったら、その時点で加算を辞める
                {
                    break;
                }
            }

            // 連続で空欄になっている行数の分、下に詰める
            for (xx = x; (xx - continurow) > 0; xx--)
            {
                for (y = 1; y < F_WIDTH - 1; y++)
                {
                    field[xx][y] = field[xx - continurow][y];
                }
            }

            // 削除した行数の分、上の方を空欄にする
            for (xx = 0; xx < continurow; xx++)
            {
                for (y = 1; y < F_WIDTH - 1; y++)
                {
                    field[xx][y] = 0;
                }
            }
            dltnum = dltnum - continurow; // 消す必要がある列の数を再演算
        }
        else
        {
            x--;
        }
    }
}

void finish_game() // 一番上の行の真ん中の４列どこかにブロックが詰まれた時、ゲームオーバーにする
{
    int x, y;

    for (y = 0; y < 2; y++) // 一番上とその下の行だけ精査
    {
        for (x = 4; x < 8; x++) //列の精査
        {
            if (field[y][x] == BLOCK_AFTER_LANDING)
            {
                printf("- GAME OVER -\n");
                system("PAUSE");
            }
        }
    }
}


int get_row_sum(int height) // 指定した行の数字の合計を演算して返す
{
    int i;
    int sum = 0;

    for (i = 1; i < F_WIDTH - 1; i++)
    {
        sum += field[height][i];
    }
    return sum;
}

void slide_block(char* direction) // ブロックを左右に移動させる
{
    if (strcmp(direction, "right") == 0)
    {
        horizon++;
    }
    else if (strcmp(direction, "left") == 0)
    {
        horizon--;
    }
}