#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <analyzer/analyzer.hpp>
#include <builtin/builtin.hpp>
#include <code/code.hpp>
#include <compiler/compiler.hpp>
#include <compiler/symbol_table.hpp>
#include <eval/environment.hpp>
#include <eval/evaluator.hpp>
#include <fmt/base.h>
#include <fmt/format.h>
#include <gc.hpp>
#include <lexer/lexer.hpp>
#include <object/object.hpp>
#include <parser/parser.hpp>
#include <vm/vm.hpp>

namespace
{
constexpr auto prompt = ">> ";

constexpr auto cappuchin_face = R"r(
             __,__
     .--. .-"     "-. .--.
    / .. \/ .-. .-. \/ .. \
   | |  '| /   Y   \ |'  | |
   | \   \ \ 0 | 0 / /  /  |
    \ '-,\.-"""""""-./,-' /
     ''-' /_  ^ ^  _\ '-''
         | \._   _./ |
         \  \ '~' /  /
          '._'-=-'_.'
            '-----'
)r";

auto cappuchin_business()
{
    std::cerr << cappuchin_face;
    std::cerr << "Woops! We ran into some cappuchin business here!\n  ";
}

auto print_parse_errors(const std::vector<std::string>& errors)
{
    cappuchin_business();
    std::cerr << "  parser errors: \n";
    for (const auto& error : errors) {
        std::cerr << "    " << error << '\n';
    }
}

auto show_error(std::string_view error_kind, std::string_view error_message)
{
    std::cerr << "Whoops! We ran into some " << error_kind << " error: \n  " << error_message << '\n';
}

auto print_compile_error(std::string_view error)
{
    show_error("compiler", error);
}

auto print_eval_error(std::string_view error)
{
    show_error("evaluation", error);
}

enum class engine : std::uint8_t
{
    vm,
    eval,
};

auto operator<<(std::ostream& strm, engine en) -> std::ostream&
{
    switch (en) {
        case engine::vm:
            return strm << "vm";
        case engine::eval:
            return strm << "eval";
    }
    return strm << "unknown";
}

struct command_line_args
{
    bool help {};
    bool debug {};
    engine mode {};
    std::string_view file;
};

auto get_compiler_identifier() -> std::string
{
#if defined(_MSC_VER)
    return "Microsoft Visual Studio Compiler (MSVC) " + std::to_string(_MSC_VER);
#elif defined(__clang__)
    return "Clang " + std::string(__clang_version__);
#elif defined(__GNUC__)
    return "GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "."
        + std::to_string(__GNUC_PATCHLEVEL__);
#else
    return "Unknown Compiler";
#endif
}

auto get_build_type() -> std::string
{
#if defined(NDEBUG)
    return "Release";
#else
    return "Debug";
#endif
}

[[noreturn]] auto show_usage(std::string_view program, std::string_view error_msg = {})
{
    auto exit_code = EXIT_SUCCESS;
    if (!error_msg.empty()) {
        fmt::print("Error: {}\n", error_msg);
        exit_code = EXIT_FAILURE;
    }
    fmt::print("Usage: {} [-d] [-i] [-h] [<file>]\n\n", program);
    // NOLINTBEGIN(concurrency-mt-unsafe)
    exit(exit_code);
    // NOLINTEND(concurrency-mt-unsafe)
}

auto parse_command_line(std::string_view program, int argc, char** argv) -> command_line_args
{
    command_line_args opts {};
    for (std::string_view arg : std::span(argv, static_cast<std::size_t>(argc))) {
        if (arg[0] == '-' && arg.size() == 1) {
            show_usage(program, fmt::format("invalid option {}", arg));
        }
        if (arg[0] == '-' && arg.size() > 1) {
            switch (arg[1]) {
                case 'i':
                    opts.mode = engine::eval;
                    break;
                case 'h':
                    opts.help = true;
                    break;
                case 'd':
                    opts.debug = true;
                    break;
                default: {
                    show_usage(program, fmt::format("invalid option {}", arg));
                }
            }
        } else {
            if (opts.file.empty()) {
                opts.file = arg;
            } else {
                fmt::print("ignoring file argument {}, already have one set.\n", arg);
            }
        }
    }
    return opts;
}

void debug_byte_code(const bytecode& byte_code, const symbol_table* symbols)
{
    std::cout << "Instructions: \n" << to_string(byte_code.instrs);
    std::cout << "Constants:\n";
    for (auto idx = 0; const auto* constant : (*byte_code.consts)) {
        std::cout << idx << ": " << constant->inspect() << '\n';
        idx++;
    }
    std::cout << "Symbols:\n";
    symbols->debug();
}

