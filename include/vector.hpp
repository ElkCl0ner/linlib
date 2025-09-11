#pragma once

#include <iostream>

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

namespace lin
{
    inline const size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);

    template <typename T>
    class vector
    {
        private:
            static constexpr size_t ELEMENT_SIZE = sizeof(T);
            inline static size_t NUM_ELEMENTS_PER_PAGE = PAGE_SIZE / ELEMENT_SIZE;

            size_t numElements = 0;
            std::vector<T*> pagePtrs;

            T* allocPage()
            {
                void* pagePtr = mmap(nullptr, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                if (pagePtr == MAP_FAILED)
                {
                    return nullptr;
                }
                return (T*)pagePtr;
            }

            void freePage(T* pagePtr)
            {
                munmap(pagePtr, PAGE_SIZE);
            }

        public:
            vector()
            {
                this->pagePtrs.push_back(this->allocPage());
                std::cout << "PAGE SIZE: " << PAGE_SIZE << std::endl
                          << "ELEMENT SIZE: " << ELEMENT_SIZE << std::endl
                          << "NUM_ELEMENTS_PER_PAGE" << NUM_ELEMENTS_PER_PAGE << std::endl;
            }

            ~vector()
            {
                for (T* pagePtr : this->pagePtrs)
                {
                    this->freePage(pagePtr);
                }
            }

            // void append(T element)
            // {
            //     
            // }
            //
            // T get(size_t index)
            // {}
            //
            // T pop(size_t index)
            // {}
            //
            // size_t find(T element)
            // {}
    };
}

