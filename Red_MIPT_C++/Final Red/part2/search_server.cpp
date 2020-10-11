#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>

vector<string> SplitIntoWords(const string& line) {
  istringstream words_input(line);
  return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    futures.push_back(async(launch::async, &SearchServer::UpdateDocumentBaseSinglehread, this, ref(document_input)));
}

void SearchServer::UpdateDocumentBaseSinglehread(istream& document_input)
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));
    InvertedIndex new_index;

    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    swap(index.GetAccess().ref_to_value, new_index);
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
     futures.push_back(async(launch::async, &SearchServer::AddQueriesStreamSingleThread,this,
        ref(query_input), ref(search_results_output)));
}

void SearchServer::AddQueriesStreamSingleThread(istream& query_input, ostream& search_results_output)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    vector<size_t> docid_count(50000, 0);
    for (string current_query; getline(query_input, current_query); ) {
        const auto words = SplitIntoWords(current_query);
        {
            auto access = index.GetAccess();
            for (const auto& word : words) {
                for (const auto& docids_number : access.ref_to_value.Lookup(word)) {
                    docid_count[docids_number.first] += docids_number.second;
                }
            }
        }

        vector<pair<size_t, size_t>> search_results(5);
        for (size_t i = 0; i < docid_count.size(); i++) {
            if (docid_count[i] != 0) {
                if (search_results[4].second < docid_count[i]) {
                    size_t pos = 4;
                    if (search_results[3].second < docid_count[i]) {
                        pos = 3;
                        if (search_results[2].second < docid_count[i]) {
                            pos = 2;
                            if (search_results[1].second < docid_count[i]) {
                                pos = 1;
                                if (search_results[0].second < docid_count[i]) {
                                    pos = 0;
                                }
                            }
                        }
                    }
                    search_results.insert(search_results.begin() + pos, make_pair(i, docid_count[i]));
                    search_results.resize(5);
                }
                docid_count[i] = 0;
            }
        }
        /*partial_sort(begin(search_results), begin(search_results) + min(search_results.size(), (size_t)5), end(search_results),
            [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
                int64_t lhs_docid = lhs.first;
                auto lhs_hit_count = lhs.second;
                int64_t rhs_docid = rhs.first;
                auto rhs_hit_count = rhs.second;
                return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
            });*/

        search_results_output << current_query << ':';
        for (auto [docid, hitcount] : Head(search_results, 5)) {
            if (hitcount == 0) {
                break;
            }
            search_results_output << " {"
                << "docid: " << docid << ", "
                << "hitcount: " << hitcount << '}';
        }
        search_results_output << "\n";
    }
}

void InvertedIndex::Add(const string& document) {
  docs.push_back(document);

  const size_t docid = docs.size() - 1;
  for (const auto& word : SplitIntoWords(document)) {
      auto& v = index[word];
      if (v.size() > 0 && v.rbegin()->first == docid) {
          v.rbegin()->second++;
      }
      else {
          index[word].push_back(make_pair(docid, 1));
      }
  }
}

const vector<pair<size_t, size_t>>& InvertedIndex::Lookup(const string& word) const {
    auto it = index.find(word);
    static const  vector<pair<size_t, size_t>> empty;
    if (it != index.end()) {
        return it->second;
    } else {
        return empty;
    }
}

/*vector<pair<size_t, size_t>> search_results;
        search_results.reserve(docid_count.size());
        for (size_t i = 0; i < docid_count.size(); i++) {
            if (docid_count[i] != 0) {
                search_results.push_back(make_pair(i, docid_count[i]));
                docid_count[i] = 0;
            }
        }
        partial_sort(begin(search_results), begin(search_results) + min(search_results.size(), (size_t)5), end(search_results),
            [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
                int64_t lhs_docid = lhs.first;
                auto lhs_hit_count = lhs.second;
                int64_t rhs_docid = rhs.first;
                auto rhs_hit_count = rhs.second;
                return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
            });

        search_results_output << current_query << ':';
        for (auto [docid, hitcount] : Head(search_results, 5)) {
            if (hitcount == 0) {
                break;
            }
            search_results_output << " {"
                << "docid: " << docid << ", "
                << "hitcount: " << hitcount << '}';
        }
        search_results_output << "\n";
*/