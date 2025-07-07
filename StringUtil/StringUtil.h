#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <vector>
#include <memory>


namespace cpptools {


bool isGBK(const std::string& str);


bool isUTF8(const std::string& str);


std::string gbkToUtf8(const std::string& gbkStr);


// Split the string with space or tab.
void SplitString(const std::string& str, std::vector<std::string>* strs);

void SplitStringToVector(const std::string& full, const char* delim,
                         bool omit_empty_strings,
                         std::vector<std::string>* out);

// NOTE(Xingchen Song): we add this function to make it possible to
// support multilingual recipe in the future, in which characters of
// different languages are all encoded in UTF-8 format.
// UTF-8 REF: https://en.wikipedia.org/wiki/UTF-8#Encoding
// Split the UTF-8 string into chars.
void SplitUTF8StringToChars(const std::string& str,
                            std::vector<std::string>* chars);

int UTF8StringLength(const std::string& str);

// Check whether the UTF-8 char is alphabet or '.
bool CheckEnglishChar(const std::string& ch);

// Check whether the UTF-8 word is only contains alphabet or '.
bool CheckEnglishWord(const std::string& word);

std::string JoinString(const std::string& c,
                       const std::vector<std::string>& strs);

bool IsAlpha(const std::string& str);

std::string Ltrim(const std::string& str);

std::string Rtrim(const std::string& str);

std::string Trim(const std::string& str);

std::string JoinPath(const std::string& left, const std::string& right);



} // namespace cpptools


#endif // STRINGUTIL_H