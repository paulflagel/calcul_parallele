// g++ mergesort.cpp -o mergesort -lpthread -std=c++17

#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <random>
#include <algorithm>

std::random_device dev;
std::mt19937 rng(dev());

const int array_size = 20;
#define NB_THREADS 4

std::mutex mutex;

void merge(int array[], int const left, int const mid, int const right)
{
    int const size_1 = mid - left + 1; // taille du sous-tableau 1
    int const size_2 = right - mid;    // taille du sous-tableau 2

    // temp arrays
    int *array_1 = new int[size_1],
        *array_2 = new int[size_2];

    // copie de l'array dans les temp arrays
    for (int i = 0; i < size_1; i++)
        array_1[i] = array[left + i];
    for (int j = 0; j < size_2; j++)
        array_2[j] = array[mid + 1 + j];

    int idx_1 = 0;
    int idx_2 = 0;
    int idx_merged = left;

    // Fusion des deux arrays
    while (idx_1 < size_1 && idx_2 < size_2)
    {
        if (array_1[idx_1] <= array_2[idx_2])
        {
            array[idx_merged] = array_1[idx_1];
            idx_1++;
        }
        else
        {
            array[idx_merged] = array_2[idx_2];
            idx_2++;
        }
        idx_merged++;
    }

    // on met les éléments restants de l'array 1 (on a mis tous ceux de l'array 2)
    while (idx_1 < size_1)
    {
        array[idx_merged] = array_1[idx_1];
        idx_1++;
        idx_merged++;
    }

    // on met les élémets restants de l'array 2 (on a mis tous ceux de l'array 1)
    while (idx_2 < size_2)
    {
        array[idx_merged] = array_2[idx_2];
        idx_2++;
        idx_merged++;
    }
}

void merge_sort(int array[], int const begin, int const end)
{
    if (begin >= end)
    {
        return; // si on a 1 seul élément, le tableau est trié
    }

    int mid = begin + (end - begin) / 2; // on prend l'indice à peu près au milieu

    merge_sort(array, begin, mid);
    merge_sort(array, mid + 1, end);
    mutex.lock();
    merge(array, begin, mid, end);
    mutex.unlock();
}

void print_array(int arr[array_size])
{
    for (int i = 0; i < array_size; i++)
    {
        std::cout << std::string(arr[i], '.') << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    int arr[array_size] = {};
    for (int i = 0; i < array_size; i++)
    {
        arr[i] = i + 1;
    }
    std::shuffle(std::begin(arr), std::end(arr), rng);

    std::cout
        << "Input :\n";
    print_array(arr);
    int begin = 0;
    int end = array_size - 1;

    int mid = begin + (end - begin) / 2;
    int midmid1 = begin + (mid - begin) / 2;
    int midmid2 = mid + 1 + (end - (mid + 1)) / 2;
    std::thread t1 = std::thread(merge_sort, arr, begin, midmid1);
    std::thread t2 = std::thread(merge_sort, arr, midmid1 + 1, mid);
    std::thread t3 = std::thread(merge_sort, arr, mid + 1, midmid2);
    std::thread t4 = std::thread(merge_sort, arr, midmid2 + 1, end);
    t1.join();
    t2.join();
    merge(arr, begin, midmid1, mid);
    t3.join();
    t4.join();
    merge(arr, mid + 1, midmid2, end);

    merge(arr, 0, mid, end);

    std::cout
        << "Output :\n";
    print_array(arr);
    return 0;
}