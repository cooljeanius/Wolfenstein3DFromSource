/*
 *	Copyright (C) 2004-2005 Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version 2
 *	of the License, or (at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 * cpuid.c:   CPU Identification
 *
 * Author:  Michael Liebscher <johnnycanuck@users.sourceforge.net>
 *
 */

/*
 *	Notes:
 *
 *	Reference documents:
 *	http://developer.intel.com/design/xeon/applnots/241618.htm
 *	http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/20734.pdf
 *	http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/24594.pdf
 *  (dead links - redirects, that is)
 *
 */

#include <string.h>
#include "processor.h"
#include "../../common/common.h"
#include "../../string/com_string.h"

cpu_info_struct main_cpu_s;


#if (i386 || __i386 || __i386__ || _M_IX86 || _I386)
# ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN 1
#  include <windows.h>
# endif /* _WIN32 */
# ifdef __linux__
#  include <sys/timeb.h>
# endif /* __linux__ */
# if defined(__FreeBSD__) || defined(__APPLE__) || defined(HAVE_SYS_TIME_H)
#  include <sys/time.h>
# endif /* __FreeBSD__ */


/********************************************************************
 *
 *	AMD
 *
 *******************************************************************/

/* ECX Standard Feature Flag Descriptions when CPUID is 1 */

#define AMD_CPUID_STD_SSE3              BIT(  0 )   /* SSE3 Instructions Supported */

#define AMD_CPUID_STD_CX16				BIT( 13 )   /* CMPXCHG16B supported */


/* EDX Standard Feature Flag Descriptions when CPUID is 1 */

#define AMD_CPUID_STD_FPU               BIT(  0 )   /* Floating-Point Unit */
#define AMD_CPUID_STD_VME               BIT(  1 )   /* Virtual Mode Extensions */
#define AMD_CPUID_STD_DEBUGEXT          BIT(  2 )   /* Debugging Extensions */
#define AMD_CPUID_STD_4MPAGE            BIT(  3 )   /* Page Size Extensions (4-Mbyte pages) */
#define AMD_CPUID_STD_TSC               BIT(  4 )   /* Time Stamp Counter (with RDTSC and CR4 disable bit) */
#define AMD_CPUID_STD_MSR               BIT(  5 )   /* AMD K86 Family of Processors Model-Specific Registers (with RDMSR and WRMSR) */
#define AMD_CPUID_STD_PAE               BIT(  6 )   /* PAE (Page Address Extensions) */
#define AMD_CPUID_STD_MCE               BIT(  7 )   /* Machine Check Exception */
#define AMD_CPUID_STD_CMPXCHG8B			BIT(  8 )   /* CMPXCHG8B Instruction */
#define AMD_CPUID_STD_APIC              BIT(  9 )   /* APIC */
                                /*      BIT( 10 )*/ /* Reserved */
#define AMD_CPUID_STD_SYSENTER          BIT( 11 )   /* SYSENTER/SYSEXIT Instructions */
#define AMD_CPUID_STD_MTRR              BIT( 12 )   /* MTRR (Memory Type Range Registers) */
#define AMD_CPUID_STD_GPE               BIT( 13 )   /* Global Paging Extension */
#define AMD_CPUID_STD_MCA               BIT( 14 )   /* Machine Check Architecture */
#define AMD_CPUID_STD_CMOV              BIT( 15 )   /* Conditional Move Instruction */
#define AMD_CPUID_STD_PAT               BIT( 16 )   /* PAT (Page Attribute Table) */
#define AMD_CPUID_STD_PSE36             BIT( 17 )   /* PSE-36 (Page Size Extensions) */
                                /*      BIT( 18 )*/ /* Reserved */
#define AMD_CPUID_STD_CLFLUSH           BIT( 19 )   /* CLFLUSH Instruction */
                                /*      BIT( 20 )*/ /* Reserved */
                                /*      BIT( 21 )*/ /* Reserved */
                                /*      BIT( 22 )*/ /* Reserved */
#define AMD_CPUID_STD_MMX               BIT( 23 )   /* MMX(tm) Instructions */
#define AMD_CPUID_STD_FXSAVE            BIT( 24 )   /* FXSAVE/FXRSTOR */
#define AMD_CPUID_STD_SSE               BIT( 25 )   /* SSE Instructions */
#define AMD_CPUID_STD_SSE2              BIT( 26 )   /* SSE2 Instructions */
								/*      BIT( 27 )*/ /* Reserved */
#define AMD_CPUID_STD_HTT				BIT( 28 )   /* Hyper-Threading Technology */



/* ECX  Extended Feature Flag Descriptions when CPUID is 0x80000001 */

#define AMD_CPUID_EXT_LAHF              BIT(  0 )   /* LAHF/SAHF Supported */
#define AMD_CPUID_EXT_CMPL              BIT(  1 )   /* CMP Legacy */
                                /*      BIT(  2 )*/ /* Reserved */
                                /*      BIT(  3 )*/ /* Reserved */
#define AMD_CPUID_EXT_CR8               BIT(  4 )   /* CR8 Available in Legacy Mode. */


/* EDX  Extended Feature Flag Descriptions when CPUID is 0x80000001 */

#define AMD_CPUID_EXT_SYSCALL           BIT( 11 )   /* SYSCALL and SYSRET Instructions */

#define AMD_CPUID_EXT_NX                BIT( 20 )   /* No-execute Page Protection (NX) */

#define AMD_CPUID_EXT_MMXEXT            BIT( 22 )   /* MMX(tm) Instruction Extensions */

#define AMD_CPUID_EXT_FXSR              BIT( 25 )   /* Fast FXSAVE/FXRSTOR */
									/*	BIT( 26 )*/ /* Reserved */
#define AMD_CPUID_EXT_RDTSCP            BIT( 27 )   /* RDTSCP Instruction */
									/*	BIT( 28 )*/   /* Reserved */
#define AMD_CPUID_EXT_LM                BIT( 29 )   /* Long Mode Capable (LM) */
#define AMD_CPUID_EXT_3DNOWEXT          BIT( 30 )   /* 3DNow!(tm) Instruction Extensions */
#define AMD_CPUID_EXT_3DNOW             BIT( 31 )   /* 3DNow!(tm) Instructions */



