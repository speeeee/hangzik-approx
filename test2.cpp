// see how frequency domain changes when signal is shifted in time domain with zero padding.
#include <fftw3.h>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#define N 102
#define PI 3.14159265358979323846264338

#define REAL 0
#define IMAG 1

struct Complex { double real; double imag;
                 Complex(double _real, double _imag) { real = _real; imag = _imag; }
                 Complex() {} };
double magnitude(Complex a) { return pow(pow(a.real,2.)+pow(a.imag,2.),0.5); }

template <typename T>
struct Signal { std::vector<T> dat; double amplitude; int pos;
  Signal(std::vector<T> _dat) {
    dat = _dat; pos = 0; amplitude = 1.; }
  Signal() {}
  T sig_next() { return dat[pos++]; }
  void sig_begin() { pos = 0; }
  int sig_rest() { return dat.size()-pos; } };

std::vector<Signal<Complex>> group_fft(std::vector<Signal<double>> sfnt, int maxsz) {
  std::vector<Signal<Complex>> sfnt_fft(sfnt.size());
  double *in = (double *)fftw_malloc(maxsz*sizeof(double));
  fftw_complex *out = (fftw_complex *)fftw_malloc((maxsz/2+1)*sizeof(fftw_complex));
  fftw_plan p = fftw_plan_dft_r2c_1d(maxsz,in,out,FFTW_ESTIMATE);
  for(int i=0;i<sfnt.size();i++) { for(int j=0;sfnt[i].sig_rest()!=0;j++) {
      in[j] = sfnt[i].sig_next(); printf("%g\n",in[j]); } sfnt[i].sig_begin();
    fftw_execute(p); sfnt_fft[i].dat.resize(sfnt[i].sig_rest()/2+1);
    for(int j=0;j<sfnt[i].sig_rest()/2+1;j++) {
      sfnt_fft[i].dat[j] = Complex(out[j][REAL],out[j][IMAG]); } }
  fftw_destroy_plan(p); fftw_free(in); fftw_free(out);
  return sfnt_fft; }

// input: fft of signal
void print_fft(Signal<Complex> a) { printf("FFT of signal ---- frequency magnitude ----\n\n");
  for(int i=0;i<a.dat.size();i++) { printf("%g\n",magnitude(a.dat[i])); } printf("\n");
  printf("---- phase ----\n\n");
  for(int i=0;i<a.dat.size();i++) { printf("%g\n",a.dat[i].imag); } printf("\n\n"); }

int main(int argc, char **argv) { srand(time(NULL));
/*  double *in; fftw_complex *out; fftw_plan p;
  //fftw_plan_dft_r2c_1d(int n, double *in, fftw_complex *out, unsigned flags);
  in = (double *) fftw_malloc(N*sizeof(double));
  out = (fftw_complex *) fftw_malloc(N*sizeof(fftw_complex));
  p = fftw_plan_dft_r2c_1d(N,in,out,FFTW_ESTIMATE);

  for(int i=0;i<N;i++) { in[i] = sin(2*PI*15*i/N); }

  fftw_execute(p);
  //for(int i=0;i<N;i++) { printf("%g\n",in[i]); } printf("\n");
  for(int i=0;i<N/2+1;i++) { printf("%g\n",magnitude(out[i])); }

  fftw_destroy_plan(p);
  fftw_free(in); fftw_free(out);*/

  std::vector<Signal<double>> sfnt(10,Signal<double>(std::vector<double>(N)));
  for(int i=0;i<10;i++) { for(int j=0;j<N;j++) { sfnt[i].dat[j] = cos(2*PI*15*j/N); } }
  auto sfnt_fft = group_fft(sfnt,N);
  print_fft(sfnt_fft[0]);
  //Signal a[] = { sin, square };
  /*for(int i=0;i<10;i++) { int e = rand()%10+1;
    for(int j=0;j<e;j++) { double freq = rand()%945+55; int f = rand()%2;
      for(int q=0;q<SAMPLE_RATE;q++) {
        sfnt[i].dat[q] = AMPLITUDE/e*a[f](2*M_PI*freq/SAMPLE_RATE*q); } } }*/
  return 0; }
