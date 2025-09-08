/*
 *
 *	"DHRYSTONE" Benchmark Program - SNES Adapted Version
 *
 *	Version:	C/1.1, 12/01/84 (Original)
 *			SNES Adaptation for SimpleCDemos
 *
 *	Original Author:	Reinhold P. Weicker,  CACM Vol 27, No 10, 10/84 pg. 1013
 *				Translated from ADA by Rick Richardson
 *	SNES Adaptation:	Modified for SNES homebrew development
 *
 *	This version has been adapted to work with the SNES environment:
 *	- Removed main() function to allow integration with mainBankZero.c
 *	- Added function declarations for external use
 *	- Adapted timing system for SNES frame counting
 *	- Made compatible with SNES C compilers
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#ifdef __WDC816CC__
#include <inttypes.h>
#endif


/* Define if should use homemade str* functions. */

/*#define MYSTRFNS */

#define SMALL_PROBLEM_SIZE

/* Accuracy of timings and human fatigue controlled by next two lines */
#ifdef SMALL_PROBLEM_SIZE
#define LOOPS	2000000
#else
#define LOOPS	100000000
#endif

/* External function declarations for SNES integration */
extern uint32_t snesTimeFrames(void);
extern void snesPrintResults(uint32_t loops, uint32_t time_used);

/* Function declarations for external use */
int dhrystone_main(int number_of_runs);

/* Compiler dependent options */
#undef	NOENUM			/* Define if compiler has no enum's */
#undef	NOSTRUCTASSIGN		/* Define if compiler can't assign structures */

/* define only one of the next three defines */
/*#define CLOCK*/
/*#define TIMES			/* Use times(2) time function */
/*#define TIME			/* Use time(2) time function */

/* define the granularity of your times(2) function (when used) */
/*#define HZ	50		/* times(2) returns 1/50 second (europe?) */
#define HZ	60		/* times(2) returns 1/60 second (most) */
/*#define HZ	100		/* times(2) returns 1/100 second (WECo) */

/* for compatibility with goofed up version */
/*#undef GOOF			/* Define if you want the goofed up version */

#ifdef GOOF
char	Version[] = "1.0";
#else
char	Version[] = "1.1";
#endif

#ifdef	NOSTRUCTASSIGN
#define	structassign(d, s)	memcpy(&(d), &(s), sizeof(d))
#else
#define	structassign(d, s)	d = s
#endif

#ifdef	NOENUM
#define	Ident1	1
#define	Ident2	2
#define	Ident3	3
#define	Ident4	4
#define	Ident5	5
typedef int	Enumeration;
#else
typedef enum	{Ident1, Ident2 = 10000, Ident3, Ident4, Ident5} Enumeration;
#endif

typedef int	OneToThirty;
typedef int	OneToFifty;
typedef char	CapitalLetter;
typedef char	String30[31];
typedef int	Array1Dim[51];
typedef int	Array2Dim[51][51];

struct	Record
{
	struct Record		*PtrComp;
	Enumeration		Discr;
	Enumeration		EnumComp;
	OneToFifty		IntComp;
	String30		StringComp;
};

typedef struct Record 	RecordType;
typedef RecordType *	RecordPtr;
typedef int		boolean;

#define	TRUE		1
#define	FALSE		0

#ifndef REG
#define	REG
#endif

extern Enumeration	Func1(CapitalLetter CharPar1, CapitalLetter CharPar2);
extern boolean		Func2(String30 StrParI1, String30 StrParI2);

#ifdef TIMES
#include <sys/types.h>
#include <sys/times.h>
#endif

#include <string.h>

int Proc0(int number_of_runs);
int Proc1(RecordPtr PtrParIn);
int Proc2(OneToFifty *IntParIO);
int Proc3(RecordPtr *PtrParOut);
int Proc4(void);
int Proc5(void);
int Proc6(Enumeration EnumParIn, Enumeration *EnumParOut);
int Proc7(OneToFifty IntParI1, OneToFifty IntParI2, OneToFifty *IntParOut);
int Proc8(Array1Dim Array1Par, Array2Dim Array2Par, OneToFifty IntParI1, OneToFifty IntParI2);

/* Main dhrystone function - adapted for SNES integration */
int dhrystone_main(int number_of_runs)
{
	Proc0(number_of_runs);
    return 0;
}

/*
 * Package 1
 */