/********************************************************************
 *
 *	INTEL
 *
 *******************************************************************/

/*  Feature Flag Values Reported in the EDX Register when CPUID is 1 */

#define INTEL_CPUID_FPU_FLAG			BIT(  0 )   /* Floating-point unit on-Chip */
#define INTEL_CPUID_VME_FLAG			BIT(  1 )   /* Virtual Mode Extension */
#define INTEL_CPUID_DE_FLAG				BIT(  2 )   /* Debugging Extension */
#define INTEL_CPUID_PSE_FLAG			BIT(  3 )   /* Page Size Extension */
#define INTEL_CPUID_TSC_FLAG			BIT(  4 )   /* Time-Stamp Counter */
#define INTEL_CPUID_MSR_FLAG			BIT(  5 )   /* Model Specific Registers */
#define INTEL_CPUID_PAE_FLAG			BIT(  6 )   /* Physical Address Extension */
#define INTEL_CPUID_MCE_FLAG			BIT(  7 )   /* Machine Check Exception */
#define INTEL_CPUID_CX8_FLAG			BIT(  8 )   /* CMPXCHG8 Instruction Supported */
#define INTEL_CPUID_APIC_FLAG			BIT(  9 )   /* On-chip APIC Hardware Supported */
									/*  BIT( 10 )*/ /* Reserved */
#define INTEL_CPUID_SEP_FLAG			BIT( 11 )   /* Fast System Call */
#define INTEL_CPUID_MTRR_FLAG			BIT( 12 )   /* Memory Type Range Registers */
#define INTEL_CPUID_PGE_FLAG			BIT( 13 )   /* Page Global Enable */
#define INTEL_CPUID_MCA_FLAG			BIT( 14 )   /* Machine Check Architecture */
#define INTEL_CPUID_CMOV_FLAG			BIT( 15 )   /* Conditional Move Instruction Supported */
#define INTEL_CPUID_PAT_FLAG			BIT( 16 )   /* Page Attribute Table */
#define INTEL_CPUID_PSE36_FLAG			BIT( 17 )   /* 36-bit Page Size Extension */
#define INTEL_CPUID_PSNUM_FLAG			BIT( 18 )   /* Processor serial number is present and enabled */
#define INTEL_CPUID_CLFLUSH_FLAG		BIT( 19 )   /* CLFLUSH Instruction supported */
									/*  BIT( 20 )*/ /* Reserved */
#define INTEL_CPUID_DS_FLAG				BIT( 21 )   /* Debug Store */
#define INTEL_CPUID_ACPI_FLAG			BIT( 22 )   /* Thermal Monitor and Software Controlled Clock Facilities supported */
#define INTEL_CPUID_MMX_FLAG			BIT( 23 )   /* Intel Architecture MMX technology supported */
#define INTEL_CPUID_FXSR_FLAG			BIT( 24 )   /* Fast floating point save and restore */
#define INTEL_CPUID_SSE_FLAG			BIT( 25 )   /* Streaming SIMD Extensions supported */
#define INTEL_CPUID_SSE2_FLAG			BIT( 26 )   /* Streaming SIMD Extensions 2 */
#define INTEL_CPUID_SS_FLAG				BIT( 27 )   /* Self-Snoop */
#define INTEL_CPUID_HTT_FLAG			BIT( 28 )   /* Hyper-Threading Technology */
#define INTEL_CPUID_TM_FLAG				BIT( 29 )   /* Thermal Monitor supported */
									/*  BIT( 30 )*/ /* Reserved */
#define INTEL_CPUID_PBE_FLAG			BIT( 31 )   /* Pending Break Enable */


/* Feature Flag Values Reported in the ECX Register when CPUID is 1 */

#define INTEL_CPUID_SSE3_FLAG			BIT(  0 )	/* Streaming SIMD Extensions 3 */
                                    /*  BIT(  1 )*/	/* Reserved */
                                    /*  BIT(  2 )*/	/* Reserved */
#define INTEL_CPUID_MONITOR_FLAG        BIT(  3 )	/* MONITOR/MWAIT supported */
#define INTEL_CPUID_DSCPL_FLAG          BIT(  4 )	/* CPL Qualified Debug Store */
                                    /*  BIT(  5 )*/ /* Reserved */
                                    /*  BIT(  6 )*/	/* Reserved */
#define INTEL_CPUID_EST_FLAG            BIT(  7 )	/* Enhanced Intel SpeedStep(R) technology */
#define INTEL_CPUID_TM2_FLAG            BIT(  8 )	/* Thermal Monitor 2 */
                                    /*  BIT(  9 )*/	/* Reserved */
#define INTEL_CPUID_CID_FLAG            BIT( 10 )	/* Context ID */
                                    /*  BIT( 11 )*/	/* Reserved */
                                    /*  BIT( 12 )*/	/* Reserved */
#define INTEL_CPUID_CX16_FLAG           BIT( 13 )	/* CMPXCHG16B supported */
#define INTEL_CPUID_XTPR_FLAG           BIT( 14 )	/* Send Task Priority Message */


/* Extended Feature Flag Values Reported in the EDX Register when CPUID is 0x80000001 */

#define INTEL_CPUID_SYSCALL_FLAG		BIT( 11 )	/* SYSCALL/SYSRET supported */

#define INTEL_CPUID_XDBIT_FLAG			BIT( 20 )	/* Execution Disabled Bit */

#define INTEL_CPUID_IEM64T_FLAG			BIT( 29 )	/* Intel Extended Memory 64 Technology */


/* Extended Feature Flag Values Reported in the ECX Register when CPUID is 0x80000001 */

#define INTEL_CPUID_LAHF_FLAG			BIT( 20 )	/* LAHF/SAHF supported */


