#include <stdarg.h>
#include "rtklib.h"

#include "rnx2rtkp.h"

static const char rcsid[]="$Id: rnx2rtkp.c,v 1.1 2008/07/17 21:55:16 ttaka Exp $";

#define PROGNAME    "rnx2rtkp"

static const char *help[]={
"",
" usage: rnx2rtkp [option]... file file [...]",
"",
" Read RINEX OBS/NAV/GNAV/HNAV/CLK, SP3, SBAS message log files and ccompute ",
" receiver (rover) positions and output position solutions.",
" The first RINEX OBS file shall contain receiver (rover) observations. For the",
" relative mode, the second RINEX OBS file shall contain reference",
" (base station) receiver observations. At least one RINEX NAV/GNAV/HNAV",
" file shall be included in input files. To use SP3 precise ephemeris, specify",
" the path in the files. The extension of the SP3 file shall be .sp3 or .eph.",
" All of the input file paths can include wild-cards (*). To avoid command",
" line deployment of wild-cards, use \"...\" for paths with wild-cards.",
" Command line options are as follows ([]:default). With -k option, the",
" processing options are input from the configuration file. In this case,",
" command line options precede options in the configuration file.",
"",
" -?        print help",
" -k file   input options from configuration file [off]",
" -o file   set output file [stdout]",
" -ts ds ts start day/time (ds=y/m/d ts=h:m:s) [obs start time]",
" -te de te end day/time   (de=y/m/d te=h:m:s) [obs end time]",
" -ti tint  time interval (sec) [all]",
" -p mode   mode (0:single,1:dgps,2:kinematic,3:static,4:moving-base,",
"                 5:fixed,6:ppp-kinematic,7:ppp-static) [2]",
" -m mask   elevation mask angle (deg) [15]",
" -f freq   number of frequencies for relative mode (1:L1,2:L1+L2,3:L1+L2+L5) [2]",
" -v thres  validation threshold for integer ambiguity (0.0:no AR) [3.0]",
" -b        backward solutions [off]",
" -c        forward/backward combined solutions [off]",
" -i        instantaneous integer ambiguity resolution [off]",
" -h        fix and hold for integer ambiguity resolution [off]",
" -e        output x/y/z-ecef position [latitude/longitude/height]",
" -a        output e/n/u-baseline [latitude/longitude/height]",
" -n        output NMEA-0183 GGA sentence [off]",
" -g        output latitude/longitude in the form of ddd mm ss.ss' [ddd.ddd]",
" -t        output time in the form of yyyy/mm/dd hh:mm:ss.ss [sssss.ss]",
" -u        output time in utc [gpst]",
" -d col    number of decimals in time [3]",
" -s sep    field separator [' ']",
" -r x y z  reference (base) receiver ecef pos (m) [average of single pos]",
" -l lat lon hgt reference (base) receiver latitude/longitude/height (deg/m)",
" -y level  output soltion status (0:off,1:states,2:residuals) [0]",
" -x level  debug trace level (0:off) [0]"
};

static void printhelp(void) {
    int i;
    for (i=0;i<(int)(sizeof(help)/sizeof(*help));i++) fprintf(stderr,"%s\n",help[i]);
    exit(0);
}

int rnx2rtkp_func(rnx_opt_t *opt) {
    prcopt_t prcopt=prcopt_default;
    solopt_t solopt=solopt_default;
    filopt_t filopt={""};
    gtime_t ts={0},te={0};
    double tint=0.0,es[]={2000,1,1,0,0,0},ee[]={2000,12,31,23,59,59},pos[3];
    int i,j,n,ret;
    char *infile[MAXFILE],*outfile="";

    prcopt.mode=PMODE_KINEMA;
    prcopt.navsys=SYS_GPS|SYS_GLO;
    prcopt.refpos=1;
    prcopt.glomodear=1;
    solopt.timef=0;
    sprintf(solopt.prog,"%s ver.%s",PROGNAME,VER_RTKLIB);
    sprintf(filopt.trace,"%s.trace",PROGNAME);

    if(opt->k) {
        resetsysopts();
        if(!loadopts(opt->k,sysopts)) return -1;
        getsysopts(&prcopt,&solopt,&filopt);
    }
    if(opt->o) outfile=opt->o;
    if(opt->ts) {
        sscanf(opt->ts,"%lf/%lf/%lf %lf:%lf:%lf",ee,ee+1,ee+2,ee+3,ee+4,ee+5);
        ts=epoch2time(es);
    }
    if(opt->te) {
        sscanf(opt->ts,"%lf/%lf/%lf %lf:%lf:%lf",ee,ee+1,ee+2,ee+3,ee+4,ee+5);
        te=epoch2time(ee);
    }
    if(opt->ti) tint=atof(opt->ti);
    if(opt->p) prcopt.mode=atoi(opt->p);
    if(opt->f) prcopt.nf=atoi(opt->f);
    if(opt->m) prcopt.elmin=atof(opt->m)*D2R;
    if(opt->v) prcopt.thresar[0]=atof(opt->v);
    if(opt->s) strcpy(solopt.sep,opt->s);
    if(opt->d) solopt.timeu=atoi(opt->d);
    if(opt->b) prcopt.soltype=1;
    if(opt->c) prcopt.soltype=2;
    if(opt->i) prcopt.modear=2;
    if(opt->h) prcopt.modear=3;
    if(opt->t) solopt.timef=1;
    if(opt->u) solopt.times=TIMES_UTC;
    if(opt->e) solopt.posf=SOLF_XYZ;
    if(opt->a) solopt.posf=SOLF_ENU;
    if(opt->n) solopt.posf=SOLF_NMEA;
    if(opt->g) solopt.degf=1;
    if(opt->r) {
        prcopt.refpos=0;
        for(j=0;j<3;j++) prcopt.rb[j]=atof(opt->r[j]);
    }
    if(opt->l) {
        prcopt.refpos=0;
        for(j=0;j<3;j++) pos[j]=atof(opt->l[j]);
        for(j=0;j<2;j++) pos[j]*=D2R;
        pos2ecef(pos,prcopt.rb);
    }
    if(opt->y)solopt.sstat=atoi(opt->y);
    if(opt->x)solopt.trace=atoi(opt->x);
    for(i=0;i<8;i++) {
        if (!opt->filename[i]) break;
        infile[i]=opt->filename[i];
    }
    ret=postpos(ts,te,tint,0.0,&prcopt,&solopt,&filopt,infile,n,outfile,"","");

    if(!ret) fprintf(stderr,"%40s\r","");
    return ret;
}
