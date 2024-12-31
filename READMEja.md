input クラス   
scanf に似た仕様で文字列から数値へ変換する C++ クラス
=========

## 概要

 C++ では、C 言語で一般的な可変引数を扱う関数は使わない文化があります。   
   
 一番の問題は、引数の引渡しはスタックを経由する点、又、スタックに引数が何個格納   
 されているのか判らない点です、これによりスタックをオーバーロードさせ、システム   
 に悪影響を与える事が出来てしまいます。   
   
 代表的な実装は「printf/scanf」関数です。   
 コンパイラはフォーマット文を解析して、引数の整合性をチェックしますが、完全には   
 チェックできません。   
 一方、「printf/scanf」は優れた柔軟性をもたらし、文字、数値を扱う事をたやすくします。   
   
 boost には、printf の柔軟性と、安全性を考慮した、format.hpp があります。   
 ※「%」オペレーターのオーバーロード機構を利用して、複数の引数を受け取る事ができます。   
 ※boost::format は優れた実装ですが、iostream に依存していて組み込みマイコンでは問題があります。    
 ※iostream を取り込むと、容量が肥大化します。   
  
 同じように、printf とは逆の動作をする scanf も安全性においては printf と同じ問題   
 を抱えています。   
 そこで、format.hpp の仕組みを継承し、scanf に代わる input クラスを実装しました。   
   
---

## 仕様

- ２進数、８進数、１０進数、１６進数、浮動小数点、文字などを受け取る事が出来ます
- 名前空間は「utils」です
- 文字列の受け取りは、ファンクタを定義して、テンプレートパラメーターとします
- 標準的なファンクタ「def_chainp」クラスが定義されており、以下のように typedef されています

```C++
   	typedef basic_input<def_chainp> input;
```

-「def_chainp」クラスも、「input.hpp」に含まれています。

標準のファンクタは、標準入力から文字を受け取りますが、キャラクター型ポインター   
を定義する事ができ、「sscanf」のように機能します。   
   
組み込みマイコンで使う事を考えて、エラーに関する処理では、「例外」を送出しません。   
入力変換時に起こったエラーは、エラー種別として取得する事ができます。   

- 一般的に、例外を使うと多くのメモリを消費します
- 例外を使った場合、エラーが発生して、正しい受取先が無い場合、致命的な問題を引き起こします
- 複数の変換で、エラーが同時に発生すると、最後のエラーが残ります
- 複数の分離キャラクターを扱える正規表現があります
- %b ---> ２進の数値
- %o ---> ８進の数値
- %d ---> １０進の数値
- %u ---> 符号無し１０進（v111 で廃止、%d で行う）
- %x ---> １６進の数値
- %f ---> 浮動小数点数（float、double）
- %c ---> １文字のキャラクター
- %a ---> 自動、２進(bnnn)、８進(onnn)、１０進、１６進(xnnn)、を判別
- %c は、半角文字のみ対応
- '%'、'['、']'、などの特殊文字を、分離キャラクターとして使う場合は、「バックスラッシュ」を使う

---
## 使い方

### input.hpp をインクルードします。

```C++
#include "input.hpp"
```

- エクスポーネント算術に「std::pow」関数を利用する為、算術ライブラリのリンクが必要です
- 全ての機能を使うのに必要なヘッダーは「input.hpp」のみです

### 名前空間は「utils」です。

### サンプル

- 標準入力から、変数「a」に１０進数を受け取ります
- 標準入力から受け取る場合、入力の終端は、'\n'（改行）とします

```C++
   int a;
   utils::input("%d") % a;
```

- 以下のように変換ステートを受け取る事が出来るので、変換に失敗した場合を検出できます。   

```C++
   int a;
   if((utils::input("%d") % a).state()) {
       // OK
   } else {
       // NG
   }
```

- 受け取り時、どんなエラーがあったのかを判別したい場合、以下のようにエラー種別を取得する事ができます。

```C++
   int a;
   auto err = (utils::input("%d") % a).get_error();
   if(err == utils::input::error::none) {
       // OK
   } else {
       // NG: 「err」種別により、相当する処理
   }
```
   
- エラー種別は enum class で定義されます

