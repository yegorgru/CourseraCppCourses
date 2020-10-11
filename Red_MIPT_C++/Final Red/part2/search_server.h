#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <future>
#include <mutex>
using namespace std;

template <typename T>
class Synchronized {
public:
    explicit Synchronized(T initial = T())
        : value(move(initial))
    {
    }

    struct Access {
        T& ref_to_value;
        lock_guard<mutex> guard;
    };

    Access GetAccess() {
        return { value, lock_guard(m) };
    }

private:
    T value;
    mutex m;
};


class InvertedIndex {
public:
  void Add(const string& document);
  const vector<pair<size_t, size_t>>& Lookup(const string& word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

private:
  map<string, vector<pair<size_t, size_t>>> index;
  vector<string> docs;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void UpdateDocumentBaseSinglehread(istream& document_input);

  void AddQueriesStream(istream& query_input, ostream& search_results_output);
  void AddQueriesStreamSingleThread(istream& query_input, ostream& search_results_output);

private:
    Synchronized<InvertedIndex> index;
    list<future<void>>futures;
};