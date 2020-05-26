//
//  main.cpp
//  DutchAuction
//  https://en.wikipedia.org/wiki/Dutch_auction
//
//  Created by Yuriy Yakimenko on 6/30/19.
//  Copyright © 2019 Yuriy Yakimenko. All rights reserved.
//
//  The problem:
//  You are the owner of a movie theater with 100 seats.
//  You are selling tickets to groups of people who are offering 
//  their a) price per ticket, b) number of seats they want.

//  You can take only the complete offer of a group, you cannot sell
//  them five seats when they are asking for ten.
//
//  All groups will be paying the lowest price among accepted offers.
//  For example, if group one offers to buy 50 tickets for 10 dollars each
//  and group 2 offers to also buy 50 tickets for 8 dollars each and you 
//  fulfill both offers, both groups will be paying 8 dollars per seat.

//  Your goal is to maximize the revenue from the movie.
//  Optimal solution does not always mean the theater will be full. 

#include <iostream>
#include <vector>
using namespace std;

#define _countof(array) (sizeof(array) / sizeof(array[0]))

bool recursive_call(vector<int> & ret, int cur_sum, const int target, int *array, int len, int first_index, vector<int> & bestFound, int & max_sum)
{
    if (cur_sum > target) return false;
    
    if (max_sum < cur_sum)
    {
        max_sum = cur_sum;
        bestFound = ret;
    }
    
    if (cur_sum == target)
        return true;
    
    if (len == first_index) return false;
    
    for (int i = first_index; i < len; i++)
    {
        ret.push_back(i);
        bool r = recursive_call(ret, cur_sum + array[i], target, array, len, i + 1, bestFound, max_sum);
        if (r == true)
        {
            return true;
        }
        ret.pop_back();
    }
    
    return false;
}

vector<int> findClosestSum(const int target, int *array, int len, int & max_sum)
{
    vector<int> ret;
    
    vector<int> bestFound;
    
    max_sum = 0;
    
    for (int i = 0; i < len; i++)
    {
        ret.push_back(i);
        bool r = recursive_call (ret, array[i], target, array, len, i + 1, bestFound, max_sum);
        if (r == true)
        {
            return ret;
        }
        ret.pop_back();
    }
    
    return bestFound;
}

struct offer
{
    unsigned int seats;
    double price;
};

int compareOffers(void const *offer1, void const *offer2)
{
    offer *o1 = (offer *)offer1;
    offer *o2 = (offer *)offer2;
    
    if (o1->price > o2->price) return -1;
    if (o1->price < o2->price) return 1;
    
    return 0;
}

// With this define we are checking first if the "simple" solution exits.
// This does not affect the outcome though: without this it will take a bit 
// longer to calculate the optimal result.

#define CHECK_PLAIN_SOLUTION

struct offer offers[] =   
{ 
    { 20, 50.0 },
    { 10, 30.0 },
    { 79, 23.0 },
    { 50, 22.0 },
    { 10, 100.0 },
    { 20, 35.0 },
    { 15, 45.0 },
    { 10, 15.0 },
    { 30, 20.0 },
    { 31, 25.0 },
    { 5, 38.0 }
};


#define MAX_SEATS 100

int main (int argc, const char * argv[])
{
    // 1. sort offers by price high to low
    const int ARR_LEN = _countof(offers);
    
    qsort (offers, ARR_LEN, sizeof(struct offer), compareOffers);
    
    printf("Offers sorted by price high to low:\n");
    for (int i = 0; i < ARR_LEN; i++)
    {
        printf("%d Seats for price %lf:\n", offers[i].seats, offers[i].price);
    }
    
    // 2. loop starting from hightest offer, adding offers and adjusting price
    int total_seats = 0;  // number of filled seats
    double max_revenue = 0; // current revenue (product of total_seats and lowest price we take)
    int best_combination = 0;
    int best_seats = 0;
    double best_price;
    
#ifdef CHECK_PLAIN_SOLUTION // faster when no permutations are needed.
    
    for (int i = 0; i < ARR_LEN; i++)
    {
        total_seats += offers[i].seats;
        if (total_seats > MAX_SEATS)
        {
            if (max_revenue > MAX_SEATS * offers[i].price)
            {
                // we cannot do better even if we fill all seats with this price.
                printf("[1] Take the following %d offers which fill %d seats for %lf per seat:\n", best_combination, best_seats, best_price);
                for (int p = 0; p < best_combination; p++)
                {
                    printf("%d Seats with original price %lf:\n", offers[p].seats, offers[p].price);
                }
                printf("** Total revenue %lf:\n", max_revenue);
                return EXIT_SUCCESS;
            }

            break; // we can stop here because next offer will have even lower price per seat.
        }
        
        double revenue = total_seats * offers[i].price;;
        if (revenue > max_revenue)
        {
            max_revenue = revenue;
            best_combination = i + 1;
            best_seats = total_seats;
            best_price = offers[i].price;
            if (total_seats == MAX_SEATS)
            {
                // we can end here: all seats are taken, with max revenue.
                printf ("Full theater. Take the following %d offers for %lf per seat:\n", best_combination, best_price);
                for (int p = 0; p < best_combination; p++)
                {
                    printf("%d Seats with original price %lf:\n", offers[p].seats, offers[p].price);
                }
                printf("** Total revenue %lf:\n", max_revenue);
                
                return EXIT_SUCCESS;
            }
        }
    }
    
    if (total_seats < MAX_SEATS)
    {
        // we did not get enough offers to fill all seats
        
        printf("[2] Take the following %d offers which fill %d seats:\n", best_combination, best_seats);
        for (int p = 0; p < best_combination; p++)
        {
            printf("%d Seats with original price %lf:\n", offers[p].seats, offers[p].price);
        }
        printf("** Total revenue %lf:\n", max_revenue);
        
        return EXIT_SUCCESS;
    }
    
#endif
    
    // there are enough offers to fill all seats so we can pick and choose. 
    // let's do a full search.
    vector<int> best;
    for (int i = 0; i < best_combination; i++)
    {
        best.push_back (i);
    }
    
    int * array = new int[ARR_LEN];
    
    for (int i=0; i < ARR_LEN; i++)
        array[i] = offers[i].seats;
    
    
    for (int i = best_combination + 1; i <= ARR_LEN; i++)
    {
        if (MAX_SEATS * offers[i - 1].price <= max_revenue) // we cannot do better: even if we fill all seats, revenue will be lower.
        {
            // print best combination
            break;
        }
        
        int max_sum = 0;
        vector<int>r = findClosestSum(MAX_SEATS, array, i, max_sum);
        
        if (max_sum * offers[i - 1].price > max_revenue)
        {
            best_price = offers[i - 1].price;
            best_seats = max_sum;
            max_revenue = max_sum * offers[i - 1].price;
            best = r;
            if (max_sum == MAX_SEATS) // we filled all seats. This is best combination. Print & exit.
            {
                break;
            }
        }
    }
    
    printf ("[3] Take the following %lu offers which fill %d seats, for %lf per seat: \n", best.size(), best_seats, best_price);
    
    for (int v : best)
    {
        printf("%d Seats with original price %lf:\n", offers[v].seats, offers[v].price);
    }
    printf("** Total revenue %lf:\n", max_revenue);
    
    delete[] array;

    return EXIT_SUCCESS;    
    
}
