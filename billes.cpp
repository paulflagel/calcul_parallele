// g++ billes.cpp -o billes -lpthread -std=c++17

#include <thread>
#include <iostream>
#include <mutex>
#include <string>

#define NB_THREADS 8
#define NB_BILLES 20

std::mutex mutex;

int nb_billes = NB_BILLES;                     // Nombre de billes disponibles
int nb_billes_workers[NB_THREADS] = {};        // Nombre de billes actuellement possédées par chaque worker
const int needs[8] = {2, 3, 4, 3, 5, 4, 3, 2}; // Nombre de billes dont a besoin chaque worker
int nb_tours = 20;

void print_billes()
{
    std::cout << "Billes disponibles : " << std::string(nb_billes, '.') << "\n"
              << std::endl;
}

bool not_finished()
{
    // Check si la simulation est terminée ou non.
    if (nb_tours < 0)
    {
        return false;
    }
    else
    {
        nb_tours -= 1;
        return true;
    }
}

void sleep(int time = 2000)
{
    // Temps pendant lequel le worker travaille (en ms)
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

int ask_billes(int thread_id, int need)
{
    mutex.lock();
    if (nb_billes >= need) // Assez de billes pour le worker
    {
        if (need < 0)
        {
            std::cout << "worker " << thread_id + 1 << " rend " << -need << " billes " << std::endl;
        }
        else
        {
            std::cout << "worker " << thread_id + 1 << " : " << std::string(need, '.') << std::endl;
        }
        nb_billes -= need;
        print_billes();
        mutex.unlock();
        return need;
    }
    else // Pas assez de billes disponibles
    {
        std::cout << "worker " << thread_id + 1 << " attend " << need << " billes " << std::endl;
        mutex.unlock();
        sleep();
        return ask_billes(thread_id, need);
    }
}

int drop_billes(int thread_id, int need)
{
    // le worker rend les billes à la réserve publique
    return ask_billes(thread_id, -need);
}

void worker(int thread_id)
{
    // main worker function
    while (not_finished())
    {
        nb_billes_workers[thread_id] += ask_billes(thread_id, needs[thread_id]);
        sleep();
        nb_billes_workers[thread_id] += drop_billes(thread_id, needs[thread_id]);
        sleep();
    }
}

void controleur(bool &flag)
{
    // main controller function
    while (flag)
    {
        mutex.lock();
        int check_nb_billes = nb_billes;
        mutex.unlock();
        if (check_nb_billes < 0 || check_nb_billes > NB_BILLES)
        {
            std::cout << "\n!!! CONTROLEUR - Erreur : nb_billes = " << check_nb_billes << " n'a pas la bonne valeur !!!\n"
                      << std::endl;
        }
        sleep(4000);
    }
}

int main()
{

    bool controleur_bool = true;

    // instanciation des threads
    std::thread threads[NB_THREADS];
    std::thread thread_controleur = std::thread(controleur, std::ref(controleur_bool));

    std::cout << "Creation de " << NB_THREADS << " threads" << std::endl;
    for (int i = 0; i < NB_THREADS; i++)
    {
        threads[i] = std::thread(worker, i);
    }
    for (int i = 0; i < NB_THREADS; i++)
    {
        threads[i].join();
    }

    controleur_bool = false;
    thread_controleur.join();
    std::cout << "\nDone !" << std::endl;

    return 0;
}