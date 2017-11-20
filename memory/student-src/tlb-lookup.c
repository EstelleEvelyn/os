#include <stdlib.h>
#include <stdio.h>
#include "tlb.h"
#include "pagetable.h"
#include "global.h" /* for tlb_size */
#include "statistics.h"

/*******************************************************************************
 * Looks up an address in the TLB. If no entry is found, calls pagetable_lookup()
 * to get the entry from the page table instead
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t tlb_lookup(vpn_t vpn, int write) {
	/* currently just skips tlb and goes to pagetable */
   pfn_t pfn;
   pfn = pagetable_lookup(vpn, write);

   /*
    * FIX ME : Step 5
    * Note that tlb is an array with memory already allocated and initialized to 0/null
    * meaning that you don't need special cases for a not-full tlb, the valid field
    * will be 0 for both invalid and empty tlb entries, so you can just check that!
    */

   /*
    * Search the TLB - hit if find valid entry with given VPN
    * Increment count_tlbhits on hit.
    */
    int i;
    for(i = 0; i < tlb_size; i++) {
      if(tlb[i].valid != 0){
        count_tlbhits++;
        tlb[i].used++;
        return tlb[i].pfn;
      }
    }
   /*
    * If it was a miss, call the page table lookup to get the pfn
    * Add current page as TLB entry. Replace any invalid entry first,
    * then do a clock-sweep to find a victim (entry to be replaced).
    */
    pfn = pagetable_lookup(vpn, write);
    for(i = 0; i < tlb_size; i++) {
      if(tlb[i].valid == 0){
        tlb[i].pfn = pfn;
        tlb[i].vpn = vpn;
        tlb[i].valid = 0;
        if(write == 1) {
          tlb[i].dirty = 1;
        } else {
          tlb[i].dirty = 0;
        }
        tlb[i].used = 0;
        return pfn;
      }
    }

    int used_bit = tlb[i % tlb_size].used;
    while (!(used_bit == 0)) {
      tlb[i % tlb_size].used--;
      used_bit = tlb[i % tlb_size].used;
      i++;
    }
    tlb[(i-1) % tlb_size].pfn = pfn;
    tlb[(i-1) % tlb_size].vpn = vpn;
    tlb[(i-1) % tlb_size].valid = 0;
    if(write == 0) {
      tlb[(i-1) % tlb_size].dirty = 0;
    } else {
      tlb[(i-1) % tlb_size].dirty = 1;
    }
    tlb[(i-1) % tlb_size].used = 0;

   /*
    * In all cases perform TLB house keeping. This means marking the found TLB entry as
    * used and if we had a write, dirty. We also need to update the page
    * table entry in memory with the same data.
    */
    current_pagetable->pfn = pfn;



   return pfn;
}
