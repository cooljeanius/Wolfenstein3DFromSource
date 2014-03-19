/* tab setting : 4
 * fmopl.c
 * FM OPL2 synth
 *
 * Copyright (C) 1999, 2000 Tatsuyuki Satoh, MultiArcadeMachineEmulator (MAME)
 * development
 * Modified for Wolfenstein 3D by Steven Fuller
 * Future Modifications by DarkOne for WolfGL! (wolfgl.narod.ru)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#ifdef __FMOPL_H_
# warning "fmopl.h was already included, including it again will have no effect."
#endif /* __FMOPL_H_*/
#include "fmopl.h"
#ifndef __FMOPL_H_
# warning "fmopl.h was not actually included."
#endif /* !__FMOPL_H_*/

#ifndef INLINE
# define INLINE __inline
#endif /* !INLINE */

/* where is the implementation of this prototype? */
void logerror(const char *text, ...);

#ifndef PI
# define PI 3.14159265358979323846
#endif /* !PI */

/* -------------------- for debug --------------------- */
#ifndef OPL_OUTPUT_LOG
# define OPL_OUTPUT_LOG 1
#endif /* !OPL_OUTPUT_LOG */
#ifdef OPL_OUTPUT_LOG
	static FILE *opl_dbg_fp = NULL;
	static FM_OPL *opl_dbg_opl[16];
	static int opl_dbg_maxchip, opl_dbg_chip;
#endif /* OPL_OUTPUT_LOG */

/* -------------------- preliminary define section --------------------- */
/* attack/decay rate */
#define OPL_ARRATE     141280  /* RATE 4 =  2826.24ms @ 3.6MHz */
#define OPL_DRRATE    1956000  /* RATE 4 = 39280.64ms @ 3.6MHz */

#define FREQ_BITS 24			/* frequency turn          */

/* counter bits = 20, octave 7 */
#define FREQ_RATE   (1<<(FREQ_BITS-20))
#define TL_BITS    (FREQ_BITS+2)

/* final output shift, limit minimum and maximum */
#define OPL_OUTSB   (TL_BITS+3-16)		/* OPL output final shift 16bit */
#define OPL_MAXOUT (0x7fff<<OPL_OUTSB)
#define OPL_MINOUT (-0x8000<<OPL_OUTSB)

/* -------------------- quality selection --------------------- */

/* sinwave entries */
/* used static memory = SIN_ENT * 4 (byte) */
#define SIN_ENT 2048

/* output level entries (envelope,sinwave) */
/* envelope counter lower bits */
#define ENV_BITS 16
/* envelope output entries */
#define EG_ENT   4096
/* used dynamic memory = EG_ENT*4*4(byte)or EG_ENT*6*4(byte) */
/* used static  memory = EG_ENT*4 (byte)                     */

#define EG_OFF   ((2*EG_ENT)<<ENV_BITS)  /* OFF          */
#define EG_DED   EG_OFF
#define EG_DST   (EG_ENT<<ENV_BITS)      /* DECAY  START */
#define EG_AED   EG_DST
#define EG_AST   0                       /* ATTACK START */

#define EG_STEP (96.0/EG_ENT) /* OPL is 0.1875 dB step  */

/* LFO table entries */
#define VIB_ENT 512
#define VIB_SHIFT (32-9)
#define AMS_ENT 512
#define AMS_SHIFT (32-9)

#define VIB_RATE 256

/* -------------------- local defines , macros --------------------- */

/* register number to channel number, slot offset */
#define SLOT1 0
#define SLOT2 1

/* envelope phase */
#define ENV_MOD_RR  0x00
#define ENV_MOD_DR  0x01
#define ENV_MOD_AR  0x02

/* -------------------- tables --------------------- */
#ifdef HAVE_PRAGMA_WARNING /* TODO: actually implement a check for this */
# pragma warning(disable:4244) // explicit roundoff (may need fix!) vvvv
#endif /* HAVE_PRAGMA_WARNING */
static const int slot_array[32]=
{
	 0, 2, 4, 1, 3, 5,-1,-1,
	 6, 8,10, 7, 9,11,-1,-1,
	12,14,16,13,15,17,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1
};

/* key scale level */
/* table is 3dB/OCT , DV converts this in TL step at 6dB/OCT */
#define DV (EG_STEP/2)
static const UINT32 KSL_TABLE[8*16]=
{
	/* OCT 0 */
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV),
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV),
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV),
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV),
	/* OCT 1 */
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV),
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV),
 (UINT32)(0.000/DV), (UINT32)(0.750/DV), (UINT32)(1.125/DV), (UINT32)(1.500/DV),
 (UINT32)(1.875/DV), (UINT32)(2.250/DV), (UINT32)(2.625/DV), (UINT32)(3.000/DV),
	/* OCT 2 */
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV),
 (UINT32)(0.000/DV), (UINT32)(1.125/DV), (UINT32)(1.875/DV), (UINT32)(2.625/DV),
 (UINT32)(3.000/DV), (UINT32)(3.750/DV), (UINT32)(4.125/DV), (UINT32)(4.500/DV),
 (UINT32)(4.875/DV), (UINT32)(5.250/DV), (UINT32)(5.625/DV), (UINT32)(6.000/DV),
	/* OCT 3 */
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(1.875/DV),
 (UINT32)(3.000/DV), (UINT32)(4.125/DV), (UINT32)(4.875/DV), (UINT32)(5.625/DV),
 (UINT32)(6.000/DV), (UINT32)(6.750/DV), (UINT32)(7.125/DV), (UINT32)(7.500/DV),
 (UINT32)(7.875/DV), (UINT32)(8.250/DV), (UINT32)(8.625/DV), (UINT32)(9.000/DV),
	/* OCT 4 */
 (UINT32)(0.000/DV), (UINT32)(0.000/DV), (UINT32)(3.000/DV), (UINT32)(4.875/DV),
 (UINT32)(6.000/DV), (UINT32)(7.125/DV), (UINT32)(7.875/DV), (UINT32)(8.625/DV),
 (UINT32)(9.000/DV), (UINT32)(9.750/DV),(UINT32)(10.125/DV),(UINT32)(10.500/DV),
