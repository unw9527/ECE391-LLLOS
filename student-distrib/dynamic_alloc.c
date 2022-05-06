// Please refer to fsdir/reference.txt for the reference

#include "dynamic_alloc.h"
#include "page.h"

page_desc_t mem_map[NUM_PAGE_DESC];
free_element_t free_area[NUM_FREE_ELEMENT];
cache_desc_t general_caches[NUM_GENERAL_CACHE];

int32_t mem_map_init()
{
    int i;
    for (i = 0; i < NUM_PAGE_DESC; i++){
        /* Step1. Clear the mem_map.*/
        mem_map[i].flags.count = 0;
        mem_map[i].flags.PG_private = 0;
        mem_map[i].flags.PG_reserved = 0;
        mem_map[i].flags.PG_slab = 0;
        mem_map[i].lru.next = 0;
        mem_map[i].lru.prev = 0;
        mem_map[i]._private = 0;
        /* Step2. Check for 0~4 MB and 8~32 MB.*/
        if (i < ((KERNEL_END / 2) >> SR) || ((i >= (KERNEL_END >> SR)) && (i < (DYNAMIC_START >> SR)))){
            mem_map[i].flags.PG_reserved = 1;
        }
        /* Step3. Check for 4~8 MB.*/
        else if (i < (KERNEL_END >> SR)){
            mem_map[i].flags.count = 1;
            mem_map[i].flags.PG_reserved = 1;
        }
        /* Step4. Check for video memory.*/
        if (i == VIDEO >> SR){
            mem_map[i].flags.count = 1;
            mem_map[i].flags.PG_reserved = 1;
        }
    }
    /* Step5. Set up the lru field and _private field for 32~128 MB.*/
    for (i = 0; i < REST_PAGE_TABLE; i++){
        mem_map[BASE_OFFSET_PAGE_TABLE + i * NUM_PAGE_ENTRY].flags.PG_private = 1;
        mem_map[BASE_OFFSET_PAGE_TABLE + i * NUM_PAGE_ENTRY]._private = NUM_FREE_ELEMENT - 1;
        mem_map[BASE_OFFSET_PAGE_TABLE + i * NUM_PAGE_ENTRY].lru.prev = &mem_map[BASE_OFFSET_PAGE_TABLE + (i-1) * NUM_PAGE_ENTRY];
        mem_map[BASE_OFFSET_PAGE_TABLE + i * NUM_PAGE_ENTRY].lru.next = &mem_map[BASE_OFFSET_PAGE_TABLE + (i+1) * NUM_PAGE_ENTRY];
    }
    mem_map[BASE_OFFSET_PAGE_TABLE].lru.prev = 0;
    mem_map[BASE_OFFSET_PAGE_TABLE + (REST_PAGE_TABLE-1) * NUM_PAGE_ENTRY].lru.next = 0;
    /* Step6. Set up the free_area array.*/
    free_area_init();
    return 0;
}

int32_t free_area_init()
{
    int i;
    /* Note: In our design, the case for 0 need to be seriously considered.*/
    for (i = 0; i < NUM_FREE_ELEMENT-1; i++){
        free_area[i].free_pt = 0;
        free_area[i].nr_free = 0;
    }
    free_area[NUM_FREE_ELEMENT-1].nr_free = REST_PAGE_TABLE;
    free_area[NUM_FREE_ELEMENT-1].free_pt = &mem_map[BASE_OFFSET_PAGE_TABLE];
    return 0;
}

page_desc_t* __rmqueue(uint32_t order)
{
    uint32_t current_order;
    page_desc_t* pt;
    /* Find the correct order for the free list.*/
    for (current_order = order; current_order < NUM_FREE_ELEMENT; current_order++){
        if (free_area[current_order].nr_free != 0)
            break;
    }
    /* Failure case.*/
    if (current_order == NUM_FREE_ELEMENT)
        return 0;
    /* Successful case.*/
    /* Check if nr_free is 1.*/
    if (free_area[current_order].nr_free == 1){
        free_area[current_order].nr_free --;
        pt = free_area[current_order].free_pt;
        pt->flags.PG_private = 0;
        free_area[current_order].free_pt = 0;
    }
    /* nr_free >= 2.*/
    else{
        free_area[current_order].nr_free --;
        pt = free_area[current_order].free_pt;
        pt->flags.PG_private = 0;
        ((page_desc_t*)pt->lru.next)->lru.prev = 0;
        free_area[current_order].free_pt = (page_desc_t*)pt->lru.next;
    }
    /* Now split the block if needed.*/
    split_block(order, current_order, pt);
    return pt;
}

