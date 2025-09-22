#pragma once

#include <iostream>

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

namespace ll
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
                // std::cout << "PAGE SIZE: " << PAGE_SIZE << std::endl
                //           << "ELEMENT SIZE: " << ELEMENT_SIZE << std::endl
                //           << "NUM_ELEMENTS_PER_PAGE" << NUM_ELEMENTS_PER_PAGE << std::endl;
            }

            ~vector()
            {
                for (T* pagePtr : this->pagePtrs)
                {
                    this->freePage(pagePtr);
                }
            }

            void append(T element)
            {
                size_t atPage = this->numElements / NUM_ELEMENTS_PER_PAGE;
                size_t offset = this->numElements % NUM_ELEMENTS_PER_PAGE;
                if (offset == 0)  // new page
                {
                    this->pagePtrs.push_back(this->allocPage());
                }
                this->pagePtrs[atPage][offset] = element;
                this->numElements++;
            }

            T* get(size_t index)
            {
                if (index >= this->numElements)
                {
                    return nullptr;
                }
            }

            bool remove(size_t index)
            {
                if (index < 0 || index >= this->numElements)
                {
                    return false;
                }
                size_t atPage = index / NUM_ELEMENTS_PER_PAGE;
                size_t offset = index % NUM_ELEMENTS_PER_PAGE;
                size_t x_atPage = (this->numElements - 1) / NUM_ELEMENTS_PER_PAGE;
                size_t x_offset = (this->numElements - 1) % NUM_ELEMENTS_PER_PAGE;
                this->pagePtrs[atPage][offset] = this->pagePtrs[x_atPage][x_offset];
                this->numElements--;
                return true;
            }

            size_t find(T element)
            {
                for (int i = 0; i < this->pagePtrs.size(); i++)
                {
                    for (int j = 0; j < i * NUM_ELEMENTS_PER_PAGE + j; j++)
                    {
                        if (this->pagePtrs[i][j] == element)
                        {
                            return i * NUM_ELEMENTS_PER_PAGE + j;
                        }
                    }
                }
                return -1;
            }
    };
}

