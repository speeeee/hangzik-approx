// modeled after example program make_sine.c of libsndfile.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>

#include <sndfile.h>
#include <fftw3.h>

#include <vector>
#include <functional>

#ifndef	M_PI
#define	M_PI 3.14159265358979323846264338
#endif

#define	SAMPLE_RATE  44100
#define	SAMPLE_COUNT (SAMPLE_RATE * 10) /* 10 seconds of audio sampled. */
#define	AMPLITUDE    (1.0 * 0x7F000000)
#define	FREQ         (55.0 / SAMPLE_RATE)

template <typename T>
inline T signum(T a) { return (a>0)-(a<0); }

typedef std::vector<int> RSignal;
typedef std::function<double(int)> Signal;
typedef std::vector<Signal> SSignal;
double square(double a) { return signum(sin(a)); }
double saw(double a, double freq) { return 2*(a*freq-floor(0.5+a*freq));
  /*-2./M_PI*atan(1./tan(a*M_PI/freq));*/
  /*2*fmod(a/freq,1.)-1.;*/ }
double triangle(double a, double freq) { return 2*abs(saw(a,freq))-1; }

typedef std::function<RSignal(std::vector<SSignal>, int, int)> ElemSigFont;

// sum expects all signals to be normalized.
double sin_sum(int amplitude, int index, std::vector<Signal> a) { double ret = 0;
  for(int i=0;i<a.size();i++) { ret += (amplitude/a.size())*a[i](index); } return ret; }

int clampl(int a, int c) { return a<c?c:a; }

int main (void) { SNDFILE *file; SNDFILE *ifile; srand(time(NULL));
  SF_INFO osfinfo; SF_INFO isfinfo;

  memset(&osfinfo, 0, sizeof(osfinfo));
  memset(&osfinfo, 0, sizeof(isfinfo));

  osfinfo.samplerate = SAMPLE_RATE;
  osfinfo.frames = SAMPLE_COUNT;
  osfinfo.channels = 1;
  osfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24);

  /* reading file and performing Fourier transform for analysis. */
  if(!(ifile = sf_open("isine.wav", SFM_WRITE, &isfinfo))) {
    printf("ERROR: Not able to open input file.  'isine.wav' may not exist.\n"); return 1; }
  std::vector<double> buf(SAMPLE_COUNT);
  int sz = sf_read_double(ifile, &buf[0], SAMPLE_COUNT); buf.resize(sz);
  fftw_complex *out = (fftw_complex *)fftw_malloc(buf.size()*sizeof(fftw_complex));
  fftw_plan p = fftw_plan_dft_r2c_1d(buf.size(),&buf[0],out,FFTW_ESTIMATE);

  fftw_execute(p); fftw_destroy_plan(p); fftw_free(out);
  sf_close(ifile);

  /* begin working with output. */
  if(!(file = sf_open ("sine.wav", SFM_WRITE, &osfinfo))) {
    printf ("ERROR: Not able to open output file.\n"); return 1; }

  // TODO: make actual "play-key" function, which has a duration, a delay, and an amplitude.
  // very rough idea of what is for later.  this "font" will be used to approximate more complex
  //   signlas.
  std::vector<RSignal> sfnt(10,std::vector<int>(SAMPLE_RATE));
  Signal a[] = { sin, square };
  for(int i=0;i<10;i++) { int e = rand()%10+1;
    for(int j=0;j<e;j++) { double freq = rand()%945+55; int f = rand()%2;
      for(int q=0;q<SAMPLE_RATE;q++) {
        sfnt[i][q] = AMPLITUDE/e*a[f](2*M_PI*freq/SAMPLE_RATE*q); } } }

  for(int i=0;i<10;i++) {
    if(sf_write_int(file, &(sfnt[i][0]), osfinfo.channels*SAMPLE_RATE)!=osfinfo.channels*SAMPLE_RATE) {
      puts(sf_strerror(file)); }
    /*sf_seek(file,SAMPLE_RATE,SEEK_SET);*/ }

  sf_close(file);
  return 0; }
