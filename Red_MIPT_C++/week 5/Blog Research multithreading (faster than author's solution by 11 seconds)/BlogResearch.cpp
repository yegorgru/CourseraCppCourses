#include "test_runner.h"
#include "profile.h"

#include <functional>
#include <map>
#include <string>

#include <future>
#include <utility>
using namespace std;

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
        : first(begin)
        , last(end)
        , size_(distance(first, last))
    {
    }

    Iterator begin() const {
        return first;
    }

    Iterator end() const {
        return last;
    }

    size_t size() const {
        return size_;
    }

private:
    Iterator first, last;
    size_t size_;
};

template <typename Iterator>
class Paginator {
private:
    vector<IteratorRange<Iterator>> pages;

public:
    Paginator(Iterator begin, Iterator end, size_t page_size) {
        for (size_t left = distance(begin, end); left > 0; ) {
            size_t current_page_size = min(page_size, left);
            Iterator current_page_end = next(begin, current_page_size);
            pages.push_back({ begin, current_page_end });

            left -= current_page_size;
            begin = current_page_end;
        }
    }

    auto begin() const {
        return pages.begin();
    }

    auto end() const {
        return pages.end();
    }

    size_t size() const {
        return pages.size();
    }
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

struct Stats {
    map<string, int> word_frequences;



    void operator += (const Stats& other) {
        for (const auto& i : other.word_frequences) {
            this->word_frequences[i.first] += i.second;
        }
    }
};

Stats ExploreLine(const set<string>& key_words, const string& line) {
    Stats answer;
    auto begin_word = line.begin();
    auto end_word = line.begin();
    while (end_word != line.end()) {
        while (*begin_word == ' ' && begin_word != line.end()) {
            begin_word++;
        }
        if (end_word != begin_word) {
            end_word = begin_word;
        }
        while (end_word != line.end() && *end_word != ' ') {
            end_word++;
        }
        string tmp(begin_word, end_word);
        if (key_words.find(tmp) != key_words.end()) {
            answer.word_frequences[tmp]++;
        }
        begin_word = end_word;
    }
    return answer;
}

template <typename T>
Stats ExploreKeyWordsSingleThread(const set<string>& key_words,T page) {
    Stats answer;
    for (auto i : page) {
        answer += ExploreLine(key_words, i);
    }
    return answer;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
    vector<string>all_strings;
    all_strings.reserve(50000);
    for (string line; getline(input, line); ) {
        all_strings.push_back(move(line));
    }
    Stats result;
    vector<future<Stats>>futures;
    for (auto page:Paginate(all_strings,all_strings.size()/4+1)) {
        futures.push_back(async(ExploreKeyWordsSingleThread<IteratorRange<vector<string>::iterator>>,key_words,page));
    }
    for (auto& f : futures) {
        result += f.get();
    }
    return result;
}

void TestBasic() {
    const set<string> key_words = { "yangle", "rocks", "sucks", "all" };

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    const auto stats = ExploreKeyWords(key_words, ss);
    const map<string, int> expected = {
      {"yangle", 6},
      {"rocks", 2},
      {"sucks", 1}
    };
    ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestBasic);
}