(UINT32)(10.875/DV),(UINT32)(11.250/DV),(UINT32)(11.625/DV),(UINT32)(12.000/DV),
	/* OCT 5 */
 (UINT32)(0.000/DV), (UINT32)(3.000/DV), (UINT32)(6.000/DV), (UINT32)(7.875/DV),
 (UINT32)(9.000/DV),(UINT32)(10.125/DV),(UINT32)(10.875/DV),(UINT32)(11.625/DV),
(UINT32)(12.000/DV),(UINT32)(12.750/DV),(UINT32)(13.125/DV),(UINT32)(13.500/DV),
(UINT32)(13.875/DV),(UINT32)(14.250/DV),(UINT32)(14.625/DV),(UINT32)(15.000/DV),
	/* OCT 6 */
 (UINT32)(0.000/DV), (UINT32)(6.000/DV), (UINT32)(9.000/DV),(UINT32)(10.875/DV),
(UINT32)(12.000/DV),(UINT32)(13.125/DV),(UINT32)(13.875/DV),(UINT32)(14.625/DV),
(UINT32)(15.000/DV),(UINT32)(15.750/DV),(UINT32)(16.125/DV),(UINT32)(16.500/DV),
(UINT32)(16.875/DV),(UINT32)(17.250/DV),(UINT32)(17.625/DV),(UINT32)(18.000/DV),
	/* OCT 7 */
 (UINT32)(0.000/DV), (UINT32)(9.000/DV),(UINT32)(12.000/DV),(UINT32)(13.875/DV),
(UINT32)(15.000/DV),(UINT32)(16.125/DV),(UINT32)(16.875/DV),(UINT32)(17.625/DV),
(UINT32)(18.000/DV),(UINT32)(18.750/DV),(UINT32)(19.125/DV),(UINT32)(19.500/DV),
(UINT32)(19.875/DV),(UINT32)(20.250/DV),(UINT32)(20.625/DV),(UINT32)(21.000/DV)
};
#undef DV

/* sustain level table (3db per step) */
/* 0 - 15: 0, 3, 6, 9,12,15,18,21,24,27,30,33,36,39,42,93 (dB)*/
#define SC(db) (db*((3/EG_STEP)*(1<<ENV_BITS)))+EG_DST
static const INT32 SL_TABLE[16]=
{
	(INT32)SC(0), (INT32)SC(1), (INT32)SC(2), (INT32)SC(3), (INT32)SC(4),
	(INT32)SC(5), (INT32)SC(6), (INT32)SC(7), (INT32)SC(8), (INT32)SC(9),
	(INT32)SC(10),(INT32)SC(11),(INT32)SC(12),(INT32)SC(13),(INT32)SC(14),
	(INT32)SC(31)
};
#undef SC

#define TL_MAX (EG_ENT*2) /* limit(tl + ksr + envelope) + sinwave */
/* TotalLevel : 48 24 12  6  3 1.5 0.75 (dB) */
/* TL_TABLE[ 0      to TL_MAX          ] : plus  section */
/* TL_TABLE[ TL_MAX to TL_MAX+TL_MAX-1 ] : minus section */
static INT32 *TL_TABLE;

/* pointers to TL_TABLE with sinwave output offset */
static INT32 **SIN_TABLE;

/* LFO table */
static INT32 *AMS_TABLE;
static INT32 *VIB_TABLE;

/* envelope output curve table */
/* attack + decay + OFF */
static INT32 ENV_CURVE[2*EG_ENT+1];

/* multiple table */
#define ML 2
static const UINT32 MUL_TABLE[16]= {
/* 1/2, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 */
   (UINT32)(0.50 * ML), (UINT32)(1.00 * ML), (UINT32)(2.00 * ML),
   (UINT32)(3.00 * ML), (UINT32)(4.00 * ML), (UINT32)(5.00 * ML),
   (UINT32)(6.00 * ML), (UINT32)(7.00 * ML), (UINT32)(8.00 * ML),
   (UINT32)(9.00 * ML),(UINT32)(10.00 * ML),(UINT32)(10.00 * ML),
  (UINT32)(12.00 * ML),(UINT32)(12.00 * ML),(UINT32)(15.00 * ML),
  (UINT32)(15.00 * ML)
};
#undef ML

/* dummy attack / decay rate ( when rate == 0 ) */
static INT32 RATE_0[16]=
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

#ifdef HAVE_PRAGMA_WARNING /* TODO: actually implement a check for this */
# pragma warning(default:4244) // explicit roundoff (may need fix!) ^^^^
#endif /* HAVE_PRAGMA_WARNING */
/* -------------------- static state --------------------- */

/* lock level of common table */
static int num_lock = 0;

/* work table */
static void *cur_chip = NULL;/* current chip point */
/* currenct chip state */
/* static OPLSAMPLE  *bufL,*bufR; */
static OPL_CH *S_CH;
static OPL_CH *E_CH;
OPL_SLOT *SLOT7_1,*SLOT7_2,*SLOT8_1,*SLOT8_2;

static INT32 outd[1];
static INT32 ams;
static INT32 vib; /* I keep having to cast this (vib)... consider other type? */
INT32  *ams_table;
INT32  *vib_table;
static INT32 amsIncr;
static INT32 vibIncr;
static INT32 feedback2; /* connect for SLOT 2 (should it really be 'static'?) */

/* log output level */
#define LOG_ERR  3      /* ERROR       */
#define LOG_WAR  2      /* WARNING     */
#define LOG_INF  1      /* INFORMATION */

#define LOG_LEVEL LOG_INF

#define LOG(n,x) if( (n)>=LOG_LEVEL ) logerror x

/* --------------------- subroutines  --------------------- */

INLINE int Limit(int val, int max, int min) {
	if (val > max) {
		val = max;
	} else if (val < min) {
		val = min;
	}

	return val;
}

