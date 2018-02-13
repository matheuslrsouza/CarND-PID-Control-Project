#ifndef PID_H
#define PID_H

#include <vector>
#include <chrono>

using namespace std::chrono;

class PID {
public:
  /*
  * Error
  */
  double error;

  double p_error;
  double i_error;
  double d_error;


  double prev_cte;

  /*
  * Coefficients
  */
  std::vector<double> p = {0.0, 0.0, 0.0};

  std::vector<double> dp = {0.0, 0.0, 0.0};

  int i_twiddle = 0;

  int case_twiddle = 1;

  double best_error;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte, high_resolution_clock::time_point prev_t, 
    high_resolution_clock::time_point t2, double speed);

  /*
  * Calculate the total PID error.
  */
  double TotalError();

};

#endif /* PID_H */
