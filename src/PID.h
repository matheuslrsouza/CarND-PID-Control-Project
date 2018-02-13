#ifndef PID_H
#define PID_H

#include <vector>

class PID {
public:
  /*
  * Error
  */
  double error;


  double prev_cte;
  double integral_cte;

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
};

#endif /* PID_H */
