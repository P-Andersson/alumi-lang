module;

#include <variant>

export module alccemy.util.variant;

import alccemy.util.unique_type_args;

namespace alccemy {

template <typename... Ts> struct JoinedVariantImpl {
   typedef typename WithUniqueTypeArgs<std::variant, Ts...> type;
};

template <typename... Ts> struct JoinedVariantImpl<std::variant<Ts...>> {
   typedef typename JoinedVariantImpl<Ts...>::type type;
};

template <typename... Ts, typename... Us> struct JoinedVariantImpl<std::variant<Ts...>, Us...> {
   typedef typename JoinedVariantImpl<Us..., Ts...>::type type;
};

export template <typename... VariantTs> using JoinedVariant = JoinedVariantImpl<VariantTs...>::type;
} // namespace alccemy