/*
-----------------------------------------------------------------------------
 Function: x86_do_cpuid -Get CPUID Information.

 Parameters: id -[in] Initial EAX Value.
			 p -[in/out] Pointer to 4 element array that will hold
						 register data.

 Returns: Nothing.

 Notes:

-----------------------------------------------------------------------------
*/
PRIVATE void x86_do_cpuid(W32 id, W32 *p)
{
#ifdef _MSC_VER
	__asm
	{
		mov esi, p

		mov eax, id

		_emit 0x0F
		_emit 0xA2 /* cpuid */

		mov dword ptr[esi],		eax
		mov dword ptr[esi+4],	ebx
		mov dword ptr[esi+8],	ecx
		mov dword ptr[esi+12],	edx
	}
#elif __GNUC__
# ifdef __x86_64__
	__asm __volatile(
	"cpuid"
	: "=a" (p[0]), "=b" (p[1]), "=c" (p[2]), "=d" (p[3])
	:  "0" (id)
	);
# else /* not sure why I have this conditional on architecture, as they both
		* work the same way... */
#  ifdef __i386__
	__asm __volatile(
	"cpuid"
	: "=a" (p[0]), "=b" (p[1]), "=c" (p[2]), "=d" (p[3])
	:  "0" (id)
	);
#  endif /* __i386__ */
# endif /* __x86_64__ */
#endif /* _MSC_VER || __GNUC__ */
}


/*
-----------------------------------------------------------------------------
 Function: x86_can_do_cpuid -Check to see if the CPU can do a CPUID.

 Parameters: Nothing.

 Returns: True if successful, otherwise false.

 Notes:
        A potential problem with this method is that an interrupt or a trap
        (such as a debug trap) can occur between the POPFD and the
        following PUSHFD, which will destroy the value of the ID bit.
-----------------------------------------------------------------------------
*/
PRIVATE _boolean x86_can_do_cpuid( void )
{
	_boolean result = false;
#ifdef _MSC_VER
        __asm
        {
                pushfd          /* Save EFLAGS to stack */
                pop  eax        /* Store EFLAGS in EAX */

                test eax, 0x00200000    /* Check ID bit (bit 21) */
                jz   set_21          /* Bit 21 is not set, so jump to set_21 */
                and  eax, 0xffdfffff    /* Clear bit 21 */

                push eax        /* Copy changed value to stack */
                popfd           /* Save changed EAX to EFLAGS */

                pushfd          /* Push EFLAGS to top of stack */
                pop  eax        /* Store EFLAGS in EAX */

                test eax, 0x00200000    /* Check ID bit (bit 21) */

                jnz  cpu_id_not_ok      /* If it is on, no CPUID */

                mov byte ptr result, 1  /* return 1 */
                jmp  done

        set_21:
                or   eax, 0x00200000  /* Turn bit 21 on */

                push eax        /* Copy changed value to stack */
                popfd           /* Save changed EAX to EFLAGS */

                pushfd          /* Push EFLAGS to top of stack */
                pop  eax        /* Store EFLAGS in EAX */

                test eax, 0x00200000    /* See if bit 21 has changed */
                jz   cpu_id_not_ok      /* If no change, no CPUID */

                mov byte ptr result, 1  /* return 1 */
                jmp  done

        cpu_id_not_ok:

                mov byte ptr result, 0  /* CPUID is not supported */

        done:
        }

#elif __GNUC__
        __asm__ __volatile__ (

                "pushf\n\t"             /* Save EFLAGS to stack */
                "popl %%eax\n\t"        /* Store EFLAGS in EAX */

                "test $0x00200000, %%eax\n\t"   /* Check ID bit (bit 21) */
                "jz   set_21\n\t"
                "and  $0xffdfffff, %%eax\n\t"   /* Clear bit 21 */

                "push %%eax\n\t"        /* Copy changed value to stack */
                "popf\n\t"              /* Save changed EAX to EFLAGS */

                "pushf\n\t"             /* Save EFLAGS to stack */
                "popl %%eax\n\t"        /* Store EFLAGS in EAX */

                "test $0x00200000, %%eax\n\t"   /* Check ID bit (bit 21) */
                "jnz  cpu_id_not_ok\n\t" /* If it is on, no CPUID */

                "movb $1, %0\n\t"       /* return 1 */

                "jmp  done\n\t"

        "set_21:"
                "or   $0x00200000, %%eax\n\t"  /* Turn bit 21 on */

                "push %%eax\n\t"        /* Copy changed value to stack */
                "popf\n\t"              /* Save changed EAX to EFLAGS */

                "pushf\n\t"             /* Save EFLAGS to stack */
                "popl %%eax\n\t"        /* Store EFLAGS in EAX */

                "test $0x00200000, %%eax\n\t"   /* See if bit 21 has changed */
                "jz   cpu_id_not_ok\n\t"        /* If no change, no CPUID */

                "movb $1, %0\n\t"       /* return 1 */
                "jmp done\n\t"

        "cpu_id_not_ok:"

                "movb $0, %0\n\t"       /* CPUID is not supported */

        "done:"
      : "=&r" (result)  /* output */
      :                 /* input */
      : "%eax" /* clobbered list */
      );


#endif

	return result;
}

/*
-----------------------------------------------------------------------------
 Function: x86_get_cpu_ProcessorName -CPU name identification.

 Parameters: Nothing.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void x86_get_cpu_ProcessorName( void )
{
	char ProcessorName[ 49 ];

#ifdef _MSC_VER
        __asm
        {
                mov eax, 0x80000002 /* Processor Brand String */

                _emit 0x0F
                _emit 0xA2 /* cpuid */

                mov dword ptr [ProcessorName   ],eax
                mov dword ptr [ProcessorName+4 ],ebx
                mov dword ptr [ProcessorName+8 ],ecx
                mov dword ptr [ProcessorName+12],edx

                mov eax, 0x80000003 /* Processor Brand String */

                _emit 0x0F
                _emit 0xA2 /* cpuid */

                mov dword ptr [ProcessorName+16],eax
                mov dword ptr [ProcessorName+20],ebx
                mov dword ptr [ProcessorName+24],ecx
                mov dword ptr [ProcessorName+28],edx
                add di, 16

                mov eax, 0x80000004 /* Processor Brand String */

                _emit 0x0F
                _emit 0xA2 /* cpuid */

                mov dword ptr [ProcessorName+32],eax
                mov dword ptr [ProcessorName+36],ebx
                mov dword ptr [ProcessorName+40],ecx
                mov dword ptr [ProcessorName+44],edx
                mov byte  ptr [ProcessorName+48],0 /* NUL-terminate string */
        }
