/****************************************************************************/
/* FlashSaurus54 概要                                                       */
/****************************************************************************/

/**
 * フラッシュサウルス54
 * 
 * 説明用に作られた、Arduino 向けフラッシュザウルス
 * 
 * 見やすくするため略記できるところもわざと冗長に書いた
 * 説明に合わせるため最善でない表現あり
 * 動けばいいやの精神、プロトタイプ宣言やヘッダファイルなどは見なかったことにする
 * コメント超多め、一部 Javadoc 風
 * 
 * @author 高校電工54期
 */

/****************************************************************************/
/* 外部ファイルの読み込み                                                      */
/****************************************************************************/

#include "Button.cpp"
#include "LED.cpp"

/****************************************************************************/
/* 変数 / 定数の宣言                                                          */
/****************************************************************************/

const unsigned int GAME_FREQUENCY = 500;

// 操作用ボタンのインスタンスを作成
Button button = Button(2);

// LED のインスタンスを作成
LED leds[] = {
  LED(13),
  LED(12),
  LED(11)
};

// 配列 leds の要素数を計算
const unsigned int ledsSize = sizeof(leds) / sizeof(LED);

// loop で呼ばれる関数のポインタ
void (* process)();

// 1 [ms] 毎に 1 インクリメントされる変数 (delay は処理のすべてが止まってしまうため基本使わない)
unsigned long timer = -1;

/****************************************************************************/
/* Arduino 要求関数の実装                                                     */
/****************************************************************************/

/**
 * 初期設定を行なう関数
 * 各種インスタンスの生成は変数の宣言時点でしてしまってるため不要
 */
void setup() {
  // シリアル通信を開始
  Serial.begin(9600);
  
  // プロトタイプ宣言をしていない都合で preparation を初期値にすることができない為ここでする
  process = preparation;
}

/**
 * ループ関数
 * process を実行し続ける
 * 管理しにくいので、1 ループ毎に 1 [ms] 停止する
 * ゲームそのものに影響はないはず
 */
void loop() {
  process();
  timer++;
  delay(1);
}

/****************************************************************************/
/* 独自の関数                                                               */
/****************************************************************************/

/**
 * loop で呼ばれる関数を設定する
 * timer をリセット、LED もいったんすべて消灯する
 * @param 設定される関数のポインタ
 */
void setProcess(void (*paramProcess)()) {
  
  process = paramProcess;
  timer = -1;
  
  for (int i = 0; i < ledsSize; i++) {
    leds[i].set(false);
  }
  
}

/**
 * 遊ばれるまでの待機中に呼ばれる関数
 * アニメーションとかしておく
 */
void preparation() {

  /*
   * アニメーション用の変数
   * 関数内変数は通常は関数の呼ばれる度にリセットされるが、static のついたものはその限りでない
   */
  // 前回アニメ―トしたときの状態を保存しておく
  static bool before = true;

  /*
   * アニメータ本体
   */

  // GAME_FREQUENCY [ms] 秒に一度切り替える <=> counter ≡ 0 (mod GAME_FREQUENCY)
  if (timer % GAME_FREQUENCY == 0) {
    
    // before を反転させる
    // 反転させたので、before といいつつこの場面では現在のことを表している
    before = !before;

    for (int i = 0; i < ledsSize; i++ ) {

    // 偶数番目の LED かそうでないかで場合分け
      if (i % 2 == 0) {
        leds[i].set(before);
      }

      else {
        leds[i].set(!before);
      }

    }
    
  }

  /*
   * ステータス遷移判定
   * ボタンが押されていれば、プロセスを game に変更
   * 関数内変数のリセットもしておく
   */
  if (button.isPushed()) {
    before = true;
    setProcess(game);
  }
  
}

/**
 * ゲーム本体を司る関数
 */
void game() {

  /*
   * ボタンが押されているかの判定
   */

  // 処理する LED を選択
  unsigned int index = timer / GAME_FREQUENCY;

  // 押されていれば終了
  if (button.isPushed()) {

    // (led の数 - 1) と index が等しいなら成功
    if (index == ledsSize - 1) {
      setProcess(successful);
    }
    
    else {
      setProcess(failed);
    }
    
  }

  // 押されていなければ LED 遷移、ただし GAME_FREQUENCY [ms] に 1 回だけ
  else if (timer % GAME_FREQUENCY == 0) {

    // led の数より index が小さければ、ゲームが続行されている
    if (index < ledsSize) {

      // 点灯
      leds[index].set(true);

      // 1 つ前の LED を消灯、ただし初回はスルー
      if (0 < index) {
        leds[index - 1].set(false);
      }

    }

    // led の数と index が等しければ失敗扱いで終了
    else {
      setProcess(failed);
    }
    
  }

}

/**
 * ゲームが成功したときに呼ばれる関数
 */
void successful() {
  Serial.println("successful!");
  setProcess(preparation);
}

/**
 * ゲームが失敗したときに呼ばれる関数
 */
void failed() {
  Serial.println("failed...");
  setProcess(preparation);
}
