#ifndef _DYNAMIC_ALLOC
#define _DYNAMIC_ALLOC

#include "lib.h"

#define NUM_PAGE_DESC 32768
#define DYNAMIC_START 0x02000000
#define DYNAMIC_END 0x08000000
#define NUM_FREE_ELEMENT 11
#define PAGE_DESC_SIZE 16
#define __GFP_WAIT 0x1
#define __GFP_NOWARN 0x2
#define __GFP_NORETRY 0x4
#define __GFP_ZERO 0x8
#define NUM_GENERAL_CACHE 17
#define FIRST_TEN 10
#define G_ASCII 0x67
#define ZEOR_ASCII 0x30
#define SLAB_DESC_SIZE 22
#define NUM_PER_SLAB0 407
#define UP_LIMIT 3

/* lru structure.*/
typedef struct lru
{
    void* prev;
    void* next;
} lru_t;

/* Flag structure*/
typedef struct flag
{
    struct{
        uint32_t PG_private : 1;
        uint32_t PG_reserved : 1;
        uint32_t PG_slab : 1;
        uint32_t count : 29;
    }__attribute__((packed));
} flag_t;

/* Page descriptor structure.*/
typedef struct page_desc
{
    flag_t flags;
    uint32_t _private;
    lru_t lru;
} page_desc_t;

/* Free area structure.*/
typedef struct free_element
{
    page_desc_t* free_pt;
    uint32_t nr_free;
} free_element_t;

/* The slab descriptor structure.*/
typedef struct slab_desc
{
    struct slab_desc* prev;
    struct slab_desc* next;
    uint32_t coloroff;
    void* s_mem;
    uint32_t inuse;
    int16_t free;
} slab_desc_t;

/* The slab list structure.*/
typedef struct kmem_list3
{
    slab_desc_t* slab_full;
    slab_desc_t* slab_partial;
    slab_desc_t* slab_free;
    uint32_t free_objects;
} kmem_list3_t;

/* Cache descriptor structure.*/
typedef struct cache_desc
{
    kmem_list3_t lists;
    uint32_t object_size;
    uint32_t num_per_slab;
    uint32_t gfp_order;
    uint32_t color;
    uint32_t align;
    uint32_t color_next;
    struct cache_desc* slabp_cache;
    uint32_t slab_size;
    int8_t name[10];
} cache_desc_t;

extern page_desc_t mem_map[NUM_PAGE_DESC];
extern free_element_t free_area[NUM_FREE_ELEMENT];
extern cache_desc_t general_caches[NUM_GENERAL_CACHE];

int32_t mem_map_init();
int32_t free_area_init();
page_desc_t* __rmqueue(uint32_t order);
int32_t __free_pages_bulk(uint32_t order, page_desc_t* page);
int32_t split_block(uint32_t order, uint32_t current_order, page_desc_t* pointer);
uint32_t power(uint32_t base, uint32_t order);
int32_t add_to_list(uint32_t order, page_desc_t* pointer);
int32_t buddy_merge(page_desc_t* pointer);
int32_t page_is_buddy(page_desc_t* page, uint32_t order);
int32_t remove_from_list(lru_t* lru, uint32_t order);
void* alloc_pages(uint32_t gfp_mask, uint32_t order);
int32_t free_pages(void* addr, uint32_t order);
int32_t kmem_cache_init();
void* kmem_getpages(cache_desc_t* cachep, uint32_t flags);
void kmem_freepages(cache_desc_t* cachep, void* addr);
int32_t cache_grow(cache_desc_t* cachep, uint32_t flags);
void set_slab_desc(cache_desc_t* cachep, void* addr);
void list_add_tail(cache_desc_t* cachep, void* addr);
void list_remove_slab(cache_desc_t* cachep, void* addr, int8_t type);
void slab_destroy(cache_desc_t* cachep, slab_desc_t* slabp);
void* kmem_alloc_obj(cache_desc_t* cachep, uint32_t flags);
void list_add_head(cache_desc_t* cachep, void* addr, int8_t type);
void kmem_free_obj(cache_desc_t* cachep, void* objp);
void* kmalloc(uint32_t size, uint32_t flags);
void kfree(const void* objp);

#endif

