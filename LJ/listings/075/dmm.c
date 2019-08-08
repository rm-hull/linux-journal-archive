#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/soundcard.h>

#define FS_MAX                          48000
#define FS_DEF                           8000
#define BL_MIN                            100
#define BL_MAX                          20000
#define BL_DEF                           1000
#define DE_MAX                             20

struct SampleType {
  unsigned char vt; /* value at given time */
  unsigned char vr; /* value of given rank */
  unsigned int  rt; /* rank  at given time */
  unsigned int  tr; /* time  of given rank */
};

struct GUIParType {
  /* first the independent parameters      */
  int fs, bl, tau, de, steps, mode;
  /* then the dependent parameters         */
  int BlockLength;
  unsigned char     * Sample;
  struct SampleType *      s;
  double            *     lg;
};

struct StaParType {
  int    Mean, Modus;
  int    Mean1 [3][256];
  int    Mean2 [3][256][256];
  double Inf, MutInf;
  double AutoCorr[DE_MAX+1], PrFlt[DE_MAX];
  double PrErHy;
};

struct DynParType {
  double FNearN, PrErLi, PrErNo, MaxLyap;
};

struct NeighbourPairType {
  int r1, r2, dist;
};

int SoundFD=0;

int InitSoundSystem(int SampleRate) {
  int OK = -1;

  if (SoundFD > 0) close(SoundFD);
  if ((SoundFD = open("/dev/dsp", O_RDONLY)) >= 0) {
    int OSS_format    =    AFMT_U8;
    /* AFMT_S8 is better, but most drivers dont know it. */
    int OSS_channels  =          1;
    int OSS_speed     = SampleRate;

    ioctl(SoundFD, SNDCTL_DSP_RESET, 0);
    if ((ioctl(SoundFD, SNDCTL_DSP_SETFMT,   &OSS_format  ) != -1)  &&
        (OSS_format   == AFMT_U8)                                   &&
        (ioctl(SoundFD, SNDCTL_DSP_CHANNELS, &OSS_channels) != -1)  &&
        (OSS_channels == 1)                                         &&
        (ioctl(SoundFD, SNDCTL_DSP_SPEED,    &OSS_speed   ) != -1)  &&
        (OSS_speed    == SampleRate)) {
      OK = SoundFD;
    }
  }
  return OK;
}

int Reset (struct GUIParType * GUIPar) {
  int i, OK = -1;

  if (GUIPar->Sample != NULL) free(GUIPar->Sample);
  if (GUIPar->s      != NULL) free(GUIPar->s     );
  if (GUIPar->lg     != NULL) free(GUIPar->lg    );
  /* depending on mode, clear count, count2 etc. */
  GUIPar->BlockLength = GUIPar->bl * GUIPar->fs / 1000;
  GUIPar->Sample      = malloc(GUIPar->BlockLength);
  GUIPar->s           = malloc(GUIPar->BlockLength * sizeof(struct SampleType));
  GUIPar->lg          = malloc((GUIPar->BlockLength + 1) * sizeof(double));
  if ((GUIPar->Sample!=NULL) && (GUIPar->s != NULL) && (GUIPar->lg != NULL)) {
    GUIPar->lg[0] = 0;
    for (i=1; i<=GUIPar->BlockLength; i++) GUIPar->lg[i] = log((double) i);
    OK = InitSoundSystem(GUIPar->fs);
  }
  return OK;
}

int fileno( FILE *stream); /* Linux 2.2.10 simply does not know this ... */

