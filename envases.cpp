#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <time.h>
#include <queue>


struct Nodo {

	Nodo() : sol(NULL), capacidad(NULL), k(0), envases(0) {}
	Nodo(std::vector<int> s, std::vector<int> c, int K, int env) : sol(s), capacidad(c), k(K), envases(env) {}

	std::vector<int> sol;
	std::vector<int> capacidad;
	int k, envases;

};

struct Solucion {
	std::vector<int> solucion;
	unsigned long long int nodosExplorados = 0;
	int envases_mejor;
};


void init(std::vector<int>&, int&);
int podaPesimistaIngenua(Nodo, std::vector<int> const&);
int podaPesimistaAjustada(Nodo, std::vector<int>);
int podaOptimistaIngenua(Nodo const&);
int podaOptimistaAjustada(Nodo const&, std::vector<int> const&, int const&);
Solucion envasesPodasIngenuas(std::vector<int> const&, int const&);
Solucion envasesPodasAjustadas(std::vector<int> const&, int const&);
Solucion envasesSinPoda(std::vector<int> const&, int const&);
bool operator<(Nodo const&, Nodo const&);
bool operator==(Nodo const&, Nodo const&);
bool operator>(Nodo const&, Nodo const&);


void main() {
	std::vector<int> objetos;
	int tamCaja;
	try {

		init(objetos, tamCaja);
		double clk1, clk2, time;
		clk1 = clock();
		Solucion solucionSinPoda = envasesSinPoda(objetos, tamCaja);
		clk2 = clock();
		time = (clk2 - clk1) / CLOCKS_PER_SEC;
		std::cout << "\nSolucion sin poda: " << solucionSinPoda.envases_mejor << "\n"
			<< "Expansiones de nodo sin poda: " << solucionSinPoda.nodosExplorados << "\n"
			<< "Tiempo de ejecucion: " << time << "s\n" 
			<< "Tiempo medio por expansion de Nodo: " << time / solucionSinPoda.nodosExplorados << "s\n";
		std::cout << "Solucion: (";
		for (int i = 0; i < solucionSinPoda.solucion.size(); ++i) {
			std::cout << solucionSinPoda.solucion[i];
			if (i < solucionSinPoda.solucion.size() - 1) std::cout << ", ";
		}
		std::cout << ")\n\n";

		clk1 = clock();
		Solucion solucionPodaIngenua = envasesPodasIngenuas(objetos, tamCaja);
		clk2 = clock();
		time = (clk2 - clk1) / CLOCKS_PER_SEC;
		std::cout << "Solucion con podas ingenuas: " << solucionPodaIngenua.envases_mejor << "\n"
			<< "Expansiones de nodo con poda ingenua: " << solucionPodaIngenua.nodosExplorados << "\n"
			<< "Tiempo de ejecucion: " << time << "s\n"
			<< "Tiempo medio por expansion de Nodo: " << time / solucionPodaIngenua.nodosExplorados << "s\n";
		std::cout << "Solucion: (";
		for (int i = 0; i < solucionPodaIngenua.solucion.size(); ++i) {
			std::cout << solucionPodaIngenua.solucion[i];
			if (i < solucionPodaIngenua.solucion.size() - 1) std::cout << ", ";
		}
		std::cout << ")\n\n";

		clk1 = clock();
		Solucion solucionPodaAjustada = envasesPodasAjustadas(objetos, tamCaja);
		clk2 = clock();
		time = (clk2 - clk1) / CLOCKS_PER_SEC;
		std::cout << "Solucion podas ajustadas: " << solucionPodaAjustada.envases_mejor << "\n"
			<< "Expansiones de nodo con poda ajustada: " << solucionPodaAjustada.nodosExplorados << "\n"
			<< "Tiempo de ejecucion: " << time << "s\n"
			<< "Tiempo medio por expansion de Nodo: " << time / solucionPodaAjustada.nodosExplorados << "s\n";
		std::cout << "Solucion: (";
		for (int i = 0; i < solucionPodaAjustada.solucion.size(); ++i) {
			std::cout << solucionPodaAjustada.solucion[i];
			if (i < solucionPodaAjustada.solucion.size() - 1) std::cout << ", ";
		}
		std::cout << ")\n\n";


		if (solucionSinPoda.envases_mejor != solucionPodaIngenua.envases_mejor || solucionPodaIngenua.envases_mejor != solucionPodaAjustada.envases_mejor)
			throw std::domain_error("Salida incorrecta"); //Claramente esto no deberia ocurrir, pero lo dejo por si en algun caso ocurriera.
		else std::cout << "Todas las soluciones coinciden";
	}
	catch (std::exception & e) {
		std::cout << e.what() << "\n";
	}
	std::cin.get();
	std::cin.get();
}

