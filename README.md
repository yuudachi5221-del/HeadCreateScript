# HeadCreateScript

## 要件定義書 v0.2

---

## 1. プロジェクト概要

### プロジェクト名

**Head Create Script**

略称：HCS

### ファイル拡張子

```text
.crs
```

### コンパイラ

C++で実装する。

コンパイラの実行ファイル名は仮に、

```text
hcs.exe
```

とする。

基本的な使用方法：

```text
hcs source.crs
```

または、

```text
hcs source.crs -o program.exe
```

とする。

### 目的

Head Create Scriptは、独自構文を持つプログラミング言語である。

コンパイラは `.crs` ファイルを読み込み、解析・変換を行い、最終的にWindows上で実行可能な `.exe` を生成する。

初期バージョンでは、**Windows + x86-64**を主な対象とする。

---

# 2. 言語設計思想

HCSでは、何らかの要素を「生成・定義」するときに `con` を使用する。

`con` は construction / construct を意味する。

つまり、

```text
con form x=10;
```

は、

> 「xという変数を生成する」

という意味になる。

一方、既に存在する変数への操作では、原則として `con` は使用しない。

例：

```text
con form x=10;

x=20;
```

HCSでは、この **「生成」と「操作」を構文上明確に区別すること**を重要な設計思想とする。

---

# 3. 基本構文

## 3.1 文末

すべての通常の文は `;` で終了する。

例：

```text
con form x=10;
x=20;
dispin("Hello");
```

ブロックを持つ構文についても、ブロック全体の終端に `;` を置く。

例：

```text
con (x<=10)<-if{
    dispin("Hello");
};
```

---

# 4. 変数

## 4.1 `form`

`form` は変数を生成するためのキーワード。

変数は数値・文字列など複数のデータ型を保持できる。

基本構文：

```text
con form <name>=<value>;
```

例：

```text
con form x=10;
con form name="Hello";
```

---

## 4.2 動的な値の変更

`form` で生成された変数は、後から異なる種類の値を代入できる。

例：

```text
con form x=10;

x="Hello";

x=3.14;
```

上記はすべて有効。

したがって、HCSの `form` は初期バージョンでは**動的型の変数**として扱う。

---

## 4.3 空の変数

値を未設定にしたい場合は `blank` を使用する。

```text
con form x=blank;
```

`blank` は特殊な値として扱う。

例：

```text
con form x=blank;

x=10;
```

---

# 5. 定数

## 5.1 `const`

変更できない値を生成する。

構文：

```text
con const <name>=<value>;
```

例：

```text
con const a=9;
```

生成後の再代入は禁止。

```text
con const a=9;

a=10;
```

これはコンパイルエラーとする。

エラーメッセージ例：

```text
Error: cannot assign to constant 'a'.
```

---

# 6. 条件分岐

HCSでは条件分岐も「生成する構造」の一つとして扱う。

そのため、`if`、`ifel`、`else` の生成には `con` が必須。

---

## 6.1 `if`

構文：

```text
con (<condition>)<-if{
    <statements>
};
```

例：

```text
con (x<=1)<-if{
    dispin("x is 1 or less");
};
```

`<-if` は、

> 条件をifへ渡す

という意味を持つ。

---

## 6.2 `ifel`

`ifel` は **if else / else if** 相当。

条件の指定方法は `if` と同じ。

構文：

```text
con (<condition>)<-ifel{
    <statements>
};
```

例：

```text
con (x<=1)<-if{
    dispin("1以下");
}
con (x<=3)<-ifel{
    dispin("3以下");
};
```

複数の `ifel` を連続して記述できる。

例：

```text
con (x<=1)<-if{
    dispin("A");
}
con (x<=3)<-ifel{
    dispin("B");
}
con (x<=5)<-ifel{
    dispin("C");
};
```

---

## 6.3 `else`

`else` は条件を持たない。

そのため空の `()` を使用する。

構文：

```text
con ()<-else{
    <statements>
};
```

例：

```text
con (x<=1)<-if{
    dispin("A");
}
con (x<=3)<-ifel{
    dispin("B");
}
con ()<-else{
    dispin("C");
};
```

意味：

```text
if x <= 1
    A
else if x <= 3
    B
else
    C
```

---

# 7. 条件式

初期バージョンでは以下の比較演算子をサポートする。

```text
==
!=
<
>
<=
>=
```

例：

