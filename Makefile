all:
	g++ -g -std=c++11 -o pt pt.cpp -lfftw3 -lm -lsndfile