int32_t split_block(uint32_t order, uint32_t current_order, page_desc_t* pointer)
{
    uint32_t assign_space;
    /* Check whether do not need split.*/
    if (order == current_order)
        return 0;
    while (current_order != order){
        current_order --;
        /* Update the new position.*/
        assign_space = power(2, current_order);
        pointer[assign_space].flags.PG_private = 1;
        pointer[assign_space]._private = current_order;
        /* Add to the list.*/
        add_to_list(current_order, pointer + assign_space);
    }
    return 1;
}

uint32_t power(uint32_t base, uint32_t order)
{
    int i;
    uint32_t val = 1;
    for (i = 0; i < order; i++)
        val *= base;
    return val;
}

int32_t add_to_list(uint32_t order, page_desc_t* pointer)
{
    /* The case for nr_free = 0.*/
    if (free_area[order].nr_free == 0){
        free_area[order].nr_free ++;
        free_area[order].free_pt = pointer;
        pointer->lru.next = 0;
        pointer->lru.prev = 0;
    }
    /* The case for nr_free != 0.*/
    else{
        free_area[order].nr_free ++;
        pointer->lru.next = free_area[order].free_pt;
        pointer->lru.prev = 0;
        free_area[order].free_pt->lru.prev = pointer;
        free_area[order].free_pt = pointer;
    }
    return 0;
}

int32_t __free_pages_bulk(uint32_t order, page_desc_t* page)
{
    uint32_t page_idx;
    uint32_t buddy_idx;
    page_desc_t* buddy;
    page_idx = (uint32_t)(page - mem_map);
    /* First change the current page descriptor to free.*/
    page->_private = 0;
    while (order < NUM_FREE_ELEMENT-1){
        /* Get the buddy.*/
        buddy_idx = page_idx ^ (1 << order);
        buddy = mem_map + buddy_idx;
        /* Check if it is really a buddy.*/
        if (page_is_buddy(buddy, order) == 0)
            break;
        /* Remove the buddy from the free list.*/
        remove_from_list(&(buddy->lru), order);
        /* No longer need private field.*/
        buddy->flags.PG_private = 0;
        buddy->_private = 0;
        /* Update the new index value.*/
        page_idx &= buddy_idx;
        order++;
    }
    /* Now add the new page_desc into the corresponding list.*/
    mem_map[page_idx].flags.PG_private = 1;
    mem_map[page_idx]._private = order;
    add_to_list(order, mem_map + page_idx);
    return 0;
}

int32_t page_is_buddy(page_desc_t* page, uint32_t order)
{
    if (page->flags.PG_reserved == 0 && page->_private == order && page->flags.PG_private == 1 && page->flags.count == 0)
        return 1;
    return 0;
}

int32_t remove_from_list(lru_t* lru, uint32_t order)
{
    page_desc_t* prev = lru->prev;
    page_desc_t* next = lru->next;
    /* Case1. Head but not tail.*/
    if (prev == 0 && next != 0){
        free_area[order].free_pt = next;
        free_area[order].nr_free --;
        next->lru.prev = 0;
        return 0;
    }
    /* Case2. Tail but not head.*/
    else if (prev != 0 && next == 0){
        prev->lru.next = 0;
        free_area[order].nr_free --;
        return 1;
    }
    /* Case3. Head and tail.*/
    else if (prev == 0 && next == 0){
        free_area[order].nr_free --;
        free_area[order].free_pt = 0;
        return 2;
    }
    /* Case4. Neither head or tail.*/
    else{
        free_area[order].nr_free --;
        prev->lru.next = next;
        next->lru.prev = prev;
        return 3;
    }
}

