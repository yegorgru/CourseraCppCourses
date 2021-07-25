#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <optional>
#include <iostream>

using namespace std;

struct Email {
    string from;
    string to;
    string body;
};

class Worker {
public:
    virtual ~Worker() = default;
    virtual void Process(unique_ptr<Email> email) = 0;
    virtual void Run() {
        throw logic_error("Unimplemented");
    }

protected:
    void PassOn(unique_ptr<Email> email) const {
        if (m_Next != std::nullopt) {
            (*m_Next)->Process(std::move(email));
        }
    }

    std::optional<std::unique_ptr<Worker>> m_Next = std::nullopt;

public:
    void SetNext(unique_ptr<Worker> next) {
        m_Next = std::move(next);
    }
};

class Reader : public Worker {
public:
    Reader(istream& is) : m_Is(is) {}

    void Run() {
        string from, to, body;
        while (getline(m_Is, from) && getline(m_Is, to) && getline(m_Is, body)) {
            PassOn(make_unique<Email>(Email{ from, to, body }));
        }
    }

    void Process(unique_ptr<Email> email) override {
        
    }

private:
    istream& m_Is;
};


class Filter : public Worker {
public:
    using Function = function<bool(const Email&)>;

private:
    Function m_Foo = nullptr;
public:
    Filter(Function foo) {
        m_Foo = foo;
    }

    void Process(unique_ptr<Email> email) override {
        if (m_Foo(*email)) {
            PassOn(move(email));
        }
    }
};

class Copier : public Worker {
public:
    Copier(const std::string& recipier) : m_Recipier(recipier) {}

    void Process(unique_ptr<Email> email) override {
        string from = email->from;
        string to = email->to;
        string body = email->body;
        PassOn(move(email));
        if (to != m_Recipier) {
            PassOn(make_unique<Email>(Email{ from, m_Recipier, body }));
        }
    }
private:
    string m_Recipier;
};


class Sender : public Worker {
public:
    Sender(ostream& os) : m_Os(os) {}

    void Process(unique_ptr<Email> email) override {
        m_Os << email->from << "\n" << email->to << "\n" << email->body << "\n";
        PassOn(move(email));
    }
private:
    ostream& m_Os;
};


class PipelineBuilder {
public:
    explicit PipelineBuilder(istream& in) {
        m_Reader = make_unique<Reader>(in);
        m_LastWorker = m_Reader.get();
    }

    PipelineBuilder& FilterBy(Filter::Function filter) {
        unique_ptr<Worker> newWorker = make_unique<Filter>(filter);
        auto nextLast = newWorker.get();
        m_LastWorker->SetNext(move(newWorker));
        m_LastWorker = nextLast;
        return *this;
    }

    PipelineBuilder& CopyTo(string recipient) {
        unique_ptr<Worker> newWorker = make_unique<Copier>(recipient);
        auto nextLast = newWorker.get();
        m_LastWorker->SetNext(move(newWorker));
        m_LastWorker = nextLast;
        return *this;
    }

    PipelineBuilder& Send(ostream& out) {
        unique_ptr<Worker> newWorker = make_unique<Sender>(out);
        auto nextLast = newWorker.get();
        m_LastWorker->SetNext(move(newWorker));
        m_LastWorker = nextLast;
        return *this;
    }

    unique_ptr<Worker> Build() {
        unique_ptr<Reader> r;
        r.swap(m_Reader);
        return r;
    }

private:
    Worker* m_LastWorker = nullptr;
    unique_ptr<Reader> m_Reader;
};


void TestSanity() {
    string input = (
        "erich@example.com\n"
        "richard@example.com\n"
        "Hello there\n"

        "erich@example.com\n"
        "ralph@example.com\n"
        "Are you sure you pressed the right button?\n"

        "ralph@example.com\n"
        "erich@example.com\n"
        "I do not make mistakes of that kind\n"
        );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email& email) {
        return email.from == "erich@example.com";
        });
    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
        "erich@example.com\n"
        "richard@example.com\n"
        "Hello there\n"

        "erich@example.com\n"
        "ralph@example.com\n"
        "Are you sure you pressed the right button?\n"

        "erich@example.com\n"
        "richard@example.com\n"
        "Are you sure you pressed the right button?\n"
        );

    ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    return 0;
}