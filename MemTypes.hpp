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
private:
	friend TYPE16b operator+(const TYPE16b& first, const TYPE16b& second) {
		TYPE16b temp;
		temp.high = first.high + second.high;
		temp.low = first.low + second.low;
		if (temp.low < first.low) {
			temp.high += 1;
			return temp;
		}
	}
public:
	TYPE8b high = 0;
	TYPE8b low = 0;
	
};