/* ----- key on  ----- */
INLINE void OPL_KEYON(OPL_SLOT *SLOT)
{
	/* sine wave restart */
	SLOT->Cnt = 0;
	/* set attack */
	SLOT->evm = ENV_MOD_AR;
	SLOT->evs = SLOT->evsa;
	SLOT->evc = EG_AST;
	SLOT->eve = EG_AED;
}
/* ----- key off ----- */
INLINE void OPL_KEYOFF(OPL_SLOT *SLOT)
{
	if (SLOT->evm > ENV_MOD_RR) {
		/* set envelope counter from envleope output */
		SLOT->evm = ENV_MOD_RR;
		if (!(SLOT->evc&EG_DST)) {
			SLOT->evc = ((ENV_CURVE[SLOT->evc >> ENV_BITS] << ENV_BITS) +
						 EG_DST);
		}
		SLOT->eve = EG_DED;
		SLOT->evs = SLOT->evsr;
	}
}

/* ---------- Envelope Generator & Phase Generator ---------- */
/* return : envelope output */
INLINE UINT32 OPL_CALC_SLOT(OPL_SLOT *SLOT)
{
	/* calculate envelope generator */
	if ((SLOT->evc+=SLOT->evs) >= SLOT->eve) {
		switch (SLOT->evm) {
		case ENV_MOD_AR: /* ATTACK -> DECAY1 */
			/* next DR */
			SLOT->evm = ENV_MOD_DR;
			SLOT->evc = EG_DST;
			SLOT->eve = SLOT->SL;
			SLOT->evs = SLOT->evsd;
			break;
		case ENV_MOD_DR: /* DECAY -> SL or RR */
			SLOT->evc = SLOT->SL;
			SLOT->eve = EG_DED;
			if(SLOT->eg_typ) {
				SLOT->evs = 0;
			} else {
				SLOT->evm = ENV_MOD_RR;
				SLOT->evs = SLOT->evsr;
			}
			break;
		case ENV_MOD_RR: /* RR -> OFF */
			SLOT->evc = EG_OFF;
			SLOT->eve = EG_OFF+1;
			SLOT->evs = 0;
			break;
		}
	}
	/* calculate envelope */
	return (UINT32)(SLOT->TLL + ENV_CURVE[(SLOT->evc >> ENV_BITS)] +
					(SLOT->ams ? ams : 0));
}

/* set algorythm connection */
static void set_algorythm(OPL_CH *CH)
{
	INT32 *carrier = &outd[0];
	CH->connect1 = (CH->CON ? carrier : &feedback2);
	CH->connect2 = carrier;
}

/* ---------- frequency counter for operater update ---------- */
INLINE void CALC_FCSLOT(OPL_CH *CH, OPL_SLOT *SLOT)
{
	int ksr;

	/* frequency step counter */
	SLOT->Incr = (CH->fc * SLOT->mul);
	ksr = (CH->kcode >> SLOT->KSR);

	if (SLOT->ksr != ksr) {
		SLOT->ksr = (UINT8)ksr;
		/* attack, decay rate recalculation */
		SLOT->evsa = SLOT->AR[ksr];
		SLOT->evsd = SLOT->DR[ksr];
		SLOT->evsr = SLOT->RR[ksr];
	}
	SLOT->TLL = (INT32)((unsigned long)(SLOT->TL) +
						(CH->ksl_base >> SLOT->ksl));
}

/* set multi,am,vib,EG-TYP,KSR,mul */
INLINE void set_mul(FM_OPL *OPL, int slot, int v)
{
	OPL_CH   *CH   = &OPL->P_CH[(slot / 2)];
	OPL_SLOT *SLOT = &CH->SLOT[(slot & 1)];

	SLOT->mul    = MUL_TABLE[(v & 0x0f)];
	SLOT->KSR    = ((v & 0x10) ? 0 : 2);
	SLOT->eg_typ = ((v & 0x20) >> 5);
	SLOT->vib    = (v & 0x40);
	SLOT->ams    = (v & 0x80);
	CALC_FCSLOT(CH, SLOT);
}

/* set ksl & tl */
INLINE void set_ksl_tl(FM_OPL *OPL, int slot, int v)
{
	OPL_CH   *CH   = &OPL->P_CH[(slot / 2)];
	OPL_SLOT *SLOT = &CH->SLOT[(slot & 1)];
	int ksl = (v >> 6); /* 0 / 1.5 / 3 / 6 db/OCT */

	SLOT->ksl = (ksl ? (UINT8)(3 - ksl) : 31);
	SLOT->TL  = (int)((v&0x3f)*(0.75/EG_STEP)); /* 0.75db step */

	if (!(OPL->mode & 0x80)) {
		/* not CSM latch total level */
		SLOT->TLL = (INT32)((unsigned long)(SLOT->TL) +
							(CH->ksl_base>>SLOT->ksl));
	}
}

/* set attack rate & decay rate  */
INLINE void set_ar_dr(FM_OPL *OPL, int slot, int v)
{
	OPL_CH   *CH   = &OPL->P_CH[(slot / 2)];
	OPL_SLOT *SLOT = &CH->SLOT[(slot & 1)];
	int ar = (v >> 4);
	int dr = (v & 0x0f);

	SLOT->AR = (ar ? &OPL->AR_TABLE[(ar << 2)] : RATE_0);
	SLOT->evsa = SLOT->AR[SLOT->ksr];
	if (SLOT->evm == ENV_MOD_AR) {
		SLOT->evs = SLOT->evsa;
	}

	SLOT->DR = (dr ? &OPL->DR_TABLE[(dr << 2)] : RATE_0);
	SLOT->evsd = SLOT->DR[SLOT->ksr];
	if (SLOT->evm == ENV_MOD_DR) {
		SLOT->evs = SLOT->evsd;
	}
}

