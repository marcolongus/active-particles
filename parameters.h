//Parametros principales:
const int    N = 100; //Cantidad de agentes.
const double L = 50; //Largo del sistema. Area = L*L.


//Parametros de evolución del sistema.
//Pre-condición para estos parámetros (dt*v<0.1):
const double delta_time      = 0.1;
const double active_velocity = 0.05; //Velocidad de las partículas cuando no interactúan.

//0 -> dist. exponencial, 1 -> dist. power law. Cualquier otro dist. uniforme.
const int velocity_distribution = -1;

//Condición inicial SIR
const double p_init  = 0.1, //Infectadas iniciales.
			 p_rinit = 0.0, //Refractarias iniciales (elegidas al azar).
			 p_dinit = 0.0; //Refractarias iniciales (elegidas a partir de lista de velocidades).

const int spin = 3; //Estados internos.

const bool animation = true;
const int  anim_step = 10;

/*Tiempos característicos y probabilidades por unidad de tiempo*/
const double tau_t = 1  , p_transmision = (1/tau_t)*delta_time, //sane---->infected
			 tau_i = 200, p_infection   = (1/tau_i)*delta_time, //infected--->refractary
			 tau_r = 500, p_recfractary = (1/tau_r)*delta_time; //refractary--->sane

const double alpha = 100, p_rotation    = (1/alpha)*delta_time;


 /*Para distribución power-law hay que fitear las constantes*/
const double k_powerl = 2.09500, //1.7713537054317126; // v=0.1
			 v_min    = 0.01,
             v_max    = 4.;

//Constantes:
const double Pi       = 3.14159265358979323846,
			 dos_Pi   = 2*Pi,
			 infinity = 1000000000;

//Características geométricas de agentes.
const double radio    = 1.,
			 diameter = 2.*radio;


//Características del medio en el que se mueven los agentes.
const double beta           = 1;                    //comentario sobre qué representa.
const double gamma_friction = 3.92*active_velocity; //transforma: vel -> fuerza.