void* alloc_pages(uint32_t gfp_mask, uint32_t order)
{
    volatile page_desc_t* pointer = 0;
    uint32_t page_idx;
    void* fill_zero_pt;
    /* If order is not in the correct range, return a null pointer.*/
    if (order >= NUM_FREE_ELEMENT)
        return 0;
    /* Check __GFP__NORETRY.*/
    if (gfp_mask & __GFP_NORETRY){
        pointer = __rmqueue(order);
        /* Check whether fail.*/
        if (!pointer){
            /* Check  __GFP_NOWARN*/
            if (gfp_mask & __GFP_NOWARN)
                return 0;
            printf("Out of Memory!\n");
            return 0;
        }
    }
    /* Check GFP_WAIT.*/
    else if (gfp_mask & __GFP_WAIT){
        while (pointer == 0)
            pointer = __rmqueue(order);
    }
    /* Check __GFP_ZERO*/
    if (gfp_mask &  __GFP_ZERO){
        page_idx = (uint32_t) (pointer - mem_map);
        /* Get the start address to fill 0.*/
        fill_zero_pt = (void*) (page_idx * BOUNDARY);
        memset(fill_zero_pt, 0, power(2, order)*BOUNDARY);
    }
    /* Update some other parameters of the page descriptor.*/
    pointer->flags.count ++;
    pointer->_private = order;
    pointer->lru.next = 0;
    pointer->lru.prev = 0;
    return (void*)pointer;
}

int32_t free_pages(void* addr, uint32_t order)
{
    uint32_t page_idx;
    page_desc_t* pointer;
    /* Check for the valid order.*/
    if (order >= NUM_FREE_ELEMENT-1)
        return -1;
    page_idx = (uint32_t)addr / BOUNDARY;
    pointer = mem_map + page_idx;
    /* Check for dynamic areas.*/
    if (pointer->flags.PG_reserved == 1){
        printf("Cannot Free Reserved Area!\n");
        return -1;
    }
    /* Check for free.*/
    else if(pointer->flags.count == 0){
        printf("Cannot Free Freed Pages.\n");
        return -1;
    }
    /* Note: In my design, _private field remains even if allocated.*/
    else if (pointer->_private != order){
        printf("Free Failed: Order not Match!\n");
        return -1;
    }
    /* Now free the pages.*/
    else{
        __free_pages_bulk(order, pointer);
        pointer->flags.count --;
        pointer->flags.PG_slab = 0;
        return 0;
    }
}

int32_t kmem_cache_init()
{
    /* Note: In our design, we use internal descriptors.*/
    int32_t i;
    for (i = 0; i < NUM_GENERAL_CACHE; i++){
        /* For the first seven, we use one page for them.*/
        if (i < FIRST_TEN){
            general_caches[i].lists.slab_full = 0;
            general_caches[i].lists.slab_partial = 0;
            general_caches[i].lists.slab_free = 0;
            general_caches[i].lists.free_objects = 0;
            general_caches[i].object_size = power(2, i);
            general_caches[i].num_per_slab = (BOUNDARY - SLAB_DESC_SIZE) / (general_caches[i].object_size + 2);
            general_caches[i].gfp_order = 0;
            if (i == 0)
                general_caches[i].align = 1;
            else if (i == 1)
                general_caches[i].align = 2;
            else
                general_caches[i].align = 4;
            general_caches[i].color = ((BOUNDARY - SLAB_DESC_SIZE) % (general_caches[i].object_size + 2)) / general_caches[i].align + 1;
            general_caches[i].color_next = 0;
            general_caches[i].slabp_cache = 0;
            general_caches[i].slab_size = BOUNDARY;
            general_caches[i].name[0] = G_ASCII;
            general_caches[i].name[1] = i + ZEOR_ASCII;
        }
        /* For the next few, we double for each iteration of i.*/
        else{
            general_caches[i].lists.slab_full = 0;
            general_caches[i].lists.slab_partial = 0;
            general_caches[i].lists.slab_free = 0;
            general_caches[i].lists.free_objects = 0;
            general_caches[i].object_size = power(2, i);
            general_caches[i].gfp_order = i - FIRST_TEN + 1;
            general_caches[i].num_per_slab = (BOUNDARY * power(2, general_caches[i].gfp_order) - SLAB_DESC_SIZE) / (general_caches[i].object_size + 2);
            general_caches[i].align = 4;
            general_caches[i].color = ((BOUNDARY * power(2, general_caches[i].gfp_order) - SLAB_DESC_SIZE) % (general_caches[i].object_size + 2)) / general_caches[i].align + 1;
            general_caches[i].color_next = 0;
            general_caches[i].slabp_cache = 0;
            general_caches[i].slab_size = BOUNDARY;
            general_caches[i].name[0] = G_ASCII;
            /* Note: for i >= 10, the value of name[1] is no longer number.*/
            general_caches[i].name[1] = i + ZEOR_ASCII;
        }
    }
    return 0;
}