#elif __GNUC__
        __asm__ __volatile__ (
                "movl $0x80000002, %%eax\n\t"   /* Processor Brand String */
                "cpuid\n\t"

                "movl %%eax, (%%edi)\n\t"
                "movl %%ebx, 4(%%edi)\n\t"
                "movl %%ecx, 8(%%edi)\n\t"
                "movl %%edx, 12(%%edi)\n\t"

                "movl $0x80000003, %%eax\n\t"   /* Processor Brand String */
                "cpuid\n\t"

                "movl %%eax, 16(%%edi)\n\t"
                "movl %%ebx, 20(%%edi)\n\t"
                "movl %%ecx, 24(%%edi)\n\t"
                "movl %%edx, 28(%%edi)\n\t"

                "movl $0x80000004, %%eax\n\t"   /* Processor Brand String */
                "cpuid\n\t"

                "movl %%eax, 32(%%edi)\n\t"
                "movl %%ebx, 36(%%edi)\n\t"
                "movl %%ecx, 40(%%edi)\n\t"
                "movl %%edx, 44(%%edi)\n\t"

                "movb $0, 48(%%edi)\n\t" /* NUL-terminate string */

                :
        : "D" (&ProcessorName)                  /* input */
        : "memory" /* clobbered list */
      );
#else
	return;
#endif /* _MSC_VER || __GNUC__ */

	Com_Printf( "%s\n", ProcessorName );
}



