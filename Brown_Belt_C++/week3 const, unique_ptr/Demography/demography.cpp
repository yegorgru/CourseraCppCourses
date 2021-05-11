#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <map>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
        : first(begin)
        , last(end)
    {
    }

    Iterator begin() const {
        return first;
    }

    Iterator end() const {
        return last;
    }

private:
    Iterator first, last;
};

struct Person {
    string name;
    int age, income;
    bool is_male;
};

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;

    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = gender == 'M';
    }

    return result;
}

int main() {
    vector<Person> people = ReadPeople(cin);

    vector<Person> incomePeople = people;

    sort(begin(people), end(people), [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
        });

    map<string, int>males;
    map<string, int>females;

    for (auto& p : people) {
        if (p.is_male) {
            ++males[p.name];
        }
        else {
            ++females[p.name];
        }
    }

    string males_answer = "";
    if (males.size() == 0) {
        males_answer = "No people of gender M";
    }
    else {
        string max_name = males.begin()->first;
        int max = males.begin()->second;
        for (auto it = next(males.begin()); it != males.end(); ++it) {
            if (it->second > max) {
                max_name = it->first;
                max = it->second;
            }
        }
        males_answer = "Most popular name among people of gender M is " + max_name;
    }

    string females_answer = "";
    if (females.size() == 0) {
        females_answer = "No people of gender W";
    }
    else {
        string max_name = females.begin()->first;
        int max = females.begin()->second;
        for (auto it = next(females.begin()); it != females.end(); ++it) {
            if (it->second > max) {
                max_name = it->first;
                max = it->second;
            }
        }
        females_answer = "Most popular name among people of gender W is " + max_name;
    }

    sort(begin(incomePeople), end(incomePeople), [](const Person& lhs, const Person& rhs) {
        return lhs.income < rhs.income;
        });

    for (string command; cin >> command; ) {
        if (command == "AGE") {
            int adult_age;
            cin >> adult_age;

            auto adult_begin = lower_bound(
                begin(people), end(people), adult_age, [](const Person& lhs, int age) {
                    return lhs.age < age;
                }
            );

            cout << "There are " << std::distance(adult_begin, end(people))
                << " adult people for maturity age " << adult_age << '\n';
        }
        else if (command == "WEALTHY") {
            int count;
            cin >> count;

            int total_income = 0;
            for (auto it = incomePeople.end() - count; it != incomePeople.end(); ++it) {
                total_income += it->income;
            }
            cout << "Top-" << count << " people have total income " << total_income << '\n';
        }
        else if (command == "POPULAR_NAME") {
            char gender;
            cin >> gender;

            if (gender == 'M') {
                cout << males_answer << '\n';
            }
            else {
                cout << females_answer << '\n';
            }
        }
    }
}