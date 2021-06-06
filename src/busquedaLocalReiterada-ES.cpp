/*  Autor: Juan Miguel Gomez
    Compilar: g++ -O2 -o busquedaLocalReiterada busquedaLocalReiterada.cpp
    Ejecutar: ./busquedaLocalReiterada datos/file.txt
    Fecha: 30/05/2021
*/
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <chrono>

#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX 100000
#define MU 0.3
#define PHI 0.3

using namespace std;
using namespace std::chrono;

class maximumDiversityProblem
{
    private:
    //Matriz de distancias (simetrica para trabajar sin complicaciones)
    vector<vector<double>> distances;

    //Tamanio del conjunto de los datos
    int n;

    //Conjunto solucion o seleccionados
    set<int> bestSolution;

    //Numero de elementos que tenemos que escoger del conjunto para generar la solucion
    int m;

    //Valor de la diversidad de la solucion actual: Se usa para calcular solucion factorizada
    double bestValue;

    public:

    //Constructor por defecto
    maximumDiversityProblem();

    //Lee los datos del problema
    void readData(string path);

    set<int> randomSolution();

    //Devuelve la contribucion (o suma acumulada de distancias) del elemento i a los elementos del conjunto sol
    double getContribution(int i, set<int> set);

    // Calcula la diversidad entre los elementos seleccionados con el metodo del MaxSum
    double evaluation();

    double evaluation(set<int> sol);

    set<int> mutate(set<int> &solution, double &value);

    // Genera un vecino aleatorio del sol
    set<int> randomNeighbor(set<int> sol);

    //Encuentra la solucion por Enfriamiento Simulado
    set<int> findSimAnnealingSolution(set<int> &solution, double &value);

    //Encuentra la solucion por Enfriamiento Simulado
    set<int> findIteratedLocalSearch();

};

int main(int argc, char const *argv[])
{
    if(argc < 3){
        cout << "Error: Numero de argumentos invalido" << endl;
        return 1;
    }

    int seed = stoi(argv[2]);
    srand(seed);

    // Declaramos el tipo y hacemos que lea los datos
    maximumDiversityProblem gd;
    gd.readData(argv[1]);

    // Cronometramos el tiempo en ms
    auto start = high_resolution_clock::now();
    gd.findIteratedLocalSearch();
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    cout << gd.evaluation() << "\t" << duration.count() << endl;


    return 0;
}




maximumDiversityProblem::maximumDiversityProblem():n(0), m(0), bestValue(-1.0)
{
}

void maximumDiversityProblem::readData(string path)
{
    ifstream file;
    file.open(path);

    //Leemos el numero de filas y columnas
    file >> n >> m;

    //Inicializamos la matriz a 0
    distances.resize(n);

    //Llenamos la matriz de distancias de 0
    int zeros[n] = {0};
    for(int i=0; i < n; i++){
        distances[i] = vector<double>(zeros,zeros+n);
    }

    int i, j;
    double value;

    //Leemos el fichero completo e introducimos los valores a la matriz
    while(!file.eof()){
        file >> i >> j >> value;
        distances[i][j] = value;
        distances[j][i] = value; // Matriz simetrica pq es mas sencillo medir distancias
    }

    file.close();
}

set<int> maximumDiversityProblem::findIteratedLocalSearch()
{
    set<int> solution = randomSolution();
    double solutionValue = evaluation(solution);

    findSimAnnealingSolution(solution,solutionValue);
    bestValue = solutionValue;
    bestSolution = solution;

    // cout << evaluation(solution) << " == " << solutionValue << endl;

    for (size_t i = 0; i < 9; i++) {
        mutate(solution, solutionValue);

        // cout << "solution: " << evaluation(solution) << " best: " << evaluation(bestSolution) << endl;

        findSimAnnealingSolution(solution,solutionValue);

        if(solutionValue > bestValue)
        {
            // cout << "bestValue: " << bestValue << " solutionValue: " << solutionValue << endl;
            bestValue = solutionValue;
            bestSolution = solution;
        }

        solution = bestSolution;
        solutionValue = bestValue;
    }

    return bestSolution;
}

