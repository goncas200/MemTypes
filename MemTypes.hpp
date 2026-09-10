#pragma once
#define TYPE8b long long
#define sizetype unsigned long long
#define MEMTYPES_MAGICNUM 0x4D454D5459504553ULL

#ifdef _WIN32
#include <Windows.h>
__forceinline void* alloc_mem(const sizetype buffer) {
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

__forceinline void free_mem(void* ptr) {
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


class TYPE16b {
public:
	friend TYPE16b operator+(const TYPE16b& first, const TYPE16b& second) {
		TYPE16b temp;
		temp.high = first.high + second.high;
		temp.low = first.low + second.low;
		if (temp.low < first.low) {
			temp.high += 1;
		}
		return temp;
	}
	friend TYPE16b operator-(const TYPE16b& first, const TYPE16b& second) {
		TYPE16b temp;
		temp.high = first.high - second.high;
		temp.low = first.low - second.low;
		if (temp.low > first.low) {
			temp.high -= 1;
		}
		return temp;
	}
	friend bool operator<(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return false;
		if (first.high < second.high) return true;
		if (first.high > second.high) return false;
		if (first.low < second.low) return true;
		if (first.low > second.low) return false;
		return false;
	}
	friend bool operator>(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return false;
		if (first.high > second.high) return true;
		if (first.high < second.high) return false;
		if (first.low > second.low) return true;
		if (first.low < second.low) return false;
		return false;
	}
	friend bool operator<=(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return true;
		if (first.high < second.high) return true;
		if (first.high > second.high) return false;
		if (first.low < second.low) return true;
		if (first.low > second.low) return false;
		return false;
	}
	friend bool operator>=(const TYPE16b& first, const TYPE16b& second) {
		if (first.high == second.high && first.low == second.low) return true;
		if (first.high > second.high) return true;
		if (first.high < second.high) return false;
		if (first.low > second.low) return true;
		if (first.low < second.low) return false;
		return false;
	}

	TYPE8b high = 0;
	TYPE8b low = 0;
	
	void operator+=(const TYPE16b& first){
		*this = *this + first;
	}
	TYPE16b(TYPE8b num) {
		this->low = num;
	}
	TYPE16b(){}
	void operator+=(const TYPE8b& first) {
		TYPE8b before = this->low;
		this->low += first;
		if (this->low < before) {
			this->high += 1;
		}
	}
	void operator-=(const TYPE8b& first) {
		TYPE8b before = this->low;
		this->low -= first;
		if (this->low > before) {
			this->high -= 1;
		}
	}

	
};

class TYPE1Gb {
private:
	TYPE16b* boxes = nullptr;
	TYPE8b n_boxes = (1024ULL*1024ULL*1024ULL) / 16ULL;
public:
	TYPE1Gb(const TYPE1Gb& other) {
		this->boxes = (TYPE16b*)alloc_mem(1024ULL * 1024ULL * 1024ULL);
		this->n_boxes = other.n_boxes;

		if (this->boxes != nullptr && other.boxes != nullptr) {
			for (TYPE8b a = 0; a < n_boxes; ++a) {
				this->boxes[a] = other.boxes[a];
			}
		}
	}
	TYPE1Gb& operator=(const TYPE1Gb& other) {
		if (this != &other) {
			if (this->boxes != nullptr && other.boxes != nullptr) {
				for (TYPE8b a = 0; a < n_boxes; ++a) {
					this->boxes[a] = other.boxes[a];
				}
			}
		}
		return *this;
	}
	friend TYPE1Gb operator+(const TYPE1Gb& first, const TYPE1Gb& second) {
		TYPE1Gb temp;
		TYPE8b plus_one = 0; //Like math (contas em pe)
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
	void operator+=(const TYPE1Gb& a) {
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