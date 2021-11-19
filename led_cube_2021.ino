// LED キューブ制御用プログラム 2021

#include "led_patterns.h"


// 1 辺の LED の個数
constexpr size_t CUBE_SIZE = 6;

// シフトレジスタの個数
constexpr size_t REGISTER_NUMBER = (CUBE_SIZE * (CUBE_SIZE + 1) - 1) / 8;

// 表示する層を切り替えるまでの時間、インターバル（単位：マイクロ秒）
constexpr unsigned int PLANE_TIME = 1;

// シフトレジスタ 74HC595 ピン配置
//           16 15 14 13 12 11 10  9
//         +--O--O--O--O--O--O--O--O--+
//         | )                        |
//         +--O--O--O--O--O--O--O--O--+
//            1  2  3  4  5  6  7  8
//    1: QB, 2: QC, 3: QD, 4: QE, 5: QF, 6: QG, 7: QH, 8: GND
//    9: GH', 10: ~SRCLR, 11: SRCLK, 12: RCLK, 13: ~OE, 14: SER, 15: QA, 16: VCC

constexpr int PIN_SER = 2;  // レジスタの SER (14番)  に繋がってるピン
constexpr int PIN_LATCH = 4;  // レジスタの RCLK (12番)  に繋がってるピン
constexpr int PIN_CLK = 3;  // レジスタの SRCLK (11番) に繋がってるピン
constexpr int PINS_PLANE[CUBE_SIZE] { A5, A4, A3, A2, A1, A0 };  // 各層に繋がっているピン


// LED 制御プログラムのステップ
struct led_program_step {
    const byte* pattern;
    unsigned long duration;
    constexpr led_program_step(const byte (&pattern)[CUBE_SIZE * CUBE_SIZE], unsigned long duration)
        : pattern(pattern), duration(duration) {}
    void output() const;
};

// LED 制御プログラム
//     { パターン名, 継続時間 },
//     上の形式で出力するパターンとその継続時間を指定したものを並べる形でプログラムを作成してください。
//     継続時間の単位はミリ秒です。
constexpr led_program_step led_program[] {
    //LED文字
    {c1, 500}, {c2, 500}, {c3, 500},
    //ランダム渦巻 
    {e1, 100}, {e2, 100}, {e3, 100}, {e4, 100}, {e5, 100}, {e6, 100}, {e7, 100}, {e8, 100}, {e9, 100}, {e10, 100}, 
    {e11, 100}, {e12, 100}, {e13, 100}, {e14, 100}, {e15, 100}, {e16, 100}, {e17, 100}, {e18, 100}, {e19, 100}, {e20, 100},
    {e21, 100}, {e22, 100}, {e23, 100}, {e24, 100}, {e25, 100}, {e26, 100},{all, 500},
    //f-9ブロック移動
    {f1, 500}, {f2, 500}, {f3, 500}, {f4, 500}, {f5, 500}, {f6, 500}, {f1, 500}, {f2, 500}, 
    //nag-流れる
    {nothing, 500},{nag2, 200}, {nag3, 200},{nag4, 200},{nag5, 200},{nag6, 200},{nag7, 200},{nag8, 200},{nag9, 200},{nag10, 200},
    {nag11, 200},{nag12, 200},{nag13, 200},{nag14, 200},{nag15, 200},{nag16, 200}, {all, 500},//nagare
    //nitac
    {nothing, 400}, {led_n, 400}, {led_i, 400}, {led_t, 400}, {led_a, 400}, {led_c, 400}, 
    //d－ランダム点灯
    {d1, 0500}, {d2, 0500}, {d3, 0500}, {d4, 0500}, {d1, 0500}, {d2, 0500}, {d3, 0500}, {d4, 0500},
    //h-滝
    {nothing, 0500}, {h1, 200}, {h2, 200}, {h3, 200}, {h4, 200}, {h5, 200}, {h6, 200}, {h7, 200}, {h8, 200},
    {h9, 200}, {h10, 200}, {h11, 200}, {h12, 200}, {h13, 200}, {h14, 200}, {h15, 200}, {h16, 200}, {h17, 200},
    //into the light 
    {nothing, 0400}, {led_i, 0400}, {led_n, 0400}, {led_t, 0400}, {led_o, 0600}, {led_t, 0400}, {led_h, 0400}, {led_e, 0600}, {led_l, 0400}, {led_i, 0400}, {led_g, 0400}, {led_h, 0400}, {led_t, 0600},
    { nothing, 500 },
    { itl01, 200 }, { itl02, 200 }, { itl03, 200 }, { itl04, 200 }, { itl05, 200 }, { itl06, 200 }, { itl07, 200 }, { itl08, 200 }, { itl09, 200 }, { itl10, 200 }, 
    { itl11, 200 }, { itl12, 200 }, { itl13, 200 }, { itl14, 200 }, { itl15, 200 }, { itl16, 200 }, { itl17, 200 }, { itl18, 200 }, { itl19, 200 }, { itl20, 200 },
    { itl21, 200 }, { itl22, 200 }, { itl23, 200 }, { itl24, 200 }, { itl25, 200 }, { itl26, 200 }, { itl27, 200 }, { itl28, 200 }, { itl29, 200 }, { itl30, 200 }, 
    { itl31, 200 }, { itl32, 200 }, { itl33, 200 }, { itl34, 200 }, { itl35, 200 }, { itl36, 200 }, { itl37, 200 }, { itl38, 200 }, { itl39, 200 }, { itl40, 200 },
    { itl41, 200 }, { itl42, 200 }, { itl43, 200 }, { itl44, 200 }, { itl45, 200 }, { itl46, 200 }, { itl47, 200 }, { itl48, 200 }, { itl49, 200 }, { itl50, 200 },
    { itl51, 200 }, { itl52, 200 }, { itl53, 200 }, { itl54, 200 }, { itl55, 200 }, { itl56, 200 }, { itl57, 200 }, { itl58, 200 }, { itl59, 200 }, { itl60, 200 },
    { itl61, 200 }, { itl62, 200 }, { itl63, 200 }, { itl64, 200 }, { itl65, 200 }, { itl66, 200 }, { itl67, 200 }, { itl68, 200 }, { itl69, 200 }, { itl70, 200 },
    { itl71, 200 }, { itl72, 200 }, { itl73, 200 }, { itl74, 200 }, { itl75, 200 }, { itl76, 200 }, { itl77, 200 }, { itl78, 200 }, { itl79, 200 }, { itl80, 200 },
    { itl81, 200 }, { itl82, 200 }, { itl83, 200 }, { itl84, 200 }, { itl85, 200 }, { itl86, 200 }, { itl87, 200 }, { itl88, 200 }, { itl89, 200 }, { itl90, 200 }, 
    { nothing, 500 },
    //chikachika
    {all,300}, {nothing,300}, {all,300}, {nothing,300} ,
    //g-4ブロック移動
    {g1, 500}, {g2, 500}, {g3, 500}, {g2, 500}, {g1, 500}, {g2, 500}, {g3, 500}, {g2, 500}, 
    //b-渦巻
    {nothing, 500},{b1, 140}, {b2, 140}, {b3, 140}, {b4, 140}, {b5, 140}, {b6, 140}, {b7, 140}, {b8, 140}, {b9, 140}, {b10, 140},
    {b11, 140}, {b12, 140}, {b13, 140}, {b14, 140}, {b15, 140}, {b16, 140}, {b17, 140}, {b18, 140}, {b19, 140}, {b20, 140}, 
    {b21, 140}, {b22, 140}, {b23, 140}, {b24, 140}, {b25, 140}, {b26, 140}, {b27, 140}, {b28, 140}, {b29, 140}, {b30, 140}, 
    {b31, 140}, {b32, 140}, {b33, 140}, {b34, 140}, {b35, 140}, {b36, 140}, {b37, 140}, {b38, 140}, {b39, 140}, {b40, 140}, {b41, 140},{all, 500},
    //上から下 
    { plane1, 500 }, { plane2, 500 }, { plane3, 500 }, { plane4, 500 }, { plane5, 500 }, { plane6, 500 },
    //色別点灯
    { blue, 300 }, { red, 300 }, { green, 300 }, { red, 300 }, { blue, 300 }, { red, 300 }, { green, 300 }, { red, 300 }, { blue, 500 },              
};


