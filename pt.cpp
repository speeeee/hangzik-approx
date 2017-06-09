// modeled after example program make_sine.c of libsndfile.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>

#include <sndfile.h>

#include <vector>
#include <functional>

#ifndef	M_PI
#define	M_PI 3.14159265358979323846264338
#endif

#define	SAMPLE_RATE  44100
#define	SAMPLE_COUNT (SAMPLE_RATE * 4)	/* 4 seconds */
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

int main (void) { SNDFILE *file; srand(time(NULL));
  SF_INFO osfinfo;
  std::vector<int> buffer(SAMPLE_COUNT);

  memset (&osfinfo, 0, sizeof(osfinfo));

  osfinfo.samplerate = SAMPLE_RATE;
  osfinfo.frames = SAMPLE_COUNT;
  osfinfo.channels = 1;
  osfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24);

  if (! (file = sf_open ("sine.wav", SFM_WRITE, &osfinfo))) {
    printf ("ERROR: Not able to open output file.\n"); return 1; }

  // very rough idea of what is for later.  this "font" will be used to approximate more complex
  //   signlas.
  std::vector<RSignal> sfnt(10,std::vector<int>(SAMPLE_RATE));
  for(int i=0;i<10;i++) { Signal a[] = { sin, square }; int e = rand()%10+1;
    for(int j=0;j<e;j++) { double freq = rand()%560+440; int f = rand()%2;
      for(int q=0;q<SAMPLE_RATE;q++) {
        sfnt[i][q] = AMPLITUDE/e*a[f](2*M_PI*freq/SAMPLE_RATE*q); } } }

  if (osfinfo.channels == 1) {
    for(int i = 0, amp = AMPLITUDE; i < SAMPLE_COUNT; i++) {
      // not quite correct for decay of piano.
      // following line modifies 'amp' in rhs.
      //buffer[i] = (amp = clampl(amp-0x7F00,0))/2*(sin(FREQ*2*i*M_PI)+sin(220./SAMPLE_RATE*2*i*M_PI));
      /*buffer[i] = sin_sum(amp = clampl(amp-0x7F00,0),i
        , { [](int q) { return sin(FREQ*2*q*M_PI); },
            [](int q) { return square(q*FREQ*2*M_PI); } }); } }*/
      buffer[i] = sfnt[0][i%SAMPLE_RATE]; } }
      //std::vector<Sin> a; for(int t=0;t<10;t++) {
      //  a.push_back([t](int q) { return sin(440./pow(1.1,t)/SAMPLE_RATE*2*q*M_PI); }); }*/
      //buffer[i] = sin_sum(amp = clampl(amp-0x7F00,0),i,a); } }

  if (sf_write_int(file, &buffer[0], osfinfo.channels*SAMPLE_COUNT)!=osfinfo.channels*SAMPLE_COUNT) {
		puts(sf_strerror(file)); }

  sf_close(file);
  return 0; }