int		IntGlob;
boolean		BoolGlob;
char		Char1Glob;
char		Char2Glob;
Array1Dim	Array1Glob;
Array2Dim	Array2Glob;
RecordPtr	PtrGlb;
RecordPtr	PtrGlbNext;

int Proc0(int number_of_runs)
{
	OneToFifty		IntLoc1;
	REG OneToFifty		IntLoc2;
	OneToFifty		IntLoc3;
	REG char		CharLoc;
	REG char		CharIndex;
	Enumeration	 	EnumLoc;
	String30		String1Loc;
	String30		String2Loc;
	register unsigned long	i;
	uint32_t		start_time;
	uint32_t		end_time;
	uint32_t		time_used;
	static RecordType	Record1, Record2;

	PtrGlbNext = &Record2;
	PtrGlb = &Record1;
	PtrGlb->PtrComp = PtrGlbNext;
	PtrGlb->Discr = Ident1;
	PtrGlb->EnumComp = Ident3;
	PtrGlb->IntComp = 40;
	strcpy(PtrGlb->StringComp, "DHRYSTONE PROGRAM, SOME STRING");
#ifndef	GOOF
	strcpy(String1Loc, "DHRYSTONE PROGRAM, 1'ST STRING");	/*GOOF*/
#endif
	Array2Glob[8][7] = 10;	/* Was missing in published program */

/*****************
-- Start Timer --
*****************/
	start_time = snesTimeFrames();
	for (i = 0; i < number_of_runs; ++i)
	{
		Proc5();
		Proc4();
		IntLoc1 = 2;
		IntLoc2 = 3;
		strcpy(String2Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
		EnumLoc = Ident2;
		BoolGlob = ! Func2(String1Loc, String2Loc);
		while (IntLoc1 < IntLoc2)
		{
			IntLoc3 = 5 * IntLoc1 - IntLoc2;
			Proc7(IntLoc1, IntLoc2, &IntLoc3);
			++IntLoc1;
		}
		Proc8(Array1Glob, Array2Glob, IntLoc1, IntLoc3);
		Proc1(PtrGlb);
		for (CharIndex = 'A'; CharIndex <= Char2Glob; ++CharIndex)
			if (EnumLoc == Func1(CharIndex, 'C'))
				Proc6(Ident1, &EnumLoc);
		IntLoc3 = IntLoc2 * IntLoc1;
		IntLoc2 = IntLoc3 / IntLoc1;
		IntLoc2 = 7 * (IntLoc3 - IntLoc2) - IntLoc1;
		Proc2(&IntLoc1);
	}

/*****************
-- Stop Timer --
*****************/
	end_time = snesTimeFrames();
	time_used = end_time - start_time;
	
	/* Print results using SNES display system */
	snesPrintResults(number_of_runs, time_used);
}

int Proc1(RecordPtr PtrParIn)
{
#define	NextRecord	(*(PtrParIn->PtrComp))

/*	structassign(NextRecord, *PtrGlb); */
	PtrParIn->IntComp = 5;
	NextRecord.IntComp = PtrParIn->IntComp;
	NextRecord.PtrComp = PtrParIn->PtrComp;
/*	Proc3(NextRecord.PtrComp);  */
/*	if (NextRecord.Discr == Ident1)
	{
		NextRecord.IntComp = 6;
		Proc6(PtrParIn->EnumComp, &NextRecord.EnumComp);
		NextRecord.PtrComp = PtrGlb->PtrComp;
		Proc7(NextRecord.IntComp, 10, &NextRecord.IntComp);
	}
	else
		structassign(*PtrParIn, NextRecord);
*/

#undef	NextRecord
	return 0;
}

int Proc2(OneToFifty *IntParIO)
{
	REG OneToFifty		IntLoc;
	REG Enumeration		EnumLoc;

	IntLoc = *IntParIO + 10;
	for(;;)
	{
		if (Char1Glob == 'A')
		{
			--IntLoc;
			*IntParIO = IntLoc - IntGlob;
			EnumLoc = Ident1;
		}
		if (EnumLoc == Ident1)
			break;
	}
	return 0;
}

int Proc3(RecordPtr *PtrParOut)
{
	if (PtrGlb != NULL)
		*PtrParOut = PtrGlb->PtrComp;
	else
		IntGlob = 100;
	Proc7(10, IntGlob, &PtrGlb->IntComp);
	return 0;
}

int Proc4(void)
{
	REG boolean	BoolLoc;

	BoolLoc = Char1Glob == 'A';
	BoolLoc |= BoolGlob;
	Char2Glob = 'B';
	return 0;
}

int Proc5(void)
{
	Char1Glob = 'A';
	BoolGlob = FALSE;
	return 0;
}

extern boolean Func3(Enumeration EnumParIn);

int Proc6(Enumeration EnumParIn, Enumeration *EnumParOut)
{
	*EnumParOut = EnumParIn;
	if (! Func3(EnumParIn) )
		*EnumParOut = Ident4;
	switch (EnumParIn)
	{
	case Ident1:	*EnumParOut = Ident1; break;
	case Ident2:	if (IntGlob > 100) *EnumParOut = Ident1;
			else *EnumParOut = Ident4;
			break;
	case Ident3:	*EnumParOut = Ident2; break;
	case Ident4:	break;
	case Ident5:	*EnumParOut = Ident3;
	}
	return 0;
}

int Proc7(OneToFifty IntParI1, OneToFifty IntParI2, OneToFifty *IntParOut)
{
	REG OneToFifty	IntLoc;

	IntLoc = IntParI1 + 2;
	*IntParOut = IntParI2 + IntLoc;
	return 0;
}

int Proc8(Array1Dim Array1Par, Array2Dim Array2Par, OneToFifty IntParI1, OneToFifty IntParI2)
{
	REG OneToFifty	IntLoc;
	REG OneToFifty	IntIndex;

	IntLoc = IntParI1 + 5;
	Array1Par[IntLoc] = IntParI2;
	Array1Par[IntLoc+1] = Array1Par[IntLoc];
	Array1Par[IntLoc+30] = IntLoc;
	for (IntIndex = IntLoc; IntIndex <= (IntLoc+1); ++IntIndex)
		Array2Par[IntLoc][IntIndex] = IntLoc;
	++Array2Par[IntLoc][IntLoc-1];
	Array2Par[IntLoc+20][IntLoc] = Array1Par[IntLoc];
	IntGlob = 5;
	return 0;
}

Enumeration Func1(CapitalLetter CharPar1, CapitalLetter CharPar2)
{
	REG CapitalLetter	CharLoc1;
	REG CapitalLetter	CharLoc2;

	CharLoc1 = CharPar1;
	CharLoc2 = CharLoc1;
	if (CharLoc2 != CharPar2)
		return (Ident1);
	else
		return (Ident2);
}

boolean Func2(String30 StrParI1, String30 StrParI2)
{
	REG OneToThirty		IntLoc;
	REG CapitalLetter	CharLoc;

	IntLoc = 1;
	while (IntLoc <= 1)
		if (Func1(StrParI1[IntLoc], StrParI2[IntLoc+1]) == Ident1)
		{
			CharLoc = 'A';
			++IntLoc;
		}
	if (CharLoc >= 'W' && CharLoc <= 'Z')
		IntLoc = 7;
	if (CharLoc == 'X')
		return(TRUE);
	else
	{
		if (strcmp(StrParI1, StrParI2) > 0)
		{
			IntLoc += 7;
			return (TRUE);
		}
		else
			return (FALSE);
	}
}

boolean Func3(Enumeration EnumParIn)
{
	REG Enumeration	EnumLoc;

	EnumLoc = EnumParIn;
	if (EnumLoc == Ident3) return (TRUE);
	return (FALSE);
}

#ifdef	NOSTRUCTASSIGN
memcpy(d, s, l)
register char	*d;
register char	*s;
register int	l;
{
	while (l--) *d++ = *s++;
}
#endif

#ifdef MYSTRFNS

strcpy(s1, s2)
char *s1, *s2;
{
    while (*s1++ = *s2++);
}

strcat(s1, s2)
char *s1, *s2;
{
    while (*s1++);
    s1--;
    while (*s1++ = *s2++);
}

strcmp(s1, s2)
char *s1, *s2;
{
    while (*s1++ == *s2++);
    if (*s1 == 0) {
	if (*s2 == 0) {
	    return 0;
	} else {
	    return -1;
	}
    } else {
	if (*s2 == 0) {
	    return 1;
	} else {
	    return (*(--s1) - *(--s2));
	}
    }
}

#endif
