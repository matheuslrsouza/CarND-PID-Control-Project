#include "PID.h"

PID::PID() {}

PID::~PID() {}

void PID::UpdateError(double cte, high_resolution_clock::time_point prev_t, 
  high_resolution_clock::time_point t2, double speed) {
  duration<double> time_span = duration_cast<duration<double>>(t2 - prev_t);
  double delta_t = time_span.count();

  delta_t = delta_t / speed;

  this->p_error = cte;
  this->d_error = (cte - this->prev_cte) / delta_t;
  this->i_error += cte;
}

double PID::TotalError() {
  double p, i, d;
  p = this->p[0];
  i = this->p[1];
  d = this->p[2];

  return -p * this->p_error - d * this->d_error - i * this->i_error;
}