```text
con (x==10)<-if{
    dispin("10");
};

con (x!=10)<-if{
    dispin("not 10");
};
```

---

# 8. 繰り返し

## 8.1 `for`

HCSでは専用の `for` 構文を使用する。

基本構文：

```text
con for(
    <start>,
    name="<variable-name>",
    <operator><end>,
    range="<step>"
){
    <statements>
};
```

例：

```text
con for(
    0,
    name="i",
    <=10,
    range="1"
){
    dispin(i);
};
```

この場合、

```text
0
1
2
3
...
10
```

と繰り返す。

---

## 8.2 `name`

`name` はループ内部で使用する変数名を指定する。

例：

```text
con for(
    0,
    name="i",
    <=10
){
    dispin(i);
};
```

ループ内部では `i` を参照できる。

---

## 8.3 `range`

`range` はループごとの増減幅。

例：

```text
range="2"
```

なら、

```text
0
2
4
6
8
10
```

のように2ずつ増加する。

`range` を省略した場合は、

```text
1
```

として扱う。

---

## 8.4 減少

負の `range` を使用することで減少ループを作れる。

例：

```text
con for(
    10,
    name="i",
    >=0,
    range="-1"
){
    dispin(i);
};
```

結果：

```text
10
9
8
7
...
0
```

---

## 8.5 `while`

`while` ループは条件が真の間、ブロックを繰り返す。

基本構文：

```text
con (<condition>)<-while{
    <statements>
};
```

例：

```text
con form x=0;

con (x<10)<-while{
    dispin(x);
    x=x+1;
};
```

この場合、

```text
0
1
2
3
...
9
```

と繰り返す。

`while` は `if` や `for` と同じく `con` を使用して生成される構造である。

---

# 9. 表示

## 9.1 `dispin`

標準出力への表示に使用する。

構文：

```text
dispin(<value>);
```

例：

```text
dispin("Hello World");
```

変数も指定可能。

```text
con form x=10;

dispin(x);
```

---

# 10. コメント

コメントは、

```text
</
<comment>
/>
```

の形式。

例：

```text
</
これはコメントです。
複数行記述できます。
/>
```

コメントの内容はコンパイル結果に影響を与えない。

---

# 11. 文字列

文字列はダブルクォーテーションで囲む。

```text
"Hello World"
```

例：

```text
con form text="Hello World";
dispin(text);
```

---

# 12. 数値

整数および小数をサポートする。

例：

```text
con form a=10;
con form b=3.14;
```

初期バージョンでは少なくとも、

```text
integer
floating point
```

を内部的に扱えるようにする。

---

# 13. 真偽値

条件式などで使用するため、真偽値をサポートする。

候補：

```text
true
false
```

例：

```text
con form enabled=true;

con (enabled==true)<-if{
    dispin("enabled");
};
```

---

# 14. 演算子

初期バージョンでは以下をサポートする。

### 算術

```text
+
-
*
/
%
```

### 比較

```text
==
!=
<
>
<=
>=
```

### 代入

```text
=
```

---

# 15. スコープ

`{}` で囲まれたブロックにはローカルスコープを持たせる。

例：

```text
con form x=10;

con (x==10)<-if{
    con form y=20;
    dispin(y);
};
```

`y` はifブロック内部でのみ有効。

ブロック外から、

```text
dispin(y);
```

とした場合はコンパイルエラー。

一方、外側の変数は内側から参照できる。

```text
con form x=10;

con (x==10)<-if{
    dispin(x);
};
```

---

# 16. コンパイラ構成

コンパイラはC++で実装する。

推奨構成：

```text
HCS Compiler
│
├── Lexer
│
├── Parser
│
├── AST
│
├── Semantic Analyzer
│
├── Intermediate Representation
│
├── Code Generator
│
└── Backend
```

---

# 17. Lexer

`.crs` ファイルを読み込み、トークン列へ変換する。

例：

```text
con form x=10;
```

↓

```text
CON
FORM
IDENTIFIER(x)
ASSIGN
NUMBER(10)
SEMICOLON
```

など。

最低限認識するトークン：

```text
con
form
const
if
ifel
else
for
while
name
range
dispin
blank
true
false

(
)
{
}
;
=
==
!=
<
>
<=
>=
+
-
*
/
%
<-
IDENTIFIER
NUMBER
STRING
```

コメントもLexer段階で処理する。

---

# 18. Parser

Lexerから受け取ったトークンをASTへ変換する。

