#include <thread>
#include <future>
#include <iostream>
#include <random>
#include <mutex>

#define NB_THREADS 8

std::mutex m;

double get_rand(int thread_id)
{
    // Thread safe generator
    static thread_local std::mt19937 rng(time(nullptr) + thread_id);
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    return uniform(rng);
}

void get_nb_hits(int &total_hits, int nb_shots, int thread_id)
{
    std::cout << "thread " << thread_id << " launched" << std::endl;
    int nb_hits = 0;
    for (int k = 0; k < nb_shots; k++)
    {
        double x = get_rand(thread_id);
        double y = get_rand(thread_id);

        bool hit = (x * x) + (y * y) <= 1.;

        if (hit)
        {
            nb_hits++;
        }
    }
    m.lock();
    total_hits += nb_hits;
    m.unlock();
}

int main()
{
    long nb_shots_per_thread = 1E5;
    int total_hits = 0;

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