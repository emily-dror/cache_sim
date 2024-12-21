#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

#include <map>
#include <string>

class argument_parser_c
{
public:
    argument_parser_c(const std::string &prog_name);
    void print_help() const;
    int parse(int argc, char *argv[]);
    bool is_present(const std::string &name) const;
    std::string get(const std::string &name) const;

    void add_argument(const std::string &name, const std::string &description,
                      bool required = false);

private:
    struct arg_info_s
    {
        std::string description;
        std::string value;
        bool required;
    };

    std::string prog_name;
    std::map<std::string, arg_info_s> arguments;
};

#endif  // ARG_PARSER_HPP
