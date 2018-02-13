# PID Controller

## Hyperparameters

The final values for the PID were `2.34547`, `0.001` and `0.1`

To choose the hyperparameters I used the twiddle algorithm. I did not find a way to run the twiddle without having to run the simulator, so it was necessary to run step by step manually using the simulator, which made the process very slow.
To implement the twiddle I had to write each step in a file so that when the simulator was restarted, the parameters could be recovered. See file `data/readme-twiddle.txt` and the method `twiddle` on `main.cpp`

I had to split the algorithm in two cases:

**Case #1:** Processes the current parameter according to `dp`values and verifies that the error has decreased

**Case #2:** Processes the opposite side, see the plots for a better understanding

About 200 iterations were executed using the twiddle, however after execution 50 the error stopped improving

<img src='/plots/pid_controll2.png'/>

<img src='/plots/cte.png'/>

Although the twiddle did its job, when running again with the best parameter selected, I got a different error value than the one obtained by the twiddle. I believe that this can happen by the variance of the time in which the values are sent by the simulator (`delta_t`), making the speed and position of the vehicle also oscillate

## P parameter

The `P` parameter is responsible for driving the car back to the trajectory, the higher the value, the faster the car will be directed to the trajectory. Below are 2 videos using the values `0.1` and` 3`

[![p=0.1](https://img.youtube.com/vi/0LmdlgHjTvI/0.jpg)](https://youtu.be/0LmdlgHjTvI)

[![p=3](https://img.youtube.com/vi/a6WqhT-X52E/0.jpg)](https://youtu.be/a6WqhT-X52E)

## I parameter

The `I` parameter was selected manually, because in the execution of the twiddle I noticed that high values made the error increase. To get to the value `0.001`, I started with` 0.1`, then `0.01` until I reached this value, when I noticed a slight improvement in the error. This parameter corrects the **Systematic Bias** problem that is a possible misalignment of the car, observing if the CTE is not lowering for a long time, in case this occurs, the integral of the CTE gets higher and higher, causing the error to decrease.

## D parameter

This parameter is responsible for smoothing the direction to the correct path. Below are 2 videos, the first uses only the parameter `P` with value equals `3` and the second uses the same `P`, and `D` with value equals `0.5`

[![p=3](https://img.youtube.com/vi/a6WqhT-X52E/0.jpg)](https://youtu.be/a6WqhT-X52E)

[![p=3, d=0.5](https://img.youtube.com/vi/KmKV1KW_3_A/0.jpg)](https://youtu.be/KmKV1KW_3_A)

## Twiddle Run Range

For the execution of the twiddle I started to calculate the error from iteration 200 to 1400. This run range was chosen because there is a straighter stretch and there are also curves.

## Speed

In order for the car to complete the lap at maximum speed, I added the speed to the equation as follows

`double diff_cte = (cte - pid.prev_cte) / (delta_t / speed);`

This makes the higher the speed of the vehicle, the more relevant the value of the parameter `D`

## Complete lap video 

Below is a video of the car making a complete lap, using the selected parameters.
I had to record using the phone, because when I tried to use QuickTime to record the screen, the performance was compromised and the car could not complete the lap.

[![complete lap](https://img.youtube.com/vi/PzIuwHrB5co/0.jpg)](https://youtu.be/PzIuwHrB5co)