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
    
   /* 
    * If it was a miss, call the page table lookup to get the pfn
    * Add current page as TLB entry. Replace any invalid entry first, 
    * then do a clock-sweep to find a victim (entry to be replaced).
    */


   /*
    * In all cases perform TLB house keeping. This means marking the found TLB entry as
    * used and if we had a write, dirty. We also need to update the page
    * table entry in memory with the same data.
    */

   return pfn;
}

