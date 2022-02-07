# Calcul parallèle et programmation concurrente

MSO 3.3 : Internet des objets et systèmes concurrents embarqués.

Introduction à la programmation parallèle en c++.

## Approximation de pi par le cercle unitaire

Pour un cercle unitaire, l'aire vaut `A = pi * r^2 = pi`. On peut donc par une méthode de Monte Carlo, effectuer un grand nombre d'impacts aléatoires dans un carré `[0,1] x [0,1]` dans lequel est inscrit le quart supérieur droit du cercle. Pour savoir si un point est dans le quart de cercle, on regarde si `x^2 + y^2 <= 1`. 

Soit `N` le nombre d'impacts et `N_hits` le nombre d'impacts dans le cercle, alors `N_hits/N = pi*r^2/4/r^2 = pi/4`.

Donc `pi = 4*N_hits/N`.

## Tri

## Billes

## Game of life