int ReadInput(struct GUIParType * GUIPar) {
  while (1) {
    fd_set stdin_set;
    struct timeval timeout = { 0, 0 };

    FD_ZERO(& stdin_set); /* Prepare a non-blocking read */
    FD_SET(fileno(stdin), & stdin_set); /* Any parameter there ? */
    switch (select(fileno(stdin)+1,&stdin_set,NULL,NULL,&timeout)) {
      case -1: sleep(1); break;
      case  1: /* fileno(stdin) */  {
        char line[128], Name[128];
        int Value;

        fgets(line, sizeof(line), stdin);
        sscanf (line, "%s %d", Name, &Value);
        if ((strcmp(Name, "fs" ) == 0) && (Value > 0) && (Value <= FS_MAX)) {
          GUIPar->fs  = Value;
        } else if ((strcmp(Name,"bl")==0)&&(Value>=BL_MIN)&&(Value<=BL_MAX)){
          GUIPar->bl  = Value;
        } else if ((strcmp(Name, "tau") == 0) && (Value > 0)) {
          GUIPar->tau = Value;
        } else if ((strcmp(Name,"de" ) == 0)  && (Value > 0) &&
                   (Value <= DE_MAX)) {
          GUIPar->de  = Value;
        } else if ((strcmp(Name,"steps" ) == 0)  && (Value > 0)) {
          GUIPar->steps  = Value; 
        } else if ((strcmp(Name,"mode") == 0) && (Value >= 0) && (Value<=1)) {
          GUIPar->mode = Value;
        } else if (strcmp(Name, "Reset" ) == 0) {
          printf("%s", line);
        } else if (strcmp(Name, "Quit" ) == 0) {
          printf("%s", line);
          fclose(stdin);
          return -1;
        }
        if (Reset(GUIPar) < 0) return -1;
        break;
      } /* case 0 */
      default: return 0;    /* timeout, nothing to read */
    }
  } /* while */
  return 0;
}

void WriteOutput(struct GUIParType * GUIPar,
                 struct StaParType * StaPar,
                 struct DynParType * FwdDynPar,
                 struct DynParType * BwdDynPar) {
  printf("fs        %d\n", GUIPar->fs);
  printf("bl        %d\n", GUIPar->bl);
  printf("tau       %d\n", GUIPar->tau);
  printf("de        %d\n", GUIPar->de);
  printf("steps     %d\n", GUIPar->steps);
  printf("Spread    %d\n", GUIPar->s[GUIPar->BlockLength-1].vr-GUIPar->s[0].vr);
  printf("Maximum   %d\n", GUIPar->s[GUIPar->BlockLength-1].vr);
  printf("Minimum   %d\n", GUIPar->s[0].vr); 
  printf("Mean      %d\n", StaPar->Mean);
  printf("Median    %d\n", GUIPar->s[GUIPar->BlockLength/2].vr);
  printf("Modus     %d\n", StaPar->Modus);
  printf("Inf       %f\n", StaPar->Inf);
  printf("MutInf    %f\n", StaPar->MutInf); 
  printf("AutoCorr  %f\n", StaPar->AutoCorr[1]);
  printf("PrErHy    %f\n", StaPar->PrErHy);
  printf("FNearN_f  %f\n", FwdDynPar->FNearN);
  printf("FNearN_b  %f\n", BwdDynPar->FNearN);
  printf("PrErLi_f  %f\n", FwdDynPar->PrErLi);
  printf("PrErLi_b  %f\n", BwdDynPar->PrErLi);
  printf("PrErNo_f  %f\n", FwdDynPar->PrErNo);
  printf("PrErNo_b  %f\n", BwdDynPar->PrErNo);
  printf("MaxLyap_f %f\n", FwdDynPar->MaxLyap);
  printf("MaxLyap_b %f\n", BwdDynPar->MaxLyap);
  fflush(stdout);
}