/* set sustain level & release rate */
INLINE void set_sl_rr(FM_OPL *OPL, int slot, int v)
{
	OPL_CH   *CH   = &OPL->P_CH[(slot / 2)];
	OPL_SLOT *SLOT = &CH->SLOT[(slot & 1)];
	int sl = (v >> 4);
	int rr = (v & 0x0f);

	SLOT->SL = SL_TABLE[sl];
	if (SLOT->evm == ENV_MOD_DR) {
		SLOT->eve = SLOT->SL;
	}
	SLOT->RR = &OPL->DR_TABLE[(rr << 2)];
	SLOT->evsr = SLOT->RR[SLOT->ksr];
	if (SLOT->evm == ENV_MOD_RR) {
		SLOT->evs = SLOT->evsr;
	}
}

/* operator output calculator */
/* casting here in the definition silences 6 warnings later on in the file: */
#define OP_OUT(slot,env,con) \
slot->wavetable[((slot->Cnt + (unsigned long)(con))/(0x1000000/SIN_ENT))&(SIN_ENT-1)][env]
/* ---------- calculate channel ---------- */
INLINE void OPL_CALC_CH(OPL_CH *CH)
{
	UINT32 env_out;
	OPL_SLOT *SLOT;

	feedback2 = 0;
	/* SLOT 1 */
	SLOT = &CH->SLOT[SLOT1];
	env_out = OPL_CALC_SLOT(SLOT);
	if (env_out < (EG_ENT - 1)) {
		/* PG */
		if (SLOT->vib) {
			SLOT->Cnt += (SLOT->Incr * (unsigned long)(vib) / VIB_RATE);
		} else {
			SLOT->Cnt += SLOT->Incr;
		}
		/* connection */
		if (CH->FB) {
			int feedback1;
			/* initialize separately: */
			feedback1 = (int)((CH->op1_out[0] + CH->op1_out[1]) >> CH->FB);
			CH->op1_out[1] = CH->op1_out[0];
			*CH->connect1 += CH->op1_out[0] = (INT32)(OP_OUT(SLOT, env_out,
															 feedback1));
		} else {
			*CH->connect1 += OP_OUT(SLOT,env_out,0);
		}
	} else {
		CH->op1_out[1] = CH->op1_out[0];
		CH->op1_out[0] = 0;
	}
	/* SLOT 2 */
	SLOT = &CH->SLOT[SLOT2];
	env_out=OPL_CALC_SLOT(SLOT);
	if (env_out < (EG_ENT - 1)) {
		/* PG */
		if (SLOT->vib) {
			SLOT->Cnt += (SLOT->Incr * (unsigned long)(vib) / VIB_RATE);
		} else {
			SLOT->Cnt += SLOT->Incr;
		}
		/* connection */
		outd[0] += OP_OUT(SLOT, env_out, feedback2);
	}
}

/* ---------- calculate rythm block ---------- */
#define WHITE_NOISE_db 6.0
INLINE void OPL_CALC_RH(OPL_CH *CH)
{
	UINT32 env_tam, env_sd, env_top, env_hh;
	int whitenoise = (int)((rand()&1)*(WHITE_NOISE_db/EG_STEP));
	INT32 tone8;

	OPL_SLOT *SLOT;
	int env_out;

	/* BD : same as FM serial mode and output level is large */
	feedback2 = 0;
	/* SLOT 1 */
	SLOT = &CH[6].SLOT[SLOT1];
	env_out = (int)OPL_CALC_SLOT(SLOT);
	if (env_out < (EG_ENT - 1)) {
		/* PG */
		if (SLOT->vib) {
			SLOT->Cnt += (SLOT->Incr * (unsigned long)(vib) / VIB_RATE);
		} else {
			SLOT->Cnt += SLOT->Incr;
		}
		/* connection */
		if (CH[6].FB) {
			int feedback1;
			/* initialize separately: */
			feedback1 = (int)(CH[6].op1_out[0]+CH[6].op1_out[1])>>CH[6].FB;
			CH[6].op1_out[1] = CH[6].op1_out[0];
			feedback2 = CH[6].op1_out[0] = OP_OUT(SLOT,env_out,feedback1);
		} else {
			feedback2 = OP_OUT(SLOT,env_out,0);
		}
	} else {
		feedback2 = 0;
		CH[6].op1_out[1] = CH[6].op1_out[0];
		CH[6].op1_out[0] = 0;
	}
	/* SLOT 2 */
	SLOT = &CH[6].SLOT[SLOT2];
	env_out = (int)OPL_CALC_SLOT(SLOT);
	if (env_out < (EG_ENT - 1)) {
		/* PG */
		if(SLOT->vib) {
			SLOT->Cnt += (SLOT->Incr * (unsigned long)(vib) / VIB_RATE);
		} else {
			SLOT->Cnt += SLOT->Incr;
		}
		/* connectoion */
		outd[0] += (OP_OUT(SLOT, env_out, feedback2) * 2);
	}

	/* SD  (17) = (mul14[fnum7] + white noise)
	 * TAM (15) = (mul15[fnum8])
	 * TOP (18) = (fnum6(mul18[fnum8] + whitenoise))
	 * HH  (14) = (fnum7(mul18[fnum8] + whitenoise) + white noise)
	 */
	env_sd  = (OPL_CALC_SLOT(SLOT7_2) + (unsigned long)(whitenoise));
	env_tam = (OPL_CALC_SLOT(SLOT8_1));
	env_top = (OPL_CALC_SLOT(SLOT8_2));
	env_hh  = (OPL_CALC_SLOT(SLOT7_1) + (unsigned long)(whitenoise));

	/* PG */
	if (SLOT7_1->vib) {
		SLOT7_1->Cnt += (2 * SLOT7_1->Incr * (unsigned long)(vib) / VIB_RATE);
	} else {
		SLOT7_1->Cnt += 2*SLOT7_1->Incr;
	}
	if (SLOT7_2->vib) {
		SLOT7_2->Cnt += ((CH[7].fc * 8) * (unsigned long)(vib) / VIB_RATE);
	} else {
		SLOT7_2->Cnt += (CH[7].fc*8);
	}
	if (SLOT8_1->vib) {
		SLOT8_1->Cnt += (SLOT8_1->Incr * (unsigned long)(vib) / VIB_RATE);
	} else {
		SLOT8_1->Cnt += SLOT8_1->Incr;
	}
	if (SLOT8_2->vib) {
		SLOT8_2->Cnt += ((CH[8].fc * 48) * (unsigned long)(vib) / VIB_RATE);
	} else {
		SLOT8_2->Cnt += (CH[8].fc * 48);
	}

	tone8 = OP_OUT(SLOT8_2, whitenoise, 0);

	/* SD */
	if (env_sd < (EG_ENT - 1)) {
		outd[0] += (OP_OUT(SLOT7_1, env_sd, 0) * 8);
	}
	/* TAM */
	if (env_tam < (EG_ENT - 1)) {
		outd[0] += (OP_OUT(SLOT8_1, env_tam, 0) * 2);
	}
	/* TOP-CY */
	if (env_top < (EG_ENT - 1)) {
		outd[0] += (OP_OUT(SLOT7_2, env_top, tone8) * 2);
	}
	/* HH */
	if (env_hh  < (EG_ENT - 1)) {
		outd[0] += (OP_OUT(SLOT7_2, env_hh, tone8) * 2);
	}
}