Solucion envasesPodasIngenuas(std::vector<int> const& obj, int const& E) {

	Solucion s;
	Nodo X, Y;

	//Cola de prioridad que ordena los Nodos segun el numero de envases de menor a mayor
	std::priority_queue<Nodo, std::deque<Nodo>, std::greater<Nodo>> C;

	int total = 0;
	for (int i = 0; i < obj.size(); ++i) total += obj[i];
	bool encontrado = false;
	//Generamos la raiz del primer elemento
	Y.capacidad = std::vector<int>(obj.size(), E);
	Y.sol = std::vector<int>(obj.size(), -1);
	Y.k = 0; Y.envases = 1; Y.sol[0] = 0;
	Y.capacidad[0] -= obj[0];

	C = std::priority_queue<Nodo, std::deque<Nodo>, std::greater<Nodo>>();
	C.push(Y);
	s.envases_mejor = podaPesimistaIngenua(Y, obj);
	int optima = podaOptimistaAjustada(Y, obj, E);

	while (!encontrado && !C.empty() && C.top().envases <= s.envases_mejor) {
		Y = C.top();
		C.pop();

		X = Y;
		++X.k;
		++s.nodosExplorados;

		int i = -1;	
		//Vas creando los nodos insertando el objeto en todos los envases que puedas hasta introducirlo en un nuevo envase vacio
		while (++i <= Y.envases) {
			if (X.capacidad[i] >= obj[X.k]) {
				if (i == Y.envases) ++X.envases;
				X.sol[X.k] = i;
				X.capacidad[i] -= obj[X.k];
				s.envases_mejor = std::min(podaPesimistaIngenua(X, obj), s.envases_mejor);
				//Si se ha encontrado la solucion y dicha solucion es como minimo tan buena como los envases de la solucion actual se pone como solucion
				if (X.k == obj.size() - 1 && X.envases <= s.envases_mejor) {
					if (X.envases == optima) encontrado = true;
					s.solucion = X.sol;
					s.envases_mejor = X.envases;
				}
				//Si no se ha terminado la solucion y la cota optimista de dicho nodo es mas baja que la pesimista se vuelve a meter en la cola de prioridad
				else if (X.k < obj.size() - 1 && podaOptimistaIngenua(X) <= s.envases_mejor)  C.push(X);


				X.capacidad[X.k] = Y.capacidad[X.k];
				X.sol[X.k] = Y.sol[X.k];
				X.envases = Y.envases;

			}
		}

	}

	return s;
}

Solucion envasesPodasAjustadas(std::vector<int> const& obj, int const& E) {
	Solucion s;
	Nodo X, Y;

	//Cola de prioridad que ordena los Nodos segun el numero de envases de menor a mayor
	std::priority_queue<Nodo, std::deque<Nodo>, std::greater<Nodo>> C;
	int total = 0;
	for (int i = 0; i < obj.size(); ++i) total += obj[i];
	bool encontrado = false;
	//Generamos la raiz del primer elemento
	Y.capacidad = std::vector<int>(obj.size(), E);
	Y.sol = std::vector<int>(obj.size(), -1);
	Y.k = 0; Y.envases = 1; Y.sol[0] = 0;
	Y.capacidad[0] -= obj[0];
	int optima = podaOptimistaAjustada(Y, obj, E);
	C = std::priority_queue<Nodo, std::deque<Nodo>, std::greater<Nodo>>();
	C.push(Y);
	s.envases_mejor = podaPesimistaAjustada(Y, obj);

	while (!encontrado && !C.empty() && C.top().envases <= s.envases_mejor) {
		Y = C.top();
		C.pop();
		X = Y;
		++X.k;
		++s.nodosExplorados;


		int i = -1;
		//Vas creando los nodos insertando el objeto en todos los envases que puedas hasta introducirlo en un nuevo envase vacio
		while (++i <= Y.envases) {
			if (X.capacidad[i] >= obj[X.k]) {
				if (i == Y.envases) ++X.envases;
				X.sol[X.k] = i;
				X.capacidad[i] -= obj[X.k];
				s.envases_mejor = std::min(podaPesimistaAjustada(X, obj), s.envases_mejor);
				//Si se ha encontrado la solucion y dicha solucion es como minimo tan buena como los envases de la solucion actual se pone como solucion
				if (X.k == obj.size() - 1 && X.envases <= s.envases_mejor) {
					if (X.envases == optima) encontrado = true;
					s.solucion = X.sol;
					s.envases_mejor = X.envases;

				}
				//Si no se ha terminado la solucion y la cota optimista de dicho nodo es mas baja que la pesimista se vuelve a meter en la cola de prioridad
				else if (X.k < obj.size() - 1 && podaOptimistaAjustada(X, obj, E) <= s.envases_mejor)  C.push(X);

				X.capacidad[X.k] = Y.capacidad[X.k];
				X.sol[X.k] = Y.sol[X.k];
				X.envases = Y.envases;
			}
		}

	}

	return s;
}