void* kmem_getpages(cache_desc_t* cachep, uint32_t flags)
{
    page_desc_t* page;
    page = alloc_pages(flags, cachep->gfp_order);
    /* Check whether fail to allocate a memory.*/
    if (!page)
        return 0;
    return (void*)((page - mem_map) * BOUNDARY);
}

void kmem_freepages(cache_desc_t* cachep, void* addr)
{
    /* Clear the slabs.*/
    free_pages(addr, cachep->gfp_order);
    return;
}

int32_t cache_grow(cache_desc_t* cachep, uint32_t flags)
{
    void* addr = kmem_getpages(cachep, flags);
    int32_t i;
    int16_t* obj_desc_pt;
    page_desc_t* page;
    /* Check for NULL.*/
    if (addr == 0)
        return -1;
    /* Set up the slab descriptor.*/
    set_slab_desc(cachep, addr);
    /* Set up the object descriptors*/
    obj_desc_pt = (int16_t*)((uint32_t)addr + SLAB_DESC_SIZE);
    for (i = 0; i < cachep->num_per_slab-1; i++){
        /* Except for the last object descriptor, we initailize it pointing to the adjacence.*/
        obj_desc_pt[i] = i + 1;
    }
    obj_desc_pt[cachep->num_per_slab-1] = -1;
    page = (uint32_t) addr / BOUNDARY + mem_map;
    /* Set up the lru field of the page descriptor.*/
    for (i = 0; i < power(2, cachep->gfp_order); i++){
        (page+i)->lru.next = cachep;
        (page+i)->lru.prev = addr;
    }
    /* Set the PG_SLAB flag*/
    page->flags.PG_slab = 1;
    /* Add the descriptor to the doubly linked list.*/
    list_add_tail(cachep, addr);
    return 0;
}

void set_slab_desc(cache_desc_t* cachep, void* addr)
{
    slab_desc_t* desc_addr = (slab_desc_t*) addr;
    /* Set the slab descriptor.*/
    desc_addr->next = 0;
    desc_addr->coloroff = cachep->color_next * cachep->align + SLAB_DESC_SIZE + cachep->num_per_slab * 2;
    desc_addr->s_mem = (void*) (desc_addr->coloroff + (uint32_t) addr);
    desc_addr->inuse = 0;
    desc_addr->free = 0;
    /* Update the cache descriptor.*/
    cachep->lists.free_objects += cachep->num_per_slab;
    cachep->color_next = (cachep->color_next+1) % cachep->color;
    return;
}

void list_add_tail(cache_desc_t* cachep, void* addr)
{
    slab_desc_t* desc_addr = (slab_desc_t*) addr;
    slab_desc_t* temp;
    /* Check for empty list.*/
    if (cachep->lists.slab_free == 0){
        cachep->lists.slab_free = desc_addr;
        desc_addr->prev = 0;
        return;
    }
    else{
        temp = cachep->lists.slab_free;
        /* Find the last element.*/
        while (temp->next != 0)
            temp = temp->next;
        /* Add the descriptor to the end of the list.*/
        temp->next = desc_addr;
        desc_addr->prev = temp;
        return;
    }
}

