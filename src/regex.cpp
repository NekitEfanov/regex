#include "regex.hpp"

#include <memory>
#include <vector>
#include <stdexcept>

using namespace std;


class elementary_pattern {
public:
	enum class pattern_state { stop, accept, try_next, keep };
	virtual pattern_state check(const char& c) = 0;
    virtual void reset() { };
};

class literal : public elementary_pattern {
	char m_lit;
public:
	literal(const char& lit) : m_lit(lit) { }

	virtual pattern_state check(const char& c) {
		return (c == m_lit) ? pattern_state::accept : pattern_state::stop;
	}
};

class dot : public literal {
public:
    dot(const char& lit) : literal(lit) //<not necessary
    { }

    virtual pattern_state check(const char& c) {
        return pattern_state::accept;
    }
};

class star : public elementary_pattern {
	shared_ptr<elementary_pattern> m_lit;
public:
	star(shared_ptr<elementary_pattern> lit) : m_lit(lit) { }

	virtual pattern_state check(const char& c) {
		auto check_result = m_lit->check(c);

		if (check_result == pattern_state::accept)
			return pattern_state::keep;
		else
			return pattern_state::try_next;
	}
};

class rplus : public elementary_pattern {
	int m_matches = 0;
	shared_ptr<elementary_pattern> m_lit;
public:
	rplus(shared_ptr<elementary_pattern>  lit) : m_lit(lit) { }

	virtual pattern_state check(const char& c) {
		auto check_result = m_lit->check(c);

		if (check_result == pattern_state::accept) {
			++m_matches;
			return pattern_state::keep;
		}
		else
			return (m_matches > 0) ? pattern_state::try_next : pattern_state::stop;
	}

    virtual void reset() { m_matches = 0; };
};

class question : public elementary_pattern {
    int m_matches = 0;
    shared_ptr<elementary_pattern> m_lit;
public:
    question(shared_ptr<elementary_pattern> lit) : m_lit(lit) { }

    virtual pattern_state check(const char& c) {
        auto check_result = m_lit->check(c);

        if (check_result == pattern_state::stop)
            return pattern_state::try_next;
        return pattern_state::accept;
    }
};


bool regex_search(const string &str, regex_result &result, const regex &pattern) {
    result.clear();

	using state_t = elementary_pattern::pattern_state;

	auto cpattern = pattern.get_compiled();
    for (auto p = cpattern.begin(); p != cpattern.end(); ++p)
        (*p)->reset();

    size_t n;
    size_t i;
	size_t pattern_pos;
	size_t match_start;
	state_t pattern_state;

    auto split_result = [&str, &match_start, &i, &result]() -> bool {
        result.m_match.append(str.cbegin() + match_start, str.cbegin() + i);
        result.m_prefix.append(str.cbegin(), str.cbegin() + match_start);
        result.m_suffix.append(str.cbegin() + i, str.cend());

        return !result.m_match.empty();
    };

    n           = str.size();
    pattern_pos = 0;
    match_start = 0;

	for (i = 0; i < n;) {
		do
			pattern_state = cpattern[pattern_pos]->check(str[i++]);
		while (pattern_state == state_t::keep && i < n);

		++pattern_pos;
        bool final_ep = (pattern_pos == cpattern.size());

		if (pattern_state == state_t::stop) {
			pattern_pos = 0;
			match_start = i;
		}
        else if (pattern_state == state_t::try_next)
            --i;
        
        if (pattern_state != state_t::stop && final_ep)
            return split_result();

	}

	return false;
}


regex::regex(const string &pattern) : m_pattern(pattern) {
    compile();
}

regex::compiled_pattern regex::get_compiled() const {
    return m_cpattern;
}

void regex::compile() {
    for (auto ep = m_pattern.cbegin(); ep != m_pattern.cend(); ++ep)
        if (*ep == '*')
            make_left_tied_pattern<star>();
        else if (*ep == '+')
            make_left_tied_pattern<rplus>();
        else if (*ep == '?')
            make_left_tied_pattern<question>();
        else if (*ep == '.')
            m_cpattern.push_back(shared_ptr<elementary_pattern>(new dot(*ep)));
        else
            m_cpattern.push_back(shared_ptr<elementary_pattern>(new literal(*ep)));
}

template <typename LTPattern>
void regex::make_left_tied_pattern() {
    if (m_cpattern.size() == 0)
        throw logic_error("left-tied patterns (such as *, +, ?) available only for literals on the left.");

    auto lit = *--m_cpattern.end();

    if (!dynamic_pointer_cast<literal>(lit))
        throw logic_error("left-tied patterns (such as *, +, ?) available only for literals on the left.");

    m_cpattern.erase(--m_cpattern.end());
    m_cpattern.push_back(shared_ptr<elementary_pattern>(new LTPattern(lit)));
}


string regex_result::str() const {
    return m_match;
}

string regex_result::prefix() const {
    return m_prefix;
}

string regex_result::suffix() const {
    return m_suffix;
}

void regex_result::clear() {
    m_match.clear();
    m_suffix.clear();
}