void StaticParameters(struct GUIParType * GUIPar,
                      struct StaParType * StaPar) {
  int i, j;
  int count     [256];
  int count2    [256][256];
    
  { /* Initializing, sorting, building up indices */
    int nfr;
    int nextfree [256];

    StaPar->Mean = 0;
    memset (StaPar->Mean1, 0, sizeof(StaPar->Mean1));
    memset (StaPar->Mean2, 0, sizeof(StaPar->Mean2));
    memset (count,         0, sizeof(count));
    memset (count2,        0, sizeof(count2));
    for (i=0; i<GUIPar->BlockLength; i++) {
      StaPar->Mean += GUIPar->Sample[i];
      count [GUIPar->Sample[i]] ++;
      if (i >= GUIPar->tau)
        StaPar->Mean1 [0][GUIPar->Sample[i]] += GUIPar->Sample[i - GUIPar->tau];
      if (i < (GUIPar->BlockLength - GUIPar->tau))
        StaPar->Mean1 [2][GUIPar->Sample[i]] += GUIPar->Sample[i + GUIPar->tau];
      if (i >= (2 * GUIPar->tau))
        StaPar->Mean2 [0][GUIPar->Sample[i]][GUIPar->Sample[i - GUIPar->tau]] +=
                GUIPar->Sample[i - (2 * GUIPar->tau)];
      if (i < (GUIPar->BlockLength - (2 * GUIPar->tau)))
        StaPar->Mean2 [2][GUIPar->Sample[i]][GUIPar->Sample[i + GUIPar->tau]] +=
                GUIPar->Sample[i + (2 * GUIPar->tau)];
    } /* for i */
    StaPar->Mean  /= GUIPar->BlockLength;
    nextfree[0]    = 0;
    for (i=1; i<256; i++)
      nextfree [i] = nextfree[i-1] + count[i-1];
    for(i=0; i < GUIPar->BlockLength; i++) { 
      GUIPar->s[i].vt      = GUIPar->Sample[i]; 
      nfr                  = nextfree[GUIPar->Sample[i]] ++ ;
      GUIPar->s[nfr].vr    = GUIPar->Sample[i];
      GUIPar->s[nfr].tr    = i;
      GUIPar->s[i].rt      = nfr ;
    } 
  }   
  { /* Autocorrelation: which delay decorrelates best ? */ 
    double L[DE_MAX+1][DE_MAX+1];
    double E[DE_MAX+1], delta=0.0, gamma=0.0;
    int p;

    memset(StaPar->AutoCorr, 0, sizeof(StaPar->AutoCorr));
    for (i=0; i < GUIPar->BlockLength - GUIPar->tau; i++) {
      count2 [GUIPar->s[i].vt] [GUIPar->s[i + GUIPar->tau].vt] ++;
      for (j=0; (j<=GUIPar->de)&&((i+j * GUIPar->tau)<GUIPar->BlockLength);j++)
        StaPar->AutoCorr[j] +=
         ((GUIPar->s[i].vt) - StaPar->Mean) *
         ((GUIPar->s[i + j * GUIPar->tau].vt) - StaPar->Mean);
    }
    for (i=0; i<256; i++) { 
      if (count[i] == 0) {  
        StaPar->Mean1 [0][i]       = StaPar->Mean;
        StaPar->Mean1 [2][i]       = StaPar->Mean;
      } else {
        StaPar->Mean1 [0][i]      /= count[i]; 
        StaPar->Mean1 [2][i]      /= count[i];
      }
    }
    for (i=0; i<256; i++) {
      for (j=0; j<256; j++) {
        if (count2 [i][j] == 0) {
          StaPar->Mean2 [0][j][i]  = StaPar->Mean1 [0][j];
          StaPar->Mean2 [2][i][j]  = StaPar->Mean1 [2][j];
        } else {
          StaPar->Mean2 [0][j][i] /= count2 [i][j];
          StaPar->Mean2 [2][i][j] /= count2 [i][j];
        }
      }
    } /* for i */   
    /* Calculate prediction filter by inversion of the autocorrelation
     * matrix with the Levinson-Durbin recursion.
     */
    L[0][0] = 1.0;
    E[0]    = (double) StaPar->AutoCorr[0];
    for ( p = 1; p <= GUIPar->de; p++) {
      delta = 0.0;
      for(i = 0; i < p; i++)
        delta += (double) StaPar->AutoCorr[i+1] * L[p-1][i];
      if (E[p-1] * E[p-1] < FLT_EPSILON) break;
      gamma  = delta / E[p-1];
      L[p][0] = -gamma;
      for(i=1; i<p; i++)
        L[p][i] = L[p-1][i-1] - gamma * L[p-1][p-1-i];
      L[p][p] = 1.0;
      E[p] = E[p-1] * (1 - gamma*gamma);
    }
    for (i=0; i < GUIPar->de; i++)
      StaPar->PrFlt[i] = L[GUIPar->de][GUIPar->de-1-i];
    if ((StaPar->AutoCorr[0] > FLT_EPSILON) && (E[GUIPar->de] >= 0.0)) {
      for (i=1; i <= GUIPar->de; i++)
        StaPar->AutoCorr[i] /= StaPar->AutoCorr[0];
      StaPar->PrErHy         = sqrt (E[GUIPar->de] / E[0]);
    } else {
      for (i=0; i <= GUIPar->de; i++)
        StaPar->AutoCorr[i]  = 0.0;
      StaPar->PrErHy         = 1.0;
    }
  }
  { /* Mutual Information: which delay decorrelates best ? */
    StaPar->Inf    = 0.0;
    StaPar->MutInf = 0.0;
    StaPar->Modus  = 0;
    for (i=0; i<256; i++) {
      if (count[i] > 0) {
        if (count[i] > count[StaPar->Modus]) StaPar->Modus = i;
        StaPar->Inf += (double) count[i] * GUIPar->lg[count[i]];
        for (j=0; j<256; j++) {
          StaPar->MutInf +=
            (double) count2[i][j] *
            (GUIPar->lg[count2[i][j]] -
             GUIPar->lg[count[i]]     -
             GUIPar->lg[count[j]]);
        }
      }
    } 
    StaPar->Inf    -= (double) GUIPar->BlockLength *
                               GUIPar->lg[GUIPar->BlockLength];
    StaPar->MutInf += (double) GUIPar->BlockLength *
                               GUIPar->lg[GUIPar->BlockLength];
    StaPar->Inf    /= -1.0 * GUIPar->lg[2] * GUIPar->BlockLength;
    StaPar->MutInf /=  1.0 * GUIPar->lg[2] * GUIPar->BlockLength;
    if (StaPar->Inf > 0.1)
      StaPar->MutInf /= StaPar->Inf; 
    else 
      StaPar->MutInf  = 1.0;
  } 
}   

