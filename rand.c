/**********************************************************************/
/*                                                                    */
/*                           File "rand.c"                            */
/*                     Random Variate Generation                      */
/*                                                                    */
/*                                         (c) 1987  M. H. MacDougall */
/*                                                                    */
/**********************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CPU 8086           /* CPU type:  8086 or 68000 or 0 (random C
							  library function)  */

typedef double real;
#define then

#define A 16807L           /* multiplier (7**5) for 'ranf' */
#define M 2147483647L      /* modulus (2**31-1) for 'ranf' */


static long In[16]= {0L,   /* seeds for streams 1 thru 15  */
  1973272912L,  747177549L,   20464843L,  640830765L, 1098742207L,
    78126602L,   84743774L,  831312807L,  124667236L, 1172177002L,
  1124933064L, 1223960546L, 1878892440L, 1449793615L,  553303732L};

static int strm=1;         /* index of current stream */

static	char	state1[256],state2[256];

#if CPU==8086
/*-------------  UNIFORM [0, 1] RANDOM NUMBER GENERATOR  -------------*/
/*                                                                    */
/* This implementation is for Intel 8086/8 and 80286/386 CPUs using   */
/* C compilers with 16-bit short integers and 32-bit long integers.   */
/*                                                                    */
/*--------------------------------------------------------------------*/
real ranf()
  {
    short *p,*q,k; long Hi,Lo;
    /* generate product using double precision simulation  (comments  */
    /* refer to In's lower 16 bits as "L", its upper 16 bits as "H")  */
/*here*/    p=(short *) & In[strm]; Hi= *(p+1)*A;                 /* 16807*H->Hi */
    *(p+1)=0; Lo=In[strm]*A;                           /* 16807*L->Lo */
    p=(short *)&Lo; Hi+= *(p+1);    /* add high-order bits of Lo to Hi */
    q=(short *)&Hi;                       /* low-order bits of Hi->LO */
    *(p+1)= *q&0X7FFF;                               /* clear sign bit */
    k= *(q+1)<<1; if (*q&0X8000) then k++;         /* Hi bits 31-45->K */
    /* form Z + K [- M] (where Z=Lo): presubtract M to avoid overflow */
    Lo-=M; Lo+=k; if (Lo<0) then Lo+=M;
    In[strm]=Lo;
    return((real)Lo*4.656612875E-10);             /* Lo x 1/(2**31-1) */
  }
#endif

#if CPU==68000
/*-------------  UNIFORM [0, 1] RANDOM NUMBER GENERATOR  -------------*/
/*                                                                    */
/* This implementation is for Motorola 680x0 CPUs using C compilers   */
/* with 16-bit short integers and 32-bit long integers.               */
/*                                                                    */
/*--------------------------------------------------------------------*/
real ranf()
  {
    short *p,*q,k; long Hi,Lo;
    /* generate product using double precision simulation  (comments  */
    /* refer to In's lower 16 bits as "L", its upper 16 bits as "H")  */
    p=(short *)&In[strm]; Hi= *(p)*A;                   /* 16807*H->Hi */
    *(p)=0; Lo=In[strm]*A;                             /* 16807*L->Lo */
    p=(short *)&Lo; Hi+= *(p);      /* add high-order bits of Lo to Hi */
    q=(short *)&Hi;                       /* low-order bits of Hi->LO */
    *(p)= *(q+1)&0X7FFF;                             /* clear sign bit */
    k= *(q)<<1; if (*(q+1)&0X8000) then k++;       /* Hi bits 31-45->K */
    /* form Z + K [- M] (where Z=Lo): presubtract M to avoid overflow */
    Lo-=M; Lo+=k; if (Lo<0) then Lo+=M;
    In[strm]=Lo;
    return((real)Lo*4.656612875E-10);             /* Lo x 1/(2**31-1) */
  }
#endif

#if CPU==0
/*-------------  UNIFORM [0, 1] RANDOM NUMBER GENERATOR  -------------*/
/*                                                                    */
/* This implementation uses the C library function "random" available */
/* on UNIX machines.												  */
/*                                                                    */
/*--------------------------------------------------------------------*/
real ranf()
  {
	return(random()/2.147483647E9);
  }