/*
-----------------------------------------------------------------------------
 Function: x86_get_cpu_ProcessorName_LUT -CPU name identification.

 Parameters: s -[in/out] Valid pointer to cpu_info_struct structure.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void x86_get_cpu_ProcessorName_LUT( cpu_info_struct *s )
{

	#define MAX_CPU_VENDORS		2
	#define MAX_CPU_FAMILY		3
	#define MAX_CPU_MODEL		16
	#define PROCESSOR_UNKNOWN	NULL


	char *cpuname
		/* Vendor */ [ MAX_CPU_VENDORS ]
		/* Family */ [ MAX_CPU_FAMILY ]
		/* Model  */ [ MAX_CPU_MODEL ]
        = {
                /* Intel Corporation, "GenuineIntel" */
                {
                        /* 5 i586 */
                        {
                                /* 0 */ PROCESSOR_UNKNOWN,
                                /* 1 */ "Intel(R) Pentium(R) Processors (60/66)",
                                /* 2 */ "Intel(R) Pentium(R) Processors (75-200)",
                                /* 3 */ PROCESSOR_UNKNOWN,
                                /* 4 */ "Intel(R) Pentium(R) Processor with MMX(tm) technology (166/200)",
                                /* 5 */ PROCESSOR_UNKNOWN,
                                /* 6 */ PROCESSOR_UNKNOWN,
                                /* 7 */ PROCESSOR_UNKNOWN,
                                /* 8 */ PROCESSOR_UNKNOWN,
                                /* 9 */ PROCESSOR_UNKNOWN,
                                /* A */ PROCESSOR_UNKNOWN,
                                /* B */ PROCESSOR_UNKNOWN,
                                /* C */ PROCESSOR_UNKNOWN,
                                /* D */ PROCESSOR_UNKNOWN,
                                /* E */ PROCESSOR_UNKNOWN,
                                /* F */ PROCESSOR_UNKNOWN
                        },
                        /* 6 i686 */
                        {
                                /* 0 */ PROCESSOR_UNKNOWN,
                                /* 1 */ "Intel(R) Pentium(R) Pro Processor",
                                /* 2 */ PROCESSOR_UNKNOWN,
                                /* 3 */ "Intel(R) Pentium(R) II Processor, model 3",
                                /* 4 */ PROCESSOR_UNKNOWN,
                                /* 5 */ "Intel(R) Pentium(R) II Processor, model 5",
                                /* 6 */ "Intel(R) Celeron(R) Processor, model 6",
                                /* 7 */ "Intel(R) Pentium(R) III Processor, model 7",
                                /* 8 */ "Intel(R) Pentium(R) III Processor, model 8",
                                /* 9 */ "Intel(R) Pentium(R) M Processor",
                                /* A */ "Intel(R) Pentium(R) III Xeon Processor, model A",
                                /* B */ "Intel(R) Pentium(R) III Processor, model B",
                                /* C */ PROCESSOR_UNKNOWN,
                                /* D */ PROCESSOR_UNKNOWN,
                                /* E */ PROCESSOR_UNKNOWN,
                                /* F */ PROCESSOR_UNKNOWN
                        }
                },
                /* Advanced Micro Devices, "AuthenticAMD" */
                {
                        /* 5 K5/K6 */
                        {
                                /* 0 */ "AMD-K5 Model 0",
                                /* 1 */ "AMD-K5 Model 1",
                                /* 2 */ "AMD-K5 Model 2",
                                /* 3 */ "AMD-K5 Model 3",
                                /* 4 */ PROCESSOR_UNKNOWN,
                                /* 5 */ PROCESSOR_UNKNOWN,
                                /* 6 */ "AMD-K6 Model 6",
                                /* 7 */ "AMD-K6 Model 7",
                                /* 8 */ "AMD-K6-2 Model 8",
                                /* 9 */ "AMD-K6-III Model 9",
                                /* A */ PROCESSOR_UNKNOWN,
                                /* B */ PROCESSOR_UNKNOWN,
                                /* C */ PROCESSOR_UNKNOWN,
                                /* D */ PROCESSOR_UNKNOWN,
                                /* E */ PROCESSOR_UNKNOWN,
                                /* F */ PROCESSOR_UNKNOWN
                        },

                        /* 6 Athlon / Duron */
                        {
                                /* 0 */ PROCESSOR_UNKNOWN,
                                /* 1 */ "AMD Athlon Model 1",
                                /* 2 */ "AMD Athlon Model 2",
                                /* 3 */ "AMD Duron Model 3",
                                /* 4 */ "AMD Athlon Model 4",
                                /* 5 */ PROCESSOR_UNKNOWN,
                                /* 6 */ "AMD Athlon MP Model 6",
                                /* 7 */ "AMD Duron Model 7",
                                /* 8 */ "AMD Athlon XP Model 8",
                                /* 9 */ PROCESSOR_UNKNOWN,
                                /* A */ "Mobile AMD Athlon XP Model 8",
                                /* B */ PROCESSOR_UNKNOWN,
                                /* C */ PROCESSOR_UNKNOWN,
                                /* D */ PROCESSOR_UNKNOWN,
                                /* E */ PROCESSOR_UNKNOWN,
                                /* F */ PROCESSOR_UNKNOWN
                        },

                        /* 7 Athlon / Opteron */
                        {
                                /* 0 */ PROCESSOR_UNKNOWN,
                                /* 1 */ PROCESSOR_UNKNOWN,
                                /* 2 */ PROCESSOR_UNKNOWN,
                                /* 3 */ PROCESSOR_UNKNOWN,
                                /* 4 */ "AMD Athlon 64 Model 4",
                                /* 5 */ "AMD Opteron Model 5",
                                /* 6 */ PROCESSOR_UNKNOWN,
                                /* 7 */ PROCESSOR_UNKNOWN,
                                /* 8 */ PROCESSOR_UNKNOWN,
                                /* 9 */ PROCESSOR_UNKNOWN,
                                /* A */ PROCESSOR_UNKNOWN,
                                /* B */ PROCESSOR_UNKNOWN,
                                /* C */ PROCESSOR_UNKNOWN,
                                /* D */ PROCESSOR_UNKNOWN,
                                /* E */ PROCESSOR_UNKNOWN,
                                /* F */ PROCESSOR_UNKNOWN
                        }
                }
}; /* cpuname */


	W32 regs[ 4 ];

	W32 Type, Family, Model;

	char *TempName;

	x86_do_cpuid( 1, regs );


	if( s->cpu_type == INTEL_X86 ) {
		if( regs[ 1 ] & 0xff ) {
			/* Table 10 from "Intel Processor Identification" document. */
			switch( regs[ 1 ] & 0xff ) {
				case 0x01:
					Com_Printf( "Intel(R) Celeron(R) Processor\n" );
					break;

				case 0x02:
				case 0x04:
					Com_Printf( "Intel(R) Pentium(R) III Processor\n" );
					break;

				case 0x03:
					if( regs[ 0 ] == 0x000006B1 )
					{
						Com_Printf( "Intel(R) Celeron(R) Processor\n" );
					} else {
						Com_Printf( "Intel(R) Pentium(R) III Xeon(tm) Processor\n" );
					}
					break;

				case 0x06:
					Com_Printf( "Mobile Intel(R) Pentium(R) III Processor-M\n" );
					break;

				case 0x07:
					Com_Printf( "Mobile Intel(R) Celeron(R) Processor\n" );
					break;

				case 0x08:
					if( regs[ 0 ] >= 0x00000F13 ) {
						Com_Printf( "Intel(R) Genuine Processor\n" );
					} else {
						Com_Printf( "Intel(R) Pentium(R) 4 Processor\n" );
					}
					break;

				case 0x09:
					Com_Printf( "Intel(R) Pentium(R) 4 Processor\n" );
					break;

				case 0x0A:
					Com_Printf( "Intel(R) Celeron(R) Processor\n" );
					break;

				case 0x0B:
					if( regs[ 0 ] < 0x00000F13 ) {
						Com_Printf( "Intel(R) Xeon(tm) Processor MP\n" );
					} else {
						Com_Printf( "Intel(R) Xeon(tm) Processor\n" );
					}
					break;

				case 0x0C:
					Com_Printf( "Intel(R) Xeon(tm) Processor MP\n" );
					break;

				case 0x0E:
					if( regs[ 0 ] < 0x00000F13 ) {
						Com_Printf( "Intel(R) Xeon(tm) Processor\n" );
					} else {
						Com_Printf( "Mobile Intel(R) Pentium(R) 4 Processor-M\n" );
					}
					break;

				case 0x0F:
				case 0x13:
					Com_Printf( "Mobile Intel(R) Celeron(R) Processor\n" );
					break;

				case 0x16:
					Com_Printf( "Intel(R) Pentium(R) M Processor\n" );
					break;

				default:
					Com_Printf( "Unknown BrandID\n" );
					break;

			} /* end switch BrandID */

			return;

		} /* end if BrandID */


		/* This only works on Pentium and Subsequent Processor Signatures */
		Type    = (regs[ 0 ] >> 12) & 0x03;
		Family  = (regs[ 0 ] >> 8) & 0x0F;
		Model   = (regs[ 0 ] >> 4) & 0x0F;

		if( Type == 1 ) {
			switch( Model ) {
				case 1:
					if( Family == 5 ) {
						Com_Printf( "Intel(R) Pentium(R) OverDrive Processor for Pentium(R) Processor (60/66)\n" );
					} else if( Family == 6 ) {
						Com_Printf( "Intel(R) Pentium(R) II OverDrive Processor\n" );
					} else {
						Com_Printf( "Unknown OverDrive Processor\n" );
					}
					break;

				case 2:
					Com_Printf( "Intel(R) Pentium(R) OverDrive Processor for Pentium(R) Processor (75-133)\n" );
					break;

				case 3:
					Com_Printf( "Intel(R) Pentium(R) OverDrive Processors for Intel486 Processor-based systems\n" );
					break;

				case 4:
					Com_Printf( "Intel(R) Pentium(R) OverDrive Processor with MMX(tm) technology for Pentium(R) Processor (75-133)\n" );
					break;

				default:
					Com_Printf( "Unknown OverDrive Processor\n" );
					break;

			} /* end switch Model */

			return;

		} /* end "if Type == 1" */

		/* Do NOT do 486 */
		if( Family < 5 || Model >= MAX_CPU_MODEL ) {
			return;
		}
		Family -= 5; /* Array lookup */

		TempName = cpuname[ 0 ][ Family ][ Model ];

		if( TempName != NULL ) {
			Com_Printf( "%s\n", TempName );
		}

		return;

	} /* end "if s->cpu_type == INTEL_X86" */


	if( s->cpu_type == AMD_X86 ) {
		Family  = (regs[ 0 ] >> 8) & 0x0F;
		Model   = (regs[ 0 ] >> 4) & 0x0F;

		/* 64-bit processor */
		if( Family == 0x0F ) {
			TempName = cpuname[ 1 ][ 2 ][ Model ];

			if( TempName != NULL ) {
				Com_Printf( "%s\n", TempName );
			}
		}

		/* Do NOT do Am486 or Am5x86 */
		if( Family >= MAX_CPU_FAMILY || Family < 5 ||
			Model >= MAX_CPU_MODEL ) {
			return;
		}
		Family -= 5; /* Array lookup */

		TempName = cpuname[ 1 ][ Family ][ Model ];

		if( TempName != NULL ) {
			Com_Printf( "%s\n", TempName );
		}

		return;
	}

	return;
}


