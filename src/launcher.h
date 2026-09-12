#pragma once

#include <string>
#include "launcher_config.h"

class Launcher {
public:
    explicit Launcher(const LauncherConfig& cfg = LauncherConfig());
    // 指定したソースファイルをコマンドに渡して実行する（例: ./hcs source.crs）
    // 戻り値は実行プロセスの終了コード（system() の戻り値そのまま）
    int run(const std::string& source) const;

    // コマンドを引数なしで起動（REPL など）
    int shell() const;

    // コマンド文字列を取得
    std::string command() const;

private:
    LauncherConfig cfg_;
};