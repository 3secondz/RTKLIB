#include "convbin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "rtklib.h"

static const char *help[]={
"",
" Synopsys",
"",
" convbin [option ...] file", 
"",
" Description",
"",
" Convert RTCM, receiver raw data log and RINEX file to RINEX and SBAS/LEX",
" message file. SBAS message file complies with RTKLIB SBAS/LEX message",
" format. It supports the following messages or files.",
"",
" RTCM 2                : Type 1, 3, 9, 14, 16, 17, 18, 19, 22",
" RTCM 3                : Type 1002, 1004, 1005, 1006, 1010, 1012, 1019, 1020",
"                         Type 1071-1127 (MSM except for compact msg)",
" NovAtel OEMV/4,OEMStar: RANGECMPB, RANGEB, RAWEPHEMB, IONUTCB, RAWWASSFRAMEB",
" NovAtel OEM3          : RGEB, REGD, REPB, FRMB, IONB, UTCB",
" u-blox LEA-4T/5T/6T   : RXM-RAW, RXM-SFRB",
" NovAtel Superstar II  : ID#20, ID#21, ID#22, ID#23, ID#67",
" Hemisphere            : BIN76, BIN80, BIN94, BIN95, BIN96",
" SkyTraq S1315F        : msg0xDD, msg0xE0, msg0xDC",
" GW10                  : msg0x08, msg0x03, msg0x27, msg0x20",
" Javad                 : [R*],[r*],[*R],[*r],[P*],[p*],[*P],[*p],[D*],[*d],",
"                         [E*],[*E],[F*],[TC],[GE],[NE],[EN],[QE],[UO],[IO],",
"                         [WD]",
" NVS                   : BINR",
" BINEX                 : big-endian, regular CRC, forward record (0xE2)",
"                         0x01-01,0x01-02,0x01-03,0x01-04,0x01-06,0x7f-05",
" Trimble               : RT17",
" RINEX                 : OBS, NAV, GNAV, HNAV, LNAV, QNAV",
"",
" Options [default]",
"",
"     file         input receiver binary log file",
"     -ts y/m/d h:m:s  start time [all]",
"     -te y/m/d h:m:s  end time [all]",
"     -tr y/m/d h:m:s  approximated time for rtcm messages",
"     -ti tint     observation data interval (s) [all]",
"     -span span   time span (h) [all]",
"     -r format    log format type",
"                  rtcm2= RTCM 2",
"                  rtcm3= RTCM 3",
"                  nov  = NovAtel OEMV/4/6,OEMStar",
"                  oem3 = NovAtel OEM3",
"                  ubx  = ublox LEA-4T/5T/6T",
"                  ss2  = NovAtel Superstar II",
"                  hemis= Hemisphere Eclipse/Crescent",
"                  stq  = SkyTraq S1315F",
"                  javad= Javad",
"                  nvs  = NVS NV08C BINR",
"                  binex= BINEX",
"                  rt17 = Trimble RT17",
"                  rinex= RINEX",
"     -ro opt      receiver options",
"     -f freq      number of frequencies [2]",
"     -hc comment  rinex header: comment line",
"     -hm marker   rinex header: marker name",
"     -hn markno   rinex header: marker number",
"     -ht marktype rinex header: marker type",
"     -ho observ   rinex header: oberver name and agency separated by /",
"     -hr rec      rinex header: receiver number, type and version separated by /",
"     -ha ant      rinex header: antenna number and type separated by /",
"     -hp pos      rinex header: approx position x/y/z separated by /",
"     -hd delta    rinex header: antenna delta h/e/n separated by /",
"     -v ver       rinex version [2.11]",
"     -od          include doppler frequency in rinex obs [off]",
"     -os          include snr in rinex obs [off]",
"     -oi          include iono correction in rinex nav header [off]",
"     -ot          include time correction in rinex nav header [off]",
"     -ol          include leap seconds in rinex nav header [off]",
"     -scan        scan input file [off]",
"     -mask [sig[,...]] signal mask(s) (sig={G|R|E|J|S|C}L{1C|1P|1W|...})",
"     -x sat       exclude satellite",
"     -y sys       exclude systems (G:GPS,R:GLONASS,E:Galileo,J:QZSS,S:SBAS,C:BeiDou)",
"     -d dir       output directory [same as input file]",
"     -c staid     use RINEX file name convention with staid [off]",
"     -o ofile     output RINEX OBS file",
"     -n nfile     output RINEX NAV file",
"     -g gfile     output RINEX GNAV file",
"     -h hfile     output RINEX HNAV file",
"     -q qfile     output RINEX QNAV file",
"     -l lfile     output RINEX LNAV file",
"     -s sfile     output SBAS message file",
"     -trace level output trace level [off]",
"",
" If any output file specified, default output files (<file>.obs,",
" <file>.nav, <file>.gnav, <file>.hnav, <file>.qnav, <file>.lnav and",
" <file>.sbs) are used.",
"",
" If receiver type is not specified, type is recognized by the input",
" file extension as follows.",
"     *.rtcm2       RTCM 2",
"     *.rtcm3       RTCM 3",
"     *.gps         NovAtel OEMV/4/6,OEMStar",
"     *.ubx         u-blox LEA-4T/5T/6T",
"     *.log         NovAtel Superstar II",
"     *.bin         Hemisphere Eclipse/Crescent",
"     *.stq         SkyTraq S1315F",
"     *.jps         Javad",
"     *.bnx,*binex  BINEX",
"     *.rt17        Trimble RT17",
"     *.obs,*.*o    RINEX OBS"
};

