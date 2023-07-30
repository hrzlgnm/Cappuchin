#include <complex>

#include "call_expression.hpp"

#include "environment.hpp"
#include "object.hpp"
#include "util.hpp"
#include "value_type.hpp"

auto call_expression::string() const -> std::string
{
    return fmt::format("{}({})", function->string(), join(arguments, ", "));
}
/*
auto evaluate_expressions(const std::vector<expression_ptr>& expressions, const environment_ptr& env)
    -> std::vector<object>
{
    std::vector<object> result;
    for (const auto& expr : expressions) {
        const auto evaluated = expr->eval(env);
        if (evaluated.is<error>()) {
            return {evaluated};
        }
        result.emplace_back(evaluated);
    }
    return result;
}

auto extended_function_environment(const object& funci, const std::vector<object>& args) -> environment_ptr
{
    const auto& as_func = funci.as<func>();
    auto env = std::make_shared<environment>(as_func->env);
    size_t idx = 0;
    for (const auto& parameter : as_func->parameters) {
        env->set(parameter->value, args[idx]);
        idx++;
    }
    return env;
}

auto apply_function(const object& funct, const std::vector<object>& args) -> object
{
    if (!funct.is<func>()) {
        return make_error("not a function: {}", funct.type_name());
    }
    auto extended_env = extended_function_environment(funct, args);
    auto evaluated = funct.as<func>()->body->eval(extended_env);
    return unwrap_return_value(evaluated);
}
*/

auto call_expression::eval(environment_ptr env) const -> object
{
    auto evaluated = function->eval(env);
    if (evaluated.is<error>()) {
        return evaluated;
    }
    auto [fn, closure_env] = evaluated.as<bound_function>();
    return fn->call(closure_env, env, arguments);
}
