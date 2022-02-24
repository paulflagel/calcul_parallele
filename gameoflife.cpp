// g++ gameoflife.cpp -o gol -lpthread -std=c++17

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <random>

#define NB_THREADS_PER_LINE 2

const int nb_threads = NB_THREADS_PER_LINE * NB_THREADS_PER_LINE;
const int nb_cells = 18; // Longueur du carré dont s'occupe chaque thread
std::mutex mutex;        // Verrou

std::random_device dev;                                   // Seed pour le rng
std::mt19937 rng(dev());                                  // Random number generator
std::uniform_real_distribution<double> uniform(0.0, 1.0); // Distribution uniforme entre 0 et 1
const int grid_size = NB_THREADS_PER_LINE * nb_cells;     // Pour avoir un multiple du nombre de threads
bool grid[grid_size + 1][grid_size + 1] = {};             // Grille de taille grid_size+1 pour éviter les effets de bord
int neighbors[grid_size + 1][grid_size + 1];              // Nombre de voisins de chaque case

void clearTerminal()
{
    // Efface tout le terminal
    std::cout << "\x1B[2J\x1B[H";
}

void printGrid()
{
    // Affiche la grille de l'état actuel
    // clearTerminal();
    std::cout << "\n"
              << std::endl;
    for (int x = 1; x < grid_size + 1; x++)
    {
        for (int y = 1; y < grid_size + 1; y++)
        {
            if (grid[x][y] == true)
            {
                std::cout << " ■ ";
            }
            else
            {
                std::cout << " . ";
            }
            if (y == grid_size)
            {
                std::cout << std::endl;
            }
        }
    }
}

void initializeGrid()
{
    // Initialisation de la grille soit depuis un fichier .txt soit de façon aléatoire
    std::string random_init;
    std::cout << "Random grid init ? (y/n)" << std::endl;
    std::cin >> random_init;

    if (random_init == "y")
    {
        double random;
        for (int x = 1; x < grid_size + 1; x++)
        {
            for (int y = 1; y < grid_size + 1; y++)
            {
                random = uniform(rng);
                if (random > 0.8)
                {
                    grid[x][y] = true;
                }
            }
        }
    }
    else
    {
        int x, y;
        // Récupère les coordonnées de tous les points de la configuration initiale dans le fichier initialboard.txt
        while (true)
        {
            std::ifstream readfile("initialboard.txt");
            if (readfile.is_open())
            {
                std::string fileline, xx, yy;

                while (getline(readfile, fileline))
                {
                    std::stringstream ss(fileline);

                    getline(ss, xx, ' ');
                    getline(ss, yy, ' ');

                    x = stoi(xx);
                    y = stoi(yy);

                    grid[x][y] = true;
                }
                break;
            }
        }
    }
}

void getNextState(int thread_id)
{
    for (int x = 1 + (thread_id % NB_THREADS_PER_LINE) * nb_cells; x < 1 + (thread_id % NB_THREADS_PER_LINE + 1) * nb_cells; x++)
    {
        for (int y = 1 + (thread_id / NB_THREADS_PER_LINE) * nb_cells; y < 1 + (thread_id / NB_THREADS_PER_LINE + 1) * nb_cells; y++)
        {
            mutex.lock();
            neighbors[x][y] = 0; // Nombre de voisins actifs
            mutex.unlock();
            for (int dx = -1; dx < 2; dx++)
            {
                for (int dy = -1; dy < 2; dy++)
                {
                    // On ne teste pas si la case en cours est active
                    if (!(dx == 0 && dy == 0))
                    {
                        if (grid[x + dx][y + dy])
                        {
                            mutex.lock();
                            ++neighbors[x][y];
                            mutex.unlock();
                        }
                    }
                }
            }
        }
    }
}

void updateGrid()
{
    for (int x = 1; x < grid_size + 1; x++)
    {
        for (int y = 1; y < grid_size + 1; y++)
        {
            // Update le statut de la case après avoir compté les voisins
            if (neighbors[x][y] < 2)
            {
                grid[x][y] = false;
            }
            else if (neighbors[x][y] == 3)
            {
                grid[x][y] = true;
            }
            else if (neighbors[x][y] > 3)
            {
                grid[x][y] = false;
            }
        }
    }
}

int main()
{
    std::thread threads[nb_threads];

    initializeGrid();
    printGrid();

    std::cout
        << "Grid setup is done. Start the game ? (y/n)" << std::endl;
    std::string start_game;
    std::cin >> start_game;
    if (start_game == "y")
    {
        while (true)
        {
            printGrid();
            for (int k = 0; k < nb_threads; k++)
            {
                threads[k] = std::thread(getNextState, k);
            }
            for (int k = 0; k < nb_threads; k++)
            {
                threads[k].join();
            }
            updateGrid();
            usleep(100000);
        }
    }
    else
    {
        return 0;
    }
}