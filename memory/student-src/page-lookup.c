#include "swapfile.h"
#include "statistics.h"
#include "pagetable.h"

/*******************************************************************************
 * Looks up an address in the current page table. If the entry for the given
 * page is not valid, traps to the OS.
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t pagetable_lookup(vpn_t vpn, int write) {
   pfn_t pfn = 0;

//    /* FIX ME - Part 2
//     * Determine the PFN corresponding to the passed in VPN.
//     * current_pagetable variable accesses the process page table.
//     * if the pagetable entry is not valid:
// 	*   increment count_pagefaults
// 	*   call pagefault_handler() (partially implemented already,
// 	*        found in file page-fault.c, returns frame #) to get pfn
// 	*   change pagetable entry to now be valid
//     */
// // pte_t pte = current_pagetable[vpn];
// // pfn = pte->pfn;
    pfn = current_pagetable->pfn;
    if (current_pagetable[vpn]->valid != 1) {
      count_pagefaults++;
      pfn = pagefault_handler();
    }
    current_pagetable[vpn]->valid = 1;
    return pfn;
}
