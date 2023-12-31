#include <cstddef>
#include <cstdint>

namespace My {
    struct  BlockHeader
    {
        BlockHeader* pNext;
    };

    struct PageHeader
    {
        PageHeader* pNext;
        BlockHeader* Blocks(){
            return reinterpret_cast<BlockHeader*>(this +1);
        }
    };

    class Allocator
    {
    public:
        static const uint8_t PATTERN_ALIGN = 0xFC;
        static const uint8_t PATTERN_ALLOC = 0xFD;
        static const uint8_t PATTERN_FREE  = 0xFE;
        
        Allocator();
        Allocator(size_t data_size, size_t page_size, size_t alignment);
        ~Allocator();

        //reset allocator to a new configuration
        void Reset(size_t data_size, size_t page_size, size_t alignment);

        //API alloc and free
        void* Allocate();
        void Free(void* p);
        void FreeAll();

    private:
#if defined(_DEBUG)
        void FillFreePage(PageHeader* pPage);
        void FillFreeBlock(BlockHeader* pBlock);
        void FillAllocatedBlock(BlockHeader* pBlock);
#endif
        // get next block
        BlockHeader* NextBlock(BlockHeader* pBlock);
        // page list
        PageHeader* m_pPageList;
        // Free block list
        BlockHeader* m_pFreeList;

        size_t m_szDataSize;
        size_t m_szPageSize;
        size_t m_szAlignmentSize;
        size_t m_szBlockSize;
        uint32_t m_nBlocksPerPage;

        //statistics 
        uint32_t m_nPages;
        uint32_t m_nBlocks;
        uint32_t m_nFreeBlocks;

        //disable copy&assignment
        Allocator(const Allocator& clone);
        Allocator &operator=(const Allocator& rhs);
    };
}