#ifndef FAKEVECTOR_H
#define FAKEVECTOR_H

#include <initializer_list>
#include <algorithm>
#include <iterator>
#include <exception>
#include <memory>
#include <cstddef>
#include <utility>

namespace fake{
	/**
	 * @brief      Vector class. A copy of std::vector.
	 *
	 * @tparam     T      Type of elements to hold
	 * @tparam     Alloc  Allocator for the vector
	 */
	template <class T, class Alloc = std::allocator<T>>
	class vector{
	public:
		typedef T 																value_type;
		typedef Alloc 															allocator_type;
		typedef value_type& 													reference;
		typedef const value_type& 												const_reference;
		typedef typename std::allocator_traits<allocator_type>::pointer 		pointer;
		typedef typename std::allocator_traits<allocator_type>::const_pointer 	const_pointer;
		typedef __gnu_cxx::__normal_iterator<pointer, vector> 					iterator;
		typedef __gnu_cxx::__normal_iterator<const_pointer, vector> 			const_iterator;
		typedef std::reverse_iterator<iterator> 								reverse_iterator;
		typedef std::reverse_iterator<const_iterator> 							const_reverse_iterator;
		typedef std::ptrdiff_t 													difference_type;
		typedef size_t 															size_type;
	private:
		/**
		 * Allocator associated to the vector.
		 */
		allocator_type allocator_;

		/**
		 * Capacity of the vector.
		 */
		size_type capacity_;
		/**
		 * Size of the vector.
		 */
		size_type size_;
		/**
		 * Pointer to the array.
		 */
		pointer array_;

		/**
		 * Range pointer to the start of the array.
		 */
		pointer array_start_;
		/**
		 * Range pointer to the end of size of the array.
		 */
		pointer array_end_;
		/**
		 * Range pointer to the end of capacity of the array.
		 */
		pointer array_range_end_;

		/**
		 * @brief      Reassigns range pointers to their correct values.
		 */

		void set_pointers(){
			array_start_ = array_;
			array_end_ = array_ + size_;  
			array_range_end_ = array_ + capacity_;
		}

		/**
		 * @brief      Creates a pointer to a new array. Allocates new_size worth of memory and constructs the elements from the old array.
		 * The old array elements are destroyed and deallocated.
		 *
		 * @param[in]  new_size  new array size
		 */
		void increase_array(size_type new_size){
			pointer new_array = allocator_.allocate(new_size);
			construct_elements(begin(), std::min(end(), begin() + new_size), new_array);
			
			destroy_elements(array_, size_);
			allocator_.deallocate(array_, capacity_);
			array_ = new_array;
			capacity_ = new_size;
			set_pointers();
		}

		/**
		 * @brief      Constructs elements from range [begin, end) to a pointer.
		 *
		 * @param[in]  begin          Iterator to begin of range
		 * @param[in]  end            Iterator to end of range
		 * @param[in]  destination    Pointer to destination
		 *
		 * @tparam     InputIterator  Class template type for iterator
		 * @tparam     <unnamed>      { description }
		 */
		template <class InputIterator, typename = std::_RequireInputIter<InputIterator>>
		void construct_elements(InputIterator begin, InputIterator end, pointer destination){
			// std::cout << "cons 1" << std::endl;
			const difference_type distance = end - begin;
			for (unsigned int i = 0; i < distance; ++i){
				allocator_.construct(destination + i, *(begin));
				begin++;
			}
		}

		/**
		 * @brief      Constructs count number of elements with value_type value to a pointer.
		 *
		 * @param[in]  destination  Pointer to destination
		 * @param[in]  count        Number of elements
		 * @param[in]  value        Value
		 */
		void construct_elements(pointer destination, size_type count, value_type value){
			// std::cout << "cons 2" << std::endl;
			for (size_type i = 0; i < count; ++i)
				allocator_.construct(destination + i, value);
		}

