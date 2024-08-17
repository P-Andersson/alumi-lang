module;

#include <string>
#include <vector>
#include <unordered_set>

#include <utf8cpp/utf8.h>>

export module alccemy.lexer.patterns;

import alccemy.lexer.concepts;
import alccemy.lexer.unicode;

export namespace alccemy
{
   enum class LexerResults
   {
      Continue,
      Failed,
      Completed,
   };

   class LexerResult
   {
   public:
      LexerResult(LexerResults type, size_t backtrack_cols)
      : type(type)
      , backtrack_cols(backtrack_cols)
      {

      }

      LexerResults type;

      // Tells the lexer that this pattern has terminated, but at an ealier point than the last
      // pattern
      size_t backtrack_cols;
   };

   class Text
   {
   public:
      Text(const std::string& str)
      {
         for (auto ite = str.begin(); ite != str.end(); )
         {
            UnicodeCodePoint cp = utf8::next(ite, str.end());
            m_text.push_back(cp);
         }
      }

      LexerResult check(UnicodeCodePoint cp, size_t index)
      {
         if (m_text[index] == cp)
         {
            if (index + 1 == m_text.size())
            {
               return LexerResult(LexerResults::Completed, 0);
            }
            return LexerResult(LexerResults::Continue, 0);
         }
         return LexerResult(LexerResults::Failed, 0);
      }

      LexerResult terminate(size_t index)
      {
         return LexerResult(LexerResults::Failed, 0);
      }
   private:
      std::vector<UnicodeCodePoint> m_text;
   };

   class AnyOf
   {
   public:
      AnyOf(const std::string& str)
      {
         for (auto ite = str.begin(); ite != str.end(); )
         {
            UnicodeCodePoint cp = utf8::next(ite, str.end());
            m_permitted.insert(cp);
         }
      }


      LexerResult check(UnicodeCodePoint cp, size_t index)
      {
         if (m_permitted.contains(cp))
         {
            return LexerResult(LexerResults::Completed, 0);
         }
         return LexerResult(LexerResults::Failed, 0);
      }


      LexerResult terminate(size_t index)
      {
         return LexerResult(LexerResults::Failed, 0);
      }

   private:
      std::unordered_set<UnicodeCodePoint> m_permitted;
   };

   class NotAnyOf
   {
   public:
      NotAnyOf(const std::string& str)
      {
         for (auto ite = str.begin(); ite != str.end(); )
         {
            UnicodeCodePoint cp = utf8::next(ite, str.end());
            m_not_permitted.insert(cp);
         }
      }

      LexerResult check(UnicodeCodePoint cp, size_t index)
      {
         if (!m_not_permitted.contains(cp))
         {
            return LexerResult(LexerResults::Completed, 0);
         }
         return LexerResult(LexerResults::Failed, 0);
      }

      LexerResult terminate(size_t index)
      {
         return LexerResult(LexerResults::Failed, 0);
      }

   private:
      std::unordered_set<UnicodeCodePoint> m_not_permitted;
   };

   template<LexerPattern T, size_t min = 0, size_t max = (size_t)std::numeric_limits<size_t>::max()>
   class Repeats
   {
   public:
      Repeats(const T& pattern)
         : m_pattern(pattern)
         , m_offset(0)
         , m_repeats(0)
      {

      }

      LexerResult check(UnicodeCodePoint cp, size_t index)
      {
         if (index == 0)
         {
            m_offset = 0;
            m_repeats = 0;
         }

         auto res = m_pattern.check(cp, index - m_offset);
         if (res.type == LexerResults::Completed)
         {
            m_repeats += 1;
            m_offset = index;
            m_offset += 1 - res.backtrack_cols;

            return LexerResult(LexerResults::Continue, res.backtrack_cols);
         }
         if (res.type == LexerResults::Failed)
         {
            if (m_offset == index)
            {
               if (m_repeats >= min)
               {
                  return LexerResult(LexerResults::Completed, 1);
               }
               else
               {
                  return LexerResult(LexerResults::Failed, 0);
               }
            }
            return res;
         }

         return res;
      }

      LexerResult terminate(size_t index)
      {
         if (m_offset == index)
         {
            if (m_repeats >= min)
            {
               return LexerResult(LexerResults::Completed, 0);
            }
         }
         return LexerResult(LexerResults::Failed, 0);;
      }

   private:
      T m_pattern;
      std::size_t m_offset;
      std::size_t m_repeats;
   };

