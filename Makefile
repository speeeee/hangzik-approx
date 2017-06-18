all:
	g++ -g -std=c++11 -o pt pt.cpp -lfftw3 -lm -lsndfile
fft:
	g++ -g -std=c++11 -o test2 test2.cpp -lfftw3 -lm
