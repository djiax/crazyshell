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

  // organize
  std::vector<std::vector<char *>> cmds = tokenize_cmds(args);

  for (size_t i = 0; i < cmds.size(); ++i) {
    std::vector<char *> cmd = cmds[i];

    bool is_last_cmd = i - 1 == cmds.size();

    pid_t pid = fork();
    if (pid < 0) {
      perror("fork failed");
      return;
    }

    if (pid == 0) {

      if (!is_last_cmd) {
        // figure out how to prompt ai to figure out smth like dup2 instead of
        // just looking tho maybe straight up copying at this level is fine
      }

      execvp(cmd[0], cmd.data());
      perror("execvp failed");
      exit(1);
    } else {
      int status;
      waitpid(pid, &status, 0);
    }
  }
}

std::vector<std::vector<char *>>
Shell::tokenize_cmds(const std::vector<std::string> &args) {
  std::vector<std::vector<char *>> cmds;
  std::vector<char *> c_args;

  for (std::string arg : args) {
    if (arg == "|") {
      c_args.push_back(nullptr);
      cmds.push_back(c_args);
      c_args.clear();
      continue;
    }
    const auto &a = arg;
    c_args.push_back(const_cast<char *>(a.c_str()));
  }

  return cmds;
}
