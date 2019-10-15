#ifndef GC_ITERATOR_H_
#define GC_ITERATOR_H_
// Exception thrown when an attempt is made to
// use an Iter that exceeds the range of the
// underlying object.
//
class OutOfRangeExc
{
    // Add functionality if needed by your application.
};


// An iterator-like class for cycling through arrays
// that are pointed to by GCPtrs. Iter pointers
// ** do not ** participate in or affect garbage
// collection. Thus, an Iter pointing to
// some object does not prevent that object
// from being recycled.
//
template <class T>
class Iter
{
  private:
	T *_ptr; 		// current pointer value
	T *_end;		// points to element one past end
	T *_begin;     	// points to start of allocated array
	unsigned _length; // length of sequence
    
  public:
  	//constructor 
    Iter()
    {
        _ptr = _end = _begin = nullptr;
        _length = 0;
    }
    Iter(T *p, T *first, T *last)
    {
        _ptr = p;
        _end = last;
        _begin = first;
        _length = last - first;
    }
    // Return length of sequence to which this
    // Iter points.
    unsigned size() { return _length; }
    // Return value pointed to by ptr.
    // Do not allow out-of-bounds access.
    T &operator*()
    {
        if ((_ptr >= _end) || (_ptr < _begin))
            throw OutOfRangeExc();
        return *_ptr;
    }
    // Return address contained in ptr.
    // Do not allow out-of-bounds access.
    T *operator->()
    {
        if ((_ptr >= _end) || (_ptr < _begin))
            throw OutOfRangeExc();
        return _ptr;
    }
    // Prefix ++.
    Iter operator++()
    {
        _ptr++;
        return *this;
    }
    // Prefix --.
    Iter operator--()
    {
        _ptr--;
        return *this;
    }
    // Postfix ++.
    Iter operator++(int notused)
    {
        T *tmp = _ptr;
        _ptr++;
        return Iter<T>(tmp, _begin, _end);
    }
    // Postfix --.
    Iter operator--(int notused)
    {
        T *tmp = _ptr;
        _ptr--;
        return Iter<T>(tmp, _begin, _end);
    }
    // Return a reference to the object at the
    // specified index. Do not allow out-of-bounds
    // access.
    T &operator[](int i)
    {
        if ((i < 0) || (i >= (_end - _begin)))
            throw OutOfRangeExc();
        return _ptr[i];
    }
    // Define the relational operators.
    bool operator==(const Iter &op2)
    {
        return _ptr == op2._ptr;
    }
    bool operator!=(const Iter &op2)
    {
        return _ptr != op2._ptr;
    }
    bool operator<(const Iter &op2)
    {
        return _ptr < op2._ptr;
    }
    bool operator<=(const Iter &op2)
    {
        return _ptr <= op2._ptr;
    }
    bool operator>(const Iter &op2)
    {
        return _ptr > op2._ptr;
    }
    bool operator>=(const Iter &op2)
    {
        return _ptr >= op2._ptr;
    }
    // Subtract an integer from an Iter.
    Iter operator-(int n)
    {
        _ptr -= n;
        return *this;
    }
    // Add an integer to an Iter.
    Iter operator+(int n)
    {
        _ptr += n;
        return *this;
    }
    // Return number of elements between two Iters.
    int operator-(Iter<T> &itr2)
    {
        return _ptr - itr2._ptr;
    }
};
#endif