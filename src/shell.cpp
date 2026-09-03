#include "shell.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

void Shell::run() {
  std::string line;
  while (true) {
    std::string path = std::filesystem::current_path().filename().string();
    std::cout << "mini-shell " << path << " >";

    // input
    if (!std::getline(std::cin, line))
      break;

    std::vector<std::string> args = tokenize(line);

    if (args.empty())
      continue;

    if (!handle_builtin(args)) {
      execute(args);
    }
  }
}

std::vector<std::string> Shell::tokenize(const std::string &input) {
  std::vector<std::string> tokens;

  // wrap raw input inside stringstream object
  std::stringstream ss(input);

  std::string token;

  while (ss >> token) {
    tokens.push_back(token);
  }

  return tokens;
}

bool Shell::handle_builtin(const std::vector<std::string> &args) {
  if (args[0] == "exit") {
    std::exit(0);
  }

  if (args[0] == "yo") {
    std::cout << "i love abgs" << std::endl;
    return true;
  }

  if (args[0] == "cd") {
    if (args.size() > 1) {
      if (chdir(args[1].c_str()) != 0) {
        perror("cd failed");
      }
    }

    return true;
  }

  return false;
}

void Shell::execute(const std::vector<std::string> &args) {
  std::vector<char *> c_args;

  for (size_t i = 0; i < args.size(); ++i) {
    const auto &arg = args[i];
    c_args.push_back(const_cast<char *>(arg.c_str()));
  }
  if (!c_args.empty()) {
    c_args.push_back(nullptr);
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed");
    return;
  }

  if (pid == 0) {

    execvp(c_args[0], c_args.data());
    perror("execvp failed");
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, 0);
  }
}
