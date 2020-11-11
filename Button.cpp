/****************************************************************************/
/* 外部ファイルの読み込み                                                   */
/****************************************************************************/

#include "Arduino.h"

/****************************************************************************/
/* クラスの宣言                                                             */
/****************************************************************************/

/**
 * プッシュボタンを扱うクラス
 */
class Button {

  private:
    // ボタンが接続されているピン番号
    int pin;
    // 一瞬前のボタンの状態
    bool beforeStatus = false;

  public:
    /**
     * コンストラクタ
     * @param paramPin ボタンのピン番号
     */
    Button(int paramPin) {
      pin = paramPin;
      pinMode(paramPin, INPUT);
    }

    /**
     * ボタンが押されているかどうかを返す関数
     * 押される→離される で 1 操作
     */
    bool isPushed() {

      // いま入力されている 且つ 一瞬前に押されていなかった
      if (digitalRead(pin) == HIGH && beforeStatus == false) 
          beforeStatus = true;

      // いま入力されていない かつ 一瞬前まで押され続けていた
      else if (beforeStatus) {
          beforeStatus = false;
          return true;
      }
      
      return false;
      
    }
  
};
