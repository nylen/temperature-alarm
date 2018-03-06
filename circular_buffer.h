// From:
// gh:embeddedartistry/embedded-resources:examples/cpp/circular_buffer.cpp

/**
* Important Usage Note: This library reserves one spare entry for queue-full detection
* Otherwise, corner cases and detecting difference between full/empty is hard.
* You are not seeing an accidental off-by-one.
*/

template <class T>
class circular_buffer {
public:
	circular_buffer(T *buf, size_t size) {
		buf_ = buf;
		size_ = size;
		head_ = 0;
		tail_ = 0;
	}

	void put(T item)
	{
		buf_[head_] = item;
		head_ = (head_ + 1) % size_;

		if(head_ == tail_)
		{
			tail_ = (tail_ + 1) % size_;
		}
	}

	T get(void)
	{
		if(empty())
		{
			return T();
		}

		//Read data and advance the tail (we now have a free space)
		T val = buf_[tail_];
		tail_ = (tail_ + 1) % size_;

		return val;
	}

	void reset(void)
	{
		head_ = tail_;
	}

	bool empty(void)
	{
		//if head and tail are equal, we are empty
		return head_ == tail_;
	}

	bool full(void)
	{
		//If tail is ahead the head by 1, we are full
		return ((head_ + 1) % size_) == tail_;
	}

	size_t size(void)
	{
		return size_ - 1;
	}

private:
	T *buf_;
	size_t head_;
	size_t tail_;
	size_t size_;
};