Solucion envasesSinPoda(std::vector<int> const& obj, int const& E) {
	Solucion s;
	Nodo X, Y;

	//Cola de prioridad que ordena los Nodos segun el numero de envases de menor a mayor
	std::priority_queue<Nodo, std::deque<Nodo>, std::greater<Nodo>> C;
	s.envases_mejor = obj.size();
	int total = 0;
	for (int i = 0; i < obj.size(); ++i) total += obj[i];
	//Generamos la raiz del primer elemento
	Y.capacidad = std::vector<int>(obj.size(), E);
	Y.sol = std::vector<int>(obj.size(), -1);
	Y.k = 0; Y.envases = 1; Y.sol[0] = 0;
	Y.capacidad[0] -= obj[0];

	C = std::priority_queue<Nodo, std::deque<Nodo>, std::greater<Nodo>>();
	C.push(Y);

	while (!C.empty() && C.top().envases <= s.envases_mejor) {
		Y = C.top();
		C.pop();
		X = Y;
		++X.k;
		++s.nodosExplorados;

		int i = -1;
		//Vas creando los nodos insertando el objeto en todos los envases que puedas hasta introducirlo en un nuevo envase vacio
		while (++i <= Y.envases) {
			if (X.capacidad[i] >= obj[X.k]) {
				if (i == Y.envases) ++X.envases;
				X.sol[X.k] = i;
				X.capacidad[i] -= obj[X.k];
				//Si se ha encontrado la solucion y dicha solucion es como minimo tan buena como los envases de la solucion actual se pone como solucion
				if (X.k == obj.size() - 1 && X.envases <= s.envases_mejor) {
					s.solucion = X.sol;
					s.envases_mejor = X.envases;
				}
				//Si no se ha terminado la solucion se vuelve a meter en la cola de prioridad
				else if (X.k < obj.size() - 1)  C.push(X);

				X.capacidad[X.k] = Y.capacidad[X.k];
				X.sol[X.k] = Y.sol[X.k];
				X.envases = Y.envases;
			}
		}

	}

	return s;
}



void init(std::vector<int> & obj, int& tam) {
	obj = std::vector<int>();
	std::ifstream in;
	std::string fname;
	std::cout << "Archivo de entrada de datos: ";
	std::cin >> fname;
	in.open(fname);
	if (!in.is_open()) throw std::domain_error("Archivo no existente");
	int aux, valMax = 0;
	while (!in.eof()) {
		in >> aux;
		if (aux <= 0) throw std::domain_error("Datos de entrada no validos");
		else if (aux > valMax) valMax = aux;
		obj.push_back(aux);
	}
	do {
		std::cout << "Introduce la capacidad de los envases (minimo " << valMax << "): ";
		std::cin >> aux;
		if (aux < valMax) std::cout << "Capacidad insuficiente\n\n";
	} while (aux < valMax);
	tam = aux;
}

//La primera poda pesimista consistira simplemente en que el resto de objetos que quedan tendran cada uno su propio envase
// y ninguno sera asignado en ningun envase ya existente, asi que se devuelve el numero de objetos que quedan sumado a los envases que ya tenemos
int podaPesimistaIngenua(Nodo n, std::vector<int> const& obj) {
	return n.envases + (obj.size() - n.k);
}


//La poda pesimista ajustada trata de ordenar el vector de capacidades de menor a a mayor, y el de objetos restantes de mas grandes a menos, e ir rellenando los envases
//en orden, intentando meter los objetos de mayor volumen en los envases con menos capacidad posible
int podaPesimistaAjustada(Nodo n, std::vector<int> obj) {
	//ordena de mayor a menor los envases asignados segun capacidad y los objetos restantes de menor a mayor por tamanyo
	//std::sort(n.capacidad.begin(), n.capacidad.end());
	//std::sort(obj.begin(), obj.end(), std::greater<int>());
	for (unsigned int i = n.k + 1; i < obj.size(); ++i) {
		int j = -1;
		while (n.capacidad[++j] < obj[i]);
		if (j == n.envases) ++n.envases;
		n.capacidad[j] -= obj[i];
	}
	return n.envases;
}

//La primera poda optimista simplemente devolvera el mismo numero de envases que ya tengamos con algun objeto en el
int podaOptimistaIngenua(Nodo const& n) {
	return n.envases;
}


//La segunda poda optimista consiste en, teniendo el volumen total de los objetos que aun quedan por envasar y la capacidad restante
//de los envases los cuales ya tienen objetos asignados, restar del volumen total dicha capacidad y el resto de lo que queda dividirlo
//por el volumen de los envases siempre redondeando para arriba
int podaOptimistaAjustada(Nodo const& n, std::vector<int> const& obj, int const& E) {
	int restante = 0, disponible = 0;
	for (unsigned int i = n.k + 1; i < obj.size(); ++i) restante += obj[i];
	for (unsigned int i = 0; i < n.envases; ++i) disponible += n.capacidad[i];
	int opt = n.envases + std::max(0, (restante - disponible) / E);
	if (restante - disponible > 0 && (restante - disponible) % E != 0) ++opt;
	return opt;
}


bool operator<(Nodo const& a, Nodo const& b) {
	return a.envases < b.envases;
}

bool operator==(Nodo const& a, Nodo const& b) {
	return a.envases == b.envases;
}

bool operator>(Nodo const& a, Nodo const& b) {
	return a.envases > b.envases;
}