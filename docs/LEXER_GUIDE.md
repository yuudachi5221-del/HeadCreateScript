# HCS Lexer 使用ガイド

## 概要

HCS（Head Create Script）のLexerは、`.crs` ファイルを読み込み、トークン列に変換するモジュールです。

このドキュメントでは、Lexerの使用方法、機能、および拡張方法について説明します。

---

## クイックスタート

### 基本的な使用方法

```cpp
#include "lexer.h"

int main() {
    // ファイルから読み込み
    Lexer lexer("program.crs");
    auto tokens = lexer.tokenize();
    
    // トークンを処理
    for (const auto& token : tokens) {
        std::cout << TokenUtility::formatTokenWithLocation(token) << std::endl;
    }
    
    // エラーチェック
    if (lexer.hasError()) {
        std::cerr << "Lexer error: " << lexer.getErrorMessage() << std::endl;
    }
    
    return 0;
}
```

### 文字列からの直接トークン化

```cpp
Lexer lexer("con form x=10;", true);  // true: 文字列入力
auto tokens = lexer.tokenize();
```

---

## トークンタイプ

HCS Lexerが認識するトークンタイプ一覧：

### キーワード

| キーワード | 説明 |
|-----------|------|
| `con` | 構造生成の開始 |
| `form` | 変数の生成 |
| `const` | 定数の生成 |
| `if` | 条件分岐 |
| `ifel` | else if相当 |
| `else` | 条件なしの分岐 |
| `for` | for ループ |
| `while` | while ループ |
| `name` | for/while内の変数名指定 |
| `range` | for内のステップ指定 |
| `dispin` | 標準出力表示 |
| `blank` | 未設定値 |
| `true` | 真偽値（true） |
| `false` | 真偽値（false） |

### 演算子と記号

| 演算子 | トークンタイプ | 説明 |
|--------|---------------|------|
| `+` | Plus | 加算 |
| `-` | Minus | 減算 |
| `*` | Star | 乗算 |
| `/` | Slash | 除算 |
| `%` | Percent | 剰余 |
| `=` | Assign | 代入 |
| `==` | Equal | 等価比較 |
| `!=` | NotEqual | 不等価比較 |
| `<` | Less | 未満 |
| `>` | Greater | より大きい |
| `<=` | LessEq | 以下 |
| `>=` | GreaterEq | 以上 |
| `<-` | Arrow | 条件をブロックへ渡す |
| `(` | LParen | 左括弧 |
| `)` | RParen | 右括弧 |
| `{` | LBrace | 左波括弧 |
| `}` | RBrace | 右波括弧 |
| `;` | Semicolon | ステートメント終了 |

### リテラル

| タイプ | 説明 |
|--------|------|
| `Number` | 整数、小数、16進数、8進数、指数表記 |
| `String` | ダブルクォートで囲まれた文字列 |
| `Bool` | `true` または `false` |
| `Identifier` | 変数名や関数名 |

---

## 高度な使用方法

### エラーハンドリング

Lexerは詳細なエラー情報を提供します：

```cpp
Lexer lexer("invalid code", true);
auto tokens = lexer.tokenize();

if (lexer.hasError()) {
    // エラーメッセージ取得
    std::string error = lexer.getErrorMessage();
    std::cerr << error << std::endl;
    // 出力例: "Unknown character '#' at 1:14"
}
```

### トークン属性へのアクセス

```cpp
for (const auto& token : tokens) {
    // トークンタイプ
    TokenType type = token.type;
    
    // トークンテキスト（原文のまま）
    std::string text = token.text;
    
    // トークン値（文字列リテラルの場合は内容、数値の場合は数値表現）
    std::string value = token.value;
    
    // 位置情報
    int line = token.line;
    int col = token.col;
    
    // エラー情報
    if (token.has_error) {
        std::cout << "Error: " << token.error_message << std::endl;
    }
}
```

### トークンユーティリティの活用

```cpp
#include "token_utility.h"

for (const auto& token : tokens) {
    // トークンタイプ名を取得
    std::string typeName = TokenUtility::getTokenTypeName(token.type);
    
    // 判定関数
    if (TokenUtility::isKeyword(token)) {
        // キーワード処理
    }
    
    if (TokenUtility::isOperator(token)) {
        // 演算子処理
        int precedence = TokenUtility::getOperatorPrecedence(token);
        bool isRightAssoc = TokenUtility::isRightAssociative(token);
    }
    
    if (TokenUtility::isLiteral(token)) {
        // リテラル処理
    }
    
    // フォーマット出力
    std::cout << TokenUtility::formatToken(token) << std::endl;
    // 出力例: "Keyword(con)" 、"Number(42) [42]"
}
```

---

## 数値の処理

Lexerは複数の数値形式をサポート：

### 10進数

```text
10
42
3.14
0.5
```

### 16進数

```text
0xFF       // 255
0x1A2B     // 6699
0xDEADBEEF
```

### 8進数

```text
010        // 8
077        // 63
0755       // 493
```

### 指数表記

