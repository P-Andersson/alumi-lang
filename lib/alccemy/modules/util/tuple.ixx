module;

#include <tuple>
#include <utility>

export module alccemy.util.tuple;

namespace alccemy {

template <typename> struct is_tuple : std::false_type {};

template <typename... T> struct is_tuple<std::tuple<T...>> : std::true_type {};

export template <typename T>
concept TupleType = is_tuple<std::decay_t<T>>::value;

template <typename FuncT, typename ArgT>
concept CallableWithIndex = requires(FuncT&& f, ArgT arg, size_t index) { f(arg, index); };

template <typename FuncT, typename ArgT> void maybe_call_with_index(FuncT&& function, ArgT&& arg, size_t index) {
   if constexpr (CallableWithIndex<FuncT, ArgT>) {
      function(std::forward<ArgT>(arg), index);
   } else if constexpr (!CallableWithIndex<FuncT, ArgT>) {
      function(std::forward<ArgT>(arg));
   }
}

template <TupleType TupleT, typename FuncT, size_t... indices>
void fold_tuple_for_each(TupleT&& tuple, FuncT&& function, std::index_sequence<indices...>) {
   (maybe_call_with_index(std::forward<FuncT>(function), (std::get<indices>(tuple)), indices), ...);
}

/**
 * Executes function for each element in the passed tuple in order
 * Must be able to take EACH elememt of the tuple as an argument
 *
 * The function may have the following formats:
 *   void f(ArgT/const ArgT/ArgT&/ArgT&& arg, (optional)size_t index);
 **/
export template <TupleType TupleT, typename FuncT> void tuple_for_each(TupleT&& tuple, FuncT&& function) {
   fold_tuple_for_each(std::forward<TupleT>(tuple), std::forward<FuncT>(function),
                       std::make_index_sequence<std::tuple_size_v<std::decay_t<TupleT>>>());
}

/**
 * Executes the function with an indice sequence of the tuple indicies
 * as the argument
 *
 * The function should have the following format:
 *  typename <size_t... indicies>
 *  ReturnT f(std::index_sequence<indicies>);
 **/
export template <TupleType TupleT, typename FuncT> auto tuple_for(TupleT&& tuple, FuncT&& function) {
   using ReturnT = decltype(function(std::make_index_sequence<std::tuple_size_v<std::decay_t<TupleT>>>{}));
   if constexpr (std::same_as<void, ReturnT>) {
      function(std::make_index_sequence<std::tuple_size_v<std::decay_t<TupleT>>>{});
   } else if constexpr (!std::same_as<void, ReturnT>) {
      return function(std::make_index_sequence<std::tuple_size_v<std::decay_t<TupleT>>>{});
   }
}

} // namespace alccemy