   template<LexerPattern... PatternT>
   class Pattern
   {
   public:
      Pattern(PatternT... pattern)
         : m_pattern(pattern...)
         , m_pattern_index(0)
         , m_offset(0)
      {

      }

      LexerResult check(UnicodeCodePoint cp, size_t index)
      {
         if (index == 0)
         {
            m_pattern_index = 0;
            m_offset = 0;
         }
         auto res = process_pattern<0>(cp, index - m_offset);
         if (res.type == LexerResults::Completed)
         {
            m_pattern_index += 1;
            if (m_pattern_index >= sizeof...(PatternT))
            {
               return LexerResult(LexerResults::Completed, res.backtrack_cols);
            }

            m_offset = index;
            m_offset += 1 - res.backtrack_cols;

            return LexerResult(LexerResults::Continue, res.backtrack_cols);
         }
         return res;
      }

      LexerResult terminate(size_t index)
      {
         return terminate_pattern<0>(index - m_offset);
      }

   private:
      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if<I == sizeof...(PatternT), LexerResult>::type
         process_pattern(UnicodeCodePoint cp, size_t index)
      {
         return LexerResult(LexerResults::Failed, 0);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if < I < sizeof...(PatternT), LexerResult>::type
         process_pattern(UnicodeCodePoint cp, size_t index)
      {
         if (m_pattern_index == I)
         {
            return std::get<I>(m_pattern).check(cp, index);
         }
         return process_pattern<I + 1>(cp, index);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if<I == sizeof...(PatternT), LexerResult>::type
         terminate_pattern(size_t index)
      {
         return LexerResult(LexerResults::Failed, 0);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if < I < sizeof...(PatternT), LexerResult>::type
         terminate_pattern(size_t index)
      {
         if (m_pattern_index == I)
         {
            return std::get<I>(m_pattern).terminate(index);
         }
         return terminate_pattern<I + 1>(index);
      }

      std::tuple<PatternT...> m_pattern;
      std::size_t m_pattern_index;
      std::size_t m_offset;
   };

   template<LexerPattern... PatternT>
   class Patterns
   {
   public:
      Patterns(PatternT... pattern)
         : m_pattern(pattern...)
      {
         for (size_t i = 0; i < sizeof...(PatternT); ++i)
         {
            m_failed[i] = false;
         }
      }

      LexerResult check(UnicodeCodePoint cp, size_t index)
      {
         if (index == 0)
         {
            for (size_t i = 0; i < sizeof...(PatternT); ++i)
            {
               m_failed[i] = false;
            }
         }
         auto res = process_pattern<0>(cp, index);
         if (res.type == LexerResults::Continue)
         {
            for (size_t i = 0; i < sizeof...(PatternT); ++i)
            {
               if (m_failed[i] == false)
               {
                  return LexerResult(LexerResults::Continue, 0);
               }
            }
            return LexerResult(LexerResults::Failed, 0);
         }
         else
         {
            return res;
         }
      }

      LexerResult terminate(size_t index)
      {
         return terminate_pattern<0>(index);
      }

   private:
      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if<I == sizeof...(PatternT), LexerResult>::type
         process_pattern(UnicodeCodePoint cp, size_t index)
      {
         return LexerResult(LexerResults::Continue, 0);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if < I < sizeof...(PatternT), LexerResult>::type
         process_pattern(UnicodeCodePoint cp, size_t index)
      {
         if (!m_failed[I])
         {
            auto res = std::get<I>(m_pattern).check(cp, index);
            if (res.type == LexerResults::Completed)
            {
               return res;
            }
            else if (res.type == LexerResults::Failed)
            {
               m_failed[I] = true;
            }
         }
         return process_pattern<I + 1>(cp, index);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if<I == sizeof...(PatternT), LexerResult>::type
         terminate_pattern(size_t index)
      {
         return LexerResult(LexerResults::Failed, 0);
      }

      template<std::size_t I = 0, typename... Tp>
      inline typename std::enable_if < I < sizeof...(PatternT), LexerResult>::type
         terminate_pattern(size_t index)
      {
         if (!m_failed[I])
         {
            auto res = std::get<I>(m_pattern).terminate(index);
            if (res.type == LexerResults::Completed)
            {
               return res;
            }
            else if (res.type == LexerResults::Failed)
            {
               m_failed[I] = true;
            }
         }
         return terminate_pattern<I + 1>(index);
      }

      std::tuple<PatternT...> m_pattern;
      bool m_failed[sizeof...(PatternT)];
   };
}