/* ----------- initialize time tabls ----------- */
static void init_timetables(FM_OPL *OPL, int ARRATE, int DRRATE)
{
	int i;
	double rate;

	/* make attack rate & decay rate tables */
	for (i = 0;i < 4;i++) OPL->AR_TABLE[i] = OPL->DR_TABLE[i] = 0;
	for (i = 4;i <= 60;i++) {
		rate  = OPL->freqbase;				 /* frequency rate */
		if( i < 60 ) rate *= 1.0+(i&3)*0.25; /* b0-1 : x1 , x1.25 , x1.5 , x1.75 */
		rate *= 1<<((i>>2)-1);				 /* b2-5 : shift bit */
		rate *= (double)(EG_ENT<<ENV_BITS);
		OPL->AR_TABLE[i] = (int)(rate/ARRATE);
		OPL->DR_TABLE[i] = (int)(rate/DRRATE);
	}
	for (i = 60;i < 76;i++) {
		OPL->AR_TABLE[i] = EG_AED-1;
		OPL->DR_TABLE[i] = OPL->DR_TABLE[60];
	}
#if 0
	for ((i = 0); (i < 64); i++) {	/* make for overflow area */
		LOG(LOG_WAR,("rate %2d , ar %f ms , dr %f ms \n",i,
			((double)(EG_ENT<<ENV_BITS) / OPL->AR_TABLE[i]) * (1000.0 / OPL->rate),
			((double)(EG_ENT<<ENV_BITS) / OPL->DR_TABLE[i]) * (1000.0 / OPL->rate) ));
	}
#endif /* 0 */
}

/* ---------- generic table initialize ---------- */
static int OPLOpenTable( void )
{
	int s,t;
	double rate;
	int i,j;
	double pom;

	/* allocate dynamic tables */
	if( (TL_TABLE = (INT32*)malloc(TL_MAX*2*sizeof(INT32))) == NULL) {
		return 0;
	}
	if( (SIN_TABLE = (INT32**)malloc(SIN_ENT*4 *sizeof(INT32 *))) == NULL) {
		free(TL_TABLE);
		return 0;
	}
	if( (AMS_TABLE = (INT32*)malloc(AMS_ENT*2 *sizeof(INT32))) == NULL) {
		free(TL_TABLE);
		free(SIN_TABLE);
		return 0;
	}
	if( (VIB_TABLE = (INT32*)malloc(VIB_ENT*2 *sizeof(INT32))) == NULL) {
		free(TL_TABLE);
		free(SIN_TABLE);
		free(AMS_TABLE);
		return 0;
	}
	/* make total level table */
	for (t = 0;t < EG_ENT-1 ;t++) {
		rate = ((1<<TL_BITS)-1)/pow(10,EG_STEP*t/20);	/* dB -> voltage */
		TL_TABLE[       t] =  (int)rate;
		TL_TABLE[TL_MAX+t] = -TL_TABLE[t];
#if 0
		LOG(LOG_INF,("TotalLevel(%3d) = %x\n",t,TL_TABLE[t]));
#endif /* 0 */
	}
	/* fill volume off area */
	for ( t = EG_ENT-1; t < TL_MAX ;t++){
		TL_TABLE[t] = TL_TABLE[TL_MAX+t] = 0;
	}

	/* make sinwave table (total level offet) */
	/* degree 0 = degree 180                   = off */
	SIN_TABLE[0] = SIN_TABLE[SIN_ENT/2]         = &TL_TABLE[EG_ENT-1];
	for (s = 1;s <= SIN_ENT/4;s++) {
		pom = sin(2*PI*s/SIN_ENT); /* sin     */
		pom = 20*log10(1/pom);	   /* decibel */
		j =(int)(pom/EG_STEP);         /* TL_TABLE steps */

        /* degree 0   -  90    , degree 180 -  90 : plus section */
		SIN_TABLE[          s] = SIN_TABLE[SIN_ENT/2-s] = &TL_TABLE[j];
        /* degree 180 - 270    , degree 360 - 270 : minus section */
		SIN_TABLE[SIN_ENT/2+s] = SIN_TABLE[SIN_ENT  -s] = &TL_TABLE[TL_MAX+j];
#if 0
		LOG(LOG_INF,("sin(%3d) = %f:%f db\n",s,pom,(double)j * EG_STEP));
#endif /* 0 */
	}
	for (s = 0;s < SIN_ENT;s++) {
		SIN_TABLE[SIN_ENT*1+s] = s<(SIN_ENT/2) ? SIN_TABLE[s] : &TL_TABLE[EG_ENT];
		SIN_TABLE[SIN_ENT*2+s] = SIN_TABLE[s % (SIN_ENT/2)];
		SIN_TABLE[SIN_ENT*3+s] = (s/(SIN_ENT/4))&1 ? &TL_TABLE[EG_ENT] : SIN_TABLE[SIN_ENT*2+s];
	}

	/* envelope counter -> envelope output table */
	for (i=0; i<EG_ENT; i++) {
		/* ATTACK curve */
		pom = pow( ((double)(EG_ENT-1-i)/EG_ENT) , 8 ) * EG_ENT;
		/* if( pom >= EG_ENT ) pom = EG_ENT-1; */
		ENV_CURVE[i] = (int)pom;
		/* DECAY ,RELEASE curve */
		ENV_CURVE[(EG_DST>>ENV_BITS)+i]= i;
	}
	/* off */
	ENV_CURVE[EG_OFF>>ENV_BITS]= EG_ENT-1;
	/* make LFO ams table */
	for (i=0; i<AMS_ENT; i++) {
		pom = (1.0+sin(2*PI*i/AMS_ENT))/2; /* sin */
		AMS_TABLE[i]         = (int)((1.0/EG_STEP)*pom); /* 1dB   */
		AMS_TABLE[AMS_ENT+i] = (int)((4.8/EG_STEP)*pom); /* 4.8dB */
	}
	/* make LFO vibrate table */
	for (i=0; i<VIB_ENT; i++) {
		/* 100cent = 1seminote = 6% ?? */
		pom = (double)VIB_RATE*0.06*sin(2*PI*i/VIB_ENT); /* +-100sect step */
		VIB_TABLE[i]         = (int)(VIB_RATE + (pom*0.07)); /* +- 7cent */
		VIB_TABLE[VIB_ENT+i] = (int)(VIB_RATE + (pom*0.14)); /* +-14cent */
		/* LOG(LOG_INF,("vib %d=%d\n",i,VIB_TABLE[VIB_ENT+i])); */
	}
	return 1;
}


