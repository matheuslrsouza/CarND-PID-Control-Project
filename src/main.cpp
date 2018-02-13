#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <fstream>
#include <chrono>

// for convenience
using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }


//twiddle function

void write_file(PID& pid) { 

  //header
  ofstream header_file;
  header_file.open("../data/twiddle_header.txt", fstream::out); //append
  header_file << pid.p[0] << " " << pid.p[1] << " " << pid.p[2] << "\n"
              << pid.dp[0] << " " << pid.dp[1] << " " << pid.dp[2] << "\n"
              << pid.i_twiddle << "\n"
              << pid.case_twiddle << "\n"
              << pid.best_error;

  header_file.close();

  //body
  //p[0] p[1] p[2] dp[0] dp[1] dp[2] tolerance err best_err
  
  ofstream body_file;
  body_file.open("../data/twiddle_body.txt", fstream::out | fstream::app); //append

  body_file << pid.p[0] << " " << pid.p[1] << " " << pid.p[2] << " " <<
            pid.dp[0] << " " << pid.dp[1] << " " << pid.dp[2] << " " <<
            (pid.dp[0] + pid.dp[1] + pid.dp[2]) << " " <<
            pid.error << " " << pid.best_error << '\n';

  body_file.close();
}

void readParams(PID& pid) {
  std::ifstream infile("../data/twiddle_header.txt");

  //reads the header (see data/readme-twiddle.txt)
  infile >> pid.p[0] >> pid.p[1] >> pid.p[2];
  infile >> pid.dp[0] >> pid.dp[1] >> pid.dp[2];
  infile >> pid.i_twiddle;
  infile >> pid.case_twiddle;
  infile >> pid.best_error;
}

void twiddle(PID& pid) {
  double tol = 0.002;
  double cur_tol = pid.dp[0] + pid.dp[1] +pid.dp[2];
  
  if (cur_tol > tol) {

    if (pid.case_twiddle == 1) {
      if (pid.error < pid.best_error) {
        pid.best_error = pid.error;
        pid.dp[pid.i_twiddle] *= 1.1; //increase dp 10%
        if (pid.i_twiddle == 2) {
          pid.i_twiddle = 0;
        } else {
          pid.i_twiddle += 1;
          //pid.i_twiddle = 2;//ignore integration param
        }
      } else {
        pid.p[pid.i_twiddle] -= 2.0*pid.dp[pid.i_twiddle]; //decrease p to opsite dp (negative)
        pid.case_twiddle = 2;
      }
    } else if (pid.case_twiddle == 2) {
      if (pid.error < pid.best_error) {
        pid.best_error = pid.error;
        pid.dp[pid.i_twiddle] *= 1.1; //increase dp 10%        
      } else {
        pid.p[pid.i_twiddle] += pid.dp[pid.i_twiddle]; //back to initial position
        pid.dp[pid.i_twiddle] *= 0.9; //decrease dp 10%
      }

      //sempre que chegamos nesse ultimo passo (case 2), devemos testar o proximo param
      if (pid.i_twiddle == 2) {
        pid.i_twiddle = 0;
      } else {
        pid.i_twiddle += 1;
        //pid.i_twiddle = 2;//ignore integration param
      }

      pid.case_twiddle = 1;
    }
  }

  write_file(pid);
}



// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  int it = 0;
  int i_twiddle_inited = 10000;
  bool init_twiddle = false;
  bool enable_twiddle = false;
  high_resolution_clock::time_point prev_t;

  // Initialize the pid variable.
  PID pid;  
  readParams(pid);

  if (pid.case_twiddle == 1 && enable_twiddle) {
    pid.p[pid.i_twiddle] += pid.dp[pid.i_twiddle];
  }

  std::cout <<"PID "<< pid.p[0] << " " << pid.p[1] << " " << pid.p[2] << "\n"
              <<"Dp "<< pid.dp[0] << " " << pid.dp[1] << " " << pid.dp[2] << "\n"
              <<"i "<< pid.i_twiddle << "\n"
              << "case #"<< pid.case_twiddle << "\n"
              << "best_err "<< pid.best_error << std::endl;  

  h.onMessage([&pid, &it, &i_twiddle_inited, &init_twiddle, &enable_twiddle, &prev_t](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          
          it+=1;

          high_resolution_clock::time_point t2 = high_resolution_clock::now();

          pid.UpdateError(cte, prev_t, t2, speed);
          double steer_value = pid.TotalError();

          //normalizing streer
          if (steer_value > 1) steer_value = 1;
          if (steer_value < -1) steer_value = -1;

          pid.prev_cte = cte;
          prev_t = t2;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.5;          
          
          double speed_limit = 30.0;

          //init twiddle when enabled
          if (it == 200 && init_twiddle == false && enable_twiddle) {
            init_twiddle = true;
            i_twiddle_inited = it;
          }

          //control the speed
          if (speed > speed_limit) {
              msgJson["throttle"] = -0.1;
          } else {
            msgJson["throttle"] = speed_limit - speed;
          }

          if (init_twiddle && it < i_twiddle_inited * 7) {
            pid.error += pow(cte, 2.0);
          } else if (it > i_twiddle_inited * 7) {            
            if (enable_twiddle) {
              std::cout << "***** error: " << pid.error << std::endl;
              twiddle(pid);
              //return -1;
            }
          }

          // DEBUG
          std::cout << "CTE: ("<< it << ") "<< cte << " Steering Value: " << steer_value << " Speed: " << speed << std::endl;
          
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
