header
first line is the params P, I and D
second line is the delta params used in twiddle
third line is the index indicating the current param, can be 0, 1, and 2
fourth line is the current case, that can be #1 and #2. 
  Case #1 
    uses the current dp 
  Case #2 
    test on opposite 
best_err first case is initialized with value of 0.0 on p[0]

#init values

1.0 0.0 0.0
1.0 1.0 1.0
0
1
650.0


0.0 0.0 0.0
1.0 1.0 1.0
0
1
40000.0

body
p[0] p[1] p[2] dp[0] dp[1] dp[2] tolerance err best_err

# init values
0 0 0 1 1 1 3 650.0 650.0