/*
-----------------------------------------------------------------------------
 Function: Intel_TLB_Cache_LUT -Intel cache lookup.

 Parameters: x -[in] Cache lookup code.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void Intel_TLB_Cache_LUT( W16 x )
{
	x &= 0xff;

	switch( x ) {
		case 0:
			break;

		case 0x01:
			Com_Printf( "Instruction TLB: 4-KBPages, 4-way set associative, 32 entries\n" );
			break;

		case 0x02:
			Com_Printf( "Instruction TLB: 4-MB Pages, fully associative, 2 entries\n" );
			break;

		case 0x03:
			Com_Printf( "Data TLB: 4-KB Pages, 4-way set associative, 64 entries\n" );
			break;

		case 0x04:
			Com_Printf( "Data TLB: 4-MB Pages, 4-way set associative, 8 entries\n" );
			break;

		case 0x06:
			Com_Printf( "1st-level instruction cache: 8-KB, 4-way set associative, 32-byte line size\n" );
			break;

		case 0x08:
			Com_Printf( "1st-level instruction cache: 16-KB, 4-way set associative, 32-byte line size\n" );
			break;

		case 0x0A:
			Com_Printf( "1st-level data cache: 8-KB, 2-way set associative, 32-byte line size\n" );
			break;

		case 0x0C:
			Com_Printf( "1st-level data cache: 16-KB, 4-way set associative, 32-byte line size\n" );
			break;

		case 0x22:
			Com_Printf( "3rd-level cache: 512 KB, 4-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x23:
			Com_Printf( "3rd-level cache: 1-MB, 8-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x25:
			Com_Printf( "3rd-level cache: 2-MB, 8-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x29:
			Com_Printf( "3rd-level cache: 4-MB, 8-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x2C:
			Com_Printf( "1st-level data cache: 32-KB, 8-way set associative, 64-byte line size\n" );
			break;

		case 0x30:
			Com_Printf( "1st-level instruction cache: 32-KB, 8-way set associative, 64-byte line size\n" );
			break;

		case 0x39:
			Com_Printf( "2nd-level cache: 128-KB, 4-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x3B:
			Com_Printf( "2nd-level cache: 128-KB, 2-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x3C:
			Com_Printf( "2nd-level cache: 256-KB, 4-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x40:
			Com_Printf( "No 2nd-level cache or, if Processor contains a valid 2nd-level cache, no 3rd-level cache\n" );
			break;

		case 0x41:
			Com_Printf( "2nd-level cache: 128-KB, 4-way set associative, 32-byte line size\n" );
			break;

		case 0x42:
			Com_Printf( "2nd-level cache: 256-KB, 4-way set associative, 32-byte line size\n" );
			break;

		case 0x43:
			Com_Printf( "2nd-level cache: 512-KB, 4-way set associative, 32 byte line size\n" );
			break;

		case 0x44:
			Com_Printf( "2nd-level cache: 1-MB, 4-way set associative, 32 byte line size\n" );
			break;

		case 0x45:
			Com_Printf( "2nd-level cache: 2-MB, 4-way set associative, 32 byte line size\n" );
			break;

		case 0x50:
			Com_Printf( "Instruction TLB: 4-KB, 2-MB or 4-MB pages, fully associative, 64 entries\n" );
			break;

		case 0x51:
			Com_Printf( "Instruction TLB: 4-KB, 2-MB or 4-MB pages, fully associative, 128 entries\n" );
			break;

		case 0x52:
			Com_Printf( "Instruction TLB: 4-KB, 2-MB or 4-MB pages, fully associative, 256 entries\n" );
			break;

		case 0x5B:
			Com_Printf( "Data TLB: 4-KB or 4-MB pages, fully associative, 64 entries\n" );
			break;

		case 0x5C:
			Com_Printf( "Data TLB: 4-KB or 4-MB pages, fully associative, 128 entries\n" );
			break;

		case 0x5D:
			Com_Printf( "Data TLB: 4-KB or 4-MB pages, fully associative, 256 entries\n" );
			break;

		case 0x60:
			Com_Printf( "1st-level data cache: 16-KB, 8-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x66:
			Com_Printf( "1st-level data cache: 8-KB, 4-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x67:
			Com_Printf( "1st-level data cache: 16-KB, 4-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x68:
			Com_Printf( "1st-level data cache: 32-KB, 4 way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x70:
			Com_Printf( "Trace cache: 12K-uops, 8-way set associative\n" );
			break;

		case 0x71:
			Com_Printf( "Trace cache: 16K-uops, 8-way set associative\n" );
			break;

		case 0x72:
			Com_Printf( "Trace cache: 32K-uops, 8-way set associative\n" );
			break;

		case 0x79:
			Com_Printf( "2nd-level cache: 128-KB, 8-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x7A:
			Com_Printf( "2nd-level cache: 256-KB, 8-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x7B:
			Com_Printf( "2nd-level cache: 512-KB, 8-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x7C:
			Com_Printf( "2nd-level cache: 1-MB, 8-way set associative, sectored cache, 64-byte line size\n" );
			break;

		case 0x82:
			Com_Printf( "2nd-level cache: 256-KB, 8-way set associative, 32 byte line size\n" );
			break;

		case 0x83:
			Com_Printf( "2nd-level cache: 512-KB, 8-way set associative, 32 byte line size\n" );
			break;

		case 0x84:
			Com_Printf( "2nd-level cache: 1-MB, 8-way set associative, 32 byte line size\n" );
			break;

		case 0x85:
			Com_Printf( "2nd-level cache: 2-MB, 8-way set associative, 32 byte line size\n" );
			break;

		case 0x86:
			Com_Printf( "2nd-level cache: 512-KB, 4-way set associative, 64 byte line size\n" );
			break;

		case 0x87:
			Com_Printf( "2nd-level cache: 1-MB, 8-way set associative, 64 byte line size\n" );
			break;

		case 0xB0:
			Com_Printf( "Instruction TLB: 4-KB Pages, 4-way set associative, 128 entries\n" );
			break;

		case 0xB3:
			Com_Printf( "Data TLB: 4-KB Pages, 4-way set associative, 128 entries\n" );
			break;

		default:
			Com_Printf( "Unknown TLB/cache descriptor\n" );
			break;

	} /* end switch x */
}