set<int> maximumDiversityProblem::mutate(set<int> &solution, double &value)
{
    const int NUM_MUT = m/10;

    for(int i=0; i<NUM_MUT; i++){
        int random_it = rand() % m;
        auto it = next(solution.begin(), random_it);

        int item2pull = (int) *it;
        int item2push = 0;

        solution.erase(item2pull);
        value -= getContribution(item2pull,solution);

        do{
            item2push = rand() % n;
            solution.insert(item2push);
        }while(solution.size() < m);

        value += getContribution(item2push,solution);

    }

    return solution;
}

set<int> maximumDiversityProblem::randomSolution()
{
    set<int> sol;

    while(sol.size() < m){
        sol.insert(rand()%n);
    }

    return sol;
}


set<int> maximumDiversityProblem::findSimAnnealingSolution(set<int> &solution, double &value)
{
  double bs_cost;
  set<int> best;

  if(!distances.empty()){
      int num_success, num_neighbor;
      const int max_neighbor = 10  * m;
      const int max_success  = (int) (0.1 * max_neighbor);
      const int NE = (int) (10000.0/max_neighbor); // NE: Numero de Enfriamientos => M

      set<int> neighbor;

      best = solution;
      double cost = value;
      bs_cost = cost;
      double nc = 0;

      const double final_tmp = 10e-3;
      double tmp = (MU * cost)/(-log(PHI));
      double beta = (tmp - final_tmp)/(NE * final_tmp * tmp);
      double delta = 0;

      while(tmp > final_tmp){

         num_neighbor = 0;
         num_success  = 0;

         while(num_success < max_success && num_neighbor < max_neighbor){

             neighbor = randomNeighbor(solution);
             nc = evaluation(neighbor); // Coste de vecino

             delta = cost - nc;

             num_neighbor++;

             if(delta <= 0) // Si delta == 0 => exp(-delta/tmp) = exp(0) = 1 => random < exp ? == true
             {
                 solution = neighbor;
                 cost = nc;
                 num_success++;

                 if(bs_cost < cost)
                 {
                     best = solution;
                     bs_cost = cost;
                 }
             }
             else
             {
                 //Generamos un random entre 0,1
                 double random = rand() % (int) 1e8;
                 random /= 1e8;


                 if(random <= exp(-delta/tmp))
                 {
                     solution = neighbor;
                     cost = nc;
                     num_success++;
                 }

             }


         }

         tmp = tmp / (1 + beta * tmp);
      }

  }
  else
  {
    cout << "Error: Se deben leer antes las distancias" << endl;
  }

  solution = best;
  value = bs_cost;

  return best;
}


//Adaptar para hacer calcular el coste factorizando
set<int> maximumDiversityProblem::randomNeighbor(set<int> sol)
{
    int i = rand() % m;

    auto it = next(sol.begin(),i);

    sol.erase(it);

    while(sol.size() < m){
        sol.insert(rand() % n);
    }

    return sol;
}

double maximumDiversityProblem::evaluation()
{
    double value = -1;
    //Si hemos generado la solucion
    if(bestSolution.size() == m){
        set<int>::iterator it = bestSolution.begin();
        auto end = prev(bestSolution.end(),1);
        value = 0;

        for(it; it != end; it++){
            auto sub_it = next(it,1);
            for(sub_it; sub_it != bestSolution.end(); sub_it++){
                value += distances[*it][*sub_it];
            }
        }
    }

    return value;
}

double maximumDiversityProblem::getContribution(int i, set<int> set)
{
    double accum = 0;

    auto it = set.begin();
    for(it; it != set.end(); it++){
        accum += distances[i][*it];
    }

    return accum;
}

double maximumDiversityProblem::evaluation(set<int> sol)
{
    double value = -1;
    //Si es una solucion
    if(sol.size() == m){
        set<int>::iterator it = sol.begin();
        auto end = prev(sol.end(),1);
        value = 0;

        for(it; it != end; it++){
            auto sub_it = next(it,1);
            for(sub_it; sub_it != sol.end(); sub_it++){
                value += distances[*it][*sub_it];
            }
        }
    }

    return value;
}
