#pragma once

#include <memory>

namespace RAII {
	template <typename Provider>
	class Booking {
	public:
		Booking(Provider* provider, int id)
			: mProvider(provider)
			, mId(id)
		{
			
		}

		Booking(Booking&& other)
		{
			mProvider = other.mProvider;
			mId = other.mId;
			other.mProvider = nullptr;
		}

		Booking& operator=(Booking&& other) 
		{
			mProvider = other.mProvider;
			mId = other.mId;
			other.mProvider = nullptr;
			return *this;
		}

		~Booking() 
		{
			if (mProvider) {
				mProvider->CancelOrComplete(*this);
			}
		}
	private:
		Provider* mProvider;
		int mId;
	};
}