/*
-----------------------------------------------------------------------------
 Function: x86_get_cpu_info -CPU identification.

 Parameters: s -[in/out] Valid pointer to cpu_info_struct structure.

 Returns: Nothing.

 Notes:
-----------------------------------------------------------------------------
*/
PRIVATE void x86_get_cpu_info(cpu_info_struct *s)
{
	W32 LargestStdFunction;
	W32 LargestExtFunction;
	W32 regs[4];
	char vendor_name[13];


    x86_do_cpuid(0, regs); /* Largest Standard Function Supported and
                              * CPU vendor name. */

    LargestStdFunction = regs[0];

    my_snprintf(vendor_name, sizeof(vendor_name), "%.4s%.4s%.4s",
				(PW8)(regs + 1), (PW8)(regs + 3), (PW8)(regs + 2));


	Com_Printf("Vendor: %s\n", vendor_name);


	if (my_strnicmp(vendor_name, "AuthenticAMD", 12) == 0) {
		s->cpu_type = AMD_X86;
	} else if (my_strnicmp(vendor_name, "GenuineIntel", 12) == 0) {
		s->cpu_type = INTEL_X86;
	} else {
		s->cpu_type = UNKNOWN_X86;
	}


	x86_do_cpuid(0x80000000, regs); /* Largest Extended Function Supported */

    LargestExtFunction = regs[0];


	if( LargestStdFunction >= 1 ) {
		x86_do_cpuid( 1, regs );

		/* Following are shared among all CPUs */
		if (regs[3] & INTEL_CPUID_TSC_FLAG) {
            s->bRDTSC = true;
        }

        if (regs[3] & INTEL_CPUID_MMX_FLAG) {
            s->bMMX = true;
        }

        if (regs[3] & INTEL_CPUID_SSE_FLAG) {
            s->bSSE = true;
        }

        if (regs[3] & INTEL_CPUID_SSE2_FLAG) {
            s->bSSE2 = true;
        }

        if (regs[2] & INTEL_CPUID_SSE3_FLAG) {
            s->bSSE3 = true;
        }

	}


	if (LargestExtFunction >= 0x80000001) {
        x86_do_cpuid(0x80000001, regs);

		if (s->cpu_type == AMD_X86) {

			if (regs[3] & AMD_CPUID_EXT_MMXEXT) {
                s->bMMXExt = true;
            }

            if (regs[3] & (unsigned long)AMD_CPUID_EXT_3DNOW) {
                s->b3DNow = true;
            }

			if (regs[3] & AMD_CPUID_EXT_3DNOWEXT) {
                s->b3DNowExt = true;
            }

        }
	}

	if( LargestExtFunction >= 0x80000004 ) {
		x86_get_cpu_ProcessorName();
	} else {
		x86_get_cpu_ProcessorName_LUT(s);
	}

	if (s->cpu_type == AMD_X86) {
#if 0
		/* 'temp' is undeclared: */
		Com_Printf("Family %d, Model %d, Stepping %d\n", ((temp >> 8) & 0x0F),
				   ((temp >> 4) & 0x0F), (temp & 0x0F));
#endif /* 0 */

		if( LargestExtFunction >= 0x80000005 ) {
			x86_do_cpuid( 0x80000005, regs );	/* Level 1 Cache Information */

			Com_Printf( "Level 1 Data Cache Size: %d KB\n", (regs[ 2 ] >> 24) & 0xFF );
			Com_Printf( "Level 1 Instruction Cache Size: %d KB\n", (regs[ 3 ] >> 24) & 0xFF );
		}

        if( LargestExtFunction >= 0x80000006 ) {
			W32 L2CacheSize;

			x86_do_cpuid( 0x80000006, regs );	/* Level 2 Cache Information */

			L2CacheSize = (regs[ 2 ] >> 16) & 0xFFFF;

			if( L2CacheSize == 1 ) { /* Duron Bug? */
				L2CacheSize = 64;
			}

			Com_Printf( "Level 2 Cache Size: %d KB\n", L2CacheSize );
		} /* end if LargestExtFunction >= 0x80000006 */
	} /* end if s->cpu_type == AMD_X86 */
	else if( s->cpu_type == INTEL_X86 ) {

		if( LargestStdFunction >= 2 ) {
			/* Decode TLB and cache info */
			W32 ntlb, i;

			ntlb = 255;
			Com_Printf( "TLB and cache info:\n" );

			for( i = 0; i < ntlb; ++i ) {
				x86_do_cpuid( 2, regs );  /* Cache and TLB Descriptors */

				ntlb =  regs[ 0 ] & 0xff;
				Intel_TLB_Cache_LUT( (W16)(regs[ 0 ] >> 8) );
				Intel_TLB_Cache_LUT( (W16)(regs[ 0 ] >> 16) );
				Intel_TLB_Cache_LUT( (W16)(regs[ 0 ] >> 24) );

				if( (regs[ 1 ] & 0x80000000) == 0 ) {
					Intel_TLB_Cache_LUT( (W16)regs[ 1 ] );
					Intel_TLB_Cache_LUT( (W16)(regs[ 1 ] >> 8) );
					Intel_TLB_Cache_LUT( (W16)(regs[ 1 ] >> 16) );
					Intel_TLB_Cache_LUT( (W16)(regs[ 1 ] >> 24) );
				}
				if( (regs[ 2 ] & 0x80000000) == 0 ) {
					Intel_TLB_Cache_LUT( (W16)regs[ 2 ] );
					Intel_TLB_Cache_LUT( (W16)(regs[ 2 ] >> 8) );
					Intel_TLB_Cache_LUT( (W16)(regs[ 2 ] >> 16) );
					Intel_TLB_Cache_LUT( (W16)(regs[ 2 ] >> 24) );
				}
				if( (regs[ 3 ] & 0x80000000) == 0 ) {
					Intel_TLB_Cache_LUT( (W16)regs[ 3 ] );
					Intel_TLB_Cache_LUT( (W16)(regs[ 3 ] >> 8) );
					Intel_TLB_Cache_LUT( (W16)(regs[ 3 ] >> 16) );
					Intel_TLB_Cache_LUT( (W16)(regs[ 3 ] >> 24) );
				}
			} /* end for i = 0; i < ntlb; ++i */
		} /* end if LargestStdFunction >= 2 */
	} /* end s->cpu_type == INTEL_X86 */
}


