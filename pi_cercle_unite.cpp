#include <thread>
#include <future>
#include <iostream>
#include <cmath>

#define NB_THREADS 8

void get_nb_hits(std::atomic<int> &total_hits, int nb_shots, int num_thread)
{
    std::srand(time(nullptr) + num_thread); // random seed
    std::cout << "thread " << num_thread << " launched" << std::endl;
    int nb_hits = 0;
    for (int k = 0; k < nb_shots; k++)
    {
        double x = ((double)rand()) / RAND_MAX;
        double y = ((double)rand()) / RAND_MAX;

        bool hit = (x * x) + (y * y) <= 1.;

        if (hit)
        {
            nb_hits++;
        }
    }
    total_hits += nb_hits;
    std::cout << 4. * nb_hits / nb_shots << std::endl;
}

int main()
{
    long nb_shots_per_thread = 1E7;
    std::atomic<int> total_hits = 0;

    std::cout << "Performing Monte-Carlo pi approximation for " << nb_shots_per_thread * NB_THREADS << " shots and " << NB_THREADS << " threads..." << std::endl;
    // Calcul du temps d'exécution
    auto start = std::chrono::high_resolution_clock::now();

    //Instanciation des threads
    std::thread threads[NB_THREADS];
    for (int k = 0; k < NB_THREADS; k++)
    {
        threads[k] = std::thread(get_nb_hits, std::ref(total_hits), nb_shots_per_thread, k);
    }

    for (int k = 0; k < NB_THREADS; k++)
    {
        threads[k].join();
    }

    double approx_pi = 4. * total_hits / nb_shots_per_thread / NB_THREADS;

    // Fin de l'exécution
    auto end = std::chrono::high_resolution_clock::now();
    auto diff_sec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Approximation pi : " << approx_pi << std::endl;
    std::cout << "Run time : " << diff_sec << "ms" << std::endl;
    return 0;
}

// g++ -std=c++17 pi_cercle_unite.cpp -lpthread