```text
1e3        // 1000
1.5e-3     // 0.0015
2E+10      // 20000000000
```

---

## 文字列の処理

### エスケープシーケンス

Lexerが認識するエスケープシーケンス：

| シーケンス | 意味 |
|-----------|------|
| `\n` | 改行 |
| `\t` | タブ |
| `\r` | キャリッジリターン |
| `\\` | バックスラッシュ |
| `\"` | ダブルクォート |

```cpp
// 入力例
Lexer lexer(R"(dispin("Hello\nWorld\t!");)", true);
auto tokens = lexer.tokenize();

// tokens[2] は String トークン
// token.text  = "\"Hello\nWorld\t!\""
// token.value = "Hello\nWorld\t!" (実際のエスケープシーケンス)
```

---

## コメント処理

HCSのコメント形式：

```text
</
複数行コメント
</と/>に囲まれた内容は無視される
/>
```

```cpp
Lexer lexer(R"(</ This is a comment /> con form x=10;)", true);
auto tokens = lexer.tokenize();

// コメントは削除されているため、
// tokens には: con, form, x, =, 10, ;, EOF のみ
```

---

## パフォーマンス最適化

### バッチ処理

大きなファイルを処理する場合：

```cpp
Lexer lexer("large_file.crs");
auto tokens = lexer.tokenize();

// トークン数のチェック
std::cout << "Total tokens: " << tokens.size() << std::endl;

// バッチ処理
const int BATCH_SIZE = 1000;
for (size_t i = 0; i < tokens.size(); i += BATCH_SIZE) {
    size_t end = std::min(i + BATCH_SIZE, tokens.size());
    process_tokens(tokens.begin() + i, tokens.begin() + end);
}
```

---

## トラブルシューティング

### 未定義の文字が見つかった場合

```
Error: Unknown character '@' at 5:10
```

**原因**: サポートされていない文字が使用されている

**解決**: HCS仕様でサポートされている文字のみ使用してください

### 文字列が閉じられていない

```
Error: Unterminated string at 3:15
```

**原因**: 開かれたダブルクォートが閉じられていない

**解決**: すべての文字列を `"` で閉じてください

### 16進数リテラルが無効

```
Error: Invalid hexadecimal literal at 7:5
```

**原因**: `0x` の後に16進数字がない

**解決**: `0x` の後に少なくとも1つの16進数字を付けてください

---

## Lexer内部構造

### 処理フロー

```
入力 (.crs ファイルまたは文字列)
  ↓
コメント削除（</ ... /> を削除）
  ↓
トークン化ループ
  ├─ 空白をスキップ
  ├─ 識別子/キーワードを認識
  ├─ 数値をトークン化
  ├─ 文字列をトークン化
  ├─ 演算子/記号を認識
  └─ エラーハンドリング
  ↓
トークンベクター
```

### 主要メソッド

| メソッド | 説明 |
|---------|------|
| `tokenize()` | メイン処理。トークンベクターを返す |
| `peek(offset)` | 現在位置から`offset`文字先を参照（消費しない） |
| `get()` | 現在位置の文字を取得して進める |
| `skipWhitespace()` | 空白をスキップ |
| `removeComments()` | コメントを削除 |
| `recognizeKeywordOrIdentifier()` | キーワード/識別子を認識 |
| `recognizeNumber()` | 数値をトークン化 |
| `recognizeString()` | 文字列をトークン化 |
| `recognizeOperatorOrSymbol()` | 演算子/記号を認識 |

---

## 拡張方法

### 新しいキーワードの追加

`src/lexer.cpp` の `kKeywords` に追加：

```cpp
static const std::unordered_set<std::string> kKeywords = {
    "con", "form", "const", "if", "ifel", "else", "for", "while",
    "name", "range", "dispin", "blank", "true", "false",
    "break",  // 新しいキーワード
    "continue" // 新しいキーワード
};
```

### 新しいトークンタイプの追加

`src/token.h` の `TokenType` enum に追加：

```cpp
enum class TokenType {
    // ... 既存のタイプ ...
    Break,      // 新しいトークン
    Continue,   // 新しいトークン
    Unknown
};
```

`src/token_utility.h` の `getTokenTypeName()` に対応を追加：

```cpp
{static_cast<int>(TokenType::Break), "Break"},
{static_cast<int>(TokenType::Continue), "Continue"},
```

---

## ベストプラクティス

1. **エラーチェック**: 常に `hasError()` でエラーをチェック
2. **メモリ効率**: トークンベクターが大きい場合はイテレータを使用
3. **デバッグ出力**: `TokenUtility::formatTokenWithLocation()` で位置情報付き出力
4. **バージョン互換性**: キーワード追加時は仕様書も更新

---

## 参考資料

- [HCS 要件定義書 (README.md)](../README.md)
- [Token クラス定義 (src/token.h)](../src/token.h)
- [Lexer クラス定義 (src/lexer.h)](../src/lexer.h)
- [Lexer テストスイート (tests/lexer_test.cpp)](../tests/lexer_test.cpp)

---