void FNN (struct GUIParType * GUIPar,
          struct NeighbourPairType * NPs, int * nn, int * fnn,
          short int span, short int direction) {
  int d, t1, t2;
  int r1 = NPs->r1;
  int r2 = NPs->r2;

  NPs->dist   = 0; 
  for (d = 1; d <= (GUIPar->de + 1); d++) {
    if (d == (GUIPar->de + 1)) (* nn) ++;
    if (abs(GUIPar->s[r1].vr - GUIPar->s[r2].vr) <= span) {
      if (d <= GUIPar->de) {
        NPs->dist += (GUIPar->s[r1].vr - GUIPar->s[r2].vr) *
                     (GUIPar->s[r1].vr - GUIPar->s[r2].vr);
        t1 = GUIPar->s[r1].tr + direction * GUIPar->tau;
        t2 = GUIPar->s[r2].tr + direction * GUIPar->tau;
        if ((t1 >= 0) && (t1 < GUIPar->BlockLength) &&
            (t2 >= 0) && (t2 < GUIPar->BlockLength)) {
          r1 = GUIPar->s[t1].rt;
          r2 = GUIPar->s[t2].rt;
        } else {
          NPs->dist = - d;
          break;
        }
      }
    } else {
      if (d == (GUIPar->de + 1)) {
        (*fnn) ++;
      } else {
        NPs->dist = - d;
        break;
      }
    }
  }
} 