// 初期化関数
//     電源を入れたときに一度だけ実行される関数です。
//     ここでピンの設定などを行います。
void setup() {
    // 使用するピンのモードを OUTPUT に設定して、出力の初期状態を設定
    for (auto pin : PINS_PLANE) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
    pinMode(PIN_SER, OUTPUT);
    digitalWrite(PIN_SER, LOW);
    pinMode(PIN_LATCH, OUTPUT);
    digitalWrite(PIN_LATCH, LOW);
    pinMode(PIN_CLK, OUTPUT);
    digitalWrite(PIN_CLK, LOW);
}

// ループ関数
//     初期化後に繰り返し実行される関数です。
//     ここで LED の点滅処理を行います。
void loop() {
    // プログラムに沿って LED を出力
    for (const auto& stp : led_program) {
        stp.output();
    }
}

// LED 出力
void led_program_step::output() const {
    // ステップの終了時間
    unsigned long endTime = millis() + duration;
    
    // レジスタへの出力データ
    byte register_output[CUBE_SIZE][REGISTER_NUMBER] {};
    // パターンからレジスタ出力データに変換
    //     111111 000000 111111 000000 1111111 000000
    //     ↓ 6 × 6 を 8 × 5 に詰める
    //     ____1111 11000000 11111100 00001111 11000000
    for (int i = 0; i < CUBE_SIZE; ++i) {
        for (int j = CUBE_SIZE - 1; j >= 0; --j) {
            byte b = pgm_read_byte(pattern + i + (CUBE_SIZE - j - 1) * CUBE_SIZE);  // フラッシュメモリからロード
            int shift = j * CUBE_SIZE % 8;
            for (int k = j * CUBE_SIZE / 8; k <= ((j + 1) * CUBE_SIZE - 1) / 8; ++k) {
                register_output[i][REGISTER_NUMBER - k - 1] |= shift < 0 ? b >> -shift : b << shift;
                shift -= 8;
            }
        }
    }

    int pre_plane = CUBE_SIZE - 1;  // 点灯中の層 (0-indexed)
    int plane = 0;  // 処理中の層 (0-indexed)
    // ステップの終了まで各層を順に繰り返し出力
    while (millis() < endTime) {
        // シフトレジスタを更新
        for (auto b : register_output[plane]) {
            shiftOut(PIN_SER, PIN_CLK, MSBFIRST, b);
        }
        // 点灯中の層を消灯
        digitalWrite(PINS_PLANE[pre_plane], LOW);
        // ストレージレジスタを更新
        digitalWrite(PIN_LATCH, HIGH);
        digitalWrite(PIN_LATCH, LOW);
        // 処理中の層を点灯
        digitalWrite(PINS_PLANE[plane], HIGH);
        // 変数を更新
        pre_plane = plane;
        ++plane;
        plane %= CUBE_SIZE;
        // 少し待機（この間に LED が光る）
        delayMicroseconds(PLANE_TIME);
    }
    // 点灯中の層を消灯
    digitalWrite(PINS_PLANE[pre_plane], LOW);
}