static void OPLCloseTable()
{
	free(TL_TABLE);
	free(SIN_TABLE);
	free(AMS_TABLE);
	free(VIB_TABLE);
}

/* ---------- opl initialize ---------- */
static void OPL_initalize(FM_OPL *OPL)
{
	int fn;

	/* frequency base */
	OPL->freqbase = ((OPL->rate) ? ((double)OPL->clock / OPL->rate) / 72  : 0);

	/* make time tables */
	init_timetables(OPL, OPL_ARRATE, OPL_DRRATE);
	/* make fnumber -> increment counter table */
	for ((fn = 0); (fn < 1024); fn++) {
		OPL->FN_TABLE[fn] = (UINT32)((int)(OPL->freqbase * fn * FREQ_RATE * (1 << 7) / 2));
	}
	/* LFO freq.table */
	OPL->amsIncr = ((int)(OPL->rate ? (double)AMS_ENT * (1 << AMS_SHIFT) /
						  OPL->rate * 3.7 * ((double)OPL->clock / 3600000) : 0));
	OPL->vibIncr = ((int)(OPL->rate ? (double)VIB_ENT * (1 << VIB_SHIFT) /
						  OPL->rate * 6.4 * ((double)OPL->clock / 3600000) : 0));
}

/* ---------- write to OPL registers ---------- */
void OPLWrite(FM_OPL *OPL, int r, int v)
{
	OPL_CH *CH;
	int slot;
	int block_fnum;

	switch(r&0xe0) {
	case 0x00: /* 00-1f:controll */
		switch(r&0x1f) {
		case 0x01:
			/* wave selector enable */
			OPL->wavesel = v&0x20;
			if(!OPL->wavesel) {
				/* preset compatible mode */
				int c;
				for((c = 0); (c < OPL->max_ch); c++) {
					OPL->P_CH[c].SLOT[SLOT1].wavetable = &SIN_TABLE[0];
					OPL->P_CH[c].SLOT[SLOT2].wavetable = &SIN_TABLE[0];
				}
			}
			return;
		case 0x02:	/* Timer 1 */
			break;
		case 0x03:	/* Timer 2 */
			return;
		case 0x04:	/* IRQ clear / mask and Timer enable */
			return;
		case 0x08:	/* MODE, DELTA-T: CSM,NOTESEL,x,x,smpl,da/ad,64k,rom */
			OPL->mode = (UINT32)(v);
			return;
		}
		break;
	case 0x20:	/* am,vib,ksr,eg type,mul */
		slot = slot_array[r&0x1f];
		if(slot == -1) return;
		set_mul(OPL,slot,v);
		return;
	case 0x40:
		slot = slot_array[r&0x1f];
		if(slot == -1) return;
		set_ksl_tl(OPL,slot,v);
		return;
	case 0x60:
		slot = slot_array[r&0x1f];
		if(slot == -1) return;
		set_ar_dr(OPL,slot,v);
		return;
	case 0x80:
		slot = slot_array[r&0x1f];
		if(slot == -1) return;
		set_sl_rr(OPL,slot,v);
		return;
	case 0xa0:
		switch (r) {
		case 0xbd:
			/* amsep, vibdep, r, bd, sd, tom, tc, hh */
			{
			UINT8 rkey;
			rkey = (UINT8)(OPL->rythm^v); /* init separately */
			OPL->ams_table = &AMS_TABLE[v&0x80 ? AMS_ENT : 0];
			OPL->vib_table = &VIB_TABLE[v&0x40 ? VIB_ENT : 0];
			OPL->rythm  = (v & 0x3f);
			if (OPL->rythm & 0x20) {
#ifdef usrintf_showmessage
				usrintf_showmessage("OPL Rythm mode select"); /*unimplemented*/
#endif /* usrintf_showmessage */
				/* BD key on/off */
				if (rkey & 0x10) {
					if (v & 0x10) {
						OPL->P_CH[6].op1_out[0] = OPL->P_CH[6].op1_out[1] = 0;
						OPL_KEYON(&OPL->P_CH[6].SLOT[SLOT1]);
						OPL_KEYON(&OPL->P_CH[6].SLOT[SLOT2]);
					} else {
						OPL_KEYOFF(&OPL->P_CH[6].SLOT[SLOT1]);
						OPL_KEYOFF(&OPL->P_CH[6].SLOT[SLOT2]);
					}
				}
				/* SD key on/off */
				if (rkey & 0x08) {
					if (v & 0x08) {
						OPL_KEYON(&OPL->P_CH[7].SLOT[SLOT2]);
					} else {
						OPL_KEYOFF(&OPL->P_CH[7].SLOT[SLOT2]);
					}
				} /* TAM key on/off */
				if (rkey & 0x04) {
					if (v & 0x04) {
						OPL_KEYON(&OPL->P_CH[8].SLOT[SLOT1]);
					} else {
						OPL_KEYOFF(&OPL->P_CH[8].SLOT[SLOT1]);
					}
				}
				/* TOP-CY key on/off */
				if (rkey & 0x02) {
					if (v & 0x02) {
						OPL_KEYON(&OPL->P_CH[8].SLOT[SLOT2]);
					} else {
						OPL_KEYOFF(&OPL->P_CH[8].SLOT[SLOT2]);
					}
				}
				/* HH key on/off */
				if (rkey & 0x01) {
					if (v & 0x01) {
						OPL_KEYON(&OPL->P_CH[7].SLOT[SLOT1]);
					} else {
						OPL_KEYOFF(&OPL->P_CH[7].SLOT[SLOT1]);
					}
				}
			} /* end "if" */
			} /* end random dangling unnecessary pair of brackets */
			return;
		} /* end switch */
		/* keyon, block, fnum */
		if ((r & 0x0f) > 8) {
			return;
		}
		CH = &OPL->P_CH[r&0x0f];
		if(!(r&0x10)) {
			/* a0-a8 */
			block_fnum  = (int)((CH->block_fnum&0x1f00) | (unsigned long)(v));
		} else {
			/* b0-b8 */
			int keyon;
			keyon = ((v >> 5) & 1); /* init separately */
			block_fnum = (int)(((unsigned long)(v & 0x1f) << 8) |
							   (CH->block_fnum & 0xff));
			if (CH->keyon != keyon) {
				if ((CH->keyon = (UINT8)(keyon))) {
					CH->op1_out[0] = (CH->op1_out[1] = 0);
					OPL_KEYON(&CH->SLOT[SLOT1]);
					OPL_KEYON(&CH->SLOT[SLOT2]);
				} else {
					OPL_KEYOFF(&CH->SLOT[SLOT1]);
					OPL_KEYOFF(&CH->SLOT[SLOT2]);
				}
			}
		}
		/* update */
		if (CH->block_fnum != (unsigned int)block_fnum) {
			int blockRv;
			int fnum;

			blockRv = (7 - (block_fnum >> 10));
			fnum    = (block_fnum & 0x3ff);

			CH->block_fnum = (UINT32)block_fnum;

			CH->ksl_base = KSL_TABLE[block_fnum>>6];
			CH->fc = OPL->FN_TABLE[fnum]>>blockRv;
			CH->kcode = (UINT8)(CH->block_fnum >> 9);
			if ((OPL->mode & 0x40) && (CH->block_fnum & 0x100)) {
				CH->kcode |= 1;
			}
			CALC_FCSLOT(CH,&CH->SLOT[SLOT1]);
			CALC_FCSLOT(CH,&CH->SLOT[SLOT2]);
		}
		return;
	case 0xc0:
		/* FB,C */
		if ((r & 0x0f) > 8) {
			return;
		}
		CH = (&OPL->P_CH[(r & 0x0f)]);
		{
			int feedback;
			feedback = ((v >> 1) & 7);
			CH->FB   = (feedback ? (UINT8)((UINT8)(8 + 1) - feedback) : 0);
			CH->CON  = (v & 1);
			set_algorythm(CH);
		} /* end random dangling unnecessary pair of brackets */
		return;
	case 0xe0: /* wave type */
		slot = slot_array[(r & 0x1f)];
		if (slot == -1) {
			return;
		}
		CH = (&OPL->P_CH[(slot / 2)]);
		if (OPL->wavesel) {
#if defined(LOG) && defined(LOG_INF) && (defined(logerror) || defined(HAVE_LOG_ERROR))
			LOG(LOG_INF, ("OPL SLOT %d wave select %d\n", slot, (v & 3)));
#endif /* LOG && LOG_INF && (logerror || HAVE_LOG_ERROR) */
			CH->SLOT[slot&1].wavetable = (&SIN_TABLE[((v & 0x03) * SIN_ENT)]);
		}
		return;
	}
}