void DynamicParameters(struct GUIParType * GUIPar,
                       struct StaParType * StaPar,
                       struct DynParType * DynPar,
                       short int span, short int direction) {
  struct NeighbourPairType Nx1y1 = {0, 0, 0};
  struct NeighbourPairType Nxsys = {0, 0, 0};
  int nb=5, nn=0, fnn=0, penn=0, mlnn=0, vnb=0, lpnb=0, dummy=0;
  int txs, tys;
  double PE;

  DynPar->FNearN   = 0.0;
  DynPar->PrErLi   = 0.0;
  DynPar->PrErNo   = 0.0;   
  DynPar->MaxLyap  = 0.0;
  for (Nx1y1.r1 = 0; Nx1y1.r1 < (GUIPar->BlockLength-nb); Nx1y1.r1 += 1 ) {
    /* Linear Prediction Error */
    if((GUIPar->s[Nx1y1.r1].tr - direction*(GUIPar->de-1)*(GUIPar->tau)>= 0) && 
       (GUIPar->s[Nx1y1.r1].tr - direction*(GUIPar->de-1)*(GUIPar->tau)<GUIPar->BlockLength)&&
       (GUIPar->s[Nx1y1.r1].tr + direction*            1 *(GUIPar->tau)>= 0) && 
       (GUIPar->s[Nx1y1.r1].tr + direction*(GUIPar->tau)<GUIPar->BlockLength)){
      int i;
      double PredictionError = 0.0;
      lpnb ++;
      for (i=0; i < GUIPar->de; i++)
        PredictionError += StaPar->PrFlt[i] * ( (double)
                           GUIPar->s[GUIPar->s[Nx1y1.r1].tr -
                                     direction*i*(GUIPar->tau)].vt -
                           StaPar->Mean);
      PredictionError += GUIPar->s[GUIPar->s[Nx1y1.r1].tr +
                                   direction * (GUIPar->tau)].vt -
                         StaPar->Mean;
      DynPar->PrErLi  += PredictionError * PredictionError; 
    }
    vnb = 0;
    txs = GUIPar->s[Nx1y1.r1].tr + direction * GUIPar->steps;
    PE = 0.0;
    for (Nx1y1.r2 = Nx1y1.r1 - nb; Nx1y1.r2 <= (Nx1y1.r1 + nb); Nx1y1.r2 ++) {
      if ((Nx1y1.r2  < 0)                   ||
          (Nx1y1.r2  > GUIPar->BlockLength) ||
          (Nx1y1.r1 == Nx1y1.r2)
          /*  || (abs(s[Nx1y1.r1].tr - s[Nx1y1.r2].tr) < GUIPar->tau)) */
         ) continue;
      FNN(GUIPar, & Nx1y1, &nn, &fnn, span, direction);
      if (Nx1y1.dist <= 0) continue;
      /* Nonlinear Prediction Error */
      if ((GUIPar->s[Nx1y1.r2].tr + direction*(GUIPar->tau) >=           0) &&
          (GUIPar->s[Nx1y1.r2].tr + direction*(GUIPar->tau)<GUIPar->BlockLength)
         ) {
        vnb  ++;
        /* Nach Abstand gewichten ? */
        PE += GUIPar->s[GUIPar->s[Nx1y1.r2].tr + direction * (GUIPar->tau)].vt;
      }
      /* Maximum Lyapunov Exponent: is there chaos ? */
      tys = GUIPar->s[Nx1y1.r2].tr + direction * GUIPar->steps;
      if ((txs >= 0) && (txs < GUIPar->BlockLength) && 
          (tys >= 0) && (tys < GUIPar->BlockLength)) {
        Nxsys.r1 = GUIPar->s[txs].rt;
        Nxsys.r2 = GUIPar->s[tys].rt;
        FNN(GUIPar, & Nxsys, & dummy, & dummy, span, direction);
        if ((Nx1y1.dist > 0) && (Nxsys.dist > 0)) {
          mlnn ++;
          /* lambda = log(d(x2 y2) / d(x1 y1)) */
          DynPar->MaxLyap  += log((double) Nxsys.dist / (double) Nx1y1.dist);
        } /* if */
      } /* if */
    } /* for Nx1y1.r2 */
    if (vnb>0) {
      penn ++;
      DynPar->PrErNo +=
        ((PE - vnb * GUIPar->s[GUIPar->s[Nx1y1.r1].tr +
                               direction * (GUIPar->tau)].vt) / vnb) *
        ((PE - vnb * GUIPar->s[GUIPar->s[Nx1y1.r1].tr +
                               direction * (GUIPar->tau)].vt) / vnb);
    } else if ((GUIPar->de >= 2) &&
               (GUIPar->s[Nx1y1.r1].tr >= (2 * GUIPar->tau)) &&
               (GUIPar->s[Nx1y1.r1].tr < (GUIPar->BlockLength-2*GUIPar->tau))){ 
      penn ++;
      DynPar->PrErNo +=    
        (StaPar->Mean2 [direction+1]
           [GUIPar->s[GUIPar->s[Nx1y1.r1].tr - direction * (GUIPar->tau)].vt]
           [GUIPar->s[Nx1y1.r1].vr] -
         GUIPar->s[GUIPar->s[Nx1y1.r1].tr + direction * (GUIPar->tau)].vt) *
        (StaPar->Mean2 [direction+1]
           [GUIPar->s[GUIPar->s[Nx1y1.r1].tr - direction * (GUIPar->tau)].vt]
           [GUIPar->s[Nx1y1.r1].vr] -            
         GUIPar->s[GUIPar->s[Nx1y1.r1].tr + direction * (GUIPar->tau)].vt) ;
    } else if ((GUIPar->de == 1) &&
               (GUIPar->s[Nx1y1.r1].tr >= GUIPar->tau) &&
               (GUIPar->s[Nx1y1.r1].tr < (GUIPar->BlockLength - GUIPar->tau))) {
      penn ++;
      DynPar->PrErNo +=
        (StaPar->Mean1[direction+1][GUIPar->s[Nx1y1.r1].vr] -
         GUIPar->s[GUIPar->s[Nx1y1.r1].tr + direction * (GUIPar->tau)].vt) *
        (StaPar->Mean1[direction+1][GUIPar->s[Nx1y1.r1].vr] - 
         GUIPar->s[GUIPar->s[Nx1y1.r1].tr + direction * (GUIPar->tau)].vt) ;
    }
  } /* for Nx1y1.r1 */
  if (nn > 0)
    DynPar->FNearN = (double) fnn / (double) nn;
  else
    DynPar->FNearN = 1.0;
  if ((lpnb > 0) && (StaPar->AutoCorr[0] > FLT_EPSILON))
    DynPar->PrErLi = sqrt(DynPar->PrErLi       / (double) lpnb / 
                          (StaPar->AutoCorr[0] / (double) GUIPar->BlockLength));
  else
    DynPar->PrErLi = 1.0;
  if (penn > 0)
    DynPar->PrErNo = sqrt(DynPar->PrErNo       / (double) penn /       
                          (StaPar->AutoCorr[0] / (double) GUIPar->BlockLength));
  else
    DynPar->PrErNo = 1.0;
  if (mlnn > 0)
    DynPar->MaxLyap  /= (double)mlnn * GUIPar->lg[2] * 2.0 * GUIPar->steps;
  else
    DynPar->MaxLyap  = 0.0;
}

int main (int argc, char *argv[]) {
  struct GUIParType GUIPar = { FS_DEF, BL_DEF, 1, 3, 20, 0 };
  struct StaParType StaPar;
  struct DynParType FwdDynPar;
  struct DynParType BwdDynPar;
  short int span;

  GUIPar.Sample = NULL;
  GUIPar.s      = NULL;
  GUIPar.lg     = NULL;
  if (Reset(& GUIPar) < 0) {
    fprintf (stderr, "could not initialize sound system\n");
  } else {
    while (ReadInput   (& GUIPar) == 0) {
      read(SoundFD, GUIPar.Sample, GUIPar.BlockLength);
      StaticParameters (& GUIPar, & StaPar);
      span =           (  GUIPar.s[GUIPar.BlockLength-1].vr-GUIPar.s[0].vr)/10;
      DynamicParameters(& GUIPar, & StaPar, & FwdDynPar, span,  1);
      DynamicParameters(& GUIPar, & StaPar, & BwdDynPar, span, -1);
      WriteOutput      (& GUIPar, & StaPar, & FwdDynPar, & BwdDynPar);
    }
  }
  return 0;
}
