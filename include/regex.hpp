#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>


class regex;
class regex_result;
class elementary_pattern;

bool regex_search(const std::string& str, regex_result& result, const regex &pattern);

class regex {
public:
    using compiled_pattern = std::vector<std::shared_ptr<elementary_pattern>>;

    std::string         m_pattern;
    compiled_pattern    m_cpattern;

public:
    regex(const std::string &pattern);
    compiled_pattern get_compiled() const;

private:
    void compile();

    template <typename LTPattern>
    void make_left_tied_pattern();
};

class regex_result {
	std::string         m_match;
    std::string         m_prefix;
	std::string         m_suffix;


public:
	std::string str() const;
	std::string suffix() const;
    std::string prefix() const;

private:
	void clear();

	friend bool regex_search(const std::string& str, regex_result& result, const regex &pattern);
};

