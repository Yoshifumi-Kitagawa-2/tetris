#ifndef _TETRIS2_H_
#define _TETRIS2_H_

// 落下ブロックの設定
#define INITX   4 //ブロックの初期位置：Y座標
#define FALL_LOOP_NUM 13 //この回数分、メインのfor文がループしたらブロックが一つ下にズレる(落ちる)

// ブロックや壁の置き換え
#define BLOCK_WHEN_FALLING  1 // 落下中のブロック
#define BLOCK_AFTER_LANDING 2 // 落下後のブロック
#define BLOCK_ON_BLOCK      BLOCK_WHEN_FALLING + BLOCK_AFTER_LANDING // 落下中のブロックが落下後のブロックに接している
#define FIELD_WALL          4 // 左右下の壁
#define BLOCK_ON_FIELD_WALL 5 // 落下中のブロックが左右下いずれかの壁に接している
#define BLOCK_OVERLAP_WITH_BLOCK  BLOCK_WHEN_FALLING + BLOCK_AFTER_LANDING // 落下中のブロックが落下後のブロックと重なっている
#define BLOCK_OVERLAP_WITH_FIELD  BLOCK_WHEN_FALLING + FIELD_WALL // 落下中のブロックが左右下いずれかの壁と重なっている

// フィールド及びブロックの設定
#define F_WIDTH  12               // フィールドの幅
#define F_HEIGHT 21               // フィールドの高さ
#define F_NUM  F_WIDTH * F_HEIGHT // フィールドのマスの数
#define NO_WALL_NUM  (F_WIDTH - 2) * (F_HEIGHT - 1) //左右下の壁を除いた時のフィールドのマスの数
#define B_WIDTH  4                // ブロックの幅
#define B_HEIGHT 4                // ブロックの高さ
#define B_NUM  B_WIDTH * B_HEIGHT // ブロックのマスの数

// キーボードの入力
#define BUTTON_ARROW  224 // いずれかの矢印キー  
#define BUTTON_LEFT    75 // ←：ブロックを左に移動させる
#define BUTTON_RIGHT   77 // →：ブロックを右に移動させる
#define BUTTON_UP      72 // ↑：ブロックを一気に落下させる
#define BUTTON_DOWN    80 // ↓：ブロックの落ちるスピードを加速させる
#define PIVOT_CW      120 // X ：ブロックを時計回りに回転させる
#define PIVOT_CCW     122 // Z ：ブロックを逆時計回りに回転させる

//それぞれのブロックの形
#define STICK   0
#define SQUARE  1
#define MOUNT   2
#define LWORD1  3
#define LWORD2  4
#define TWIST1  5
#define TWIST2  6

//関数ポインタ用プリプロセッサ(引数：なし　戻り値：なし)
#define MAKE_WALL           0
#define BLOCK_PLUS_FIELD    1
#define DRAW_FIELD          2
#define INIT_FIELD          3
#define SAVE_FIELD          4
#define STOP_BLOCK          5
#define ERASE_BLOCK         6
#define CONTROL_BLOCK       7
#define FALL_BLOCK_AT_ONCE  8
#define FINISH_GAME         9

//プロトタイプ（引数なし、戻り値なし）
void make_wall(void);          // フィールドの壁を生成
void block_plus_field(void);   // フィールドとブロックを足し合わせる
void draw_field(void);         // フィールド上の壁やブロックを認識しやすい記号に置換
void init_field(void);         // フィールドを初期化
void save_field(void);         // フィールドの状態を保存
void stop_block(void);         // 着地したブロックの動きを止める
void erase_block(void);        // ブロックで埋まった行を削除する
void control_block(void);      // ブロックを操作する（左右上下、回転）
void fall_block_at_once(void); // ブロックを一気に下に落下させる
void finish_game(void);        // ゲームを終了させる

//プロトタイプ（その他）
void fill_erased_row(int rownum);                // ブロック消去時に全て空欄の行ができた場合、下に詰める
void slide_block(char* direction);               // ブロックを左右に移動させる
void roll(struct BLOCK* hblock, int direction);  // ブロックを回転させる
char* get_field_state_when_slide(int direction); // ブロックを左右に移動させた時の周囲の状態を取得
char* get_field_state_when_roll(void);           // ブロックを回転させた時の周囲の状態を取得
char* get_block_landing_state(void);             // ブロックが着地したかどうかを確認
int get_row_sum(int height);                     // 指定した行の状態を取得

// 落下させるブロック
struct BLOCK {
    int num;         // 番号
    char* name;      // ブロックの名前
    int shape[4][4]; // 実際のブロックの形
};

#endif _TETRIS2_H_

