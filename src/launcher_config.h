#pragma once

#include <string>

struct LauncherConfig {
    // コマンドは相対/絶対パスで指定。デフォルトは同じディレクトリの hcs 実行ファイル
    std::string command = "./hcs";

    // 追加設定（将来）:
    // - working_dir
    // - environment
    // - timeout
};