#pragma once
#define TYPE8b unsigned long long
#define sizetype unsigned long long
#define MEMTYPES_MAGICNUM 0x4D454D5459504553ULL
#define MAXULL 0xFFFFFFFFFFFFFFFFULL


#ifdef _WIN32
#include <Windows.h>
inline void* alloc_mem(const sizetype buffer) {
	if (buffer == 0) return nullptr;
	sizetype buffer_with_metadata = buffer + 16;
	sizetype alligned_buffer = (buffer_with_metadata + 4095) & ~4095;
	void* ptr = VirtualAlloc(nullptr, alligned_buffer, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (ptr != nullptr) {
		((sizetype*)ptr)[0] = MEMTYPES_MAGICNUM;
		((sizetype*)ptr)[1] = alligned_buffer;
		return (void*)(((unsigned char*)ptr) + 16);
	}
	return ptr;
}

inline void free_mem(void* ptr) {
	if (ptr == nullptr) return;
	void* raw_ptr = (void*)(((unsigned char*)ptr) - 16);
	if (((sizetype*)raw_ptr)[0] != MEMTYPES_MAGICNUM) return;
	VirtualFree(raw_ptr, 0, MEM_RELEASE);
}

#else
inline void* alloc_mem(const sizetype buffer) {
	if (buffer == 0) return nullptr;
	sizetype buffer_with_metadata = buffer + 16;
	sizetype alligned_buffer = (buffer_with_metadata + 4095) & ~4095;
	void* ptr = nullptr;
	__asm__ volatile(
	"movq $9, %%rax\n\t" //syscall 9
	"movq $0, %%rdi\n\t" //address nullptr
	"movq %1, %%rsi\n\t" //buffer
	"movq $3, %%rdx\n\t" //PROT_READ | PROT_WRITE
	"movq $34, %%r10\n\t" //MAP_PRIVATE | MAP_ANONYMOUS
	"movq $-1, %%r8\n\t" //fd -1
	"movq $0, %%r9\n\t" //offsett 0
	"syscall\n\t"
	"movq %%rax, %0\n\t" //move to ptr
	: "=r" (ptr)
	: "g" (alligned_buffer)
	: "rax", "rdi", "rsi", "rdx", "r10", "r8", "r9"
	);
	if ((unsigned long long) ptr >= (unsigned long long) -4095) return nullptr;
	((sizetype*)ptr)[0] = MEMTYPES_MAGICNUM;
	((sizetype*)ptr)[1] = alligned_buffer;
	return (void*) (((unsigned char*)ptr) + 16);
}

inline void free_mem(void* ptr) {
	if (ptr == nullptr) return;
	void* raw_ptr = (void*) (((unsigned char*)ptr) - 16);
	if (((sizetype*)raw_ptr)[0] != MEMTYPES_MAGICNUM) return;
	sizetype buffer = ((sizetype*)raw_ptr)[1];
	__asm__ volatile(
	"movq $11, %%rax\n\t" //syscall 11
	"movq %0, %%rdi\n\t" //move ptr
	"movq %1, %%rsi\n\t"
	"syscall\n\t"
	:
	: "r" (raw_ptr), "g" (buffer)
	: "rax", "rdi", "rsi"
	);
}

#endif 


#ifdef _WIN32

class TYPE16b {
public:
	inline friend TYPE16b operator+(const TYPE16b& first, const TYPE16b& second) {
		TYPE16b temp;
		temp.high = first.high + second.high;
		temp.low = first.low + second.low;
		if (temp.low < first.low) {
			temp.high += 1;
		}
		return temp;
	}
	inline friend TYPE16b operator-(const TYPE16b& first, const TYPE16b& second) {
		TYPE16b temp;
		temp.high = first.high - second.high;
		temp.low = first.low - second.low;
		if (temp.low > first.low) {
			temp.high -= 1;
		}
		return temp;
	}
	inline friend bool operator<(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return false;
		if (first.high < second.high) return true;
		if (first.high > second.high) return false;
		if (first.low < second.low) return true;
		if (first.low > second.low) return false;
		return false;
	}
	inline friend bool operator>(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return false;
		if (first.high > second.high) return true;
		if (first.high < second.high) return false;
		if (first.low > second.low) return true;
		if (first.low < second.low) return false;
		return false;
	}
	inline friend bool operator<=(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return true;
		if (first.high < second.high) return true;
		if (first.high > second.high) return false;
		if (first.low < second.low) return true;
		if (first.low > second.low) return false;
		return false;
	}
	inline friend bool operator>=(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return true;
		if (first.high > second.high) return true;
		if (first.high < second.high) return false;
		if (first.low > second.low) return true;
		if (first.low < second.low) return false;
		return false;
	}
	inline friend bool operator==(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return true;
		return false;
	}

	TYPE8b low = 0;
	TYPE8b high = 0;
	
	
	inline void operator+=(const TYPE16b& first){
		*this = *this + first;
	}
	inline void operator-=(const TYPE16b& first) {
		*this = *this - first;
	}
	TYPE16b(TYPE8b num) {
		this->low = num;
	}
	TYPE16b(){}
	inline void operator+=(const TYPE8b& first) {
		TYPE8b before = this->low;
		this->low += first;
		if (this->low < before) {
			this->high += 1;
		}
	}
	inline void operator-=(const TYPE8b& first) {
		TYPE8b before = this->low;
		this->low -= first;
		if (this->low > before) {
			this->high -= 1;
		}
	}
	

	
};

class TYPE32b {
public:
	inline friend TYPE32b operator+(const TYPE32b& first, const TYPE32b& second) {
		TYPE32b temp;
		TYPE8b carry;
		temp.low.low = first.low.low + second.low.low;
		if (temp.low.low < first.low.low) carry = 1;
		else carry = 0;
		temp.low.high = first.low.high + second.low.high + carry;
		if (temp.low.high < first.low.high || (carry == 1 && temp.low.high == first.low.high)) carry = 1;
		else carry = 0;
		temp.high.low = first.high.low + second.high.low + carry;
		if (temp.high.low < first.high.low || (carry == 1 && temp.high.low == first.high.low)) carry = 1;
		else carry = 0;
		temp.high.high = first.high.high + second.high.high + carry;
		return temp;
	}
	inline friend TYPE32b operator-(const TYPE32b& first, const TYPE32b& second) {
		TYPE32b temp;
		TYPE8b carry;
		temp.low.low = first.low.low - second.low.low;
		if (temp.low.low > first.low.low) carry = 1;
		else carry = 0;
		temp.low.high = first.low.high - second.low.high - carry;
		if (temp.low.high > first.low.high || (carry == 1 && temp.low.high == first.low.high)) carry = 1;
		else carry = 0;
		temp.high.low = first.high.low - second.high.low - carry;
		if (temp.high.low > first.high.low || (carry == 1 && temp.high.low == first.high.low)) carry = 1;
		else carry = 0;
		temp.high.high = first.high.high - second.high.high - carry;
		return temp;
	}
	inline friend bool operator<(const TYPE32b& first, const TYPE32b& second) {
		if (first.high == second.high && first.low == second.low) return false;
		if (first.high < second.high) return true;
		if (first.high > second.high) return false;
		if (first.low < second.low) return true;
		if (first.low > second.low) return false;
		return false;
	}
	inline friend bool operator>(const TYPE32b& first, const TYPE32b& second) {
		if (first.high == second.high && first.low == second.low) return false;
		if (first.high > second.high) return true;
		if (first.high < second.high) return false;
		if (first.low > second.low) return true;
		if (first.low < second.low) return false;
		return false;
	}
	inline friend bool operator<=(const TYPE32b& first, const TYPE32b& second) {
		if (first.high == second.high && first.low == second.low) return true;
		if (first.high < second.high) return true;
		if (first.high > second.high) return false;
		if (first.low < second.low) return true;
		if (first.low > second.low) return false;
		return false;
	}
	inline friend bool operator>=(const TYPE32b& first, const TYPE32b& second) {
		if (first.high == second.high && first.low == second.low) return true;
		if (first.high > second.high) return true;
		if (first.high < second.high) return false;
		if (first.low > second.low) return true;
		if (first.low < second.low) return false;
		return false;
	}
	inline friend bool operator==(const TYPE32b& first, const TYPE32b& second) {
		if (first.high == second.high && first.low == second.low) return true;
		return false;
	}

	TYPE16b low = 0;
	TYPE16b high = 0;
	

	inline void operator+=(const TYPE32b& first) {
		*this = *this + first;
	}
	inline void operator-=(const TYPE32b& first) {
		*this = *this - first;
	}
	TYPE32b(TYPE16b num) {
		this->low = num;
	}
	TYPE32b() {}
	inline void operator+=(const TYPE16b& first) {
		TYPE16b before = this->low;
		int plusone = 0;
		this->low.low += first.low;
		if (before.low > this->low.low) {
			plusone = 1;
		}
		this->low.high = this->low.high + first.high + plusone;
		if (before.high > this->low.high || (plusone == 1 && before.high == this->low.high)) plusone = 1;
		else plusone = 0;
		this->high.low += plusone;
		if (this->high.low == 0 && plusone == 1) this->high.high += 1;
	}
	inline void operator-=(const TYPE16b& first) {
		TYPE16b before = this->low;
		int plusone = 0;
		this->low.low -= first.low;
		if (before.low < this->low.low) {
			plusone = 1;
		}
		this->low.high = this->low.high - first.high - plusone;
		if (before.high < this->low.high || (plusone == 1 && before.high == this->low.high)) plusone = 1;
		else plusone = 0;
		this->high.low -= plusone;
		if (this->high.low == MAXULL && plusone == 1) this->high.high -= 1;
	}


};

inline void my_memcpy(const void* src, void* dest, sizetype buffer) {
	if (buffer == 0 || dest == nullptr || src == nullptr) return;
	if (buffer < 8) {
		for (sizetype i = 0; i < buffer; ++i) {
			((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
		}
	}
	else {
		sizetype n_times = buffer / sizeof(sizetype);
		const sizetype* d_src = (const sizetype*)src;
		sizetype* d_dest = (sizetype*)dest;
		for (sizetype i = 0; i < n_times; ++i) {
			d_dest[i] = d_src[i];
		}
		sizetype bytes_cp = n_times * sizeof(sizetype);
		for (sizetype i = bytes_cp; i < buffer; ++i) {
			((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
		}
	}

}

template<class T>
class vector {
private:
	T* data = nullptr;
	bool large = false;
	sizetype current_size = 0;
	int max = 0;
	int index = 0;
	
	inline bool make_new_max() {
		T* new_ptr = nullptr;
		if (large) {
			new_ptr = (T*)alloc_mem(current_size + sizeof(T));
			if (new_ptr != nullptr) {
				my_memcpy(data, new_ptr, current_size);
				current_size += sizeof(T);
				free_mem(data);
				data = new_ptr;
				max += 1;
				return true;
			} 
			return false;
		}
		else {
			new_ptr = (T*)alloc_mem(current_size + (sizeof(T) * 100));
			if (new_ptr != nullptr) {
				my_memcpy(data, new_ptr, current_size);
				current_size += sizeof(T) * 100;
				free_mem(data);
				data = new_ptr;
				max += 100;
				return true;
			}
			return false;
		}
	}
	inline vector(bool) {}
public:

	inline friend vector<T> operator+(const vector<T>& first, const vector<T>& second) { 
		vector<T> new_vector(true);
		if (first.large) {
			new_vector.large = true;
			new_vector.current_size = (first.index + second.index)* sizeof(T) + (sizeof(T) * 100);
		}
		else {
			new_vector.large = false;
			new_vector.current_size = (first.index + second.index) * sizeof(T) + (sizeof(T) * 2);
		}
		new_vector.data = (T*)alloc_mem(new_vector.current_size);
		new_vector.index = 0;
		new_vector.max = new_vector.current_size / sizeof(T);
		int second_index = 0;
		if (new_vector.data != nullptr) {
			while (new_vector.index < first.index) {
				new_vector.data[new_vector.index] = first.data[new_vector.index];
				++new_vector.index;
			}
			while (second_index < second.index) {
				new_vector.data[new_vector.index] = second.data[second_index];
				++new_vector.index;
				++second_index;
			}
		}
		return new_vector;
	}

	inline void operator=(const vector& a) {
		free_mem(this->data);		//When using operator= there is already allocatted data so when need to free it
		this->index = a.index;
		this->current_size = a.current_size;
		this->large = a.large;
		this->data = (T*)alloc_mem(a.current_size);
		this->max = a.max;
		my_memcpy(a.data, this->data, a.current_size);
	}
	vector() {
		if (sizeof(T) <= 128) {
			current_size = sizeof(T) * 100;
			data = (T*)alloc_mem(current_size);
			max = 100;
		}
		else {
			large = true;
			current_size = sizeof(T);
			data = (T*)alloc_mem(current_size);
			max = 1;
		}
	}
	vector(const vector& a) {
		this->index = a.index;
		this->current_size = a.current_size;
		this->large = a.large;
		this->data = (T*)alloc_mem(a.current_size);
		this->max = a.max;
		my_memcpy(a.data, this->data, a.current_size);
	}
	~vector() {
		free_mem(data);
	}

	inline bool add(const T& value) {
		if (max > index) {
			data[index] = value;
			++index;
			return true;
		}
		else {
			if (make_new_max()) {
				data[index] = value;
				++index;
				return true;
			}
			return false;
		}
	}
	T& operator[](int index) {
		return this->data[index];
	}
	T* begin() {
		return data;
	}
	T* end() {
		return data + index;
	}

};

class TYPE1Gb {
private:
	TYPE16b* boxes = nullptr;
	TYPE8b n_boxes = (1024ULL*1024ULL*1024ULL) / 16ULL;
public:

	/*friend ostream& operator<<(ostream& os, TYPE1Gb big_num) {
		
	}*/

	TYPE1Gb(const TYPE1Gb& other) {
		this->boxes = (TYPE16b*)alloc_mem(1024ULL * 1024ULL * 1024ULL);
		this->n_boxes = other.n_boxes;

		if (this->boxes != nullptr && other.boxes != nullptr) {
			for (TYPE8b a = 0; a < n_boxes; ++a) {
				this->boxes[a] = other.boxes[a];
			}
		}
	}
	inline TYPE1Gb& operator=(const TYPE1Gb& other) {
		if (this != &other) {
			if (this->boxes != nullptr && other.boxes != nullptr) {
				for (TYPE8b a = 0; a < n_boxes; ++a) {
					this->boxes[a] = other.boxes[a];
				}
			}
		}
		return *this;
	}
	inline friend TYPE1Gb operator+(const TYPE1Gb& first, const TYPE1Gb& second) {
		TYPE1Gb temp;
		TYPE8b plus_one = 0;				//Needed to carry the overflow
		for (TYPE8b a = 0; a < first.n_boxes; ++a) {
			TYPE8b before = first.boxes[a].high; 
			TYPE16b transporter;             // Need to transport thew plus_one
			transporter.low = plus_one;
			transporter.high = 0;
			temp.boxes[a] = first.boxes[a] + second.boxes[a] + transporter; //Adds
			if (temp.boxes[a].high < before || (plus_one == 1 && temp.boxes[a].high == before)) plus_one = 1; //Check if it overflows
			else plus_one = 0;

		}
		return temp;
	}
	TYPE1Gb() {
		boxes = (TYPE16b*) alloc_mem(1024ULL * 1024ULL * 1024ULL);
		for (TYPE8b a = 0; a < n_boxes; ++a) {
			boxes[a].low = 0;
			boxes[a].high = 0;
		}
	}
	~TYPE1Gb() {
		free_mem(boxes);
	}
	inline void operator+=(const TYPE1Gb& a) {
		*this = *this + a;
	}
	TYPE1Gb(const char* num_cstr) {
		boxes = (TYPE16b*)alloc_mem(1024ULL * 1024ULL * 1024ULL);
		for (TYPE8b a = 0; a < n_boxes; ++a) {
			boxes[a].low = 0;
			boxes[a].high = 0;
		}
		int i = 0;
		TYPE1Gb temp_num;
		while (num_cstr[i] != '\0') {
			if (num_cstr[i] >= '0' && num_cstr[i] <= '9') {
				int num = num_cstr[i] - '0';
				//Multiplies by 10
				TYPE1Gb x2 = *this;
				x2 += *this;
				TYPE1Gb x8 = x2;
				x8 += x8;
				x8 += x8;
				*this = x8 + x2;
				temp_num.boxes[0].low = num;
				*this += temp_num;
			}
			++i;
		}
	}


};
#endif