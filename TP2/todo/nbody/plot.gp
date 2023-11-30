set term png size 1900,1000

set grid

set ylabel "Latency in cycles"

set xlabel "Simulation iteration"

plot "out0.dat" w lp "C version", "out0_sd.dat" w lp "SSE scalar", "out0_pd.dat" w lp "SSE packed"