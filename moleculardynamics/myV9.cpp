#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

/*
  Este programa simula un cuerpo que cae bajo accion de la gravedad y en el
  futuro va a rebotar contra el suelo y contra otros cuerpos.
  V8 : Anhadir fuerza con una pared a la derecha
  V9 : Anhadir techo // quitar la gravedad
*/

  /*
    z
    |
    |____ y
   /
  x  
  */


// cuerpo
struct body {
  double mass, radius;
  double r[3], v[3], f[3];
};

// simulation conditions
const int N = 1;
const double G = 0.0; // 9.81;
const double DT = 0.01;
const double K = 323.9; 
const double B = 0.9; // -mass*B*vz
const double LX = 3.2; 
const double LXMIN = -0.5;
const double LZMAX = 10.32;

// helper function
void initial_conditions(std::vector<body> & bodies);
void timestep(std::vector<body> & bodies, double dt);
void start_timeintegration(std::vector<body> & bodies, double dt);
void compute_force(std::vector<body> & bodies);
void print_system(const std::vector<body> & bodies, double time);
void print_csv(const std::vector<body> & bodies, int step);

int main(void)
{
  // declaracion de los cuerpos
  std::vector<body> bodies(N);

  // pre-processing
  initial_conditions(bodies); // r(t=0), v(t=0)
  compute_force(bodies); // f(t=0)
  start_timeintegration(bodies, DT);

  // processing
  for (int step = 0; step < 1000; ++step) {
    double tstep = step*DT; 
    std::cout << tstep << "  " 
              << bodies[0].r[0] << "  "
              << bodies[0].r[1] << "  "
              << bodies[0].r[2] << "  "
              << bodies[0].v[0] << "  "
              << bodies[0].v[1] << "  "
              << bodies[0].v[2] << "  "
              << std::endl;
    timestep(bodies, DT); // actualiza r y v -> actualizar f
    compute_force(bodies); // actualizar la fuerza
    //print_csv(bodies, step);
  }

  print_system(bodies, 0);

  return 0;
}

// helper function
void initial_conditions(std::vector<body> & bodies)
{
  bodies[0].mass = 1.23;
  bodies[0].radius = 0.16;
  bodies[0].r[2] = 7.86;
  bodies[0].v[0] = 0.87;
  bodies[0].v[2] = 1.32;
}

void timestep(std::vector<body> & bodies, double dt)
{
  for (auto & cuerpo : bodies) {
    for(int ii = 0; ii <= 2; ++ii) {
      cuerpo.v[ii] += dt*cuerpo.f[ii]/cuerpo.mass;
      cuerpo.r[ii] += cuerpo.v[ii]*dt;
    }
  }
}

void start_timeintegration(std::vector<body> & bodies, double dt)
{
  for (auto & cuerpo : bodies) {
    for(int ii = 0; ii <= 2; ++ii) {
      cuerpo.v[ii] = cuerpo.v[ii] - dt*cuerpo.f[ii]/(2*cuerpo.mass);
    }
  }
}

void compute_force(std::vector<body> & bodies)
{
  // reset forces
  for (auto & cuerpo : bodies) {
    cuerpo.f[0] = cuerpo.f[1] = cuerpo.f[2] = 0.0;
  }

  for (auto & cuerpo : bodies) {
    // add gravity
    cuerpo.f[2] -= cuerpo.mass*G;
    // add floor force
    double delta = cuerpo.radius - cuerpo.r[2];
    if (delta > 0) {
      cuerpo.f[2] += K*delta - B*cuerpo.mass*cuerpo.v[2];
    }
    // add right wall force
    delta = cuerpo.r[0] + cuerpo.radius - LX;
    if (delta > 0) {
      cuerpo.f[0] += -K*delta - B*cuerpo.mass*cuerpo.v[0];
    }
    // add left wall force
    delta = LXMIN - (cuerpo.r[0] - cuerpo.radius);
    if (delta > 0) {
      cuerpo.f[0] += +K*delta - B*cuerpo.mass*cuerpo.v[0];
    }

    // add upper force
    delta = cuerpo.r[2] + cuerpo.radius - LZMAX;
    if (delta > 0) {
      cuerpo.f[2] += -K*delta - B*cuerpo.mass*cuerpo.v[2];
    }

  }
}

void print_system(const std::vector<body> & bodies, double time)
{
  std::ofstream fout("datos.txt", std::ofstream::out);
  fout.precision(15); fout.setf(std::ios::scientific);

  for (const auto & cuerpo : bodies) {
    fout << cuerpo.r[0] << "  " << cuerpo.r[1] << "  " << cuerpo.r[2] << "  "  
         << cuerpo.v[0] << "  " << cuerpo.v[1] << "  " << cuerpo.v[2] << "  "
         << cuerpo.f[0] << "  " << cuerpo.f[1] << "  " << cuerpo.f[2] << "  "  
         << cuerpo.mass << "\n";
  }
}

void print_csv(const std::vector<body> & bodies, int step)
{
  std::string fname = "data-" + std::to_string(step) + ".csv";  
  std::ofstream fout(fname, std::ofstream::out);
  fout.precision(15); fout.setf(std::ios::scientific);

  for (const auto & cuerpo : bodies) {
    fout << cuerpo.r[0] << ", " << cuerpo.r[1] << ", " << cuerpo.r[2] << ", " << cuerpo.radius << "\n";
  }
  fout.close();
}
