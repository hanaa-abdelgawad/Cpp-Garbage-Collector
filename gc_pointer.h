#ifndef GC_POINTER_H_
#define GC_POINTER_H_
#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <class T, int size = 0>
class Pointer{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise. 
    */
    bool isArray; 
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    unsigned arraySize; // size of the array
    static bool first; // true when first Pointer is created
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    // Define an iterator type for Pointer<T>.
    //typedef Iter<T> GCiterator;
  	using GCiterator = Iter<T>;
  	template <class S, int size_S>
      friend void swap(Pointer<S,size_S> &, Pointer<S,size_S> &);
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
    Pointer():Pointer(nullptr){}
    Pointer(T*);
    // Copy constructor.
    Pointer(const Pointer &copy):Pointer(copy.addr){};
    // Destructor for Pointer.
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*(){
        return *addr;
    }
    // Return the address being pointed to.
    T *operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i){ return addr[i];}
    // Conversion function to T *.
    operator T *() { return addr; }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end(){
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

//Extra Methods
template<class T,int size>
void swap(Pointer<T ,size> &first_element, Pointer<T ,size> &second_element){
	using std::swap;
    swap(first_element.addr, second_element.addr);
    swap(first_element.isArray,second_element.isArray);
    swap(first_element.arraySize,second_element.arraySize);
}

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t){
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown); //atexit destructor of static object
    first = false;

    // Hanaa: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab
    //check the size and set the Pointer object's isArray field accordingly.
    isArray= (size > 0);
    arraySize=size;

    //set the Pointer object's addr field to the given parameter t.
    addr= t;
    
    // Create an instance of the PtrDetails object
    //give it the parameter t instead of the generic T type.
    //check isArray is not assigned 
    
    //Get an iterator into the refContainer using t 
    //(the findPtrInfo method does this).  
    //typename std::list<PtrDetails<T> >::iterator pointer;
    auto findPtrInfo_value = findPtrInfo(t);
     
    //You chas non-pointer type an then use the iterator to check whether 
    //a PtrDetails object already points to this address. 
    //If so, you'll just increment the existing PtrDetails 
    //refcount! If it isn't in the list yet, then you'll need 
    //to increment the refcount of the PtrDetails object you created 
    //and push it back into the refContainer.
    if(findPtrInfo_value != refContainer.end()){
        ++findPtrInfo_value->refcount;
    }
    else {
     	PtrDetails<T> PtrDetails_value;
       	PtrDetails_value.refcount=1;
        PtrDetails_value.memPtr=t;
        PtrDetails_value.isArray=isArray;
        PtrDetails_value.arraySize=arraySize;
        // adds the newly pointer object to the refContainer 
        refContainer.push_back(PtrDetails_value);
    }
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer(){
    // Hanaa: Implement Pointer destructor
    // Lab: New and Delete Project Lab
    auto findPtrInfo_value = findPtrInfo(addr);
    // For real use, you might want to collect unused memory less frequently,
    // such as after refContainer has reached a certain size, after a certain number of Pointers have gone out of scope,
    // or when memory is low.
    if(findPtrInfo_value != refContainer.end())
    {
    	if(findPtrInfo_value->refcount > 0)
        	// decrement ref count
        	--findPtrInfo_value->refcount;
       	if(findPtrInfo_value->refcount ==0)
        	// Collect garbage when a pointer goes out of scope.
    		collect();
    }
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){

    // Hanaa: Implement collect function
    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor
    bool memfreed = false;
    typename std::list<PtrDetails<T> >::iterator p;
    do{
        // Scan refContainer looking for unreferenced pointers.
        for (p = refContainer.begin(); p != refContainer.end(); p++){
            // Hanaa: Implement collect()
            // If in-use, skip.
            if (p->refcount > 0) continue;
            // Remove unused entry from refContainer.
            refContainer.erase(p);
            // Free memory unless the Pointer is null.
            if(p->isArray){
                delete[] p->memPtr;
            }
            else
            {
                delete p->memPtr;
            }
            memfreed=true;
            // Restart the search.
            break;
        }
    } while (p != refContainer.end());
    return memfreed; 
}


// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t){
    // Hanaa: Implement operator==
    // LAB: Smart Pointer Project Lab
    Pointer<T, size> temp(t);
    swap(*this, temp);
    return t;
}
// Overload assignment of Pointer to Pointer.(i.e ptr = ptr)
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){
    swap(*this, *rv);
    return *this;
}


// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}
#endif