void slab_destroy(cache_desc_t* cachep, slab_desc_t* slabp)
{
    /* Remove the slab descriptor from the list.*/
    list_remove_slab(cachep, slabp, 1);
    kmem_freepages(cachep, slabp);
    /* Update the cache descriptor*/
    cachep->lists.free_objects -= cachep->num_per_slab;
    return;
}

void* kmem_alloc_obj(cache_desc_t* cachep, uint32_t flags)
{
    slab_desc_t* avail_slab;
    void* addr;
    int16_t* obj_desc_pt;
    /* Case1: The number of free objects reaches 0.*/
    if (cachep->lists.free_objects == 0){
        /* Call cache_grow and check for NULL*/
        if (cache_grow(cachep, flags) == -1)
            return 0;
        /* Only call this function once, not large recursive call.*/
        addr = kmem_alloc_obj(cachep, flags);
        return addr;
    }
    /* Case2. The number of free objects is not 0.*/
    else{
        /* Case2.1. There is still free objects in partial.*/
        if (cachep->lists.slab_partial != 0){
            avail_slab = cachep->lists.slab_partial;
            /* Load the address of the object to addr.*/
            addr = (void*)((uint32_t)avail_slab->s_mem + cachep->object_size * avail_slab->free);
            /* Update the number of free objects.*/
            cachep->lists.free_objects --;
            /* Update the inuse field.*/
            avail_slab->inuse++;
            /* Update the free index.*/
            obj_desc_pt = (int16_t*)((uint32_t)avail_slab + SLAB_DESC_SIZE);
            avail_slab->free = obj_desc_pt[avail_slab->free];
            /* Check if the slab is full.*/
            if (avail_slab->free == -1){
                list_remove_slab(cachep, avail_slab, 0);
                list_add_head(cachep, avail_slab, 0);
            }
            return addr;
        }
        /* Case2.2. There is no free objects in slab_partial.*/
        else{
            avail_slab = cachep->lists.slab_free;
            /* Load the address of the object to addr.*/
            addr = (void*)((uint32_t)avail_slab->s_mem + cachep->object_size * avail_slab->free);
            /* Update the number of free objects.*/
            cachep->lists.free_objects --;
            /* Update the inuse field.*/
            avail_slab->inuse++;
            /* Update the free index.*/
            obj_desc_pt = (int16_t*)((uint32_t)avail_slab + SLAB_DESC_SIZE);
            avail_slab->free = obj_desc_pt[avail_slab->free];
            /* Move the free slab descriptor to partail list.*/
            list_remove_slab(cachep, avail_slab, 1);
            list_add_head(cachep, avail_slab, 1);
            return addr;
        }
    }
}

void list_add_head(cache_desc_t* cachep, void* addr, int8_t type)
{
    slab_desc_t* desc_addr = (slab_desc_t*) addr;
    /* Switch for different types.*/
    switch(type){
        /* Case 0: To full.*/
        case 0:
            /* The case for empty list.*/
            if (cachep->lists.slab_full == 0){
                desc_addr->prev = 0;
                desc_addr->next = 0;
                cachep->lists.slab_full = desc_addr;
            }
            /* The case for non-empty list.*/
            else{
                cachep->lists.slab_full->prev = desc_addr;
                desc_addr->next = cachep->lists.slab_full;
                desc_addr->prev = 0;
                cachep->lists.slab_full = desc_addr;
            }
            break;
        /* Case 1: To partial.*/
        case 1:
            /* The case for empty list.*/
            if (cachep->lists.slab_partial == 0){
                desc_addr->prev = 0;
                desc_addr->next = 0;
                cachep->lists.slab_partial = desc_addr;
            }
            /* The case for non-empty list.*/
            else{
                cachep->lists.slab_partial->prev = desc_addr;
                desc_addr->next = cachep->lists.slab_partial;
                desc_addr->prev = 0;
                cachep->lists.slab_partial = desc_addr;
            }
            break;
        /* Case 2: To free.*/
        case 2:
            /* The case for empty list.*/
            if (cachep->lists.slab_free == 0){
                desc_addr->prev = 0;
                desc_addr->next = 0;
                cachep->lists.slab_free = desc_addr;
            }
            /* The case for non-empty list.*/
            else{
                cachep->lists.slab_free->prev = desc_addr;
                desc_addr->next = cachep->lists.slab_free;
                desc_addr->prev = 0;
                cachep->lists.slab_free = desc_addr;
            }
            break;
    }
    return;
}

