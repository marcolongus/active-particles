/* Autor: Benjamín R. Marcolongo. FAMAF-UNC.
 *---------------------------------------------------------------------------------------------------------
 * Programa para evolucionar un sistema de N agentes en el tiempo.
 *---------------------------------------------------------------------------------------------------------
 * Agentes:
 *		i.   Las velocidades de los N agentes pueden ser uniformes o tomadas aleatoriamente.
 *		      1. Las distribuciones por defecto son de tipo exponencial o de ley de potencias.
 *		ii.  Pueden interactuar a través de un potencial u otra forma (una red neuronal i.e).
 *			  1. El potencial por defecto es de esferas blandas.
 *		ii.  Están confinados a un toroide topológico-
 *			  1. El tamaño característico del toro se denota con L (lado del cuadrado).
 *		iii. Poseen un estado interno, caracterizado por un número entero
 *			  1. Este estado puede o no interactaur con la dinámica espacial.
 *		iV.  El estado interno puede evolucionar en el tiempo.
 *			  1. Esta dinámica está regulada, genéricamente, con una distribución de poisson.
 *---------------------------------------------------------------------------------------------------------
 * Red compleja:
 *			i.  El programa trackea todo el sistema de interacciones y guarda la red compleja resultante.
 *			ii. La red compleja puede ser la asociada a la propagación del estado interno o la de contactos
 *			para alguna escala temporal.
 *---------------------------------------------------------------------------------------------------------
 */
#include <bits/stdc++.h>
#include "classparticle.h" //Módulo con la clase definida para los agentes.

#define forn(i,a,b) for(int i=a; i<b; i++)
using namespace std;

int main(void){

	cout << "Memoria del sistema: " << 2*36*N << " Bytes" << endl;
	cout << "seed: " << seed << endl << endl;
	int start_s = clock();
	/*DEFINICIÓN DE ARCHIVOS DE SALIDA DEL PROGRAMA*/
	//Para modelado de epidemias:
	ofstream FinalState ("data/evolution.txt");
	ofstream epidemic   ("data/epidemia.txt") ;//Estado de la epidemia en cada instante modulo m.
	ofstream anim       ("data/animacion.txt");
	ofstream imax       ("data/imax.txt")     ;//Máxima cantidad de infectados.
	ofstream mips       ("data/mips.txt")     ;//Busqueda de mobility induced phase-separetion.
	//Red compleja:
	//Comentario sobre cómo está guardada esta información.
	ofstream topology   ("data/topology.txt") ; //Se guarda la red compleja.

	cout << "-------------" << endl;
	cout << "Simulacion "	<< endl;
	cout << "-------------" << endl << endl;
	/*DECLARACIÓN DE VARIABLES*/
	vector<particle> system    ,
					 system_new;

	vector<bool>     inter;        //Flag de interacción.
	vector<int>      state_vector; //En cada lugar contiene la población de cada estado.
	/*Estuctura de datos para optimizar la búsqueda de interacciones entre agentes:
	 *	1. Utiliza un red-and-black tree implementado en c++ como set.
	 *	2. Cada agente está indexado por un int que representa su posición en
	 *	   los vectores system y system_new.
	 *	3. Se construye una grilla con cuadrículas de tamaño 1 y cada a una se le asigna un set.
	 *	4. Cada set contiene los agentes que están en cada cuadrícula.
	 */
	vector<vector<set<int>>> box;
	int num_boxes = floor(L);

	//Inicializamos los vectores declarados previamente:
	inter.resize(N,false);
	state_vector.resize(spin,0);

	box.resize(num_boxes);
	for (int i=0; i<box.size(); i++) box[i].resize(num_boxes);


	/*CONDICIÓN INICIAL*/
	for(int p = 0; p < N; p++){
		particle Agent;
		bool accepted = false;
		while(!accepted){
			accepted = true;
			Agent = create_particle();
			int i_index = floor(Agent.x),
				j_index = floor(Agent.y);
			//Si interactúa con otra partícula cambiamos la condición a no aceptada.
			forn(l,-2,3){
				forn(m,-2,3){
					int i = b_condition(i_index + l),
						j = b_condition(j_index + m);
					if (!box[i][j].empty()){
						for (auto element: box[i][j]){
							if (interact(Agent,system[element])) accepted = false;
						}//for auto
					}//if not empty
				}//for m
			}//for l
			if (accepted) box[i_index][j_index].insert(p);
		}//while
		system.push_back(Agent);
		state_vector[Agent.get_state()]++;
	}//for N
	print_state(state_vector);


	/*EVOLUCIÓN DEL SISTEMA*/
	int TimeStep   = 0;  //Contador de tiempo.
	system_new.resize(system.size());
	while (state_vector[1] > 0){
		TimeStep ++;
		if (TimeStep % 10000 == 0) cout << "Tiempo: " << TimeStep*delta_time << endl;
		state_vector = {0,0,0};
		#pragma omp parallel for
		for (int p=0; p<N; p++){
			vector<int> index;
			index.push_back(p);
			inter[p] = false;
			/*chequeamos interacciones*/
			forn(l,-2,3) forn(m,-2,3){
				int i_index = b_condition(floor(system[p].x)+l),
					j_index = b_condition(floor(system[p].y)+m);
				if(!box[i_index][j_index].empty()){
					for(auto element: box[i_index][j_index]){
						if (element !=p && interact(system[p],system[element])){
							inter[p] = true;
							index.push_back(element);
						}
					}//for
				}//if not empty
			} //for m, l
			/*fin de chequeo de interacciones*/
			system_new[p] = evolution(system, index, inter[p]);
			state_vector[system_new[p].get_state()]++;
		}//for p
		//Animacion:
		if (animation and TimeStep % anim_step == 0){
			forn(p,0,system_new.size()){
				anim << system_new[p].x           << " ";
				anim << system_new[p].y           << " ";
				anim << TimeStep*delta_time       << " ";
				anim << system_new[p].get_state() << endl;
			}
		}//if animacion
		/*Estabilzamos el set*/
		for(int p=0; p<N; p++){
			int i_new = floor(system_new[p].x),
				j_new = floor(system_new[p].y);
			int i_old = floor(system[p].x),
				j_old = floor(system[p].y);

			if (box[i_new][j_new].find(p) == box[i_new][j_new].end()){
				box[i_old][j_old].erase(p);
				box[i_new][j_new].insert(p);
			}//if
		}//cirra el for p set.
		system = system_new;
	}//while


	/*ESCRITURA DE RESULTADOS*/
	cout << endl;
	cout << "--------------------" << endl;
	cout << "Experimento data:"    << endl;
	cout << "--------------------" << endl;
	print_state(state_vector);
	cout << endl;

	int stop_s = clock();
	cout << "Time[min]: " << (((stop_s-start_s)/double(CLOCKS_PER_SEC)*1000)/1000)/60 << endl;

	//Cerramos los archivos:
	FinalState.close();
	epidemic.close();
	anim.close();
	imax.close();
	mips.close();
	topology.close();

	return 0;
}