auto run_file(const command_line_args& opts) -> int
{
    std::ifstream ifs(std::string {opts.file});
    if (!ifs) {
        std::cerr << "ERROR: could not open file: " << opts.file << '\n';
        return 1;
    }
    const std::string contents {(std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())};
    auto lxr = lexer {contents, opts.file};
    auto prsr = parser {lxr};
    auto* prgrm = prsr.parse_program();
    if (!prsr.errors().empty()) {
        print_parse_errors(prsr.errors());
        return 1;
    }
    analyze_program(prgrm, nullptr, nullptr);
    if (opts.mode == engine::vm) {
        auto cmplr = compiler::create();
        cmplr.compile(prgrm);
        if (opts.debug) {
            debug_byte_code(cmplr.byte_code(), cmplr.all_symbols());
        }
        auto machine = vm::create(cmplr.byte_code());
        machine.run();
        const auto* result = machine.last_popped();
        if (!result->is_null()) {
            std::cout << result->inspect() << '\n';
        }
    } else {
        auto* global_env = allocate<environment>();
        for (const auto& builtin : builtin::builtins()) {
            global_env->set(builtin->name, allocate<builtin_object>(builtin));
        }
        evaluator ev {global_env};
        const auto* result = ev.evaluate(prgrm);
        if (!result->is_null()) {
            std::cout << result->inspect() << '\n';
        }
        if (opts.debug) {
            global_env->debug();
        }
    }
    return 0;
}

auto run_repl(const command_line_args& opts) -> int
{
    std::cout << "Cappuchin programming language using engine: " << opts.mode << ".\n";
    std::cout << get_build_type() << " built with " << get_compiler_identifier() << '\n';
    std::cout << "Feel free to type in commands\n";
    auto* global_env = opts.mode == engine::eval ? allocate<environment>() : nullptr;
    auto* symbols = opts.mode == engine::vm ? symbol_table::create() : nullptr;
    constants consts;
    constants globals(globals_size);
    for (auto idx = 0; const auto& builtin : builtin::builtins()) {
        if (global_env != nullptr) {
            global_env->set(builtin->name, allocate<builtin_object>(builtin));
        }
        if (symbols != nullptr) {
            symbols->define_builtin(idx, builtin->name);
        }
        idx++;
    }

    auto show_prompt = []() { std::cout << prompt; };
    auto input = std::string {};
    show_prompt();
    while (getline(std::cin, input)) {
        auto lxr = lexer {input};
        auto prsr = parser {lxr};
        auto* prgrm = prsr.parse_program();
        if (!prsr.errors().empty()) {
            print_parse_errors(prsr.errors());
            show_prompt();
            continue;
        }

        try {
            analyze_program(prgrm, symbols, global_env);
        } catch (const std::exception& e) {
            fmt::println("{}", e.what());
            show_prompt();
            continue;
        }

        if (opts.mode == engine::vm) {
            try {
                auto cmplr = compiler::create_with_state(&consts, symbols);
                cmplr.compile(prgrm);
                if (opts.debug) {
                    debug_byte_code(cmplr.byte_code(), cmplr.all_symbols());
                }
                auto machine = vm::create_with_state(cmplr.byte_code(), &globals);
                machine.run();
                const auto* result = machine.last_popped();
                if (!result->is_null()) {
                    std::cout << result->inspect() << '\n';
                }
            } catch (const std::exception& e) {
                print_compile_error(e.what());
                show_prompt();
                continue;
            }
        } else {
            try {
                evaluator ev {global_env};

                const auto* result = ev.evaluate(prgrm);
                if (!result->is_null()) {
                    std::cout << result->inspect() << '\n';
                }
            } catch (const std::exception& e) {
                print_eval_error(e.what());
                show_prompt();
                continue;
            }
            if (opts.debug) {
                global_env->debug();
            }
        }
        show_prompt();
    }
    return 0;
}
}  // namespace

auto main(int argc, char* argv[]) -> int
{
    auto program = std::string_view(*argv);
    auto opts = parse_command_line(program, argc - 1, ++argv);
    if (opts.help) {
        show_usage(program);
    }
    try {
        if (!opts.file.empty()) {
            return run_file(opts);
        }
        return run_repl(opts);

    } catch (const std::exception& e) {
        cappuchin_business();
        std::cerr << "Caught an exception: " << e.what() << '\n';
        return 1;
    }
}
