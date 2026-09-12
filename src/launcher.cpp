#include "launcher.h"
#include <iostream>
#include <cstdlib> // std::system
#include <sstream>

Launcher::Launcher(const LauncherConfig& cfg) : cfg_(cfg) {}

std::string Launcher::command() const {
    return cfg_.command;
}

int Launcher::run(const std::string& source) const {
    // シンプルにコマンドと引数をつなげて system() で実行する実装。
    // 将来的にプロセス制御／stdoutキャプチャが必要なら改良してください。
    std::ostringstream cmd;
    cmd << cfg_.command << " " << source;
    std::string s = cmd.str();
    std::cout << "[launcher] running: " << s << std::endl;

    int rc = std::system(s.c_str());
    if (rc == -1) {
        std::cerr << "[launcher] failed to start process\n";
        return 1;
    }

    std::cout << "[launcher] system() returned: " << rc << std::endl;
    return rc;
}

int Launcher::shell() const {
    std::string s = cfg_.command;
    std::cout << "[launcher] starting shell/command: " << s << std::endl;

    int rc = std::system(s.c_str());
    if (rc == -1) {
        std::cerr << "[launcher] failed to start process\n";
        return 1;
    }
    std::cout << "[launcher] system() returned: " << rc << std::endl;
    return rc;
}