void list_remove_slab(cache_desc_t* cachep, void* addr, int8_t type)
{
    slab_desc_t* desc_addr = (slab_desc_t*)addr;
    /* Head but not tail.*/
    if (desc_addr->prev == 0 && desc_addr->next != 0){
        desc_addr->next->prev = 0;
        switch(type){
            /* Case 0: partial list.*/
            case 0:
                cachep->lists.slab_partial = desc_addr->next;
                break;
            /* Case 1: free list.*/
            case 1:
                cachep->lists.slab_free = desc_addr->next;
                break;
            /* Case 2: full list.*/
            case 2:
                cachep->lists.slab_full = desc_addr->next;
                break;
        }
    }
    /* Tail but not head.*/
    else if(desc_addr->prev != 0 && desc_addr->next == 0)
        desc_addr->prev->next = 0;
    /* Head and Tail.*/
    else if (desc_addr->prev == 0 && desc_addr->next == 0){
        switch(type){
            case 0:
                cachep->lists.slab_partial = 0;
                break;
            case 1:
                cachep->lists.slab_free = 0;
                break;
            case 2:
                cachep->lists.slab_full = 0;
                break;
        }
    }
    /* Neither head or tail.*/
    else{
        desc_addr->prev->next = desc_addr->next;
        desc_addr->next->prev = desc_addr->prev;
    }
    return;
}

void kmem_free_obj(cache_desc_t* cachep, void* objp)
{
    slab_desc_t* slabp = (slab_desc_t*)(((uint32_t)objp / BOUNDARY + mem_map)->lru.prev);
    int16_t* obj_desc_pt;
    int16_t free;
    int16_t index;
    /* Update the number of free objects.*/
    cachep->lists.free_objects ++;
    /* Update the inuse field.*/
    slabp->inuse --;
    /* Update the free index.*/
    free = slabp->free;
    index = (int16_t)(((uint32_t)objp - (uint32_t)slabp->s_mem) / cachep->object_size);
    obj_desc_pt = (int16_t*)((uint32_t)slabp + SLAB_DESC_SIZE);
    slabp->free = index;
    obj_desc_pt[index] = free;
    /* Check whether we need to move the slab descriptor.*/
    /* Move from full to partial.*/
    if (free == -1){
        list_remove_slab(cachep, slabp, 2);
        list_add_head(cachep, slabp, 1);
    }
    /* Move from partial to free.*/
    if (slabp->inuse == 0){
        list_remove_slab(cachep, slabp, 0);
        list_add_head(cachep, slabp, 2);
    }
    /* Check the case that free objects are too many.*/
    if (cachep->lists.free_objects >= UP_LIMIT * cachep->num_per_slab && cachep->lists.slab_free != 0)
        slab_destroy(cachep, cachep->lists.slab_free);
    return;
}

void* kmalloc(uint32_t size, uint32_t flags)
{
    int32_t i;
    void* addr;
    /* Find the appropriate size for the required size.*/
    for (i = 0; i < NUM_GENERAL_CACHE; i++){
        if (power(2, i) >= size)
            break;
    }
    /* Check for validity.*/
    if (i == NUM_GENERAL_CACHE){
        return 0;
    }
    else{
        addr = kmem_alloc_obj(general_caches+i, flags);
        return addr;
    }
}

void kfree(const void* objp)
{
    /* Check for NULL.*/
    cache_desc_t* cachep = (cache_desc_t*)(((uint32_t)objp / BOUNDARY + mem_map)->lru.next);
    if (!objp)
        return;
    /* Note: Currently, we cannot kfree the same page.*/
    kmem_free_obj(cachep, (void*)objp);
    return;
}
