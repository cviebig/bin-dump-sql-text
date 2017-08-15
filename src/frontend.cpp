#include "dump/console.hpp"
#include "dump/forest.hpp"
#include "dump/latex_support.hpp"
#include "dump/plaintext.hpp"
#include "dump/sql/dumper.hpp"
#include "dump/tikz.hpp"
#include "sql/text/parser.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <stdexcept>

enum class output_generator { console, plaintext, latex };

template <typename ParseResult>
void gen_info(ParseResult s, std::ostream& info_out) {
    info_out << "SQL parsing status: " << std::endl;
    info_out << "Successful: " << (s.success ? "yes" : "no") << std::endl;
    info_out << "Messages: " << std::endl << s.messages;
    info_out << std::endl;
}

template <typename ParseResult>
void gen_console(ParseResult s) {
    ::dump::sql::dumper<::dump::console_dumper>{std::cout}(s.attribute);
    std::cout << std::endl << std::endl;
}

template <typename ParseResult>
void gen_plaintext(ParseResult s) {
    ::dump::sql::dumper<::dump::plaintext_dumper>{std::cout}(s.attribute);
    std::cout << std::endl << std::endl;
}

template <typename PR>
void gen_latex(PR s) {
    for(auto const& node : s.attribute) {
        ::dump::sql::dumper<::dump::forest_dumper>{std::cout}(node);
    }
    for(auto const& node : s.attribute) {
        ::dump::sql::dumper<::dump::tikz_dumper>{std::cout}(node);
    }
}

void parse(std::string i, output_generator gen, std::ostream& info_out) {
    auto const& s = ::sql::text::parse(::sql::text::parser, i);

    switch(gen) {
    case output_generator::console:
        gen_info(s, info_out);
        gen_console(s);
        break;
    case output_generator::plaintext:
        gen_info(s, info_out);
        gen_plaintext(s);
        break;
    case output_generator::latex:
        gen_info(s, info_out);
        gen_latex(s);
        break;
    default:
        std::cout << "Unsupported generator selected.";
    }
}

void interactive(output_generator gen, std::ostream& info_out) {
    info_out << "Give input to parse or type [q or Q] to quit" << std::endl;
    std::string s;
    info_out << std::endl << "> " << std::flush;
    while (getline(std::cin, s)) {
        info_out << std::endl;
        if (s.empty() || s[0] == 'q' || s[0] == 'Q') {
            break;
        }
        parse(s, gen, info_out);
        info_out << std::endl << "> " << std::flush;
    }
}

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;

    try {
        po::options_description desc("SQL parser");
        desc.add_options()
                ("help,h", "produce help message")
                ("generator,g",  po::value<std::string>(),
                 "Output generator (console, plaintext, latex)")
                ("statement,s",  po::value<std::string>(),
                 "SQL statement(s) to parse")
                ("interactive,i",po::bool_switch(),
                 "Interactive mode, prompting for inputs")
                ("stdout,o",     po::bool_switch(),
                 "Force use of stdout for all outputs");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 1;
        }

        output_generator gen = output_generator::console;
        if(vm.count("generator")) {
            if(vm["generator"].as<std::string>() == "console") {
                gen = output_generator::console;
            } else if(vm["generator"].as<std::string>() == "plaintext") {
                gen = output_generator::plaintext;
            } else if(vm["generator"].as<std::string>() == "latex") {
                gen = output_generator::latex;
            }
        }

        if(gen == output_generator::latex) {
            std::cout << ::dump::latex_support{}.get_header();
        }

        /* Print command prompt and debug information on parsed queries to
         * std::cerr if not explicitly told to so.
         */
        std::ostream& info_out =
            vm["stdout"].as<bool>() ? std::cout : std::cerr;

        if (vm.count("statement")) {
            parse(vm["statement"].as<std::string>(), gen, info_out);
        }

        if (vm["interactive"].as<bool>()) {
            interactive(gen, info_out);
        }

        if(gen == output_generator::latex) {
            std::cout << ::dump::latex_support{}.get_footer();
        }

    } catch(const std::runtime_error& e) {
        std::cerr << "A runtime error occurred: " << e.what() << "\n";
    } catch(const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << "\n";
    }
}
