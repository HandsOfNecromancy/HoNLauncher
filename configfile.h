#include <string>
#include <map>

std::string get_config_path();
void save_options(std::string option1, std::string option2);
std::map<std::string, std::string> load_options();