/* lock/unlock for common table */
static int OPL_LockTable(void)
{
	num_lock++;
	if(num_lock>1) return 0;
	/* first time */
	cur_chip = NULL;
	/* allocate total level table (128kb space) */
	if( !OPLOpenTable() ) {
		num_lock--;
		return -1;
	}
	return 0;
}

static void OPL_UnLockTable(void)
{
	if (num_lock) {
		num_lock--;
	}
	if (num_lock) {
		return;
	}
	/* last time */
	cur_chip = NULL;
	OPLCloseTable();
}

/* ---------- update chip ----------- */
void YM3812UpdateOne(FM_OPL *OPL, void *buffer, int length)
{
    int i;
	int data;
	OPLSAMPLE *buf;
	UINT32 amsCnt;
	UINT32 vibCnt;
	UINT8 rythm;
	OPL_CH *CH, *R_CH;

	buf    = (OPLSAMPLE *)(buffer);
	amsCnt = (UINT32)(OPL->amsCnt);
	vibCnt = (UINT32)(OPL->vibCnt);
	rythm  = (OPL->rythm & 0x20);

	if ((void *)OPL != cur_chip) {
		cur_chip = (void *)OPL;
		/* channel pointers */
		S_CH = OPL->P_CH;
		E_CH = &S_CH[9];
		/* rythm slot */
		SLOT7_1 = &S_CH[7].SLOT[SLOT1];
		SLOT7_2 = &S_CH[7].SLOT[SLOT2];
		SLOT8_1 = &S_CH[8].SLOT[SLOT1];
		SLOT8_2 = &S_CH[8].SLOT[SLOT2];
		/* LFO state */
		amsIncr = OPL->amsIncr;
		vibIncr = OPL->vibIncr;
		ams_table = OPL->ams_table;
		vib_table = OPL->vib_table;
	}
	R_CH = (rythm ? &S_CH[6] : E_CH);
    for ((i = 0); (i < length); i++ ) {
		/*            channel A         channel B         channel C      */
		/* LFO */
		ams = ams_table[((amsCnt += (unsigned long)amsIncr) >> AMS_SHIFT)];
		vib = vib_table[((vibCnt += (unsigned long)vibIncr) >> VIB_SHIFT)];
		outd[0] = 0;
		/* FM part */
		for ((CH = S_CH); (CH < R_CH); CH++) {
			OPL_CALC_CH(CH);
		}
		/* Rythn part */
		if (rythm) {
			OPL_CALC_RH(S_CH);
		}
		/* limit check */
		data = Limit((int)outd[0], OPL_MAXOUT, OPL_MINOUT);
		/* store to sound buffer */
		buf[i] = (OPLSAMPLE)(data >> OPL_OUTSB);
	}

	OPL->amsCnt = (INT32)(amsCnt);
	OPL->vibCnt = (INT32)(vibCnt);
#ifdef OPL_OUTPUT_LOG
	if (opl_dbg_fp) {
		for ((opl_dbg_chip = 0); (opl_dbg_chip < opl_dbg_maxchip);
			 (opl_dbg_chip++)) {
			if (opl_dbg_opl[opl_dbg_chip] == OPL) {
				break;
			}
		}
		fprintf(opl_dbg_fp, "%c%c%c", (0x20 + opl_dbg_chip), (length & 0xff),
				(length / 256));
	}
#endif /* OPL_OUTPUT_LOG */
}

