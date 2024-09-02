module;

#include <type_traits>

export module alccemy.util.unique_type_args;

namespace alccemy {

// Based on:
// https://stackoverflow.com/a/57528226/1779056
// and with this generalization:
// https://stackoverflow.com/a/62089731/1779056

template <typename T, typename... Ts> struct unique_impl {
   using type = T;
};

template <template <typename...> typename TemplateT, typename... Ts, typename U, typename... Us>
struct unique_impl<TemplateT<Ts...>, U, Us...>
    : std::conditional_t<((std::is_same_v<U, Ts> || ...) || std::is_same_v<U, void>),
                         unique_impl<TemplateT<Ts...>, Us...>, unique_impl<TemplateT<Ts..., U>, Us...>> {};

export template <template <typename...> typename TemplateT, typename... ArgTs>
using WithUniqueTypeArgs = typename unique_impl<TemplateT<>, ArgTs...>::type;
} // namespace alccemy
