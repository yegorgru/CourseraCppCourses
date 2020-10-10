#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>

vector<string> SplitIntoWords(const string& line) {
  istringstream words_input(line);
  return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  InvertedIndex new_index;

  for (string current_document; getline(document_input, current_document); ) {
    new_index.Add(move(current_document));
  }

  index = move(new_index);
}

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {
    vector<size_t> docid_count(50000, 0);
      for (string current_query; getline(query_input, current_query); ) {

        const auto words = SplitIntoWords(current_query);

        for (const auto& word : words) {
          for (const auto& docids_number : index.Lookup(word)) {
            docid_count[docids_number.first]+= docids_number.second;
          }
        }

        vector<pair<size_t, size_t>> search_results;
        search_results.reserve(docid_count.size());
        for (size_t i = 0; i < docid_count.size(); i++) {
            if (docid_count[i] != 0) {
                search_results.push_back(make_pair(i,docid_count[i]));
                docid_count[i] = 0;
            }
        }
        partial_sort(begin(search_results), begin(search_results)+min(search_results.size(), (size_t)5), end(search_results),
            [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
                int64_t lhs_docid = lhs.first;
                auto lhs_hit_count = lhs.second;
                int64_t rhs_docid = rhs.first;
                auto rhs_hit_count = rhs.second;
                return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
            });

        search_results_output << current_query << ':';
        for (auto [docid, hitcount] : Head(search_results, 5)) {
          search_results_output << " {"
            << "docid: " << docid << ", "
            << "hitcount: " << hitcount << '}';
        }
        search_results_output << endl;
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

vector<pair<size_t, size_t>> InvertedIndex::Lookup(const string& word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return {};
  }
}