/* ---------- reset chip ---------- */
void OPLResetChip(FM_OPL *OPL)
{
	int c,s;
	int i;

	/* reset chip */
	OPL->mode   = 0;	/* normal mode */
	/* reset with register write */
	OPLWrite(OPL,0x01,0); /* wavesel disable */
	OPLWrite(OPL,0x02,0); /* Timer1 */
	OPLWrite(OPL,0x03,0); /* Timer2 */
	OPLWrite(OPL,0x04,0); /* IRQ mask clear */
	for(i = 0xff ; i >= 0x20 ; i-- ) OPLWrite(OPL,i,0);
	/* reset OPerator paramater */
	for(c=0; c<OPL->max_ch; c++) {
		OPL_CH *CH = &OPL->P_CH[c];
		/* OPL->P_CH[c].PAN = OPN_CENTER; */
		for(s = 0 ; s < 2 ; s++ )
		{
			/* wave table */
			CH->SLOT[s].wavetable = &SIN_TABLE[0];
#if 0
			CH->SLOT[s].evm = ENV_MOD_RR;
#endif /* 0 */
			CH->SLOT[s].evc = EG_OFF;
			CH->SLOT[s].eve = EG_OFF+1;
			CH->SLOT[s].evs = 0;
		}
	}
}

/* ----------  Create a virtual YM3812 ----------       */
FM_OPL *OPLCreate(int type, int clock, int rate)
{
	char *ptr;
	FM_OPL *OPL;
	int state_size;
	int max_ch;
	max_ch = 9; /* normaly 9 channels */

	if (OPL_LockTable() == -1) {
		return NULL;
	}
	/* allocate OPL state space */
	state_size  = sizeof(FM_OPL);
	state_size += (sizeof(OPL_CH) * (unsigned long)max_ch);

	/* allocate memory block */
	ptr = (char*)malloc((size_t)state_size);
	if (ptr == NULL) {
		return NULL;
	}
	/* clear */
	memset(ptr,0,state_size);

	OPL        = (FM_OPL *)ptr;
	ptr += sizeof(FM_OPL);
	OPL->P_CH  = (OPL_CH *)ptr;
	ptr += (sizeof(OPL_CH) * (unsigned long)max_ch);

#if __clang_analyzer__
	fprintf(stdout, "ptr is %s", ptr);
#endif /* __clang_analyzer__ (hack) */

	/* set channel state pointer */
	OPL->type  = (UINT8)type;
	OPL->clock = clock;
	OPL->rate  = rate;
	OPL->max_ch = max_ch;
	/* init grobal tables */
	OPL_initalize(OPL);
	/* reset chip */
	OPLResetChip(OPL);
#ifdef OPL_OUTPUT_LOG
	if (!opl_dbg_fp) {
		opl_dbg_fp = fopen("opllog.opl","wb");
		opl_dbg_maxchip = 0;
	}
	if (opl_dbg_fp) {
		opl_dbg_opl[opl_dbg_maxchip] = OPL;
		fprintf(opl_dbg_fp, "%c%c%c%c%c%c", (0x00 + opl_dbg_maxchip),
				type,
				(clock & 0xff),
				((clock / 0x100) & 0xff),
				((clock / 0x10000) & 0xff),
				((clock / 0x1000000) & 0xff));
		opl_dbg_maxchip++;
	}
#endif /* OPL_OUTPUT_LOG */
	return OPL;
}

/* ----------  Destroy a virtual YM3812 ----------       */
void OPLDestroy(FM_OPL *OPL)
{
#ifdef OPL_OUTPUT_LOG
	if(opl_dbg_fp) {
		fclose(opl_dbg_fp);
		opl_dbg_fp = NULL;
	}
#endif /* OPL_OUTPUT_LOG */
	OPL_UnLockTable();
	free(OPL);
}

/* EOF */
