#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
using namespace std;

//3

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 2) {
		return;
	}
	else {
		vector<typename RandomIt::value_type> elements(range_begin, range_end);
		size_t first = elements.size() / 3;
		size_t second = elements.size() * 2 / 3;
		MergeSort(begin(elements), begin(elements) + first);
		MergeSort(begin(elements) + first, begin(elements) + second);
		MergeSort(begin(elements) + second, end(elements));
		vector<typename RandomIt::value_type> tmp;
		merge(begin(elements), begin(elements) + first, begin(elements) + first, begin(elements) + second, back_inserter(tmp));
		merge(begin(tmp), end(tmp), begin(elements) + second, end(elements), range_begin);
	}
}

//2

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 2) {
		return;
	}
	else {
		vector<typename RandomIt::value_type> elements(range_begin, range_end);
		size_t middle = elements.size() / 2;
		MergeSort(begin(elements), begin(elements) + middle);
		MergeSort(begin(elements) + middle, end(elements));
		merge(begin(elements), begin(elements) + middle, begin(elements) + middle, end(elements), range_begin);
	}
}