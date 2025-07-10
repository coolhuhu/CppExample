#include "StringUtil.h"

#include <iconv.h>

#include <cassert>
#include <iostream>

namespace cpptools {

bool isGBK(const std::string &str) {
  for (size_t i = 0; i < str.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(str[i]);
    if (c >= 0x80) {  // GBK字符的第一个字节范围是0x80-0xFF
      if (i + 1 >= str.size()) {
        return false;  // 不足两个字节，不是有效的GBK字符
      }
      unsigned char nextC = static_cast<unsigned char>(str[i + 1]);
      if (!((nextC >= 0x40 && nextC <= 0x7E) ||
            (nextC >= 0x80 && nextC <= 0xFE))) {
        return false;  // 第二个字节不在GBK的有效范围内
      }
      ++i;  // 跳过第二个字节
    }
  }
  return true;
}

bool isUTF8(const std::string &str) {
  size_t i = 0;
  while (i < str.size()) {
    unsigned char c = static_cast<unsigned char>(str[i]);
    if (c < 0x80) {
      ++i;                            // ASCII字符
    } else if ((c & 0xE0) == 0xC0) {  // 2字节UTF-8字符
      if (i + 1 >= str.size() ||
          (static_cast<unsigned char>(str[i + 1]) & 0xC0) != 0x80) {
        return false;
      }
      i += 2;
    } else if ((c & 0xF0) == 0xE0) {  // 3字节UTF-8字符
      if (i + 2 >= str.size() ||
          (static_cast<unsigned char>(str[i + 1]) & 0xC0) != 0x80 ||
          (static_cast<unsigned char>(str[i + 2]) & 0xC0) != 0x80) {
        return false;
      }
      i += 3;
    } else if ((c & 0xF8) == 0xF0) {  // 4字节UTF-8字符
      if (i + 3 >= str.size() ||
          (static_cast<unsigned char>(str[i + 1]) & 0xC0) != 0x80 ||
          (static_cast<unsigned char>(str[i + 2]) & 0xC0) != 0x80 ||
          (static_cast<unsigned char>(str[i + 3]) & 0xC0) != 0x80) {
        return false;
      }
      i += 4;
    } else {
      return false;  // 无效的UTF-8字符
    }
  }
  return true;
}

std::string gbkToUtf8(const std::string &gbkStr) {
  std::string utf8Str;
  size_t inBytes = gbkStr.size();
  size_t outBytes = inBytes * 4;  // UTF-8编码的字符最多占用4个字节
  utf8Str.resize(outBytes);

  iconv_t cd = iconv_open("UTF-8", "GBK");
  if (cd == (iconv_t)-1) {
    std::cerr << "iconv_open failed" << std::endl;
    return gbkStr;
  }

  char *inBuf = const_cast<char *>(gbkStr.data());
  char *outBuf = &utf8Str[0];
  size_t inLeft = inBytes;
  size_t outLeft = outBytes;

  if (iconv(cd, &inBuf, &inLeft, &outBuf, &outLeft) == (size_t)-1) {
    std::cerr << "iconv failed" << std::endl;
    iconv_close(cd);
    return gbkStr;
  }

  utf8Str.resize(outBytes - outLeft);
  iconv_close(cd);
  return utf8Str;
}

// kSpaceSymbol in UTF-8 is: ▁
const char kSpaceSymbol[] = "\xe2\x96\x81";

const char WHITESPACE[] = " \n\r\t\f\v";

void SplitString(const std::string &str, std::vector<std::string> *strs) {
  SplitStringToVector(Trim(str), " \t", true, strs);
}

void SplitStringToVector(const std::string &full, const char *delim,
                         bool omit_empty_strings,
                         std::vector<std::string> *out) {
  size_t start = 0, found = 0, end = full.size();
  out->clear();
  while (found != std::string::npos) {
    found = full.find_first_of(delim, start);
    // start != end condition is for when the delimiter is at the end
    if (!omit_empty_strings || (found != start && start != end))
      out->push_back(full.substr(start, found - start));
    start = found + 1;
  }
}

void SplitUTF8StringToChars(const std::string &str,
                            std::vector<std::string> *chars) {
  chars->clear();
  int bytes = 1;
  for (size_t i = 0; i < str.length(); i += bytes) {
    assert((str[i] & 0xF8) <= 0xF0);
    if ((str[i] & 0x80) == 0x00) {
      // The first 128 characters (US-ASCII) in UTF-8 format only need one byte.
      bytes = 1;
    } else if ((str[i] & 0xE0) == 0xC0) {
      // The next 1,920 characters need two bytes to encode,
      // which covers the remainder of almost all Latin-script alphabets.
      bytes = 2;
    } else if ((str[i] & 0xF0) == 0xE0) {
      // Three bytes are needed for characters in the rest of
      // the Basic Multilingual Plane, which contains virtually all characters
      // in common use, including most Chinese, Japanese and Korean characters.
      bytes = 3;
    } else if ((str[i] & 0xF8) == 0xF0) {
      // Four bytes are needed for characters in the other planes of Unicode,
      // which include less common CJK characters, various historic scripts,
      // mathematical symbols, and emoji (pictographic symbols).
      bytes = 4;
    }
    chars->push_back(str.substr(i, bytes));
  }
}

int UTF8StringLength(const std::string &str) {
  int len = 0;
  int bytes = 1;
  for (size_t i = 0; i < str.length(); i += bytes) {
    if ((str[i] & 0x80) == 0x00) {
      bytes = 1;
    } else if ((str[i] & 0xE0) == 0xC0) {
      bytes = 2;
    } else if ((str[i] & 0xF0) == 0xE0) {
      bytes = 3;
    } else if ((str[i] & 0xF8) == 0xF0) {
      bytes = 4;
    }
    ++len;
  }
  return len;
}

bool CheckEnglishChar(const std::string &ch) {
  // all english characters should be encoded in one byte
  if (ch.size() != 1) return false;
  // english words may contain apostrophe, i.e., "He's"
  return isalpha(ch[0]) || ch[0] == '\'';
}

bool CheckEnglishWord(const std::string &word) {
  std::vector<std::string> chars;
  SplitUTF8StringToChars(word, &chars);
  for (size_t k = 0; k < chars.size(); k++) {
    if (!CheckEnglishChar(chars[k])) {
      return false;
    }
  }
  return true;
}

std::string JoinString(const std::string &c,
                       const std::vector<std::string> &strs) {
  std::string result;
  if (strs.size() > 0) {
    for (int i = 0; i < strs.size() - 1; i++) {
      result += (strs[i] + c);
    }
    result += strs.back();
  }
  return result;
}

bool IsAlpha(const std::string &str) {
  for (size_t i = 0; i < str.size(); i++) {
    if (!isalpha(str[i])) {
      return false;
    }
  }
  return true;
}

std::string Ltrim(const std::string &str) {
  size_t start = str.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : str.substr(start);
}

std::string Rtrim(const std::string &str) {
  size_t end = str.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string Trim(const std::string &str) { return Rtrim(Ltrim(str)); }

std::string JoinPath(const std::string &left, const std::string &right) {
  std::string path(left);
  if (path.size() && path.back() != '/') {
    path.push_back('/');
  }
  path.append(right);
  return path;
}

}  // namespace cpptools