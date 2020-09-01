#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
using namespace std;


//3

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 2) {
		return;
	}
	else {
		vector<typename RandomIt::value_type> elements(
			make_move_iterator(range_begin),
			make_move_iterator(range_end));
		size_t first = elements.size() / 3;
		size_t second = elements.size() * 2 / 3;
		MergeSort(begin(elements), begin(elements) + first);
		MergeSort(begin(elements) + first, begin(elements) + second);
		MergeSort(begin(elements) + second, end(elements));
		vector<typename RandomIt::value_type> tmp;
		merge(make_move_iterator(begin(elements)),
			make_move_iterator(begin(elements) + first), 
			make_move_iterator(begin(elements) + first),
			make_move_iterator(begin(elements) + second), back_inserter(tmp));
		merge(make_move_iterator(begin(tmp)), 
			make_move_iterator(end(tmp)),
			make_move_iterator(begin(elements) + second),
			make_move_iterator(end(elements)), range_begin);
	}
}

//2

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 2) {
		return;
	}
	else {
		vector<typename RandomIt::value_type> elements(
			make_move_iterator(range_begin),
			make_move_iterator(range_end));
		size_t middle = elements.size() / 2;
		MergeSort(begin(elements), begin(elements) + middle);
		MergeSort(begin(elements) + middle, end(elements));
		merge(make_move_iterator(begin(elements)), 
			make_move_iterator(begin(elements) + middle), 
			make_move_iterator(begin(elements) + middle),
			make_move_iterator(end(elements)), range_begin);
	}
}