		/**
		 * @brief      Destroys the first n elements of array start.
		 *
		 * @param[in]  start  Pointer to the start of range
		 * @param[in]  n      Number of elements
		 */
		void destroy_elements(pointer start, size_type n){
			for (int i = 0; i < n; ++i){
				allocator_.destroy(start + i);
			}
		}

	public:
		// Constructors

		/**
		 * @brief      Default constructor with a possible custom allocator.
		 *
		 * @param[in]  alloc  Custom allocator
		 */
		explicit
		vector(const allocator_type& alloc = allocator_type()) : 
			allocator_(alloc),
			capacity_(0), 
			size_(0), 
			array_(nullptr),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_)
			{}; 

		/**
		 * @brief      Constructor with defined size. Allocates memory for n elements.
		 *
		 * @param[in]  n     Element count
		 */
		explicit 
		vector(size_type n) : 
			allocator_(),
			capacity_(n), 
			size_(n), 
			array_(allocator_.allocate(n)),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_) 
			{};

		/**
		 * @brief      Constructor with defined size, value and a possible custom allocator.
		 * Allocates memory for n elements and constructs n values val.
		 * 
		 *
		 * @param[in]  n      { parameter_description }
		 * @param[in]  val    The value
		 * @param[in]  alloc  The allocate
		 */
		vector(size_type n, value_type val, const allocator_type& alloc = allocator_type()) : 
			allocator_(alloc),
			capacity_(n), 
			size_(n), 
			array_(allocator_.allocate(n)),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_) 
			{
				construct_elements(array_, n, val);
			};

		/**
		 * @brief      Constructor with defined iterator range [first, last). Allocates memory for enough elements in this range and constructs them. 
		 *
		 * @param[in]  first          Iterator to begin of range
		 * @param[in]  last           Iterator to end of range
		 * @param[in]  alloc          The allocator
		 *
		 * @tparam     InputIterator  Class template type for iterator
		 * @tparam     <unnamed>      { description }
		 */
		template <class InputIterator, typename = std::_RequireInputIter<InputIterator>>
		vector(InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type()) : 
			allocator_(alloc),
			capacity_(static_cast<size_type>(last-first)), 
			size_(static_cast<size_type>(last-first)), 
			array_(allocator_.allocate(static_cast<size_type>(last-first))),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_)
			{
				construct_elements(first, last, array_start_);
			};

		/**
		 * @brief      Copy constructor for fake::vector type. 
		 *
		 * @param[in]  x     Vector to be copied
		 */
		vector(const vector& x) : 
			allocator_(),
			capacity_(x.capacity()),
			size_(x.size()),
			array_(allocator_.allocate(x.capacity())),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_)
			{
				construct_elements(x.begin(), x.end(), array_start_);
			};

		/**
		 * @brief      Copy constructor for fake::vector type with custom allocator. 
		 *
		 * @param[in]  x     Vector to be copied
		 * @param[in]  alloc  The allocator
		 */
		vector(const vector& x, const allocator_type& alloc) : 
			allocator_(alloc),
			capacity_(x.capacity()),
			size_(x.size()),
			array_(allocator_.allocate(x.capacity())),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_)
			{
				construct_elements(x.begin(), x.end(), array_start_);
			};
		
		/**
		 * @brief      Move constructor for fake::vector.
		 *
		 * @param[in]  x 	Vector to be moved
		 */
		vector(vector&& x) : 
			allocator_(),
			capacity_(x.capacity_),
			size_(x.size_),
			array_(x.array_),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_)
			{
				x.array_ = nullptr;
				x.size_ = 0;
				x.capacity_ = 0;
				x.set_pointers();
			};

		/**
		 * @brief      Move constructor for fake::vector with custom allocator.
		 *
		 * @param[in]  x 	Vector to be moved
		 * @param[in]  alloc      The allocator
		 */
		vector(vector&& x, const allocator_type& alloc) : 
			allocator_(alloc),
			capacity_(x.capacity_),
			size_(x.size_),
			array_(x.array_),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_)
			{
				x.array_ = nullptr;
				x.size_ = 0;
				x.capacity_ = 0;
				x.set_pointers();
			};

		/**
		 * @brief      Constructor for an initializer list with possible custom allocator. Allocates enough memory for elements in the list and constructs them. 
		 *
		 * @param[in]  il    The initializer list
		 * @param[in]  alloc  The allocator
		 */
		vector(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type()) : 
			allocator_(alloc),
			capacity_(il.size()),
			size_(il.size()),
			array_(allocator_.allocate(il.size())),
			array_start_(array_),
			array_end_(array_ + size_),
			array_range_end_(array_ + capacity_)
			{
				construct_elements(il.begin(), il.end(), array_start_);
			};

		/**
		 * @brief      Destructor for fake::vector.
		 */
		~vector() {
			destroy_elements(array_, size_);
			allocator_.deallocate(array_, capacity_);
			array_start_ = nullptr;
			array_end_ = nullptr;
			array_range_end_ = nullptr;
		}

		/**
		 * @brief      Copy assign operator for vector. Copies the properties and elements from vector on the right to the vector on the left.
		 *
		 * @param[in]  x    Vector on the right hand side
		 *
		 * @return     Returns reference to the copied vector.
		 */
		vector& operator=(const vector& x){
			if (capacity_ >= x.size_){
				size_ = x.size();
				set_pointers();
				construct_elements(x.begin(), x.end(), array_start_);
			} else {
				destroy_elements(array_, size_);
				allocator_.deallocate(array_, capacity_);
				capacity_ = x.size();
				size_ = x.size();
				array_ = allocator_.allocate(x.size());
				set_pointers();
				construct_elements(x.begin(), x.end(), array_start_);
			}
			return *this;
		}

		/**
		 * @brief      Move assign operator. Moves the properties and elements from vector on the right to the vector on the left.
		 *
		 * @param[in] x Vector on the right hand side
		 *
		 * @return     Returns reference to the copied vector.
		 */
		vector& operator=(vector&& x){
			destroy_elements(array_, capacity_);
			allocator_.deallocate(array_, capacity_);
			size_ = x.size_;
			capacity_ = x.capacity_;
			array_ = x.array_;
			set_pointers();
			x.size_ = 0;
			x.capacity_ = 0;
			x.array_ = nullptr;
			x.set_pointers();
			return *this;
		}

		/**
		 * @brief      Assign operator for initializer list. Destroys previuosly held vector values. Creates a vector with initializer list values.
		 *
		 * @param[in]  il   	The initializer list
		 *
		 * @return     Reference to the new vector.
		 */
		vector& operator=(std::initializer_list<value_type> il){
			if (capacity_ >= il.size()){
				size_ = il.size();
				set_pointers();
				construct_elements(il.begin(), il.end(), array_start_);
			} else {
				destroy_elements(array_, size_);
				allocator_.deallocate(array_, capacity_);
				capacity_ = il.size();
				size_ = il.size();
				array_ = allocator_.allocate(il.size());
				set_pointers();
				construct_elements(il.begin(), il.end(), array_start_);
			}
			return *this;
		}

		// Iterators

		/**
		 * @brief      Iterator to the start of the array.
		 *
		 * @return     Iterator to the start of the array.
		 */
		inline iterator begin(){return iterator(array_start_);}
		/**
		 * @brief      Iterator to the end of the array.
		 *
		 * @return     Iterator to the end of the array.
		 */
		inline iterator end(){return iterator(array_end_);}
		
		/**
		 * @brief      Const_iterator to the start of the array.
		 *
		 * @return     Const_iterator to the start of the array.
		 */
		inline const_iterator begin() const {return const_iterator(array_start_);}
		/**
		 * @brief      Const_iterator to the end of the array.
		 *
		 * @return     Const_iterator to the end of the array.
		 */
		inline const_iterator end() const {return const_iterator(array_end_);}

		/**
		 * @brief      Const_iterator to the start of the array.
		 *
		 * @return     Const_iterator to the start of the array.
		 */
		inline const_iterator cbegin() const {return const_iterator(array_start_);}
		/**
		 * @brief      Const_iterator to the end of the array.
		 *
		 * @return     Const_iterator to the end of the array.
		 */
		inline const_iterator cend() const {return const_iterator(array_end_);}

		/**
		 * @brief      Reverse iterator the end of the array.
		 *
		 * @return     Reverse iterator the end of the array.
		 */
		inline reverse_iterator rbegin(){return reverse_iterator(end());}
		/**
		 * @brief      Reverse iterator the start of the array.
		 *
		 * @return     Reverse iterator the start of the array.
		 */
		inline reverse_iterator rend(){return reverse_iterator(begin());}
		
		/**
		 * @brief      Const_reverse iterator the end of the array.
		 *
		 * @return     Const_reverse iterator the end of the array.
		 */
		inline const_reverse_iterator rbegin() const {return reverse_iterator(cend());}
		/**
		 * @brief      Const_reverse iterator the start of the array.
		 *
		 * @return     Const_reverse iterator the start of the array.
		 */
		inline const_reverse_iterator rend() const {return reverse_iterator(cbegin());}

		/**
		 * @brief      Const_reverse iterator the end of the array.
		 *
		 * @return     Const_reverse iterator the end of the array.
		 */
		inline const_reverse_iterator crbegin() const {return reverse_iterator(cend());}
		/**
		 * @brief      Const_reverse iterator the start of the array.
		 *
		 * @return     Const_reverse iterator the start of the array.
		 */
		inline const_reverse_iterator crend() const {return reverse_iterator(cbegin());}

		// Capacity

		/**
		 * @brief      Getter for current size of the vector.
		 *
		 * @return     Size.
		 */
		inline size_type size() const {return size_;}
		//max_size
		
		/**
		 * @brief      Changes the size of the vector. If the new size is lower or equal to the current size destroys out of range elements and the capacity stays untouched.
		 * If the size is greater than the current size allocates memory for n more elements.
		 *
		 * @param[in]  n     New vector size
		 */
		void resize (size_type n){
			if (n <= size_){
				destroy_elements(array_start_ + n, size_ - n);
				size_ = n;
			} else {
				increase_array(std::max(n, 2*capacity_));
				size_ = n;
			}
		}

		/**
		 * @brief      Changes the size of the vector. If the new size is lower or equal to the current size destroys out of range elements and the capacity stays untouched.
		 * If the size is greater than the current size allocates memory and constructs n elements with value val.
		 *
		 * @param[in]  n     New vector size
		 * @param[in]  val   Value to fill empty space
		 */
		void resize (size_type n, const value_type& val){
			if (n <= size_){
				destroy_elements(array_start_ + n, size_ - n);
				size_ = n;
			} else {
				increase_array(std::max(n, 2*capacity_));
				construct_elements(end(), array_range_end_ - array_end_, val);
				size_ = n;
				set_pointers();
			}
		}

		/**
		 * @brief      Getter for current capacity of the vector.
		 *
		 * @return     Capacity.
		 */
		inline size_type capacity() const {return capacity_;}

		/**
		 * @brief      Checks if the vector is empty.
		 *
		 * @return     True if the vector is empty, False otherwise.
		 */
		inline bool empty(){return size_ == 0;}

		/**
		 * @brief      Increases the capacity of the array if n is greater than the current capacity, does nothing otherwise.
		 *
		 * @param[in]  n     New capacity.
		 */
		void reserve(size_type n){
			if (n > capacity_)
				increase_array(n);
		}

		/**
		 * @brief      Reduces the capacity to equal the size.
		 */
		void shrink_to_fit(){
			increase_array(size_);
		}

		// Element access

		/**
		 * @brief      Overloads the [] operator to work like in an array. 
		 *
		 * @param[in]  n     Index of an element in the vector
		 *
		 * @return     Reference to the n'th element in the vector.
		 */
		reference operator[](size_type n){return *(array_start_ + n);}
		/**
		 * @brief      Overloads the [] operator to work like in an array. 
		 *
		 * @param[in]  n     Index of an element in the vector
		 *
		 * @return     Const_reference to the n'th element in the vector.
		 */
		const_reference operator[](size_type n) const {return *(array_start_ + n);}

		/**
		 * @brief      Function to access an element in the array. Checks whether the requested element is the the range of the vector or not. If the element is out of range
		 * throws an std::out_of_range exception.
		 *
		 * @param[in]  n     Index of an element in the vector
		 *
		 * @return    Reference to the n'th element in the vector.
		 */
		reference at(size_type n){
			if (n < capacity_ && n > 0)
				return *(array_start_ + n);
			else throw std::out_of_range("out of vector range.");
		}


		/**
		 * @brief      Function to access an element in the array. Checks whether the requested element is the the range of the vector or not. If the element is out of range
		 * throws an std::out_of_range exception.
		 *
		 * @param[in]  n     Index of an element in the vector
		 *
		 * @return    Const_reference to the n'th element in the vector.
		 */
		const_reference at(size_type n) const {
			if (n < capacity_ && n > 0)
				return *(array_start_ + n);
			else throw std::out_of_range("out of vector range.");
		}

		/**
		 * @brief      Reference to the start of the array
		 *
		 * @return     Reference to the start of the array
		 */
		inline reference front(){
			return *(array_start_);
		}

		/**
		 * @brief      Const_reference to the start of the array
		 *
		 * @return     Const_reference to the start of the array
		 */
		inline const_reference front() const {
			return *(array_start_);
		}
		/**
		 * @brief      Reference to the end of the array
		 *
		 * @return     Reference to the end of the array
		 */
		inline reference back(){
			return *(array_end_);
		}
		/**
		 * @brief      Const_reference to the end of the array
		 *
		 * @return     Const_reference to the end of the array
		 */
		inline const_reference back() const {
			return *(array_end_);
		}

		/**
		 * @brief      Returns the pointer to the vector array.
		 *
		 * @return    	Pointer to the vector array.
		 */
		inline pointer data() const {return array_;}

		// Modifiers

		/**
		 * @brief      Replaces the vectors contents with the values in range [first, end).
		 *
		 * @param[in]  first          Iterator to the start of the range
		 * @param[in]  last           Iterator to the end of the range
		 *
		 * @tparam     InputIterator  Template parameter to only allow iterators to call this function.
		 * @tparam     <unnamed>      { description }
		 */
		template <class InputIterator, typename = std::_RequireInputIter<InputIterator>>
		void assign(InputIterator first, InputIterator last){
			if (last - first <= capacity_){
				size_ = last-first;
				construct_elements(first, last, array_start_);
				set_pointers();
			} else {
				size_ = last-first;
				increase_array(last-first);
				construct_elements(first, last, array_start_);
				set_pointers();
			}
		}

		/**
		 * @brief      Replaces the vectors contents with n number of values val.
		 *
		 * @param[in]  n     Number of elements
		 * @param[in]  val   Value of the elements
		 */
		void assign (size_type n, const_reference val){
			if (n <= capacity_){
				size_ = n;
				construct_elements(array_, n, val);
				set_pointers();
			} else {
				size_ = n;
				increase_array(n);
				construct_elements(array_, n, val);
				set_pointers();
			}
		}

		/**
		 * @brief      Replaces the vectors contents with the values in an initializer list.
		 *
		 * @param[in]  il    The initializer list
		 */
		void assign (std::initializer_list<value_type> il){
			if (il.size() <= capacity_){
				size_ = il.size();
				construct_elements(il.begin(), il.end(), array_start_);
				set_pointers();
			} else {
				size_ = il.size();
				increase_array(il.size());
				s(il.begin(), il.end(), begin());
				set_pointers();
			}
		}

		/**
		 * @brief      Adds a value to the end of a vector.
		 *
		 * @param[in]  val   The value
		 */
		void push_back(const value_type& val){
			if (array_end_ != array_range_end_){
				allocator_.construct(array_end_, val);
				size_++;
				set_pointers();
			} else {
				increase_array(std::max(1, static_cast<int>(capacity_*2)));
				allocator_.construct(array_end_, val);
				size_++;
				set_pointers();
			}
		}

		/**
		 * @brief      Moves a value to the end of a vector.
		 *
		 * @param[in]  val  The value
		 */
		void push_back(value_type&& val){
			if (array_end_ != array_range_end_){
				allocator_.construct(array_end_, val);
				size_++;
				set_pointers();
			} else {
				increase_array(std::max(1, static_cast<int>(capacity_*2)));
				allocator_.construct(array_end_, val);
				size_++;
				set_pointers();
			}
		}

		/**
		 * @brief      Destroys the last item in the vector.
		 */
		void pop_back(){
			allocator_.destroy(array_end_);
			size_--;
			set_pointers();
		}

		/**
		 * @brief      Inserts an element with a value val to a position pointed by an iterator position.
		 *
		 * @param[in]  position  The position
		 * @param[in]  val       The value
		 *
		 * @return     An iterator to the inserted value.
		 */
		iterator insert (const_iterator position, const value_type& val){
			const difference_type distance = position - cbegin();
			if (array_end_ == array_range_end_)
				increase_array(std::max(1, static_cast<int>(capacity_*2)));
			size_++;
			iterator it = begin() + distance;
			construct_elements(it, end(), array_start_ + distance + 1);
			allocator_.construct(array_start_ + distance, val);
			return it;
		}

		/**
		 * @brief      Moves and inserts an element with a value val to a position pointed by an iterator position.
		 *
		 * @param[in]  position  The position
		 * @param[in]  val       The value
		 *
		 * @return     An iterator to the inserted value.
		 */
		iterator insert(const_iterator position, value_type&& val){
			const difference_type distance = position - cbegin();
			if (array_end_ == array_range_end_)
				increase_array(std::max(1, static_cast<int>(capacity_*2)));
			size_++;
			iterator it = begin() + distance;
			construct_elements(it, end(), array_start_ + distance + 1);
			allocator_.construct(array_start_ + distance, val);
			return it;
		}

		/**
		 * @brief      Inserts count number of elements with values val to a position pointed by an iterator position.
		 *
		 * @param[in]  position  The position
		 * @param[in]  count     The count
		 * @param[in]  val       The value
		 *
		 * @return     Iterator to the first of the inserted elements.
		 */
		iterator insert(const_iterator position, size_type count, const value_type& val){
			const difference_type distance = position - cbegin();
			if (size_ + count > capacity_)
				increase_array(std::max(static_cast<int>(capacity_*2), static_cast<int>(size_ + count)));
			size_ += count;
			iterator it = begin() + distance;
			construct_elements(it, end(), array_start_ + distance + count);
			construct_elements(it, count, val);
			return it;
		}

		/**
		 * @brief      Inserts the elements from range [first, last) to a position pointed by an iterator position.
		 *
		 * @param[in]  position       The position
		 * @param[in]  first          Iterator to the start of the range
		 * @param[in]  last           Iterator to the end of the range
		 *
		 * @tparam     InputIterator  Template argument to only allow iterators to pass
		 * @tparam     <unnamed>      { description }
		 *
		 * @return    Iterator to the first of the inserted elements
		 */
		template <class InputIterator, typename = std::_RequireInputIter<InputIterator>>
		iterator insert(const_iterator position, InputIterator first, InputIterator last){
			const difference_type distance = position - cbegin();
			const difference_type count = last - first;
			if (size_ + count > capacity_)
				increase_array(std::max(static_cast<int>(capacity_*2), static_cast<int>(size_ + count)));
			size_ += count;
			iterator it = begin() + distance;
			construct_elements(it, end(), array_start_ + distance + count);
			construct_elements(first, last, array_start_ + distance);
			return it;
		}

		/**
		 * @brief      Inserts the elements from an initializer list to a position pointed by an iterator position.
		 *
		 * @param[in]  position  The position
		 * @param[in]  il        The initializer list
		 *
		 * @return     Iterator to the first of the inserted elements
		 */
		iterator insert(const_iterator position, std::initializer_list<value_type> il){
			const difference_type distance = position - cbegin();
			const difference_type count = il.end() - il.begin();
			if (size_ + count > capacity_)
				increase_array(std::max(static_cast<int>(capacity_*2), static_cast<int>(size_ + count)));
			size_ += count;
			iterator it = begin() + distance;
			construct_elements(it, end(), array_start_ + distance + count);
			construct_elements(il.begin(), il.end(), array_start_ + distance);
			return it;
		}

		/**
		 * @brief      Removes an element from the vector pointed by the iterator position.
		 *
		 * @param[in]  position  The position
		 *
		 * @return     Iterator to the removed element.
		 */
		iterator erase(const_iterator position){
			const difference_type distance = position - cbegin();
			iterator it = begin() + distance;
			construct_elements(it + 1, end(), array_start_ + distance);
			size_--;
			return it;

		}

		/**
		 * @brief      Removes the elements in the range [first, last) from the vector pointed by the iterator position.
		 *
		 * @param[in]  first          Iterator to the start of the range
		 * @param[in]  last           Iterator to the end of the range
		 *
		 * @return     Iterator to the first of the removes elements.
		 */
		iterator erase(const_iterator first, const_iterator last){
			const difference_type distance = first - cbegin();
			const difference_type distance_first_last = last - cbegin();
			iterator it_first = begin() + distance;
			iterator it_last = begin() + distance_first_last;
			construct_elements(it_last, end(), array_start_ + distance);
			size_ -= last-first;
			return it_first;

		}

		/**
		 * @brief      Swaps the values and contents of two vectors.
		 *
		 * @param      x     Vector to swap the contents with.
		 */
		void swap(vector& x){
			std::swap(capacity_, x.capacity_);
			std::swap(size_, x.size_);
			std::swap(array_, x.array_);
			set_pointers();
			x.set_pointers();
		}

		/**
		 * @brief      Destroys the vectors contents and sets the size to 0.
		 */
		void clear(){
			destroy_elements(array_, size_);
			size_ = 0;
			set_pointers();
		}

		/**
		 * @brief      Inserts and constructs an element to the position position. 
		 *
		 * @param[in]  position   The position
		 * @param[in]  args       Arguments to forward to the constructor of the element
		 *
		 * @tparam     Args       Arguments template
		 *
		 * @return     An iterator the inserted element.
		 */
		template <class... Args>
		iterator emplace (const_iterator position, Args&&... args){
			difference_type distance = position - cbegin();
			if (array_end_ == array_range_end_)
				increase_array(std::max(1, static_cast<int>(capacity_*2)));
			construct_elements(begin() + distance, end(), array_start_ + distance + 1);
			allocator_.construct(array_start_ + distance, std::forward<Args>(args)...);
			size_++;
			set_pointers();
			return begin() + distance;
		}

		/**
		 * @brief      Inserts and constructs an element to the end of the vector.
		 *
		 * @param[in]  args       Arguments to forward to the constructor of the element
		 *
		 * @tparam     Args       Arguments template
		 */
		template <class... Args>
		void emplace_back (Args&&... args){
			if (array_end_ == array_range_end_)
				increase_array(std::max(1, static_cast<int>(capacity_*2)));
			allocator_.construct(array_end_, std::forward<Args>(args)...);
			size_++;
			set_pointers();
		}

		// Allocator

		/**
		 * @brief      Returns the allocator associated to the vector.
		 *
		 * @return     The allocator.
		 */
		inline allocator_type get_allocator(){ return allocator_;}

	}; 
}

#endif