ASTノードの例：

```text
Program
VariableDeclaration
ConstantDeclaration
Assignment
IfStatement
ElseIfStatement
ElseStatement
ForStatement
WhileStatement
DisplayStatement
BinaryExpression
LiteralExpression
IdentifierExpression
```

`if / elif / else` は同一の条件構造として管理してもよい。

---

# 19. Semantic Analyzer

Parser後に意味解析を行う。

最低限以下を検査する。

### 未定義変数

```text
dispin(x);
```

`x` が存在しない場合：

```text
Error: undefined variable 'x'.
```

### 定数への代入

```text
con const x=10;
x=20;
```

↓

```text
Error: cannot assign to constant 'x'.
```

### 重複定義

同一スコープ内で、

```text
con form x=10;
con form x=20;
```

を禁止する。

---

# 20. コード生成

初期バージョンでは、HCSから直接x86-64機械語を生成する必要はない。

推奨方式：

```text
.crs
 ↓
Lexer
 ↓
Parser
 ↓
AST
 ↓
Semantic Analysis
 ↓
C++ Code Generation
 ↓
C++ Compiler
 ↓
.exe
```

HCSコンパイラ自身がC++コードを生成し、それを外部C++コンパイラでコンパイルする。

---

# 21. C++ Backend

Windows環境では以下のいずれかを使用可能にする。

```text
MSVC
```

または、

```text
Clang
```

初期実装では環境変数PATHからコンパイラを探す方式を推奨。

例えば、

```text
clang++
```

または、

```text
cl.exe
```

を利用する。

---

# 22. HCS Runtime

HCSの動的変数をC++上で扱うため、必要に応じてRuntime Libraryを用意する。

例えば概念的には、

```cpp
class HCSValue
{
    // integer
    // floating point
    // string
    // bool
    // blank
};
```

のような型を用意する。

これにより、

```text
con form x=10;
x="Hello";
x=true;
```

のような動的型変数を実現する。

Runtimeは生成された`.exe`に必要なコードとして組み込むか、静的リンクする。

**最終的なHCSプログラムはRuntime DLLなどに依存せず、単独の`.exe`として動作できる設計を優先する。**

---

# 23. コンパイラCLI

基本：

```text
hcs program.crs
```

出力：

```text
program.exe
```

出力ファイルを指定：

```text
hcs program.crs -o hello.exe
```

ヘルプ：

```text
hcs --help
```

バージョン：

```text
hcs --version
```

---

# 24. エラー処理

エラーは、

```text
filename:line:column
```

を表示する。

例：

```text
main.crs:5:10: Error: cannot assign to constant 'x'.
```

可能ならエラー箇所のソースコードも表示する。

例：

```text
main.crs:5:1: Error: cannot assign to constant 'x'.

x=20;
^
```

---

# 25. Lexer実装状況

## 完了した機能

- [x] キーワード認識（con, form, const, if, ifel, else, for, **while**, dispin, blank, true, false）
- [x] 識別子の認識
- [x] 数値のトークン化（整数、小数、16進数、8進数、指数表記）
- [x] 文字列のトークン化（エスケープシーケンス対応）
- [x] コメント削除（`</ ... />` 形式）
- [x] 演算子の認識（`+`, `-`, `*`, `/`, `%`, `=`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `<-`）
- [x] デリミタの認識（`(`, `)`, `{`, `}`, `;`）
- [x] エラーハンドリング（詳細なエラーメッセージ）
- [x] トークンユーティリティ（型判定、優先度、アソシエーション）

## テスト

包括的なLexerテストスイート（13個のテストケース）が実装済み。

---

# 26. プロジェクト構成

推奨：

```text
HeadCreateScript/
├── src/
│   ├── main.cpp
│   ├── lexer.h              ✓ 完成
│   ├── lexer.cpp            ✓ 完成
│   ├── lexer_error.h        ✓ 完成
│   ├── token.h              ✓ 完成
│   ├── token_utility.h      ✓ 完成
│   ├── parser.h
│   ├── parser.cpp
│   ├── ast.h
│   ├── semantic.h
│   ├── interpreter.h
│   ├── interpreter.cpp
│   ├── hcs_value.h
│   ├── hcs_value.cpp
│   ├── runtime.cpp
│   └── launcher.cpp
├── tests/
│   └── lexer_test.cpp       ✓ 完成
├── CMakeLists.txt
└── README.md
```

---