static const char rcsid[]="$Id: convbin.c,v 1.1 2008/07/17 22:13:04 ttaka Exp $";

#define PRGNAME "CONVBIN"
#define TRACEFILE "convbin.trace"

static void printhelp(void) {
    int i;
    for (i=0;i<(int)(sizeof(help)/sizeof(*help));i++) fprintf(stderr,"%s\n",help[i]);
    exit(0);
}

static int set_opt(rnxopt_t *opt, option_t *input, char **ifile,
                   char **ofile, char **dir, int *trace) {
    double eps[]={1980,1,1,0,0,0},epe[]={2037,12,31,0,0,0};
    double epr[]={2010,1,1,0,0,0},span=0.0;
    int i,j,k,sat,code,nf=2,nc=2,format=-1;
    char *p,*sys,*fmt="",*paths[1],path[1024],buff[1024];

    opt->rnxver=2.11;
    opt->obstype=OBSTYPE_PR|OBSTYPE_CP;
    opt->navsys=SYS_GPS|SYS_GLO|SYS_GAL|SYS_QZS|SYS_SBS|SYS_CMP;

    for (i=0;i<6;i++) for (j=0;j<64;j++) opt->mask[i][j]='1';

    if (input->ts) {
        sscanf(input->ts,"%lf/%lf/%lf %lf:%lf:%lf",eps,eps+1,eps+2,eps+3,eps+4,eps+5);
        opt->ts=epoch2time(eps);
    }
    if (input->te) {
        sscanf(input->te,"%lf/%lf/%lf %lf:%lf:%lf",epe,epe+1,epe+2,epe+3,epe+4,epe+5);
        opt->te=epoch2time(epe);
    }
    if (input->tr) {
        sscanf(input->tr,"%lf/%lf/%lf %lf:%lf:%lf",epr,epr+1,epr+2,epr+3,epr+4,epr+5);
        opt->trtcm=epoch2time(epr);
    }
    if (input->ti) {
        opt->tint=atof(input->ti);
    }
    if (input->span) {
        span=atof(input->span);
    }
    if (input->r) {
        fmt=input->r;
    }
    if (input->ro) {
        strcpy(opt->rcvopt,input->ro);
    }
    if (input->f) {
        nf=atoi(input->f);
    }
    if (input->hc) {
        if (nc<MAXCOMMENT) strcpy(opt->comment[nc++],input->hc);
    }
    if (input->hm) {
        strcpy(opt->marker,input->hm);
    }
    if (input->hn) {
        strcpy(opt->markerno,input->hn);
    }
    if (input->ht) {
        strcpy(opt->markertype,input->ht);
    }
    if (input->ho) {
        for (j=0,p=strtok(input->ho,"/");j<2&&p;j++,p=strtok(NULL,"/")) {
            strcpy(opt->name[j],p);
        }
    }
    if (input->hr) {
        for (j=0,p=strtok(input->hr,"/");j<3&&p;j++,p=strtok(NULL,"/")) {
            strcpy(opt->rec[j],p);
        }
    }
    if (input->ha) {
        for (j=0,p=strtok(input->ha,"/");j<3&&p;j++,p=strtok(NULL,"/")) {
            strcpy(opt->ant[j],p);
        }
    }
    if (input->hp) {
        for (j=0,p=strtok(input->hp,"/");j<3&&p;j++,p=strtok(NULL,"/")) {
            opt->apppos[j]=atof(p);
        }
    }
    if (input->hd) {
        for (j=0,p=strtok(input->hd,"/");j<3&&p;j++,p=strtok(NULL,"/")) {
            opt->antdel[j]=atof(p);
        }
    }
    if (input->v) {
        opt->rnxver=atof(input->v);
    }
    if (input->od) {
        opt->obstype|=OBSTYPE_DOP;
    }
    if (input->os) {
        opt->obstype|=OBSTYPE_SNR;
    }
    if (input->oi) {
        opt->outiono=1;
    }
    if (input->ot) {
        opt->outtime=1;
    }
    if (input->ol) {
        opt->outleaps=1;
    }
    if (input->scan) {
        opt->scanobs=1;
    }
    if (input->mask) {
        for (j=0;j<6;j++) for (k=0;k<64;k++) opt->mask[j][k]='0';
        strcpy(buff,input->mask);
        for (p=strtok(buff,",");p;p=strtok(NULL,",")) {
            if (strlen(p)<4||p[1]!='L') continue;
            if      (p[0]=='G') j=0;
            else if (p[0]=='R') j=1;
            else if (p[0]=='E') j=2;
            else if (p[0]=='J') j=3;
            else if (p[0]=='S') j=4;
            else if (p[0]=='C') j=5;
            else continue;
            if ((code=obs2code(p+2,NULL))) {
                opt->mask[j][code-1]='1';
            }
        }
    }
    if (input->x) {
        if ((sat=satid2no(input->x))) opt->exsats[sat-1]=1;
    }
    if (input->y) {
        sys=input->y;
        if      (!strcmp(sys,"G")) opt->navsys&=~SYS_GPS;
        else if (!strcmp(sys,"R")) opt->navsys&=~SYS_GLO;
        else if (!strcmp(sys,"E")) opt->navsys&=~SYS_GAL;
        else if (!strcmp(sys,"J")) opt->navsys&=~SYS_QZS;
        else if (!strcmp(sys,"S")) opt->navsys&=~SYS_SBS;
        else if (!strcmp(sys,"C")) opt->navsys&=~SYS_CMP;
    }
    if (input->d) {
        *dir=input->d;
    }
    if (input->c) {
        strcpy(opt->staid,input->c);
    }
    if (input->o) {
        ofile[0]=input->o;
    }
    if (input->n) {
        ofile[1]=input->n;
    }
    if (input->g) {
        ofile[2]=input->g;
    }
    if (input->h) {
        ofile[3]=input->h;
    }
    if (input->q) {
        ofile[4]=input->q;
    }
    if (input->l) {
        printf("test3\n");
        ofile[5]=input->l;
    }
    if (input->s) {
        ofile[6]=input->s;
    }
    if (input->trace) {
        *trace=input->trace;
    }
    if (input->filename) {
        *ifile = input->filename;
    }

    if (span>0.0&&opt->ts.time) {
        opt->te=timeadd(opt->ts,span*3600.0-1e-3);
    }
    if (nf>=1) opt->freqtype|=FREQTYPE_L1;
    if (nf>=2) opt->freqtype|=FREQTYPE_L2;
    if (nf>=3) opt->freqtype|=FREQTYPE_L5;
    if (nf>=4) opt->freqtype|=FREQTYPE_L6;
    if (nf>=5) opt->freqtype|=FREQTYPE_L7;
    if (nf>=6) opt->freqtype|=FREQTYPE_L8;

    if (*fmt) {
        printf("format\n");
        if      (!strcmp(fmt,"rtcm2")) format=STRFMT_RTCM2;
        else if (!strcmp(fmt,"rtcm3")) format=STRFMT_RTCM3;
        else if (!strcmp(fmt,"nov"  )) format=STRFMT_OEM4;
        else if (!strcmp(fmt,"oem3" )) format=STRFMT_OEM3;
        else if (!strcmp(fmt,"ubx"  )) format=STRFMT_UBX;
        else if (!strcmp(fmt,"ss2"  )) format=STRFMT_SS2;
        else if (!strcmp(fmt,"hemis")) format=STRFMT_CRES;
        else if (!strcmp(fmt,"stq"  )) format=STRFMT_STQ;
        else if (!strcmp(fmt,"javad")) format=STRFMT_JAVAD;
        else if (!strcmp(fmt,"nvs"  )) format=STRFMT_NVS;
        else if (!strcmp(fmt,"binex")) format=STRFMT_BINEX;
        else if (!strcmp(fmt,"rt17" )) format=STRFMT_RT17;
        else if (!strcmp(fmt,"rinex")) format=STRFMT_RINEX;
    } else {
        paths[0] = path;
        if  (!(p=strrchr(*ifile,'.'))) return -1;
        if      (!strcmp(p,".rtcm2")) format=STRFMT_RTCM2;
        else if (!strcmp(p,".rtcm3")) format=STRFMT_RTCM3;
        else if (!strcmp(p,".gps"  )) format=STRFMT_OEM4;
        else if (!strcmp(p,".ubx"  )) format=STRFMT_UBX;
        else if (!strcmp(p,".log"  )) format=STRFMT_SS2;
        else if (!strcmp(p,".bin"  )) format=STRFMT_CRES;
        else if (!strcmp(p,".stq"  )) format=STRFMT_STQ;
        else if (!strcmp(p,".jps"  )) format=STRFMT_JAVAD;
        else if (!strcmp(p,".bnx"  )) format=STRFMT_BINEX;
        else if (!strcmp(p,".binex")) format=STRFMT_BINEX;
        else if (!strcmp(p,".rt17" )) format=STRFMT_RT17;
        else if (!strcmp(p,".obs"  )) format=STRFMT_RINEX;
        else if (!strcmp(p+3,"o"   )) format=STRFMT_RINEX;
        else if (!strcmp(p+3,"O"   )) format=STRFMT_RINEX;
    }
    return format;
}

