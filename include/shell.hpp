#ifndef SHELL_HPP
#define SHELL_HPP

#include <string>
#include <vector>

class Shell {
public:
  void run();

private:
  std::vector<std::string> tokenize(const std::string &input);
  bool handle_builtin(const std::vector<std::string> &args);
  void execute(const std::vector<std::string> &args);
};

#endif // !SHELL_HPP
