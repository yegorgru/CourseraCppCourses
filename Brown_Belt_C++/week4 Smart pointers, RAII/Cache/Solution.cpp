#include "Common.h"

#include <map>
#include <limits>
#include <stdexcept>
#include <mutex>

using namespace std;

using UnpackerPtr = shared_ptr<IBooksUnpacker>;

class LruCache : public ICache {
public:
    LruCache(UnpackerPtr books_unpacker, const Settings& settings)
        : mSettings(settings)
        , mUnpacker(books_unpacker)
    {
        mCurrentMemory = 0;
        mCurrentRank = numeric_limits<int>::min();
    }

    BookPtr GetBook(const string& book_name) override 
    {
        lock_guard guard(mMutex);
        auto found = mCache.find(book_name);
        if (found != mCache.end()) {
            found->second.rank = ++mCurrentRank;
            return found->second.book;
        }
        BookPtr newBook = std::move(mUnpacker->UnpackBook(book_name));
        if (newBook->GetContent().length() > mSettings.max_memory) {
            mCache.clear();
            mCurrentMemory = 0;
            mCurrentRank = numeric_limits<int>::min();
            return newBook;
        }
        mCurrentMemory += newBook->GetContent().size();
        mCache[book_name] = {++mCurrentRank, newBook};
        while (mSettings.max_memory < mCurrentMemory && mCache.size() > 0) {
            eraseMin();
        }
        return newBook;
    }

private:
    struct CacheBook {
        int rank;
        BookPtr book;
    };
    using Cache = std::map<std::string, CacheBook>;
private:
    void eraseMin() {
        auto min = findMin();
        mCurrentMemory -= min->second.book->GetContent().size();
        mCache.erase(min);
    }

    Cache::const_iterator findMin() const {
        if (mCache.size() == 0) {
            throw runtime_error("Cache is empty");
        }
        int min = numeric_limits<int>::max();
        Cache::const_iterator result = mCache.begin();
        for (auto it = mCache.begin(); it != mCache.end(); ++it) {
            if (it->second.rank < min) {
                min = it->second.rank;
                result = it;
            }
        }
        return result;
    }
private:
    Cache mCache;
    Settings mSettings;
    UnpackerPtr mUnpacker;
    size_t mCurrentMemory;
    mutex mMutex;
    int mCurrentRank;
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker, const ICache::Settings& settings) {
    return make_unique<LruCache>(books_unpacker, settings);
}