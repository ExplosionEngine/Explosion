//
// Created by LiZhen on 2021/8/24.
//

#ifndef EXPLOSION_LINKED_FREELIST_H
#define EXPLOSION_LINKED_FREELIST_H

#include <vector>
#include <Common/Templates/MemoryUtil.h>

namespace Explosion {

    template <int N, int BlockSize>
    class LinkedFreeList {
    public:
        struct LinkedNode {
            LinkedNode* next = nullptr;
        };

        static constexpr int64_t BLOCK_ALIGNMENT = 64;
        static constexpr int64_t HEAD_ALIGNMENT = Alignment(sizeof(LinkedNode), 16);
        static constexpr int NODE_SIZE = N;
        static constexpr int BLOCK_SIZE = Alignment(BlockSize + HEAD_ALIGNMENT, BLOCK_ALIGNMENT);

        static_assert(IsPowerOfTwo(N), "Block must be power of two");
        static_assert(BLOCK_SIZE > NODE_SIZE, "BLOCK_SIZE must larger than NODE_SIZE");

        LinkedFreeList() : offset(0), head(nullptr), tail(nullptr), link(&head) {}

        ~LinkedFreeList()
        {
            while (head != nullptr) {
                LinkedNode* tmp = head;
                head = tmp->next;
                free((void*)tmp);
            }
        }

        void* Pop()
        {
            if (!freeList.empty()) {
                void* res = freeList.back();
                freeList.pop_back();
                return res;
            }
            if (head == nullptr || offset + NODE_SIZE > BLOCK_SIZE) {
                AllocateNode();
                offset = HEAD_ALIGNMENT;
            }
            void* res = Data(tail, offset);
            offset += NODE_SIZE;
            return res;
        }

        void Push(void* ptr)
        {
            freeList.emplace_back(ptr);
        }

    private:
        void AllocateNode()
        {
            void* block = malloc(BLOCK_SIZE);
            memset(block, 0, BLOCK_SIZE);
            LinkedNode* ptr = reinterpret_cast<LinkedNode*>(block);
            tail = ptr;
            *link = ptr;
            link = &ptr->next;
        }

        void* Data(LinkedNode* node, uint32_t offset) {
            return (uint8_t*)node + HEAD_ALIGNMENT + offset;
        }

        uint32_t offset;
        LinkedNode* head;
        LinkedNode* tail;
        LinkedNode** link;
        std::vector<void*> freeList;
    };

}

#endif // EXPLOSION_LINKED_FREELIST_H