/*
-----------------------------------------------------------------------------
 Function: x86_Get_CPU_frequency -Calculates x86 CPU frequency.

 Parameters: Nothing.

 Returns: On success returns CPU frequency in Mhz, otherwise zero.

 Notes: CPU must be capable of RDTSC.

	The measured accuracy is dependent on the accuracy of the reference
	period. A longer reference period produces a more accurate result. In
	addition, repeating the calculation multiple times may also improve
	accuracy.

-----------------------------------------------------------------------------
*/
PRIVATE W32 x86_Get_CPU_frequency( void )
{
#ifdef _MSC_VER
	W32 startLOW, dtime;
	LARGE_INTEGER StartCount, EndCount, WaitTime;

	if( ! QueryPerformanceFrequency( &WaitTime ) ) {
		return 0;
	}

	QueryPerformanceCounter( &StartCount );

	__asm
	{
		/* Read Time Stamp Counter and save TSC immediately. */
		rdtsc
		mov [startLOW], eax

		mov ebx, 5000000
		AGAIN:
		dec ebx
		jnz AGAIN

		/* Read Time Stamp Counter and calculate time delta. */
		rdtsc
		sub eax, startLOW /* Find the difference */
		mov [dtime], eax
	}

	QueryPerformanceCounter( &EndCount );

	EndCount.QuadPart -= StartCount.QuadPart;
	if( EndCount.QuadPart == 0 ) {
		return 0;
	}

	return (W32)( dtime * WaitTime.QuadPart / EndCount.QuadPart / 1000000 );
#elif __unix__
	unsigned long long tscstart, tscstop;
	unsigned int start, stop;

	struct timeval tv;
	struct timespec ts;

	/* Get start time in micro-seconds */
	gettimeofday( &tv, NULL );
	start = tv.tv_sec * 1000000 + tv.tv_usec;

	/* tscstart time */
	__asm__ __volatile__(	"rdtsc\n\t"
		: "=A" (tscstart)
	);

	/* Sleep */
	ts.tv_sec  =  50000 / 1000000;
    ts.tv_nsec = (50000 % 1000000) * 1000;
	nanosleep( &ts, NULL );

	/* tscstop time */
	__asm__ __volatile__(	"rdtsc\n\t"
		: "=A" (tscstop)
	);

	/* Get stop time in micro-seconds */
	gettimeofday( &tv, NULL );
	stop = tv.tv_sec * 1000000 + tv.tv_usec;

	return( (tscstop - tscstart) / (stop - start) );
#else
	return 0;
#endif /* _MSC_VER || __unix__ */
}



/*
-----------------------------------------------------------------------------
 Function: Get_CPU_info -CPU identification.

 Parameters: Nothing.

 Returns: Nothing.

 Notes: Fills in cpu_info_struct main_cpu_s structure.
-----------------------------------------------------------------------------
*/
PUBLIC void Get_CPU_info(void)
{

	memset( &main_cpu_s, 0, sizeof(cpu_info_struct));
#ifdef _MSC_VER
	__asm
	{
		pusha      /* Save all registers on the stack */
	}
#endif /* _MSC_VER */
	if (! x86_can_do_cpuid()) {
		main_cpu_s.cpu_type = UNKNOWN_X86;
		Com_Printf("CPU recognized as some sort of x86, but not sure which.\n");
	} else {
		x86_get_cpu_info(&main_cpu_s);


		main_cpu_s.cpu_frequency = x86_Get_CPU_frequency();

		if (main_cpu_s.cpu_frequency) {
			Com_Printf("CPU Frequency: %lu Mhz\n", main_cpu_s.cpu_frequency);
		} else {
			Com_Printf("CPU Frequency: unknown\n");
		}
	}
#ifdef _MSC_VER
	__asm
	{
		popa       /* Restore saved registers */
	}
#endif /* _MSC_VER */
}


#else /* !__i386__: */
PUBLIC void Get_CPU_info(void)
{

	memset(&main_cpu_s, 0, sizeof(cpu_info_struct));

	main_cpu_s.cpu_type = NON_X86;

	Com_Printf("CPU was NOT recognized as i386, cannot say more about it.\n");

}
#endif /* __i386__ */

/* EOF */