```C++
  enum class error : uint8_t {
    none,            ///< エラー無し
    cha_sets,        ///< 文字セットの不一致
    partition,       ///< 分離キャラクターの不一致
    input_type,      ///< 無効な入力タイプ
    not_integer,     ///< 整数型の不一致
    different_sign,  ///< 符号の不一致（v111 で廃止）
    sign_type,       ///< 符号無し整数にマイナス符号
    not_float,       ///< 浮動小数点型の不一致
    terminate,       ///< 終端文字の不一致
    overflow,        ///< オーバーフロー
  };
```
   
- 文字列から受け取る場合(sscanf 相当)は、以下のようにします
- 第二引数が省略された場合、上の例のように、標準入力となります

```C++
   int a;
   static const char* inp = { "1234" };
   utils::input("%d", inp) % a;
```

- 特殊文字「%, [, ]」を分離キャラクターとして使う場合、「\」（バックスラッシュ）で除外します
-「\」を文字列に１文字含める場合、「\\」とします

```C++
    static const char* inp = { "123%456[789]5678" };
    int a[4] = { -1 };
    auto err = (input("%d\\%%d\\[%d\\]%d", inp) % a[0] % a[1] % a[2] % a[3]).get_error();
    std::cout << "Special character separator as '" << inp << "': "
        << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3];
    if(err == input::error::none) {

    }
```

---
## 拡張機能、分離キャラクタ

数値ブロックを分離するキャラクターには、数値（１０進の場合 0 から 9）以外を指定が出来ます。   
さらに。「[」、「]」で囲まれた任意のキャラクターを指定出来ます。   
以下のサンプルでは、分離キャラクターとして、「 」（スペース）、「,」のどちらかを指定出来ます。   

```C++
    int a = 0;
    uint32_t b = 0;
    int c = 0;
    static const char* inp = { "-99 100,200" };
    auto n = (input("%d[, ]%x[ ,]%d", inp) % a % b % c).num();
    std::cout << "Test05, multi scan for integer: " << a << ", " << b << ", " << c;
    if(n == 3 && a == -99 && b == 0x100 && c == 200) {
        std::cout << "  Pass." << std::endl;
        ++pass;
    } else {
        std::cout << "  Scan NG!" << std::endl;
    }
```

---
## 拡張機能、「%a」オート

- 接頭子として、「b,0b」二進数、「o,0o」八進数、「x,0x」１６進数、「なし」１０進数、浮動小数点を受け付ける機能
- 先頭の「0」は省略可能

```C++
    static const char* inp = { "100 0x9a 0b1101 0o775" 123.456 };
    int a[4] = { -1 };
	float b;
    auto n = (input("%a %a %a %a %a", inp) % a[0] % a[1] % a[2] % a[3] % b).num();
    std::cout << "Multi scan for 'auto': "
        << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3] << " (" << n << ")";
    if(n == 4 && a[0] == 100 && a[1] == 0x9a && a[2] == 0b1101 && a[3] == 0775 && b == 123.456f) {
        std::cout << "  Pass." << std::endl;
        ++pass;
    } else {
        std::cout << "  Scan NG!" << std::endl;
    }
```

---
## 変換エラーが発生した場合：

- 変換エラーが発生した場合、参照へは、結果を返さず、破棄されます

```C++
    int a = -1;
    input("%d", "1O5") % a;
```

上記のように、変換に失敗した（文字の O が含まれる）場合は、初期化の値「-1」が保持されます。   

---

## オーバーフローエラーの挙動：

- 浮動小数点の小数点以下は、桁あふれ以降の数値は無視され、オーバーフローエラーは返しません
- 実数部では、「utils::input::error::overflow」がセットされます
- オーバーフローエラーの場合、オペレーターで指定された参照へは、オーバーフローする前の値を返します

---
## プロジェクト（全体テスト）

input クラスは、全体テストと共に提供されます。   

```sh
make
```
で全体テストがコンパイルされます。   

```sh
make run
```

全体テストが走り、全てのテストが通過すると、正常終了となります。   
※テストに失敗すると、-1 を返します。   

---
      
License   
----

[MIT](LICENSE)

```
Copyright (c) 2020, Hiramatsu Kunihito
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the <organization> nor the　names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