#endif

/*--------------------  SELECT GENERATOR STREAM  ---------------------*/
stream(n)
  int n;
    { 
#if CPU==0
	  char	*initstate();
	  int	i;
#endif
	/* set stream for 1<=n<=15, return stream for n=0 */
      if ((n<0)||(n>15)) then {
		error(0,"stream Argument Error");
      }
      if (n) then {
		strm=n;
#if CPU==0
	  	/* srandom(strm); */
		for (i=0;i<=255;i++) state1[i]='\0';
		initstate(strm,state1,256);
#endif
	  }
      return(strm);
    }

/*--------------------------  SET/GET SEED  --------------------------*/
long seed(Ik,n)
  long Ik; int n;
    { /* set seed of stream n for Ik>0, return current seed for Ik=0  */
      if ((n<1)||(n>15)) then error(0,"seed Argument Error");
      if (Ik>0L) then  In[n]=Ik;
      return(In[n]);
    }

/*------------  UNIFORM [a, b] RANDOM VARIATE GENERATOR  -------------*/
real uniform(a,b)
  real a,b;
    { /* 'uniform' returns a psuedo-random variate from a uniform     */
      /* distribution with lower bound a and upper bound b.           */
      if (a>b) then error(0,"uniform Argument Error: a > b");
      return(a+(b-a)*ranf());
    }

/*--------------------  RANDOM INTEGER GENERATOR  --------------------*/
irandom(i,n)
  int i,n;
    { /* 'irandom' returns an integer equiprobably selected from the   */
      /* set of integers i, i+1, i+2, . . , n.                        */
      if (i>n) then error(0,"random Argument Error: i > n");
      n-=i; n=(n+1.0)*ranf();
      return(i+n);
    }

/*--------------  EXPONENTIAL RANDOM VARIATE GENERATOR  --------------*/
real expntl(x)
  real x;
    { /* 'expntl' returns a psuedo-random variate from a negative     */
      /* exponential distribution with mean x.                        */
      return(-x*log(ranf()));
    }

/*----------------  ERLANG RANDOM VARIATE GENERATOR  -----------------*/
real erlang(x,s)
  real x,s;
    { /* 'erlang' returns a psuedo-random variate from an erlang      */
      /* distribution with mean x and standard deviation s.           */
      int i,k; real z;
      if (s>x) then error(0,"erlang Argument Error: s > x");
      z=x/s; k=(int)z*z;
      z=1.0; for (i=0; i<k; i++) z*=ranf();
      return(-(x/k)*log(z));
    }

/*-----------  HYPEREXPONENTIAL RANDOM VARIATE GENERATION  -----------*/
real hyperx(x,s)
  real x,s;
    { /* 'hyperx' returns a psuedo-random variate from Morse's two-   */
      /* stage hyperexponential distribution with mean x and standard */
      /* deviation s, s>x.  */
      real cv,z,p;
      if (s<=x) then error(0,"hyperx Argument Error: s not > x");
      cv=s/x; z=cv*cv; p=0.5*(1.0-sqrt((z-1.0)/(z+1.0)));
      z=(ranf()>p)? (x/(1.0-p)):(x/p);
      return(-0.5*z*log(ranf()));
    }

/*-----------------  NORMAL RANDOM VARIATE GENERATOR  ----------------*/
real normal(x,s)
  real x,s;
    { /* 'normal' returns a psuedo-random variate from a normal dis-  */
      /* tribution with mean x and standard deviation s.              */
      real v1,v2,w,z1; static real z2=0.0;
      if (z2!=0.0)
        then {z1=z2; z2=0.0;}  /* use value from previous call */
        else
          {
            do
              {v1=2.0*ranf()-1.0; v2=2.0*ranf()-1.0; w=v1*v1+v2*v2;}
            while (w>=1.0);
            w=sqrt((-2.0*log(w))/w); z1=v1*w; z2=v2*w;
          }
      return(x+z1*s);
  }