static int convbin(int format, rnxopt_t *opt, const char *ifile, char **file,
                   char *dir)
{
    int i,def;
    char work[1024],ofile_[7][1024],*ofile[7],*p;
    char *extnav=opt->rnxver<=2.99||opt->navsys==SYS_GPS?"N":"P";
    char *extlog=format==STRFMT_LEXR?"lex":"sbs";

    def=!file[0]&&!file[1]&&!file[2]&&!file[3]&&!file[4]&&!file[5]&&!file[6];
    for (i=0;i<7;i++) ofile[i]=ofile_[i];

    if (file[0]) strcpy(ofile[0],file[0]);
    else if (*opt->staid) {
        strcpy(ofile[0],"%r%n0.%yO");
    }
    else if (def) {
        strcpy(ofile[0],ifile);
        if ((p=strrchr(ofile[0],'.'))) strcpy(p,".obs");
        else strcat(ofile[0],".obs");
    }
    if (file[1]) strcpy(ofile[1],file[1]);
    else if (*opt->staid) {
        strcpy(ofile[1],"%r%n0.%y");
        strcat(ofile[1],extnav);
    }
    else if (def) {
        strcpy(ofile[1],ifile);
        if ((p=strrchr(ofile[1],'.'))) strcpy(p,".nav");
        else strcat(ofile[1],".nav");
    }
    if (file[2]) strcpy(ofile[2],file[2]);
    else if (opt->rnxver<=2.99&&*opt->staid) {
        strcpy(ofile[2],"%r%n0.%yG");
    }
    else if (opt->rnxver<=2.99&&def) {
        strcpy(ofile[2],ifile);
        if ((p=strrchr(ofile[2],'.'))) strcpy(p,".gnav");
        else strcat(ofile[2],".gnav");
    }
    if (file[3]) strcpy(ofile[3],file[3]);
    else if (opt->rnxver<=2.99&&*opt->staid) {
        strcpy(ofile[3],"%r%n0.%yH");
    }
    else if (opt->rnxver<=2.99&&def) {
        strcpy(ofile[3],ifile);
        if ((p=strrchr(ofile[3],'.'))) strcpy(p,".hnav");
        else strcat(ofile[3],".hnav");
    }
    if (file[4]) strcpy(ofile[4],file[4]);
    else if (opt->rnxver<=2.99&&*opt->staid) {
        strcpy(ofile[4],"%r%n0.%yQ");
    }
    else if (opt->rnxver<=2.99&&def) {
        strcpy(ofile[4],ifile);
        if ((p=strrchr(ofile[4],'.'))) strcpy(p,".qnav");
        else strcat(ofile[4],".qnav");
    }
    if (file[5]) strcpy(ofile[5],file[5]);
    else if (*opt->staid) {
        strcpy(ofile[5],"%r%n0.%yL");
    }
    else if (opt->rnxver<=2.99&&def) {
        strcpy(ofile[5],ifile);
        if ((p=strrchr(ofile[5],'.'))) strcpy(p,".lnav");
        else strcat(ofile[5],".qnav");
    }
    if (file[6]) strcpy(ofile[6],file[6]);
    else if (*opt->staid) {
        strcpy(ofile[6],"%r%n0_%y.");
        strcat(ofile[6],extlog);
    }
    else if (def) {
        strcpy(ofile[6],ifile);
        if ((p=strrchr(ofile[6],'.'))) strcpy(p,".");
        else strcat(ofile[6],".");
        strcat(ofile[6],extlog);
    }

    for (i=0;i<7;i++) {
        if (!*dir||!*ofile[i]) continue;
        if ((p=strrchr(ofile[i],FILEPATHSEP))) strcpy(work,p+1);
        else strcpy(work,ofile[i]);
        sprintf(ofile[i],"%s%c%s",dir,FILEPATHSEP,work);
    }
    fprintf(stderr,"input file  : %s (%s)\n",ifile,formatstrs[format]);

    if (*ofile[0]) fprintf(stderr,"->rinex obs : %s\n",ofile[0]);
    if (*ofile[1]) fprintf(stderr,"->rinex nav : %s\n",ofile[1]);
    if (*ofile[2]) fprintf(stderr,"->rinex gnav: %s\n",ofile[2]);
    if (*ofile[3]) fprintf(stderr,"->rinex hnav: %s\n",ofile[3]);
    if (*ofile[4]) fprintf(stderr,"->rinex qnav: %s\n",ofile[4]);
    if (*ofile[5]) fprintf(stderr,"->rinex lnav: %s\n",ofile[5]);
    if (*ofile[6]) fprintf(stderr,"->sbas log  : %s\n",ofile[6]);

    if (!convrnx(format,opt,ifile,ofile)) {
        fprintf(stderr,"\n");
        return -1;
    }
    fprintf(stderr,"\n");
    return 0;
}

int convbin_func(option_t *option) {
    rnxopt_t opt={{0}};
    int format,trace=0,stat;
    char *ifile="",*ofile[7]={0},*dir="";

    format=set_opt(&opt,option,&ifile,ofile,&dir,&trace);
    if (!*ifile) {
        fprintf(stderr,"no input file\n");
        return -1;
    }
    if (format<0) {
        fprintf(stderr,"input format can not be recognized\n");
        return -1;
    }
    sprintf(opt.prog,"%s %s",PRGNAME,VER_RTKLIB);
    sprintf(opt.comment[0],"log: %-55.55s",ifile);
    sprintf(opt.comment[1],"format: %s",formatstrs[format]);
    if (*opt.rcvopt) {
        strcat(opt.comment[1],", option: ");
        strcat(opt.comment[1],opt.rcvopt);
    }
    if (trace>0) {
        traceopen(TRACEFILE);
        tracelevel(trace);
    }
    stat=convbin(format,&opt,ifile,ofile,dir);

    traceclose();

    return stat;
}