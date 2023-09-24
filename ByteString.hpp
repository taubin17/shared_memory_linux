#pragma once

#include <unordered_map>
#include <math.h>
#include <string>

template <typename T>
class ByteString
{
  public:
    ByteString(const T bytes)
    {
      double copy = bytes;
      int64_t div_1024_count = 0;
      while (copy > 1024)
      {
        div_1024_count += 1;
        copy /= 1024;
      }

      m_byte_string = std::to_string(static_cast<int64_t> (std::round(copy))) + " " + exponent_map.at(div_1024_count) + "iB";
    }

    inline std::string str() const { return m_byte_string; };

  private:
    std::string m_byte_string;

    const std::unordered_map<int64_t, std::string> exponent_map
    {
      {0, ""},
      {1, "k"},
      {2, "M"},
      {3, "G"},
      {4, "T"}
    };
};