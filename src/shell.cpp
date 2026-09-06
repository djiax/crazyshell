#include "shell.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <print>
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

  int in_fd = 0;

  for (size_t i = 0; i < cmds.size(); ++i) {
    std::vector<char *> cmd = cmds[i];
    bool is_last_cmd = (i == cmds.size() - 1);

    int pipe_fds[2];

    if (!is_last_cmd) {
      pipe(pipe_fds);
    }

    pid_t pid = fork();

    if (pid < 0) {
      perror("fork failed");
      return;
    }

    if (pid == 0) {

      // correct
      if (in_fd != 0) {
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
      }

      if (!is_last_cmd) {
        close(pipe_fds[0]);
        dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[1]);
      }

      execvp(cmd[0], cmd.data());
      perror("execvp failed");
      exit(1);
    } else {
      if (in_fd != 0) {
        close(in_fd);
      }
      if (!is_last_cmd) {
        close(pipe_fds[1]);
        in_fd = pipe_fds[0];
      }

      int status;
      waitpid(pid, &status, 0);
    }
  }
}

std::vector<std::vector<char *>>
Shell::tokenize_cmds(const std::vector<std::string> &args) {
  std::vector<std::vector<char *>> cmds;
  std::vector<char *> c_args;

  for (const std::string &arg : args) {
    if (arg == "|") {
      c_args.push_back(nullptr);
      cmds.push_back(c_args);
      c_args.clear();
      continue;
    }
    c_args.push_back(const_cast<char *>(arg.c_str()));
  }
  c_args.push_back(nullptr);
  cmds.push_back(c_args);

  return cmds;
}
