/*============================================================================*/
/*
 Copyright (c) 2022, Isaac Marino Bavaresco
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
	 * Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	 * Neither the name of the author nor the
	   names of its contributors may be used to endorse or promote products
	   derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*============================================================================*/
/*
WARNING:

1) 'rad' is radians. 'Rad' is radiation.

2) The below combinations of prefix+unit are understood incorrectly by the routines.

centi-u (atomic mass unit)		(mass)		-> cu		-> cup						(volume)
atto-u (atomic mass unit)		(mass)		-> au		-> astronomical unit		(distance)
quecto-tonne (metric tonne)		(mass)		-> qt		-> quart					(volume)
femto-tonne (metric tonne)		(mass)		-> ft		-> foot						(distance)
centi-tonne (metric tonne)		(mass)		-> ct		-> carat					(mass)
yocto-radian					(angle)		-> yr		-> year						(time)
hecto-radian					(angle)		-> hr		-> hour						(time)
milli-phot						(luminance)	-> mph		-> mile per hour			(speed)
mega-phot						(luminance)	-> Mph		-> mile per hour			(speed)
kilo-phot						(luminance)	-> kph		-> kilometer per hour		(speed)
nano-mile						(distance)	-> nmile	-> nautical mile			(distance)
nano-mile						(distance)	-> nmi		-> nautical mile			(distance)
milli-league					(distance)	-> mleague	-> marine league			(distance)
mega-league						(distance)	-> Mleague	-> marine league			(distance)
femto-lambert					(luminance)	-> flam		-> footlambert				(luminance)
milli-inch						(distance)	-> min		-> minute					(time)
pico-hour						(time)		-> ph		-> phot						(luminance)
yocto-day						(time)		-> yd		-> yard						(distance)
ronto-day						(time)		-> rd		-> rod						(distance)
centi-day						(time)		-> cd		-> candela					(luminous intensity)
pico-c (speed of light)			(speed)		-> pc		-> parsec					(distance)
femto-c (speed of light)		(speed)		-> fc		-> footcandle				(luminance)
atto-c (speed of light)			(speed)		-> ac		-> acre						(area)
deci-au	(astronomical unit)		(distance)	-> dau		-> deca-u (atomic mass unit)(mass)

nano-anoJ (portuguese for year)	(time)		-> nanoJ	-> nano-Joule				(energy) Use 'nanoanoJ' or 'nyrJ'
nano-anoG (portuguese for year)	(time)		-> nanoG	-> nano-Gauss				(magnetic flux density) Use 'nanoanoG' of 'nyrG'
nano-anos (portuguese for years)(time)		-> nanos	-> nano-second				(time) Use 'nanoanos' or 'nyr'
nano-ano (portuguese for year)	(time)		-> nano		-> Error					Use 'nanoano' or 'nyr'

deci-ac	(acre)					(area)		-> dac		-> deca-c (speed of light)	(speed)
peta-are						(area)		-> Pa		-> Pascal					(pressure)

*/
/*============================================================================*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "UnitsParser.h"
/*============================================================================*/
#define	LENGTH(a) (sizeof (a)/sizeof (a)[0])
/*============================================================================*/
#define	CHARACTER_ANGSTROM			0xc5
#define	CHARACTER_MICRO				0xe6
#define CHARACTER_DEGREE			0xf8
#define	CHARACTER_SUPERSCRIPT_TWO	0xfd
#define	CHARACTER_SUPERSCRIPT_THREE	0xfc
/*============================================================================*/
typedef struct
	{
	const char		*Name;
	const char		*Symbol;
	long double		Value;
	} prefix_t;
/*============================================================================*/
static const prefix_t Prefixes[]	=
	{
	[PREFIX_NONE]	= { .Name = "",			.Symbol = "",		.Value = 1.0 },

	[PREFIX_QUECTO]	= { .Name = "quecto",	.Symbol = "q",		.Value = 1.0e-30 },
	[PREFIX_RONTO]	= { .Name = "ronto",	.Symbol = "r",		.Value = 1.0e-27 },
	[PREFIX_YOCTO]	= { .Name = "yocto",	.Symbol = "y",		.Value = 1.0e-24 },
	[PREFIX_ZEPTO]	= { .Name = "zepto",	.Symbol = "z",		.Value = 1.0e-21 },
	[PREFIX_ATTO]	= { .Name = "atto",		.Symbol = "a",		.Value = 1.0e-18 },
	[PREFIX_FEMTO]	= { .Name = "femto",	.Symbol = "f",		.Value = 1.0e-15 },
	[PREFIX_PICO]	= { .Name = "pico",		.Symbol = "p",		.Value = 1.0e-12 },
	[PREFIX_NANO]	= { .Name = "nano",		.Symbol = "n",		.Value = 1.0e-9 },
	[PREFIX_MICRO]	= { .Name = "micro",	.Symbol = "\xe6",	.Value = 1.0e-6 },
	[PREFIX_MILLI]	= { .Name = "milli",	.Symbol = "m",		.Value = 1.0e-3 },
	[PREFIX_CENTI]	= { .Name = "centi",	.Symbol = "c",		.Value = 1.0e-2 },
	[PREFIX_DECI]	= { .Name = "deci",		.Symbol = "d",		.Value = 1.0e-1 },

	[PREFIX_DECA]	= { .Name = "deca",		.Symbol = "da",		.Value = 1.0e+1 },
	[PREFIX_HECTO]	= { .Name = "hecto",	.Symbol = "h",		.Value = 1.0e+2 },
	[PREFIX_KILO]	= { .Name = "kilo",		.Symbol = "k",		.Value = 1.0e+3 },
	[PREFIX_MEGA]	= { .Name = "mega",		.Symbol = "M",		.Value = 1.0e+6 },
	[PREFIX_GIGA]	= { .Name = "giga",		.Symbol = "G",		.Value = 1.0e+9 },
	[PREFIX_TERA]	= { .Name = "tera",		.Symbol = "T",		.Value = 1.0e+12 },
	[PREFIX_PETA]	= { .Name = "peta",		.Symbol = "P",		.Value = 1.0e+15 },
	[PREFIX_EXA]	= { .Name = "exa",		.Symbol = "E",		.Value = 1.0e+18 },
	[PREFIX_ZETTA]	= { .Name = "zetta",	.Symbol = "Z",		.Value = 1.0e+21 },
	[PREFIX_YOTTA]	= { .Name = "yotta",	.Symbol = "Y",		.Value = 1.0e+24 },
	[PREFIX_RONNA]	= { .Name = "ronna",	.Symbol = "R",		.Value = 1.0e+27 },
	[PREFIX_QUETTA]	= { .Name = "quetta",	.Symbol = "Q",		.Value = 1.0e+30 }
	};
/*============================================================================*/
double GetPrefixValue( prefixid_t PrefixId )
	{
	if( PrefixId < PREFIX_NONE || PrefixId >= LENGTH( Prefixes ))
		return 0.0;

	return Prefixes[PrefixId].Value;
	}
/*============================================================================*/
const char *GetPrefixSymbol( prefixid_t PrefixId )
	{
	if( PrefixId < PREFIX_NONE || PrefixId >= LENGTH( Prefixes ))
		return NULL;

	return Prefixes[PrefixId].Symbol;
	}
/*============================================================================*/
const char *GetPrefixName( prefixid_t PrefixId )
	{
	if( PrefixId < PREFIX_NONE || PrefixId >= LENGTH( Prefixes ))
		return NULL;

	return Prefixes[PrefixId].Name;
	}
/*============================================================================*/
typedef struct
	{
	const char	*String;
	prefixid_t	PrefixId;
	int			Flags;
	} prefixalias_t;
/*============================================================================*/
#define	IS_UTF8					1
#define	IS_ANSI					2
#define	IS_U					4
/*============================================================================*/
const prefixalias_t	PrefixAlias[]	=
	{
		{ .String = "zetta",	.PrefixId = PREFIX_ZETTA,	.Flags = 0 },
		{ .String = "zepto",	.PrefixId = PREFIX_ZEPTO,	.Flags = 0 },
		{ .String = "yotta",	.PrefixId = PREFIX_YOTTA,	.Flags = 0 },
		{ .String = "yocto",	.PrefixId = PREFIX_YOCTO,	.Flags = 0 },
		{ .String = "micro",	.PrefixId = PREFIX_MICRO,	.Flags = 0 },
		{ .String = "tera",		.PrefixId = PREFIX_TERA,	.Flags = 0 },
		{ .String = "ronto",	.PrefixId = PREFIX_RONTO,	.Flags = 0 },
		{ .String = "ronna",	.PrefixId = PREFIX_RONNA,	.Flags = 0 },
		{ .String = "quecto",	.PrefixId = PREFIX_QUECTO,	.Flags = 0 },
		{ .String = "quetta",	.PrefixId = PREFIX_QUETTA,	.Flags = 0 },
		{ .String = "pico",		.PrefixId = PREFIX_PICO,	.Flags = 0 },
		{ .String = "peta",		.PrefixId = PREFIX_PETA,	.Flags = 0 },
		{ .String = "nano",		.PrefixId = PREFIX_NANO,	.Flags = 0 },
		{ .String = "milli",	.PrefixId = PREFIX_MILLI,	.Flags = 0 },
		{ .String = "mili",		.PrefixId = PREFIX_MILLI,	.Flags = 0 },
		{ .String = "mega",		.PrefixId = PREFIX_MEGA,	.Flags = 0 },
		{ .String = "kilo",		.PrefixId = PREFIX_KILO,	.Flags = 0 },
		{ .String = "quilo",	.PrefixId = PREFIX_KILO,	.Flags = 0 },
		{ .String = "hecto",	.PrefixId = PREFIX_HECTO,	.Flags = 0 },
		{ .String = "giga",		.PrefixId = PREFIX_GIGA,	.Flags = 0 },
		{ .String = "femto",	.PrefixId = PREFIX_FEMTO,	.Flags = 0 },
		{ .String = "exa",		.PrefixId = PREFIX_EXA,		.Flags = 0 },
		{ .String = "deca",		.PrefixId = PREFIX_DECA,	.Flags = 0 },
		{ .String = "deci",		.PrefixId = PREFIX_DECI,	.Flags = 0 },
		{ .String = "centi",	.PrefixId = PREFIX_CENTI,	.Flags = 0 },
		{ .String = "atto",		.PrefixId = PREFIX_ATTO,	.Flags = 0 },
		{ .String = "Z",		.PrefixId = PREFIX_ZETTA,	.Flags = 0 },
		{ .String = "z",		.PrefixId = PREFIX_ZEPTO,	.Flags = 0 },
		{ .String = "Y",		.PrefixId = PREFIX_YOTTA,	.Flags = 0 },
		{ .String = "y",		.PrefixId = PREFIX_YOCTO,	.Flags = 0 },
		{ .String = "u",		.PrefixId = PREFIX_MICRO,	.Flags = IS_U },
		{ .String = "T",		.PrefixId = PREFIX_TERA,	.Flags = 0 },
		{ .String = "r",		.PrefixId = PREFIX_RONTO,	.Flags = 0 },
		{ .String = "R",		.PrefixId = PREFIX_RONNA,	.Flags = 0 },
		{ .String = "q",		.PrefixId = PREFIX_QUECTO,	.Flags = 0 },
		{ .String = "Q",		.PrefixId = PREFIX_QUETTA,	.Flags = 0 },
		{ .String = "p",		.PrefixId = PREFIX_PICO,	.Flags = 0 },
		{ .String = "P",		.PrefixId = PREFIX_PETA,	.Flags = 0 },
		{ .String = "n",		.PrefixId = PREFIX_NANO,	.Flags = 0 },
		{ .String = "m",		.PrefixId = PREFIX_MILLI,	.Flags = 0 },
		{ .String = "M",		.PrefixId = PREFIX_MEGA,	.Flags = 0 },
		{ .String = "k",		.PrefixId = PREFIX_KILO,	.Flags = 0 },
		{ .String = "h",		.PrefixId = PREFIX_HECTO,	.Flags = 0 },
		{ .String = "G",		.PrefixId = PREFIX_GIGA,	.Flags = 0 },
		{ .String = "f",		.PrefixId = PREFIX_FEMTO,	.Flags = 0 },
		{ .String = "E",		.PrefixId = PREFIX_EXA,		.Flags = 0 },
		{ .String = "da",		.PrefixId = PREFIX_DECA,	.Flags = 0 },
		{ .String = "d",		.PrefixId = PREFIX_DECI,	.Flags = 0 },
		{ .String = "c",		.PrefixId = PREFIX_CENTI,	.Flags = 0 },
		{ .String = "a",		.PrefixId = PREFIX_ATTO,	.Flags = 0 },
//		{ .String = "\xce\xbc",	.PrefixId = PREFIX_MICRO,	.Flags = IS_UTF8 },	// UTF-8 Mu
//		{ .String = "\xc2\xb5",	.PrefixId = PREFIX_MICRO,	.Flags = IS_UTF8 },	// UTF-8 Micro
//		{ .String = "\xb5",		.PrefixId = PREFIX_MICRO,	.Flags = IS_ANSI },	// ANSI Micro
		{ .String = "\xe6",		.PrefixId = PREFIX_MICRO,	.Flags = IS_ANSI }	// ? Micro
	};
/*============================================================================*/
prefixid_t ParsePrefix( const char ** const p, int Flags )
	{
	int		i;

	if( p == NULL || *p == NULL )
		return PREFIX_ERROR;

	for( i = 0; i < LENGTH( PrefixAlias ); i++ )
		{
		size_t	len;

		if(( PrefixAlias[i].Flags & Flags ) == 0 && strncmp( *p, PrefixAlias[i].String, len = strlen( PrefixAlias[i].String )) == 0 )
			{
			*p += len;
			return PrefixAlias[i].PrefixId;
			}
		}

	return PREFIX_NONE;
	}
/*============================================================================*/
typedef union
	{
	struct
		 {
		int32_t		Exp_s		: 4;
		int32_t		Exp_m		: 4;
		int32_t		Exp_kg		: 4;
		int32_t		Exp_A		: 4;
		int32_t		Exp_K		: 4;
		int32_t		Exp_mol		: 4;
		int32_t		Exp_cd		: 4;
		uint32_t	IsAngle		: 2;	// Used to tell apart dimensionless, angle and solid angle (all three are dimensionless).
		uint32_t	IsRadiation	: 2;	// Used to tell apart hertz from becquerel and gray from sievert.
		};
	uint32_t		Value;
	} exp_t;
/*============================================================================*/
typedef struct units_tag
	{
	unitid_t		Id;
	const char		*Name;
	const char		*Symbol;
	const char		*WhatItMeasures;
	exp_t			Exponents;
	} units_t;
/*============================================================================*/
static const units_t	Units[]	=
	{
	[UNIT_NONE]						= { .Id = UNIT_NONE,						.Name = "dimensionless",				.Symbol = "",			.WhatItMeasures = "",						.Exponents = { .Exp_s =  0, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},

	[UNIT_SECOND]					= { .Id = UNIT_SECOND,						.Name = "second",	 					.Symbol = "s",			.WhatItMeasures = "time",					.Exponents = { .Exp_s =  1, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_METER]					= { .Id = UNIT_METER,						.Name = "meter",	 					.Symbol = "m",			.WhatItMeasures = "distance/length",		.Exponents = { .Exp_s =  0, .Exp_m =  1, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_KILOGRAM]					= { .Id = UNIT_KILOGRAM,					.Name = "kilogram",	 					.Symbol = "kg",			.WhatItMeasures = "mass",			 		.Exponents = { .Exp_s =  0, .Exp_m =  0, .Exp_kg =  1, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_AMPERE]					= { .Id = UNIT_AMPERE,						.Name = "ampere",	 					.Symbol = "A",			.WhatItMeasures = "electric current",		.Exponents = { .Exp_s =  0, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  1, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_KELVIN]					= { .Id = UNIT_KELVIN,						.Name = "kelvin",	 					.Symbol = "K",			.WhatItMeasures = "temperature",			.Exponents = { .Exp_s =  0, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  1, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_MOLE]						= { .Id = UNIT_MOLE,						.Name = "mole",		 					.Symbol = "mol",		.WhatItMeasures = "quantity of particles",	.Exponents = { .Exp_s =  0, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  1, .Exp_cd =  0 }},
	[UNIT_CANDELA]					= { .Id = UNIT_CANDELA,						.Name = "candela",	 					.Symbol = "cd",			.WhatItMeasures = "luminous intensity",		.Exponents = { .Exp_s =  0, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  1 }},

	[UNIT_RADIAN]					= { .Id = UNIT_RADIAN,						.Name = "radian",	 					.Symbol = "r",			.WhatItMeasures = "angle",					.Exponents = { .Exp_s =  0, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0, .IsAngle = 1 }},
	[UNIT_STERADIAN]				= { .Id = UNIT_STERADIAN,					.Name = "steradian", 					.Symbol = "sr",			.WhatItMeasures = "solid angle",			.Exponents = { .Exp_s =  0, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0, .IsAngle = 2 }},
	[UNIT_HERTZ]					= { .Id = UNIT_HERTZ,						.Name = "hertz",	 					.Symbol = "Hz",			.WhatItMeasures = "frequency",				.Exponents = { .Exp_s = -1, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_NEWTON]					= { .Id = UNIT_NEWTON,						.Name = "newton",	 					.Symbol = "N",			.WhatItMeasures = "force",					.Exponents = { .Exp_s = -2, .Exp_m =  1, .Exp_kg =  1, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_PASCAL]					= { .Id = UNIT_PASCAL,						.Name = "pascal",	 					.Symbol = "Pa",			.WhatItMeasures = "pressure",				.Exponents = { .Exp_s = -2, .Exp_m = -1, .Exp_kg =  1, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_JOULE]					= { .Id = UNIT_JOULE,						.Name = "joule",	 					.Symbol = "J",			.WhatItMeasures = "work/energy",			.Exponents = { .Exp_s = -2, .Exp_m =  2, .Exp_kg =  1, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_WATT]						= { .Id = UNIT_WATT,						.Name = "watt",		 					.Symbol = "W",			.WhatItMeasures = "power",					.Exponents = { .Exp_s = -3, .Exp_m =  2, .Exp_kg =  1, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_COULOMB]					= { .Id = UNIT_COULOMB,						.Name = "coulomb",	 					.Symbol = "C",			.WhatItMeasures = "electric charge",		.Exponents = { .Exp_s =  1, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  1, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_VOLT]						= { .Id = UNIT_VOLT,						.Name = "volt",		 					.Symbol = "V",			.WhatItMeasures = "electric potential",	.Exponents = { .Exp_s = -3, .Exp_m =  2, .Exp_kg =  1, .Exp_A = -1, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_OHM]						= { .Id = UNIT_OHM,	 						.Name = "ohm",		 					.Symbol = "ohm" /*"\xce\xa9"*/,	.WhatItMeasures = "electrical resistance",	.Exponents = { .Exp_s = -3, .Exp_m =  2, .Exp_kg =  1, .Exp_A = -2, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_SIEMENS]					= { .Id = UNIT_SIEMENS,						.Name = "siemens",	 					.Symbol = "S",			.WhatItMeasures = "electrical conductance",	.Exponents = { .Exp_s =  3, .Exp_m = -2, .Exp_kg = -1, .Exp_A =  2, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_FARAD]					= { .Id = UNIT_FARAD,						.Name = "farad",	 					.Symbol = "F",			.WhatItMeasures = "electrical capacitance",	.Exponents = { .Exp_s =  4, .Exp_m = -2, .Exp_kg = -1, .Exp_A =  2, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_HENRY]					= { .Id = UNIT_HENRY,						.Name = "henry",	 					.Symbol = "H",			.WhatItMeasures = "electrical inductance",	.Exponents = { .Exp_s = -2, .Exp_m =  2, .Exp_kg =  1, .Exp_A = -2, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_TESLA]					= { .Id = UNIT_TESLA,						.Name = "tesla",	 					.Symbol = "T",			.WhatItMeasures = "magnetic flux density",	.Exponents = { .Exp_s = -2, .Exp_m =  0, .Exp_kg =  1, .Exp_A = -1, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_WEBER]					= { .Id = UNIT_WEBER,						.Name = "weber",	 					.Symbol = "Wb",			.WhatItMeasures = "magnetic flux",			.Exponents = { .Exp_s = -2, .Exp_m =  2, .Exp_kg =  1, .Exp_A = -1, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_BECQUEREL]				= { .Id = UNIT_BECQUEREL,					.Name = "becquerel",	 				.Symbol = "Bq",			.WhatItMeasures = "radioactivity",			.Exponents = { .Exp_s = -1, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0, .IsRadiation = 1 }},
	[UNIT_GRAY]						= { .Id = UNIT_GRAY,						.Name = "gray",	 						.Symbol = "Gy",			.WhatItMeasures = "absorbed radiation dose",.Exponents = { .Exp_s = -2, .Exp_m =  2, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0, .IsRadiation = 1 }},
	[UNIT_SIEVERT]					= { .Id = UNIT_SIEVERT,						.Name = "sievert",	 					.Symbol = "Sv",			.WhatItMeasures = "ionizing radiation dose",.Exponents = { .Exp_s = -2, .Exp_m =  2, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0, .IsRadiation = 2 }},
	[UNIT_KATAL]					= { .Id = UNIT_KATAL,						.Name = "katal",	 					.Symbol = "kat",		.WhatItMeasures = "catalytic activity",		.Exponents = { .Exp_s = -1, .Exp_m =  0, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  1, .Exp_cd =  0 }},

	[UNIT_CANDELA_PER_SQUARE_METER]	= { .Id = UNIT_CANDELA_PER_SQUARE_METER,	.Name = "candela per square meter",		.Symbol = "cd/m\xfd",	.WhatItMeasures = "luminance",				.Exponents = { .Exp_s =  0, .Exp_m = -2, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  1 }},
	[UNIT_PASCAL_SECOND]			= { .Id = UNIT_PASCAL_SECOND,				.Name = "pascal-second",				.Symbol = "Pa.s",		.WhatItMeasures = "dynamic viscosity",		.Exponents = { .Exp_s = -1, .Exp_m = -1, .Exp_kg =  1, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_SQUARE_METER_PER_SECOND]	= { .Id = UNIT_SQUARE_METER_PER_SECOND,		.Name = "square meter per second",		.Symbol = "m\xfd/s",	.WhatItMeasures = "kinematic viscosity",	.Exponents = { .Exp_s = -1, .Exp_m =  2, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_CUBIC_METER_PER_SECOND]	= { .Id = UNIT_CUBIC_METER_PER_SECOND,		.Name = "cubic meter per second",		.Symbol = "m\xfc/s",	.WhatItMeasures = "flow rate",				.Exponents = { .Exp_s = -1, .Exp_m =  3, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_COULOMB_PER_KG]			= { .Id = UNIT_COULOMB_PER_KG,				.Name = "coulomb per kilogram",			.Symbol = "C/kg",		.WhatItMeasures = "charge-to-mass ratio",	.Exponents = { .Exp_s =  1, .Exp_m =  0, .Exp_kg = -1, .Exp_A =  1, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0, .IsRadiation = 1 }},

	[UNIT_SQUARE_METER]				= { .Id = UNIT_SQUARE_METER,				.Name = "square meter",					.Symbol = "m\xfd",		.WhatItMeasures = "area",					.Exponents = { .Exp_s =  0, .Exp_m =  2, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_CUBIC_METER]				= { .Id = UNIT_CUBIC_METER,					.Name = "cubic meter",					.Symbol = "m\xfc",		.WhatItMeasures = "volume",					.Exponents = { .Exp_s =  0, .Exp_m =  3, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},

	[UNIT_METER_PER_SECOND]			= { .Id = UNIT_METER_PER_SECOND,			.Name = "meter per second",				.Symbol = "m/s",		.WhatItMeasures = "speed",					.Exponents = { .Exp_s = -1, .Exp_m =  1, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	[UNIT_METER_PER_SECOND_SQUARED]	= { .Id = UNIT_METER_PER_SECOND_SQUARED,	.Name = "meter per second squared",		.Symbol = "m/s\xfd",	.WhatItMeasures = "acceleration",			.Exponents = { .Exp_s = -2, .Exp_m =  1, .Exp_kg =  0, .Exp_A =  0, .Exp_K =  0, .Exp_mol =  0, .Exp_cd =  0 }},
	};
/*============================================================================*/
typedef struct
	{
	unitid_t	MainUnitId;
	int			Flags;
	long double	PreOffset;
	long double	Scale;
	long double	PostOffset;
	} unitsequivalence_t;
/*============================================================================*/
static unitsequivalence_t	Equivalence[]	=
	{
	/*------------------------------------------------------------------------------------------------*/
	/* The seven SI base units. */
	/*------------------------------------------------------------------------------------------------*/
	[UNIT_SECOND]					= { .MainUnitId = UNIT_SECOND,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_METER]					= { .MainUnitId = UNIT_METER,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_KILOGRAM]					= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_AMPERE]					= { .MainUnitId = UNIT_AMPERE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_KELVIN]					= { .MainUnitId = UNIT_KELVIN,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_MOLE]						= { .MainUnitId = UNIT_MOLE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_CANDELA]					= { .MainUnitId = UNIT_CANDELA,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },

	/*------------------------------------------------------------------------------------------------*/
	/* 19 of the 22 SI derived units. */
	/*------------------------------------------------------------------------------------------------*/
	[UNIT_RADIAN]					= { .MainUnitId = UNIT_RADIAN,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_STERADIAN]				= { .MainUnitId = UNIT_STERADIAN,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_HERTZ]					= { .MainUnitId = UNIT_HERTZ,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_NEWTON]					= { .MainUnitId = UNIT_NEWTON,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_PASCAL]					= { .MainUnitId = UNIT_PASCAL,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_JOULE]					= { .MainUnitId = UNIT_JOULE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_WATT]						= { .MainUnitId = UNIT_WATT,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_COULOMB]					= { .MainUnitId = UNIT_COULOMB,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_VOLT]						= { .MainUnitId = UNIT_VOLT,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_OHM]						= { .MainUnitId = UNIT_OHM,							.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_SIEMENS]					= { .MainUnitId = UNIT_SIEMENS,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_FARAD]					= { .MainUnitId = UNIT_FARAD,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_HENRY]					= { .MainUnitId = UNIT_HENRY,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_TESLA]					= { .MainUnitId = UNIT_TESLA,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_WEBER]					= { .MainUnitId = UNIT_WEBER,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_BECQUEREL]				= { .MainUnitId = UNIT_BECQUEREL,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_GRAY]						= { .MainUnitId = UNIT_GRAY,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_SIEVERT]					= { .MainUnitId = UNIT_SIEVERT,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_KATAL]					= { .MainUnitId = UNIT_KATAL,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },

	/*------------------------------------------------------------------------------------------------*/
	/* Coherent derived units. */
	/*------------------------------------------------------------------------------------------------*/
	[UNIT_CANDELA_PER_SQUARE_METER]	= { .MainUnitId = UNIT_CANDELA_PER_SQUARE_METER,	.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_PASCAL_SECOND]			= { .MainUnitId = UNIT_PASCAL_SECOND,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_SQUARE_METER_PER_SECOND]	= { .MainUnitId = UNIT_SQUARE_METER_PER_SECOND,		.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_COULOMB_PER_KG]			= { .MainUnitId = UNIT_COULOMB_PER_KG,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },

	[UNIT_SQUARE_METER]				= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_CUBIC_METER]				= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },

	[UNIT_METER_PER_SECOND]			= { .MainUnitId = UNIT_METER_PER_SECOND,			.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_METER_PER_SECOND_SQUARED]	= { .MainUnitId = UNIT_METER_PER_SECOND_SQUARED,	.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	/*------------------------------------------------------------------------------------------------*/
	[UNIT_POISE]					= { .MainUnitId = UNIT_PASCAL_SECOND,				.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.1,				.PostOffset	=	0.0 },
	[UNIT_STOKES]					= { .MainUnitId = UNIT_SQUARE_METER_PER_SECOND,		.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.0001,			.PostOffset	=	0.0 },

	[UNIT_LUMEN]					= { .MainUnitId = UNIT_CANDELA,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  7.95774715459e-2,	.PostOffset	=	0.0 },

	[UNIT_DEGREE]					= { .MainUnitId = UNIT_RADIAN,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	= M_PI/180.0,				.PostOffset	=	0.0 },
	[UNIT_GRADE]					= { .MainUnitId = UNIT_RADIAN,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	= 2.0*M_PI/400.0,			.PostOffset	=	0.0 },
	[UNIT_ARC_MINUTE]				= { .MainUnitId = UNIT_RADIAN,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	= M_PI/(180.0*60.0),		.PostOffset	=	0.0 },
	[UNIT_ARC_SECOND]				= { .MainUnitId = UNIT_RADIAN,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	= M_PI/(180.0*3600.0),		.PostOffset	=	0.0 },

	[UNIT_DEGREE_CELSIUS]			= { .MainUnitId = UNIT_KELVIN,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	= 273.15 },
	[UNIT_DEGREE_FAHRENHEIT]		= { .MainUnitId = UNIT_KELVIN,						.Flags = REJECT_PREFIX,		.PreOffset	= -32.0,	.Scale	=		  5.0/9.0,			.PostOffset	= 273.15 },
	[UNIT_DEGREE_RANKINE]			= { .MainUnitId = UNIT_KELVIN,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  5.0/9.0,			.PostOffset	=	0.0 },
	[UNIT_DEGREE_REAUMUR]			= { .MainUnitId = UNIT_KELVIN,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },

	[UNIT_FERMI]					= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0e-15,			.PostOffset	=	0.0 },
	[UNIT_ANGSTROM]					= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0e-10,			.PostOffset	=	0.0 },
	[UNIT_MICRON]					= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0e-6,			.PostOffset	=	0.0 },
	[UNIT_MIL]						= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.0000254,		.PostOffset	=	0.0 },
	[UNIT_POINT]					= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.0254/72,		.PostOffset	=	0.0 },
	[UNIT_PICA]						= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.0254/6,			.PostOffset	=	0.0 },
	[UNIT_INCH]						= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.0254,			.PostOffset	=	0.0 },
	[UNIT_FOOT]						= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.3048,			.PostOffset	=	0.0 },
	[UNIT_SURVEY_FOOT]				= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.304800609601,	.PostOffset	=	0.0 },
	[UNIT_YARD]						= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.9144,			.PostOffset	=	0.0 },
	[UNIT_FATHOM]					= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.82880365761,	.PostOffset	=	0.0 },
	[UNIT_ROD]						= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  5.02921005842,	.PostOffset	=	0.0 },
	[UNIT_CHAIN]					= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 20.1168402337,		.PostOffset	=	0.0 },
	[UNIT_FURLONG]					= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		201.168,			.PostOffset	=	0.0 },
	[UNIT_MILE]						= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   1609.344,			.PostOffset	=	0.0 },
	[UNIT_NAUTICAL_MILE]			= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   1852.0,				.PostOffset	=	0.0 },
	[UNIT_LAND_LEAGUE]				= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   4830.0,				.PostOffset	=	0.0 },
	[UNIT_MARINE_LEAGUE]			= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   5556.0,				.PostOffset	=	0.0 },
	[UNIT_BRAZILIAN_LEAGUE]			= { .MainUnitId = UNIT_METER,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   6600.0,				.PostOffset	=	0.0 },
	[UNIT_ASTRONOMICAL_UNIT]		= { .MainUnitId = UNIT_METER,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.495979e11,		.PostOffset	=	0.0 },
	[UNIT_LIGHT_YEAR]				= { .MainUnitId = UNIT_METER,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  9.460528400488e15,.PostOffset	= 	0.0 },
	[UNIT_PARSEC]					= { .MainUnitId = UNIT_METER,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  3.08567818585e16,	.PostOffset	=	0.0 },

	[UNIT_BARN]						= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0e-28,			.PostOffset	=	0.0 },
	[UNIT_SQUARE_INCH]				= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.00064516,		.PostOffset	=	0.0 },
	[UNIT_SQUARE_FOOT]				= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.09290304,		.PostOffset	=	0.0 },
	[UNIT_SQUARE_YARD]				= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.83612736,		.PostOffset	=	0.0 },
	[UNIT_ARE]						= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		100.0,				.PostOffset	=	0.0 },
	[UNIT_ACRE]						= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   4046.8564224,		.PostOffset	=	0.0 },
	[UNIT_ALQUEIRE_PAULISTA]		= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	  24200.0,				.PostOffset	=	0.0 },
	[UNIT_ALQUEIRE_GOIANO]			= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	  48400.0,				.PostOffset	=	0.0 },
	[UNIT_SQUARE_MILE]				= { .MainUnitId = UNIT_SQUARE_METER,				.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=   2589988.11034, 			.PostOffset	=	0.0 },

	[UNIT_TEASPOON]					= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  4.92892159375e-6,	.PostOffset	=	0.0 },
	[UNIT_TABLESPOON]				= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.47867647813e-5,	.PostOffset	=	0.0 },
	[UNIT_CUBIC_INCH]				= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.6387064e-5,		.PostOffset	=	0.0 },
	[UNIT_UK_FLUID_OUNCE]			= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.000028413075,	.PostOffset	=	0.0 },
	[UNIT_US_FLUID_OUNCE]			= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  2.95735295625e-5,	.PostOffset	=	0.0 },
	[UNIT_CUP]						= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  2.365882365e-4,	.PostOffset	=	0.0 },
	[UNIT_US_PINT]					= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.000473176473,	.PostOffset	=	0.0 },
	[UNIT_UK_PINT]					= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.00056826125,	.PostOffset	=	0.0 },
	[UNIT_QUART]					= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.000946352946,	.PostOffset	=	0.0 },
	[UNIT_LITER]					= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.001,			.PostOffset	=	0.0 },
	[UNIT_BOARDFOOT]				= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.002359737216,	.PostOffset	=	0.0 },
	[UNIT_US_GALLON]				= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.003785411784,	.PostOffset	=	0.0 },
	[UNIT_CANADA_GALLON]			= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.00454609,		.PostOffset	=	0.0 },
	[UNIT_UK_GALLON]				= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.004546092,		.PostOffset	=	0.0 },
	[UNIT_PECK]						= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.0088097675,		.PostOffset	=	0.0 },
	[UNIT_CUBIC_FOOT]				= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.02831684659,	.PostOffset	=	0.0 },
	[UNIT_BUSHEL]					= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.03523907,		.PostOffset	=	0.0 },
	[UNIT_BARREL]					= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.158987294928,	.PostOffset	=	0.0 },
	[UNIT_CUBIC_YARD]				= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.76455485798,	.PostOffset	=	0.0 },
	[UNIT_STERE]					= { .MainUnitId = UNIT_CUBIC_METER,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },

	[UNIT_KNOT]						= { .MainUnitId = UNIT_METER_PER_SECOND,			.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.51444444444,	.PostOffset	=	0.0 },
	[UNIT_KILOMETER_PER_HOUR]		= { .MainUnitId = UNIT_METER_PER_SECOND,			.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   1000.0/3600.0,		.PostOffset	=	0.0 },
	[UNIT_MILE_PER_HOUR]			= { .MainUnitId = UNIT_METER_PER_SECOND,			.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   1609.344/3600.0,		.PostOffset	=	0.0 },
	[UNIT_SPEED_OF_LIGHT]			= { .MainUnitId = UNIT_METER_PER_SECOND,			.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	= 299792458.0,				.PostOffset	=	0.0 },

	[UNIT_STANDARD_GRAVITY]			= { .MainUnitId = UNIT_METER_PER_SECOND_SQUARED,	.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  9.80665,			.PostOffset	=	0.0 },

	[UNIT_GRAM]						= { .MainUnitId = UNIT_KILOGRAM,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.001,			.PostOffset	=	0.0 },
	[UNIT_CARAT]					= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.0002,			.PostOffset	=	0.0 },
	[UNIT_GRAIN]					= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  0.00006479891,	.PostOffset	=	0.0 },
	[UNIT_POUND]					= { .MainUnitId = UNIT_KILOGRAM,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.45359237,		.PostOffset	=	0.0 },
	[UNIT_POUND_TROY]				= { .MainUnitId = UNIT_KILOGRAM,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.3732417,		.PostOffset	=	0.0 },
	[UNIT_OUNCE]					= { .MainUnitId = UNIT_KILOGRAM,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.028349523125, 	.PostOffset	=	0.0 },
	[UNIT_OUNCE_TROY]				= { .MainUnitId = UNIT_KILOGRAM,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.031103475,		.PostOffset	=	0.0 },
	[UNIT_SLUG]						= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 14.5939029372,		.PostOffset	=	0.0 },
	[UNIT_METRIC_TONNE]				= { .MainUnitId = UNIT_KILOGRAM,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   1000.0,				.PostOffset	=	0.0 },
	[UNIT_SHORT_TONNE]				= { .MainUnitId = UNIT_KILOGRAM,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		907.18474,			.PostOffset	=	0.0 },
	[UNIT_LONG_TONNE]				= { .MainUnitId = UNIT_KILOGRAM,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   1016.0469088,		.PostOffset	=	0.0 },
	[UNIT_ATOMIC_MASS_UNIT]			= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_DIVIDER,	.PreOffset	=	0.0,	.Scale	=		  1.66057e-27,		.PostOffset	=	0.0 },
	[UNIT_STONE]					= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  6.35029318,		.PostOffset	=	0.0 },
	[UNIT_US_HUNDREDWEIGHT]			= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 45.359237,			.PostOffset	=	0.0 },
	[UNIT_UK_HUNDREDWEIGHT]			= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 50.80234544,		.PostOffset	=	0.0 },
	[UNIT_ARROBA]					= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 15.0,				.PostOffset	=	0.0 },
	[UNIT_PORTUGAL_ARROBA]			= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 14.688,			.PostOffset	=	0.0 },
	[UNIT_CASTELA_ARROBA]			= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 11.3,				.PostOffset	=	0.0 },
	[UNIT_CATALUNA_ARROBA]			= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 10.4,				.PostOffset	=	0.0 },
	[UNIT_ARAGON_ARROBA]			= { .MainUnitId = UNIT_KILOGRAM,					.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 12.5,				.PostOffset	=	0.0 },

	[UNIT_JULIAN_YEAR]				= { .MainUnitId = UNIT_SECOND,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=  31557600,				.PostOffset	=	0.0 },
	[UNIT_GREGORIAN_YEAR]			= { .MainUnitId = UNIT_SECOND,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=  31556952,				.PostOffset	=	0.0 },
	[UNIT_TROPICAL_YEAR]			= { .MainUnitId = UNIT_SECOND,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=  31556925.9747,			.PostOffset	=	0.0 },
	[UNIT_FORTNIGHT]				= { .MainUnitId = UNIT_SECOND,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=   1209600.0,				.PostOffset	=	0.0 },
	[UNIT_WEEK]						= { .MainUnitId = UNIT_SECOND,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	 604800.0,				.PostOffset	=	0.0 },
	[UNIT_DAY]						= { .MainUnitId = UNIT_SECOND,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	  86400.0,				.PostOffset	=	0.0 },
	[UNIT_HOUR]						= { .MainUnitId = UNIT_SECOND,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   3600.0,				.PostOffset	=	0.0 },
	[UNIT_MINUTE]					= { .MainUnitId = UNIT_SECOND,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		 60.0,				.PostOffset	=	0.0 },

	[UNIT_RPM]						= { .MainUnitId = UNIT_HERTZ,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		  1.0/60.0,			.PostOffset	=	0.0 },

	[UNIT_ELECTRON_VOLT]			= { .MainUnitId = UNIT_JOULE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.60219e-19,		.PostOffset	=	0.0 },
	[UNIT_ERG]						= { .MainUnitId = UNIT_JOULE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0e-7,			.PostOffset	=	0.0 },
	[UNIT_CALORIE]					= { .MainUnitId = UNIT_JOULE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  4.1868,			.PostOffset	=	0.0 },
	[UNIT_BRITISH_THERMAL_UNIT]		= { .MainUnitId = UNIT_JOULE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   1055.05585262,		.PostOffset	=	0.0 },
	[UNIT_WATT_HOUR]				= { .MainUnitId = UNIT_JOULE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   3600.0,				.PostOffset	=	0.0 },
	[UNIT_THERM]					= { .MainUnitId = UNIT_JOULE,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	= 105506000.0,				.PostOffset	=	0.0 },

	[UNIT_TORRICELLI]				= { .MainUnitId = UNIT_PASCAL,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		133.322368421,		.PostOffset	=	0.0 },
	[UNIT_POUND_PER_SQUARE_INCH]	= { .MainUnitId = UNIT_PASCAL,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   6894.75729317,		.PostOffset	=	0.0 },
	[UNIT_MILLIMETER_OF_MERCURY]	= { .MainUnitId = UNIT_PASCAL,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		133.322368421,		.PostOffset	=	0.0 },
	[UNIT_INCH_OF_MERCURY]			= { .MainUnitId = UNIT_PASCAL,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=	   3386.38815789,		.PostOffset	=	0.0 },
	[UNIT_INCH_OF_WATER]			= { .MainUnitId = UNIT_PASCAL,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		248.84,				.PostOffset	=	0.0 },
	[UNIT_METER_OF_WATER]			= { .MainUnitId = UNIT_PASCAL,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   9806.65,				.PostOffset	=	0.0 },
	[UNIT_BAR]						= { .MainUnitId = UNIT_PASCAL,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	 100000.0,				.PostOffset	=	0.0 },
	[UNIT_STANDARD_ATMOSPHERE]		= { .MainUnitId = UNIT_PASCAL,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	 101325.0,				.PostOffset	=	0.0 },

	[UNIT_HORSEPOWER]				= { .MainUnitId = UNIT_WATT,						.Flags = REJECT_PREFIX,		.PreOffset	=	0.0,	.Scale	=		745.699871582,		.PostOffset	=	0.0 },

	[UNIT_POUND_FORCE]				= { .MainUnitId = UNIT_NEWTON,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  4.44822161526,	.PostOffset	=	0.0 },
	[UNIT_POUNDAL]					= { .MainUnitId = UNIT_NEWTON,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.138254954376,	.PostOffset	=	0.0 },
	[UNIT_GRAM_FORCE]				= { .MainUnitId = UNIT_NEWTON,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.00980665,		.PostOffset	=	0.0 },
	[UNIT_DYNE]						= { .MainUnitId = UNIT_NEWTON,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0e-5,			.PostOffset	=	0.0 },
	[UNIT_KIP]						= { .MainUnitId = UNIT_NEWTON,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   4448.22161526,		.PostOffset	=	0.0 },

	[UNIT_CURIE]					= { .MainUnitId = UNIT_BECQUEREL,					.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		 37.0e9,			.PostOffset	=	0.0 },
	[UNIT_ROENTGEN]					= { .MainUnitId = UNIT_COULOMB_PER_KG,				.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  2.58e-4,			.PostOffset	=	0.0 },
	[UNIT_RAD]						= { .MainUnitId = UNIT_GRAY,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.01,				.PostOffset	=	0.0 },

	[UNIT_LUX]						= { .MainUnitId = UNIT_CANDELA_PER_SQUARE_METER,	.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  7.95774715459e-2,	.PostOffset	=	0.0 },
	[UNIT_STILB]					= { .MainUnitId = UNIT_CANDELA_PER_SQUARE_METER,	.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	  10000.00,				.PostOffset	=	0.0 },
	[UNIT_PHOT]						= { .MainUnitId = UNIT_CANDELA_PER_SQUARE_METER,	.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		795.774715459,		.PostOffset	=	0.0 },
	[UNIT_LAMBERT]					= { .MainUnitId = UNIT_CANDELA_PER_SQUARE_METER,	.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   3183.09886184,		.PostOffset	=	0.0 },
	[UNIT_FOOTCANDLE]				= { .MainUnitId = UNIT_CANDELA_PER_SQUARE_METER,	.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  0.856564774909,	.PostOffset	=	0.0 },
	[UNIT_FOOTLAMBERT]				= { .MainUnitId = UNIT_CANDELA_PER_SQUARE_METER,	.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  3.42625909964,	.PostOffset	=	0.0 },

	[UNIT_FARADAY]					= { .MainUnitId = UNIT_COULOMB,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	  96487.0,				.PostOffset	=	0.0 },
	[UNIT_AMPERE_HOUR]				= { .MainUnitId = UNIT_COULOMB,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=	   3600.0,				.PostOffset	=	0.0 },

	[UNIT_MHO]						= { .MainUnitId = UNIT_SIEMENS,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0,				.PostOffset	=	0.0 },
	[UNIT_GAUSS]					= { .MainUnitId = UNIT_TESLA,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0e-4,			.PostOffset	=	0.0 },
	[UNIT_MAXWELL]					= { .MainUnitId = UNIT_WEBER,						.Flags = 0,					.PreOffset	=	0.0,	.Scale	=		  1.0e-8,			.PostOffset	=	0.0 }
	};
/*============================================================================*/
#define	IGNORE_CASE		2
/*============================================================================*/
typedef struct
	{
	const char	*String;
	unitid_t	UnitId;
	int			Flags;
	} unitsalias_t;
/*============================================================================*/
/*
This array must be kept ordered in reverse alphabetical order, with longer
names first.
*/
/*============================================================================*/
const unitsalias_t	UnitsAlias[]	=
	{
		{ .String = "yrJ"			, .UnitId = UNIT_JULIAN_YEAR				, .Flags = IGNORE_CASE },
		{ .String = "yrG"			, .UnitId = UNIT_GREGORIAN_YEAR				, .Flags = IGNORE_CASE },
		{ .String = "yr"			, .UnitId = UNIT_TROPICAL_YEAR				, .Flags = 0 },	// Cannot ignore case because it would clash with yotta/yocto roentgen
		{ .String = "yearJ"			, .UnitId = UNIT_JULIAN_YEAR				, .Flags = IGNORE_CASE },
		{ .String = "yearG"			, .UnitId = UNIT_GREGORIAN_YEAR				, .Flags = IGNORE_CASE },
		{ .String = "year"			, .UnitId = UNIT_TROPICAL_YEAR				, .Flags = IGNORE_CASE },
//		{ .String = "yd³"			, .UnitId = UNIT_CUBIC_YARD					, .Flags = 0 },
//		{ .String = "yd²"			, .UnitId = UNIT_SQUARE_YARD				, .Flags = 0 },
//		{ .String = "yd^3"			, .UnitId = UNIT_CUBIC_YARD					, .Flags = 0 },
//		{ .String = "yd^2"			, .UnitId = UNIT_SQUARE_YARD				, .Flags = 0 },
		{ .String = "yd"			, .UnitId = UNIT_YARD						, .Flags = 0 },
		{ .String = "yard"			, .UnitId = UNIT_YARD						, .Flags = IGNORE_CASE },
		{ .String = "Wh"			, .UnitId = UNIT_WATT_HOUR					, .Flags = 0 },
		{ .String = "wk"			, .UnitId = UNIT_WEEK						, .Flags = 0 },
		{ .String = "week"			, .UnitId = UNIT_WEEK						, .Flags = IGNORE_CASE },
		{ .String = "weber"			, .UnitId = UNIT_WEBER						, .Flags = IGNORE_CASE },
		{ .String = "Wb"			, .UnitId = UNIT_WEBER						, .Flags = 0 },
		{ .String = "watt"			, .UnitId = UNIT_WATT						, .Flags = IGNORE_CASE },
		{ .String = "W"				, .UnitId = UNIT_WATT						, .Flags = 0 },
		{ .String = "volt"			, .UnitId = UNIT_VOLT						, .Flags = IGNORE_CASE },
		{ .String = "V"				, .UnitId = UNIT_VOLT						, .Flags = 0 },
		{ .String = "u"				, .UnitId = UNIT_ATOMIC_MASS_UNIT			, .Flags = 0 },
		{ .String = "tsp"			, .UnitId = UNIT_TEASPOON					, .Flags = IGNORE_CASE },
		{ .String = "torricelli"	, .UnitId = UNIT_TORRICELLI					, .Flags = IGNORE_CASE },
		{ .String = "torr"			, .UnitId = UNIT_TORRICELLI					, .Flags = 0 },
		{ .String = "tonUK"			, .UnitId = UNIT_LONG_TONNE					, .Flags = IGNORE_CASE },
		{ .String = "tonUS"			, .UnitId = UNIT_SHORT_TONNE				, .Flags = IGNORE_CASE },
		{ .String = "tonne"			, .UnitId = UNIT_METRIC_TONNE				, .Flags = IGNORE_CASE },
		{ .String = "tonelada"		, .UnitId = UNIT_METRIC_TONNE				, .Flags = IGNORE_CASE },
		{ .String = "ton"			, .UnitId = UNIT_SHORT_TONNE				, .Flags = IGNORE_CASE },
		{ .String = "thou"			, .UnitId = UNIT_MIL						, .Flags = IGNORE_CASE },
		{ .String = "therm"			, .UnitId = UNIT_THERM						, .Flags = 0 },
		{ .String = "tesla"			, .UnitId = UNIT_TESLA						, .Flags = IGNORE_CASE },
		{ .String = "teaspoon"		, .UnitId = UNIT_TEASPOON					, .Flags = IGNORE_CASE },
		{ .String = "tbsp"			, .UnitId = UNIT_TABLESPOON					, .Flags = IGNORE_CASE },
		{ .String = "tablespoon"	, .UnitId = UNIT_TABLESPOON					, .Flags = IGNORE_CASE },
		{ .String = "T"				, .UnitId = UNIT_TESLA						, .Flags = 0 },
		{ .String = "t"				, .UnitId = UNIT_METRIC_TONNE				, .Flags = 0 },
		{ .String = "Sv"			, .UnitId = UNIT_SIEVERT					, .Flags = 0 },
		{ .String = "stone"			, .UnitId = UNIT_STONE						, .Flags = IGNORE_CASE },
		{ .String = "stokes"		, .UnitId = UNIT_STOKES						, .Flags = IGNORE_CASE },
		{ .String = "stilb"			, .UnitId = UNIT_STILB						, .Flags = 0 },
		{ .String = "stere"			, .UnitId = UNIT_STERE						, .Flags = 0 },
		{ .String = "steradian"		, .UnitId = UNIT_STERADIAN					, .Flags = 0 },
		{ .String = "St"			, .UnitId = UNIT_STOKES						, .Flags = 0 },
		{ .String = "st"			, .UnitId = UNIT_STERE						, .Flags = 0 },
		{ .String = "srad"			, .UnitId = UNIT_STERADIAN					, .Flags = 0 },
		{ .String = "sr"			, .UnitId = UNIT_STERADIAN					, .Flags = 0 },
		{ .String = "slug"			, .UnitId = UNIT_SLUG						, .Flags = 0 },
		{ .String = "sievert"		, .UnitId = UNIT_SIEVERT					, .Flags = IGNORE_CASE },
		{ .String = "siemens"		, .UnitId = UNIT_SIEMENS					, .Flags = IGNORE_CASE },
		{ .String = "semanas"		, .UnitId = UNIT_WEEK						, .Flags = IGNORE_CASE },
		{ .String = "semana"		, .UnitId = UNIT_WEEK						, .Flags = IGNORE_CASE },
		{ .String = "segundos"		, .UnitId = UNIT_SECOND						, .Flags = IGNORE_CASE },
		{ .String = "segundo"		, .UnitId = UNIT_SECOND						, .Flags = IGNORE_CASE },
		{ .String = "second"		, .UnitId = UNIT_SECOND						, .Flags = IGNORE_CASE },
		{ .String = "sec"			, .UnitId = UNIT_SECOND						, .Flags = IGNORE_CASE },
		{ .String = "sb"			, .UnitId = UNIT_STILB						, .Flags = 0 },
		{ .String = "S"				, .UnitId = UNIT_SIEMENS					, .Flags = 0 },
		{ .String = "s"				, .UnitId = UNIT_SECOND						, .Flags = 0 },
		{ .String = "rpm"			, .UnitId = UNIT_RPM						, .Flags = IGNORE_CASE },
		{ .String = "roentgen"		, .UnitId = UNIT_ROENTGEN					, .Flags = IGNORE_CASE },
		{ .String = "rod"			, .UnitId = UNIT_ROD						, .Flags = 0 },
		{ .String = "rd"			, .UnitId = UNIT_ROD						, .Flags = 0 },
		{ .String = "radian"		, .UnitId = UNIT_RADIAN						, .Flags = IGNORE_CASE },
		{ .String = "rad"			, .UnitId = UNIT_RADIAN						, .Flags = 0 },
		{ .String = "Rad"			, .UnitId = UNIT_RAD						, .Flags = 0 },
		{ .String = "R"				, .UnitId = UNIT_ROENTGEN					, .Flags = 0 },
		{ .String = "r"				, .UnitId = UNIT_RADIAN						, .Flags = 0 },
		{ .String = "quart"			, .UnitId = UNIT_QUART						, .Flags = IGNORE_CASE },
		{ .String = "qt"			, .UnitId = UNIT_QUART						, .Flags = 0 },
		{ .String = "ptUS"			, .UnitId = UNIT_US_PINT					, .Flags = IGNORE_CASE },
		{ .String = "ptUK"			, .UnitId = UNIT_UK_PINT					, .Flags = IGNORE_CASE },
		{ .String = "pt"			, .UnitId = UNIT_UK_PINT					, .Flags = IGNORE_CASE },
		{ .String = "psi"			, .UnitId = UNIT_POUND_PER_SQUARE_INCH		, .Flags = IGNORE_CASE },
		{ .String = "poundtroy"		, .UnitId = UNIT_POUND_TROY					, .Flags = IGNORE_CASE },
		{ .String = "poundal"		, .UnitId = UNIT_POUNDAL					, .Flags = IGNORE_CASE },
		{ .String = "pound"			, .UnitId = UNIT_POUND						, .Flags = IGNORE_CASE },
		{ .String = "poise"			, .UnitId = UNIT_POISE						, .Flags = IGNORE_CASE },
		{ .String = "point"			, .UnitId = UNIT_POINT						, .Flags = IGNORE_CASE },
		{ .String = "pk"			, .UnitId = UNIT_PECK						, .Flags = 0 },
		{ .String = "pintUS"		, .UnitId = UNIT_US_PINT					, .Flags = IGNORE_CASE },
		{ .String = "pintUK"		, .UnitId = UNIT_UK_PINT					, .Flags = IGNORE_CASE },
		{ .String = "pint"			, .UnitId = UNIT_UK_PINT					, .Flags = IGNORE_CASE },
		{ .String = "pica"			, .UnitId = UNIT_PICA						, .Flags = IGNORE_CASE },
		{ .String = "phot"			, .UnitId = UNIT_PHOT						, .Flags = IGNORE_CASE },
		{ .String = "ph"			, .UnitId = UNIT_PHOT						, .Flags = 0 },
		{ .String = "peck"			, .UnitId = UNIT_PECK						, .Flags = IGNORE_CASE },
		{ .String = "pdl"			, .UnitId = UNIT_POUNDAL					, .Flags = 0 },
		{ .String = "pc"			, .UnitId = UNIT_PARSEC						, .Flags = 0 },
		{ .String = "pascal"		, .UnitId = UNIT_PASCAL						, .Flags = IGNORE_CASE },
		{ .String = "parsec"		, .UnitId = UNIT_PARSEC						, .Flags = 0 },
//		{ .String = "Pa.s"			, .UnitId = UNIT_PASCAL_SECOND				, .Flags = 0 },
//		{ .String = "Pa*s"			, .UnitId = UNIT_PASCAL_SECOND				, .Flags = 0 },
		{ .String = "Pa"			, .UnitId = UNIT_PASCAL						, .Flags = 0 },
		{ .String = "P"				, .UnitId = UNIT_POISE						, .Flags = 0 },
		{ .String = "ozUK"			, .UnitId = UNIT_UK_FLUID_OUNCE				, .Flags = IGNORE_CASE },
		{ .String = "ozt"			, .UnitId = UNIT_OUNCE_TROY					, .Flags = IGNORE_CASE },
		{ .String = "ozfl"			, .UnitId = UNIT_US_FLUID_OUNCE				, .Flags = 0 },
		{ .String = "oz"			, .UnitId = UNIT_OUNCE						, .Flags = 0 },
		{ .String = "ouncetroy"		, .UnitId = UNIT_OUNCE_TROY					, .Flags = IGNORE_CASE },
		{ .String = "ounce"			, .UnitId = UNIT_OUNCE						, .Flags = IGNORE_CASE },
		{ .String = "ohm"			, .UnitId = UNIT_OHM						, .Flags = IGNORE_CASE },
		{ .String = "nmile"			, .UnitId = UNIT_NAUTICAL_MILE				, .Flags = IGNORE_CASE },
		{ .String = "nmi"			, .UnitId = UNIT_NAUTICAL_MILE				, .Flags = 0 },
		{ .String = "newton"		, .UnitId = UNIT_NEWTON						, .Flags = IGNORE_CASE },
		{ .String = "N"				, .UnitId = UNIT_NEWTON						, .Flags = 0 },
//		{ .String = "m³"			, .UnitId = UNIT_CUBIC_METER				, .Flags = 0 },
//		{ .String = "m²/s"			, .UnitId = UNIT_SQUARE_METER_PER_SECOND	, .Flags = 0 },
//		{ .String = "m²"			, .UnitId = UNIT_SQUARE_METER				, .Flags = 0 },
		{ .String = "Mx"			, .UnitId = UNIT_MAXWELL					, .Flags = 0 },
		{ .String = "mph"			, .UnitId = UNIT_MILE_PER_HOUR				, .Flags = IGNORE_CASE },
		{ .String = "mole"			, .UnitId = UNIT_MOLE						, .Flags = IGNORE_CASE },
		{ .String = "mol"			, .UnitId = UNIT_MOLE						, .Flags = 0 },
		{ .String = "mmHg"			, .UnitId = UNIT_MILLIMETER_OF_MERCURY		, .Flags = 0 },
		{ .String = "mlg"			, .UnitId = UNIT_MARINE_LEAGUE				, .Flags = 0 },
		{ .String = "mleague"		, .UnitId = UNIT_MARINE_LEAGUE				, .Flags = IGNORE_CASE },
//		{ .String = "mi²"			, .UnitId = UNIT_SQUARE_MILE				, .Flags = 0 },
		{ .String = "minutos"		, .UnitId = UNIT_MINUTE						, .Flags = IGNORE_CASE },
		{ .String = "minuto"		, .UnitId = UNIT_MINUTE						, .Flags = IGNORE_CASE },
		{ .String = "minute"		, .UnitId = UNIT_MINUTE						, .Flags = IGNORE_CASE },
		{ .String = "min"			, .UnitId = UNIT_MINUTE						, .Flags = 0 },
		{ .String = "milha"			, .UnitId = UNIT_MILE						, .Flags = IGNORE_CASE },
		{ .String = "mile"			, .UnitId = UNIT_MILE						, .Flags = IGNORE_CASE },
		{ .String = "mil"			, .UnitId = UNIT_MIL						, .Flags = 0 },
//		{ .String = "mi^2"			, .UnitId = UNIT_SQUARE_MILE				, .Flags = 0 },
		{ .String = "mi"			, .UnitId = UNIT_MILE						, .Flags = 0 },
		{ .String = "mho"			, .UnitId = UNIT_MHO						, .Flags = 0 },
		{ .String = "mH2O"			, .UnitId = UNIT_METER_OF_WATER				, .Flags = 0 },
		{ .String = "metro"			, .UnitId = UNIT_METER						, .Flags = IGNORE_CASE },
		{ .String = "metre"			, .UnitId = UNIT_METER						, .Flags = IGNORE_CASE },
		{ .String = "meter"			, .UnitId = UNIT_METER						, .Flags = IGNORE_CASE },
//		{ .String = "m^3"			, .UnitId = UNIT_CUBIC_METER				, .Flags = 0 },
//		{ .String = "m^2/s"			, .UnitId = UNIT_SQUARE_METER_PER_SECOND	, .Flags = 0 },
//		{ .String = "m^2.s^-1"		, .UnitId = UNIT_SQUARE_METER_PER_SECOND	, .Flags = 0 },
//		{ .String = "m^2*s^-1"		, .UnitId = UNIT_SQUARE_METER_PER_SECOND	, .Flags = 0 },
//		{ .String = "m^2"			, .UnitId = UNIT_SQUARE_METER				, .Flags = 0 },
//		{ .String = "m/s²"			, .UnitId = UNIT_METER_PER_SECOND_SQUARED	, .Flags = 0 },
//		{ .String = "m/s"			, .UnitId = UNIT_METER_PER_SECOND			, .Flags = 0 },
//		{ .String = "m.s^-2"		, .UnitId = UNIT_METER_PER_SECOND_SQUARED	, .Flags = 0 },
//		{ .String = "m.s^-1"		, .UnitId = UNIT_METER_PER_SECOND			, .Flags = 0 },
//		{ .String = "m*s^-2"		, .UnitId = UNIT_METER_PER_SECOND_SQUARED	, .Flags = 0 },
//		{ .String = "m*s^-1"		, .UnitId = UNIT_METER_PER_SECOND			, .Flags = 0 },
		{ .String = "m"				, .UnitId = UNIT_METER						, .Flags = 0 },
		{ .String = "lyr"			, .UnitId = UNIT_LIGHT_YEAR					, .Flags = 0 },
		{ .String = "lx"			, .UnitId = UNIT_LUX						, .Flags = 0 },
		{ .String = "lux"			, .UnitId = UNIT_LUX						, .Flags = IGNORE_CASE },
		{ .String = "lumen"			, .UnitId = UNIT_LUMEN						, .Flags = IGNORE_CASE },
		{ .String = "lm"			, .UnitId = UNIT_LUMEN						, .Flags = 0 },
		{ .String = "lea"			, .UnitId = UNIT_LAND_LEAGUE				, .Flags = 0 },
		{ .String = "litro"			, .UnitId = UNIT_LITER						, .Flags = IGNORE_CASE },
		{ .String = "litre"			, .UnitId = UNIT_LITER						, .Flags = IGNORE_CASE },
		{ .String = "liter"			, .UnitId = UNIT_LITER						, .Flags = IGNORE_CASE },
		{ .String = "libra"			, .UnitId = UNIT_POUND						, .Flags = IGNORE_CASE },
		{ .String = "lgBR"			, .UnitId = UNIT_BRAZILIAN_LEAGUE			, .Flags = 0 },
		{ .String = "legua"			, .UnitId = UNIT_BRAZILIAN_LEAGUE			, .Flags = IGNORE_CASE },
		{ .String = "league"		, .UnitId = UNIT_LAND_LEAGUE				, .Flags = IGNORE_CASE },
		{ .String = "lbt"			, .UnitId = UNIT_POUND_TROY					, .Flags = 0 },
		{ .String = "lbf"			, .UnitId = UNIT_POUND_FORCE				, .Flags = 0 },
		{ .String = "lb"			, .UnitId = UNIT_POUND						, .Flags = 0 },
		{ .String = "lambert"		, .UnitId = UNIT_LAMBERT					, .Flags = IGNORE_CASE },
		{ .String = "lam"			, .UnitId = UNIT_LAMBERT					, .Flags = 0 },
		{ .String = "l"				, .UnitId = UNIT_LITER						, .Flags = IGNORE_CASE },
		{ .String = "kph"			, .UnitId = UNIT_KILOMETER_PER_HOUR			, .Flags = IGNORE_CASE },
		{ .String = "knot"			, .UnitId = UNIT_KNOT						, .Flags = 0 },
		{ .String = "kip"			, .UnitId = UNIT_KIP						, .Flags = 0 },
		{ .String = "kg"			, .UnitId = UNIT_KILOGRAM					, .Flags = REJECT_PREFIX },
		{ .String = "kelvin"		, .UnitId = UNIT_KELVIN						, .Flags = IGNORE_CASE },
		{ .String = "katal"			, .UnitId = UNIT_KATAL						, .Flags = IGNORE_CASE },
		{ .String = "kat"			, .UnitId = UNIT_KATAL						, .Flags = 0 },
		{ .String = "K"				, .UnitId = UNIT_KELVIN						, .Flags = 0 },
		{ .String = "joule"			, .UnitId = UNIT_JOULE						, .Flags = IGNORE_CASE },
		{ .String = "jarda"			, .UnitId = UNIT_YARD						, .Flags = IGNORE_CASE },
		{ .String = "J"				, .UnitId = UNIT_JOULE						, .Flags = 0 },
//		{ .String = "in³"			, .UnitId = UNIT_CUBIC_INCH					, .Flags = 0 },
//		{ .String = "in²"			, .UnitId = UNIT_SQUARE_INCH				, .Flags = 0 },
		{ .String = "inHg"			, .UnitId = UNIT_INCH_OF_MERCURY			, .Flags = 0 },
		{ .String = "inH2O"			, .UnitId = UNIT_INCH_OF_WATER				, .Flags = 0 },
		{ .String = "inch"			, .UnitId = UNIT_INCH						, .Flags = IGNORE_CASE },
//		{ .String = "in^3"			, .UnitId = UNIT_CUBIC_INCH					, .Flags = 0 },
//		{ .String = "in^2"			, .UnitId = UNIT_SQUARE_INCH				, .Flags = 0 },
		{ .String = "in"			, .UnitId = UNIT_INCH						, .Flags = 0 },
		{ .String = "Hz"			, .UnitId = UNIT_HERTZ						, .Flags = 0 },
		{ .String = "hr"			, .UnitId = UNIT_HOUR						, .Flags = 0 },
		{ .String = "hp"			, .UnitId = UNIT_HORSEPOWER					, .Flags = 0 },
		{ .String = "hour"			, .UnitId = UNIT_HOUR						, .Flags = IGNORE_CASE },
		{ .String = "horas"			, .UnitId = UNIT_HOUR						, .Flags = IGNORE_CASE },
		{ .String = "hora"			, .UnitId = UNIT_HOUR						, .Flags = IGNORE_CASE },
		{ .String = "hertz"			, .UnitId = UNIT_HERTZ						, .Flags = IGNORE_CASE },
		{ .String = "henry"			, .UnitId = UNIT_HENRY						, .Flags = IGNORE_CASE },
		{ .String = "h"				, .UnitId = UNIT_HOUR						, .Flags = 0 },
		{ .String = "H"				, .UnitId = UNIT_HENRY						, .Flags = 0 },
		{ .String = "Gy"			, .UnitId = UNIT_GRAY						, .Flags = 0 },
		{ .String = "gray"			, .UnitId = UNIT_GRAY						, .Flags = IGNORE_CASE },
		{ .String = "grau"			, .UnitId = UNIT_DEGREE						, .Flags = IGNORE_CASE },
		{ .String = "grama"			, .UnitId = UNIT_GRAM						, .Flags = IGNORE_CASE },
		{ .String = "gram"			, .UnitId = UNIT_GRAM						, .Flags = IGNORE_CASE },
		{ .String = "grain"			, .UnitId = UNIT_GRAIN						, .Flags = IGNORE_CASE },
		{ .String = "grado"			, .UnitId = UNIT_GRADE						, .Flags = IGNORE_CASE },
		{ .String = "grade"			, .UnitId = UNIT_GRADE						, .Flags = IGNORE_CASE },
		{ .String = "grad"			, .UnitId = UNIT_GRADE						, .Flags = 0 },
		{ .String = "gf"			, .UnitId = UNIT_GRAM_FORCE					, .Flags = 0 },
		{ .String = "galUS"			, .UnitId = UNIT_US_GALLON					, .Flags = IGNORE_CASE },
		{ .String = "galUK"			, .UnitId = UNIT_UK_GALLON					, .Flags = IGNORE_CASE },
		{ .String = "gallonUS"		, .UnitId = UNIT_US_GALLON					, .Flags = IGNORE_CASE },
		{ .String = "gallonUK"		, .UnitId = UNIT_UK_GALLON					, .Flags = IGNORE_CASE },
		{ .String = "gallonC"		, .UnitId = UNIT_CANADA_GALLON				, .Flags = IGNORE_CASE },
		{ .String = "galC"			, .UnitId = UNIT_CANADA_GALLON				, .Flags = IGNORE_CASE },
		{ .String = "ga"			, .UnitId = UNIT_STANDARD_GRAVITY			, .Flags = 0 },
		{ .String = "g"				, .UnitId = UNIT_GRAM						, .Flags = 0 },
		{ .String = "G"				, .UnitId = UNIT_GAUSS						, .Flags = 0 },
		{ .String = "furlong"		, .UnitId = UNIT_FURLONG					, .Flags = IGNORE_CASE },
		{ .String = "fur"			, .UnitId = UNIT_FURLONG					, .Flags = 0 },
//		{ .String = "ft³"			, .UnitId = UNIT_CUBIC_FOOT					, .Flags = 0 },
//		{ .String = "ft²"			, .UnitId = UNIT_SQUARE_FOOT				, .Flags = 0 },
		{ .String = "ftUS"			, .UnitId = UNIT_SURVEY_FOOT				, .Flags = IGNORE_CASE },
//		{ .String = "ft^3"			, .UnitId = UNIT_CUBIC_FOOT					, .Flags = 0 },
//		{ .String = "ft^2"			, .UnitId = UNIT_SQUARE_FOOT				, .Flags = 0 },
		{ .String = "ft"			, .UnitId = UNIT_FOOT						, .Flags = 0 },
		{ .String = "fortnight"		, .UnitId = UNIT_FORTNIGHT					, .Flags = IGNORE_CASE },
		{ .String = "footUS"		, .UnitId = UNIT_SURVEY_FOOT				, .Flags = IGNORE_CASE },
		{ .String = "footlambert"	, .UnitId = UNIT_FOOTLAMBERT				, .Flags = IGNORE_CASE },
		{ .String = "footcandle"	, .UnitId = UNIT_FOOTCANDLE					, .Flags = IGNORE_CASE },
		{ .String = "foot"			, .UnitId = UNIT_FOOT						, .Flags = IGNORE_CASE },
		{ .String = "feet"			, .UnitId = UNIT_FOOT						, .Flags = IGNORE_CASE },
		{ .String = "fn"			, .UnitId = UNIT_FORTNIGHT					, .Flags = 0 },
		{ .String = "flam"			, .UnitId = UNIT_FOOTLAMBERT				, .Flags = 0 },
		{ .String = "fermi"			, .UnitId = UNIT_FERMI						, .Flags = IGNORE_CASE },
		{ .String = "Fdy"			, .UnitId = UNIT_FARADAY					, .Flags = 0 },
		{ .String = "fc"			, .UnitId = UNIT_FOOTCANDLE					, .Flags = 0 },
		{ .String = "fbm"			, .UnitId = UNIT_BOARDFOOT					, .Flags = 0 },
		{ .String = "fathom"		, .UnitId = UNIT_FATHOM						, .Flags = IGNORE_CASE },
		{ .String = "fath"			, .UnitId = UNIT_FATHOM						, .Flags = 0 },
		{ .String = "faraday"		, .UnitId = UNIT_FARADAY					, .Flags = IGNORE_CASE },
		{ .String = "farad"			, .UnitId = UNIT_FARAD						, .Flags = IGNORE_CASE },
		{ .String = "F"				, .UnitId = UNIT_FARAD						, .Flags = 0 },
		{ .String = "eV"			, .UnitId = UNIT_ELECTRON_VOLT				, .Flags = 0 },
		{ .String = "erg"			, .UnitId = UNIT_ERG						, .Flags = 0 },
		{ .String = "dyne"			, .UnitId = UNIT_DYNE						, .Flags = IGNORE_CASE },
		{ .String = "dyn"			, .UnitId = UNIT_DYNE						, .Flags = 0 },
		{ .String = "dias"			, .UnitId = UNIT_DAY						, .Flags = IGNORE_CASE },
		{ .String = "dia"			, .UnitId = UNIT_DAY						, .Flags = IGNORE_CASE },
		{ .String = "degreeRe"		, .UnitId = UNIT_DEGREE_REAUMUR				, .Flags = IGNORE_CASE },
		{ .String = "degreeR"		, .UnitId = UNIT_DEGREE_RANKINE				, .Flags = IGNORE_CASE },
		{ .String = "degreeF"		, .UnitId = UNIT_DEGREE_FAHRENHEIT			, .Flags = IGNORE_CASE },
		{ .String = "degreeC"		, .UnitId = UNIT_DEGREE_CELSIUS				, .Flags = IGNORE_CASE },
		{ .String = "degree"		, .UnitId = UNIT_DEGREE						, .Flags = IGNORE_CASE },
		{ .String = "degRe"			, .UnitId = UNIT_DEGREE_REAUMUR				, .Flags = IGNORE_CASE },
		{ .String = "degR"			, .UnitId = UNIT_DEGREE_RANKINE				, .Flags = IGNORE_CASE },
		{ .String = "degF"			, .UnitId = UNIT_DEGREE_FAHRENHEIT			, .Flags = IGNORE_CASE },
		{ .String = "degC"			, .UnitId = UNIT_DEGREE_CELSIUS				, .Flags = IGNORE_CASE },
		{ .String = "deg"			, .UnitId = UNIT_DEGREE						, .Flags = 0 },
		{ .String = "day"			, .UnitId = UNIT_DAY						, .Flags = IGNORE_CASE },
		{ .String = "d"				, .UnitId = UNIT_DAY						, .Flags = 0 },
		{ .String = "cwtUS"			, .UnitId = UNIT_US_HUNDREDWEIGHT			, .Flags = IGNORE_CASE },
		{ .String = "cwtUK"			, .UnitId = UNIT_UK_HUNDREDWEIGHT			, .Flags = IGNORE_CASE },
		{ .String = "curie"			, .UnitId = UNIT_CURIE						, .Flags = IGNORE_CASE },
		{ .String = "cup"			, .UnitId = UNIT_CUP						, .Flags = IGNORE_CASE },
		{ .String = "cu"			, .UnitId = UNIT_CUP						, .Flags = 0 },
		{ .String = "ct"			, .UnitId = UNIT_CARAT						, .Flags = 0 },
		{ .String = "coulomb"		, .UnitId = UNIT_COULOMB					, .Flags = IGNORE_CASE },
		{ .String = "copo"			, .UnitId = UNIT_CUP						, .Flags = IGNORE_CASE },
		{ .String = "Ci"			, .UnitId = UNIT_CURIE						, .Flags = 0 },
		{ .String = "chain"			, .UnitId = UNIT_CHAIN						, .Flags = IGNORE_CASE },
//		{ .String = "cd/m²"			, .UnitId = UNIT_CANDELA_PER_SQUARE_METER	, .Flags = 0 },
//		{ .String = "cd/m^2"		, .UnitId = UNIT_CANDELA_PER_SQUARE_METER	, .Flags = 0 },
//		{ .String = "cd.m^-2"		, .UnitId = UNIT_CANDELA_PER_SQUARE_METER	, .Flags = 0 },
//		{ .String = "cd*m^-2"		, .UnitId = UNIT_CANDELA_PER_SQUARE_METER	, .Flags = 0 },
		{ .String = "cd"			, .UnitId = UNIT_CANDELA					, .Flags = 0 },
		{ .String = "carat"			, .UnitId = UNIT_CARAT						, .Flags = IGNORE_CASE },
		{ .String = "candela"		, .UnitId = UNIT_CANDELA					, .Flags = IGNORE_CASE },
		{ .String = "calorie"		, .UnitId = UNIT_CALORIE					, .Flags = IGNORE_CASE },
		{ .String = "caloria"		, .UnitId = UNIT_CALORIE					, .Flags = IGNORE_CASE },
		{ .String = "cal"			, .UnitId = UNIT_CALORIE					, .Flags = 0 },
		{ .String = "c"				, .UnitId = UNIT_SPEED_OF_LIGHT				, .Flags = 0 },
		{ .String = "C"				, .UnitId = UNIT_COULOMB					, .Flags = 0 },
		{ .String = "bushel"		, .UnitId = UNIT_BUSHEL						, .Flags = IGNORE_CASE },
		{ .String = "bu"			, .UnitId = UNIT_BUSHEL						, .Flags = 0 },
		{ .String = "btu"			, .UnitId = UNIT_BRITISH_THERMAL_UNIT		, .Flags = IGNORE_CASE },
		{ .String = "Bq"			, .UnitId = UNIT_BECQUEREL					, .Flags = 0 },
		{ .String = "boardfoot"		, .UnitId = UNIT_BOARDFOOT					, .Flags = IGNORE_CASE },
		{ .String = "becquerel"		, .UnitId = UNIT_BECQUEREL					, .Flags = IGNORE_CASE },
		{ .String = "bbl"			, .UnitId = UNIT_BARREL						, .Flags = 0 },
		{ .String = "barril"		, .UnitId = UNIT_BARREL						, .Flags = IGNORE_CASE },
		{ .String = "barrel"		, .UnitId = UNIT_BARREL						, .Flags = IGNORE_CASE },
		{ .String = "barn"			, .UnitId = UNIT_BARN						, .Flags = 0 },
		{ .String = "bar"			, .UnitId = UNIT_BAR						, .Flags = 0 },
		{ .String = "b"				, .UnitId = UNIT_BARN						, .Flags = 0 },
		{ .String = "au"			, .UnitId = UNIT_ASTRONOMICAL_UNIT			, .Flags = 0 },
		{ .String = "atm"			, .UnitId = UNIT_STANDARD_ATMOSPHERE		, .Flags = 0 },
		{ .String = "arrobaPT"		, .UnitId = UNIT_PORTUGAL_ARROBA			, .Flags = IGNORE_CASE },
		{ .String = "arrobaCAT"		, .UnitId = UNIT_CATALUNA_ARROBA			, .Flags = IGNORE_CASE },
		{ .String = "arrobaCAS"		, .UnitId = UNIT_CASTELA_ARROBA				, .Flags = IGNORE_CASE },
		{ .String = "arrobaARA"		, .UnitId = UNIT_ARAGON_ARROBA				, .Flags = IGNORE_CASE },
		{ .String = "arroba"		, .UnitId = UNIT_ARROBA						, .Flags = IGNORE_CASE },
		{ .String = "are"			, .UnitId = UNIT_ARE						, .Flags = 0 },
		{ .String = "arcsecond"		, .UnitId = UNIT_ARC_SECOND					, .Flags = IGNORE_CASE },
		{ .String = "arcs"			, .UnitId = UNIT_ARC_SECOND					, .Flags = 0 },
		{ .String = "arcminute"		, .UnitId = UNIT_ARC_MINUTE					, .Flags = IGNORE_CASE },
		{ .String = "arcmin"		, .UnitId = UNIT_ARC_MINUTE					, .Flags = 0 },
		{ .String = "anoJ"			, .UnitId = UNIT_JULIAN_YEAR				, .Flags = IGNORE_CASE },
		{ .String = "anoG"			, .UnitId = UNIT_GREGORIAN_YEAR				, .Flags = IGNORE_CASE },
		{ .String = "anos"			, .UnitId = UNIT_TROPICAL_YEAR				, .Flags = IGNORE_CASE },
		{ .String = "ano"			, .UnitId = UNIT_TROPICAL_YEAR				, .Flags = IGNORE_CASE },
		{ .String = "angstrom"		, .UnitId = UNIT_ANGSTROM					, .Flags = IGNORE_CASE },
		{ .String = "ampere"		, .UnitId = UNIT_AMPERE						, .Flags = IGNORE_CASE },
		{ .String = "alqueireSP"	, .UnitId = UNIT_ALQUEIRE_PAULISTA			, .Flags = IGNORE_CASE },
		{ .String = "alqueireGO"	, .UnitId = UNIT_ALQUEIRE_GOIANO			, .Flags = IGNORE_CASE },
		{ .String = "alqSP"			, .UnitId = UNIT_ALQUEIRE_PAULISTA			, .Flags = IGNORE_CASE },
		{ .String = "alqGO"			, .UnitId = UNIT_ALQUEIRE_GOIANO			, .Flags = IGNORE_CASE },
		{ .String = "Ah"			, .UnitId = UNIT_AMPERE_HOUR				, .Flags = 0 },
		{ .String = "acre"			, .UnitId = UNIT_ACRE						, .Flags = IGNORE_CASE },
		{ .String = "ac"			, .UnitId = UNIT_ACRE						, .Flags = 0 },
		{ .String = "a"				, .UnitId = UNIT_ARE						, .Flags = 0 },
		{ .String = "A"				, .UnitId = UNIT_AMPERE						, .Flags = 0 },
//		{ .String = "\xce\xa9"		, .UnitId = UNIT_OHM						, .Flags = 0 },
//		{ .String = "\xc5"			, .UnitId = UNIT_ANGSTROM					, .Flags = 0 },
//		{ .String = "\xc2\xb5"		, .UnitId = UNIT_MICRON						, .Flags = 0 },
//		{ .String = "\xc2\xb0""Re"	, .UnitId = UNIT_DEGREE_REAUMUR				, .Flags = 0 },
//		{ .String = "\xc2\xb0""R"	, .UnitId = UNIT_DEGREE_RANKINE				, .Flags = 0 },
//		{ .String = "\xc2\xb0""F"	, .UnitId = UNIT_DEGREE_FAHRENHEIT			, .Flags = 0 },
//		{ .String = "\xc2\xb0""C"	, .UnitId = UNIT_DEGREE_CELSIUS				, .Flags = 0 },
//		{ .String = "\xc2\xb0"		, .UnitId = UNIT_DEGREE						, .Flags = 0 },
		{ .String = "\xf8""Re"		, .UnitId = UNIT_DEGREE_REAUMUR				, .Flags = 0 },
		{ .String = "\xf8""R"		, .UnitId = UNIT_DEGREE_RANKINE				, .Flags = 0 },
		{ .String = "\xf8""F"		, .UnitId = UNIT_DEGREE_FAHRENHEIT			, .Flags = 0 },
		{ .String = "\xf8""C"		, .UnitId = UNIT_DEGREE_CELSIUS				, .Flags = 0 },
		{ .String = "\xf8"			, .UnitId = UNIT_DEGREE						, .Flags = 0 },
		{ .String = "@PT"			, .UnitId = UNIT_PORTUGAL_ARROBA			, .Flags = IGNORE_CASE },
		{ .String = "@CAT"			, .UnitId = UNIT_CATALUNA_ARROBA			, .Flags = IGNORE_CASE },
		{ .String = "@CAS"			, .UnitId = UNIT_CASTELA_ARROBA				, .Flags = IGNORE_CASE },
		{ .String = "@ARA"			, .UnitId = UNIT_ARAGON_ARROBA				, .Flags = IGNORE_CASE },
		{ .String = "@"				, .UnitId = UNIT_ARROBA						, .Flags = 0 },
	};
/*============================================================================*/
const char *GetUnitName( unitid_t UnitId )
	{
	if( UnitId < UNIT_NONE || UnitId >= LENGTH( Units ))
		return NULL;

	return Units[UnitId].Name;
	}
/*============================================================================*/
const char *GetUnitSymbol( unitid_t UnitId )
	{
	if( UnitId < UNIT_NONE || UnitId >= LENGTH( Units ))
		return NULL;

	return Units[UnitId].Symbol;
	}
/*============================================================================*/
const char *GetUnitDescription( unitid_t UnitId )
	{
	if( UnitId < UNIT_NONE || UnitId >= LENGTH( Units ))
		return NULL;

	return Units[UnitId].WhatItMeasures;
	}
/*============================================================================*/
int GetUnitExponents( unitid_t UnitId, unitsignature_t *Signature )
	{
	if( UnitId < UNIT_NONE || UnitId >= LENGTH( Units ) || Signature == NULL )
		return RESULT_ERROR_ARGUMENTS;

	Signature->Exp_s		= Units[UnitId].Exponents.Exp_s;
	Signature->Exp_m		= Units[UnitId].Exponents.Exp_m;
	Signature->Exp_kg		= Units[UnitId].Exponents.Exp_kg;
	Signature->Exp_A		= Units[UnitId].Exponents.Exp_A;
	Signature->Exp_K		= Units[UnitId].Exponents.Exp_K;
	Signature->Exp_mol		= Units[UnitId].Exponents.Exp_mol;
	Signature->Exp_cd		= Units[UnitId].Exponents.Exp_cd;
	Signature->IsAngle		= Units[UnitId].Exponents.IsAngle;
	Signature->IsRadiation	= Units[UnitId].Exponents.IsRadiation;

	return RESULT_OK;
	}
/*============================================================================*/
unitid_t FindUnitId( const unitsignature_t * const Signature )
	{
	exp_t	Exp;
	int		i;

	for( i = 0; i < LENGTH( Signature->Exponents ); i++ )
		if( Signature->Exponents[i] < -3 || Signature->Exponents[i] > 4 )
			return UNIT_CUSTOM;

	Exp.Exp_s		= Signature->Exp_s;
	Exp.Exp_m		= Signature->Exp_m;
	Exp.Exp_kg		= Signature->Exp_kg;
	Exp.Exp_A		= Signature->Exp_A;
	Exp.Exp_K		= Signature->Exp_K;
	Exp.Exp_mol		= Signature->Exp_mol;
	Exp.Exp_cd		= Signature->Exp_cd;
	Exp.IsAngle		= Signature->IsAngle;
	Exp.IsRadiation	= Signature->IsRadiation;

	for( i = 0; i < LENGTH( Units ); i++ )
		if( Exp.Value == Units[i].Exponents.Value )
			return i;

	return UNIT_CUSTOM;
	}
/*============================================================================*/
void SkipBlanks( const char ** const p )
	{
	if( p == NULL || *p == NULL )
		return;

	while( isspace( **p ))
		(*p)++;
	}
/*============================================================================*/
unitid_t ParseUnit( const char ** const p )
	{
	int	i;

	if( p == NULL || *p == NULL )
		return UNIT_INVALID;

	for( i = 0; i < LENGTH( UnitsAlias ); i++ )
		{
		int	len;

		if(( UnitsAlias[i].Flags & IGNORE_CASE ?
			strncasecmp( *p, UnitsAlias[i].String, len = strlen( UnitsAlias[i].String )) == 0 :
			strncmp( *p, UnitsAlias[i].String, len = strlen( UnitsAlias[i].String )) == 0 )
			&& !isalnum( *( *p + len )))
			{
			*p += len;
			return UnitsAlias[i].UnitId;
			}
		}

	return UNIT_NONE;
	}
/*============================================================================*/
int ParsePrefixedUnit( const char ** const p, prefixid_t * const PrefixId, unitid_t *const UnitId, int Flags )
	{
	const char	*q;

	if( p == NULL || *p == NULL || PrefixId == NULL || UnitId == NULL )
		return RESULT_ERROR_ARGUMENTS;

	*PrefixId	= PREFIX_NONE;
	q			= *p;

	/* Let's first try to find a unit that is the entire word... */
	if(( *UnitId = ParseUnit( &q )) > UNIT_NONE )
		{
		/* ...found. */
		*p	= q;

		return RESULT_OK;
		}

	/* No luck. */

	/* Let's try to find a prefix. Found none... */
	if(( *PrefixId = ParsePrefix( &q, Flags )) == PREFIX_NONE )
		/* ...we already know that there is no unit. */
		return RESULT_NO_UNIT;

	/* Now try to find a unit that is the remaining of the word and that accepts prefixes... */
	if(( *UnitId = ParseUnit( &q )) > UNIT_NONE && ( Equivalence[*UnitId].Flags & REJECT_PREFIX ) == 0 )
		{
		/* ...found. */
		*p	= q;

		return RESULT_OK;
		}

	/* We have a problem with prefix 'deci' ('d') before units that begin
	   in 'a', the prefix will be confused with 'deca' ('da')... */
	if( *PrefixId == PREFIX_DECA )
		{
		/* ...let's try again assuming that the prefix is 'deci' ('d')... */
		*PrefixId	= PREFIX_DECI;
		/* ...and skipping only the 'd'. */
		q	= *p + 1;

		/* Now try to find a unit that is the remaining of the word and that accepts prefixes... */
		if(( *UnitId = ParseUnit( &q )) > UNIT_NONE && ( Equivalence[*UnitId].Flags & REJECT_PREFIX ) == 0 )
			{
			/* ...found. */
			*p	= q;

			return RESULT_OK;
			}
		}

	*PrefixId	= PREFIX_NONE;
	*UnitId		= UNIT_NONE;

	return RESULT_NO_UNIT;
	}
/*============================================================================*/
int ParseExponent( const char ** const p, int * const Exponent, int *Style )
	{
	const char	*q;

	if( p == NULL || *p == NULL || Exponent == NULL || Style == NULL )
		return RESULT_ERROR_ARGUMENTS;

	*Exponent	= 1;
	q			= *p;

	/* Superscript 1, 2 or 3 (style 2)... */
	if( *q == '\xfb' || *q == '\xfd' || *q == '\xfc' )
		{
		if( *Style > STYLE_SUPERSCRIPT )
			return RESULT_ERROR_MIXING_EXPONENT_STYLES;

		*Style		= STYLE_SUPERSCRIPT;
		*Exponent	= *q == '\xfb' ? 1 : *q == '\xfd' ? 2 : 3;
		*p			= ++q;

		return RESULT_OK;
		}

	/* We found a circumflex... */
	if( *q == '^' )
		{
		int	Sign	= +1;

		if( *Style == STYLE_SUPERSCRIPT )
			return RESULT_ERROR_MIXING_EXPONENT_STYLES;

		if( *Style == STYLE_ANY )
			*Style	= STYLE_CIRCUMFLEX;
		else if( *Style == STYLE_DIVISION_BAR )
			*Style	= STYLE_UNSIGNED;

		*p	= ++q;

		/* We found a sign for the exponent... */
		if( *q == '+' || *q == '-' )
			{
			/* ...but Style is 3... */
			if( *Style == STYLE_UNSIGNED )
				/* ...that doesn't allow signed exponents. */
				return RESULT_ERROR_MIXING_EXPONENT_STYLES;
			*Style	= STYLE_SIGNED;
			/* The sign is negative... */
			if( *q == '-' )
				/* ...let's change it. */
				Sign	= -1;
			*p	= ++q;
			}

		if( !isdigit( *q ))
			return RESULT_ERROR_INVALID_EXPONENT_VALUE;

		*Exponent	= Sign * ( *q - '0' );
		*p	= ++q;

		/* We found another digit... */
		if( isdigit( *q ))
			/* ...but we allow only one digit exponents. */
			return RESULT_ERROR_EXPONENT_WITH_MORE_THAN_ONE_DIGIT;

		return RESULT_OK;
		}

	return 0;
	}
/*============================================================================*/
int ParseUnitWithExponent( const char ** const p, unitsignature_t * const Signature, int * const Style, int Flags )
	{
	prefixid_t	PrefixId;
	unitid_t	UnitId;
	int			Result;
	int			Exponent;

	if( p == NULL || *p == NULL || Signature == NULL || Style == NULL )
		return RESULT_ERROR_ARGUMENTS;

	memset( Signature, 0x00, sizeof *Signature );
	Signature->ScaleFactor	= 1.0;

	if(( Result = ParsePrefixedUnit( p, &PrefixId, &UnitId, Flags )) <= 0 )
		return Result;

	if(( Result = ParseExponent( p, &Exponent, Style )) < 0 )
		return Result;

	int	BaseUnit			= Equivalence[UnitId].MainUnitId;
	Signature->Exp_s		= Exponent * Units[BaseUnit].Exponents.Exp_s;
	Signature->Exp_m		= Exponent * Units[BaseUnit].Exponents.Exp_m;
	Signature->Exp_kg		= Exponent * Units[BaseUnit].Exponents.Exp_kg;
	Signature->Exp_A		= Exponent * Units[BaseUnit].Exponents.Exp_A;
	Signature->Exp_K		= Exponent * Units[BaseUnit].Exponents.Exp_K;
	Signature->Exp_mol		= Exponent * Units[BaseUnit].Exponents.Exp_mol;
	Signature->Exp_cd		= Exponent * Units[BaseUnit].Exponents.Exp_cd;
	Signature->IsAngle		= Units[BaseUnit].Exponents.IsAngle;
	Signature->IsRadiation	= Units[BaseUnit].Exponents.IsRadiation;
	Signature->ScaleFactor	= pow( Prefixes[PrefixId].Value * Equivalence[UnitId].Scale, Exponent );

	return RESULT_OK;
	}
/*============================================================================*/
int ParseSimpleUnitExpression( const char ** const p, unitsignature_t * const Signature, int * const Multiplier, int * const Style, int Flags )
	{
	unitsignature_t	Signature2;
	const char		*q;
	int				Result, Result2;

	if( p == NULL || *p == NULL || Multiplier == NULL || Style == NULL )
		return RESULT_ERROR_ARGUMENTS;

	memset( Signature, 0x00, sizeof *Signature );
	Signature->ScaleFactor	= 1.0;

	q	= *p;

	Result	= 0;
	while( 1 )
		{
		memset( &Signature2, 0x00, sizeof Signature2 );
		Signature2.ScaleFactor	= 1.0;

		if(( Result2 = ParseUnitWithExponent( &q, &Signature2, Style, Flags )) < 0 )
			return Result2;

		if( Result2 == 0 )
			return Result;

		Signature->Exp_s	   += Signature2.Exp_s;
		Signature->Exp_m	   += Signature2.Exp_m;
		Signature->Exp_kg	   += Signature2.Exp_kg;
		Signature->Exp_A	   += Signature2.Exp_A;
		Signature->Exp_K	   += Signature2.Exp_K;
		Signature->Exp_mol	   += Signature2.Exp_mol;
		Signature->Exp_cd	   += Signature2.Exp_cd;
		Signature->IsAngle	   |= Signature2.IsAngle;
		Signature->IsRadiation |= Signature2.IsRadiation;
		Signature->ScaleFactor *= Signature2.ScaleFactor;

		Result	= 1;

		*p	= q;

		if(( *q != '.' && *q != '*' ) || ( *Multiplier == '.' && *q == '*' ))
			return RESULT_OK;

		if( *Multiplier != '\0' && *q != *Multiplier )
			return RESULT_ERROR_MIXING_DOT_AND_ASTERISK;

		q++;
		}
	}
/*============================================================================*/
int ParseUnitExpression( const char ** const p, unitsignature_t * const Signature, int *Multiplier, int * const Style, int Flags )
	{
	int				HasSpace;
	int				Result;
	const char		*q;

	if( p == NULL || *p == NULL || Signature == NULL || Multiplier == NULL )
		return RESULT_ERROR_ARGUMENTS;

	memset( Signature, 0x00, sizeof *Signature );
	Signature->ScaleFactor	= 1.0;

	q			= *p;

	HasSpace	= isspace( *q );

	SkipBlanks( &q );

	if(( Result = ParseSimpleUnitExpression( &q, Signature, Multiplier, Style, Flags )) < 0 )
		return Result;

	if( Result > 0 )
		{
		if(( Flags & REQUIRE_SPACE ) && !HasSpace )
			return RESULT_ERROR_MISSING_SPACE;

		*p	= q;
		}

	if( *Style != STYLE_SIGNED && *q == '/' )
		{
		unitsignature_t	Signature2;

		if( *++q == '(' )
			{
			q++;
			if(( Result = ParseSimpleUnitExpression( &q, &Signature2, Multiplier, Style, Flags )) <= 0 )
				return Result;

			*p	= q;

			if( *q != ')' )
				return RESULT_ERROR_UNMATCHED_PARENTHESIS;

			if( *Style == STYLE_ANY )
				*Style	= STYLE_DIVISION_BAR;

			q++;
			}
		else
			{
			int	Style2	= *Style == STYLE_ANY ? STYLE_DIVISION_BAR : *Style;

			if(( Result = ParseSimpleUnitExpression( &q, &Signature2, Multiplier, &Style2, Flags )) <= 0 )
				return Result;

			*Style	= Style2;
			}

		Signature->Exp_s	   -= Signature2.Exp_s;
		Signature->Exp_m	   -= Signature2.Exp_m;
		Signature->Exp_kg	   -= Signature2.Exp_kg;
		Signature->Exp_A	   -= Signature2.Exp_A;
		Signature->Exp_K	   -= Signature2.Exp_K;
		Signature->Exp_mol	   -= Signature2.Exp_mol;
		Signature->Exp_cd	   -= Signature2.Exp_cd;
		Signature->IsAngle	   |= Signature2.IsAngle;
		Signature->IsRadiation |= Signature2.IsRadiation;
		Signature->ScaleFactor /= Signature2.ScaleFactor;
		}

	SkipBlanks( &q );

	*p	= q;

	return Result;
	}
/*============================================================================*/
int	ParseNumber( const char ** const p, double * const Value )
	{
	const char	*q, *r;
	int			HasDigits;

	if( p == NULL || *p == NULL || Value == NULL )
		return RESULT_ERROR_ARGUMENTS;

	*Value	= 0.0;
	q		= *p;

	SkipBlanks( &q );

	if( !isdigit( *q ) && *q != '+' && *q != '-' && *q != '.' )
		return 0;

	r	= q;

	if( *q == '+' || *q == '-' )
		q++;

	for( HasDigits = 0; isdigit( *q ); q++ )
		HasDigits	= 1;

	if( *q == '.' )
		q++;

	for( ; isdigit( *q ); q++ )
		HasDigits	= 1;

	if( !HasDigits )
		return 0;

	*p	= q;

	if( *q == 'e' || *q == 'E' )
		{
		q++;

		if( *q == '+' || *q == '-' )
			q++;

		*p	= q;

		if( !isdigit( *q ))
			return RESULT_INVALID_NUMBER;

		for( ; isdigit( *q ); q++ )
			{}
		}

/*
	if( *q == '_' || isalpha( *q ))
		return RESULT_INVALID_NUMBER;
*/

	*p	= q;

	if( sscanf( r, "%lf", Value ) < 1 )
		return RESULT_INVALID_NUMBER;

	return RESULT_OK;
	}
/*============================================================================*/
static int ParseValue( const char ** const p, dimensionalvalue_t * const Value, int *Multiplier, int *Style, int Flags )
	{
	int		Result;

	if( p == NULL || *p == NULL || Multiplier == NULL || Style == NULL )
		return RESULT_ERROR_ARGUMENTS;

	if(( Result = ParseNumber( p, &Value->Value )) <= 0 )
		return Result;

	if(( Result = ParseUnitExpression( p, &Value->Unit, Multiplier, Style, Flags )) < 0 )
		return Result;

	Value->Value	*= Value->Unit.ScaleFactor;
	Value->Unit.ScaleFactor	= 1.0;

	return RESULT_OK;
	}
/*============================================================================*/
typedef enum
	{
	TOKEN_ERROR		= -2,
	TOKEN_INVALID	= -1,
	TOKEN_EOL		=  0,
	TOKEN_OPENPAR,
	TOKEN_CLOSEPAR,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_ASTERISK,
	TOKEN_SLASH,
	TOKEN_CIRCUMFLEX,
	TOKEN_NUMBER,
	TOKEN_VALUE
	} token_t;
/*============================================================================*/
static token_t GetToken( const char ** const p, dimensionalvalue_t * Value, int *Multiplier, int *Style, int Flags )
	{
	dimensionalvalue_t	Dummy;

	if( p == NULL || *p == NULL || Multiplier == NULL || Style == NULL )
		return TOKEN_ERROR;

	if( Value == NULL )
		Value	= &Dummy;

	while( isspace( **p ))
		(*p)++;

	if( **p == '\0' )
		return TOKEN_EOL;

	switch( *(*p)++ )
		{
		case '(':
			return TOKEN_OPENPAR;
		case ')':
			return TOKEN_CLOSEPAR;
		case '+':
			return TOKEN_PLUS;
		case '-':
			return TOKEN_MINUS;
		case '*':
			return TOKEN_ASTERISK;
		case '/':
			return TOKEN_SLASH;
		case '^':
			return TOKEN_CIRCUMFLEX;
		}

	(*p)--;

	if( ParseValue( p, Value, Multiplier, Style, Flags ) < RESULT_OK )
		return TOKEN_INVALID;

	if( Value->Unit.IsAngle || Value->Unit.IsRadiation )
		return TOKEN_VALUE;
	for( int i = 0; i < LENGTH( Value->Unit.Exponents ); i++ )
		if( Value->Unit.Exponents[i] != 0 )
			return TOKEN_VALUE;

	return TOKEN_NUMBER;
	}
/*============================================================================*/
static type_t EvaluateValueExpression( const char ** const p, dimensionalvalue_t * const Result, int *Multiplier, int *Style, int Flags )
	{
	token_t		Token;
	type_t		Type1;

	Token = GetToken( p, Result, Multiplier, Style, Flags );

	switch( Token )
		{
		/*--------------------------------------------------------------------*/
		case TOKEN_PLUS:
			/* The unary plus sign does nothing, just returns the expression that follows it. */
			Type1 = EvaluateValueExpression( p, Result, Multiplier, Style, Flags );

			if( Type1 != TYPE_NUMBER && Type1 != TYPE_VALUE )
				{
				printf( "\nInvalid expression" );
				return TYPE_INVALID;
				}

			return Type1;
		/*--------------------------------------------------------------------*/
		case TOKEN_MINUS:
			/* The unary minus sign inverts the sign of the expression. */
			Type1	= EvaluateValueExpression( p, Result, Multiplier, Style, Flags );

			if( Type1 != TYPE_NUMBER && Type1 != TYPE_VALUE )
				{
				printf( "\nInvalid expression" );
				return TYPE_INVALID;
				}

			Result->Value	= -Result->Value;
			return Type1;
		/*--------------------------------------------------------------------*/
		case TOKEN_NUMBER:
			return TYPE_NUMBER;
		/*--------------------------------------------------------------------*/
		case TOKEN_VALUE:
			return TYPE_VALUE;
		/*--------------------------------------------------------------------*/
		case TOKEN_OPENPAR:
			Type1			= EvaluateExpression( p, Result, Multiplier, Style, Flags );
			Token			= GetToken( p, NULL, Multiplier, Style, Flags );;
			if( Token != TOKEN_CLOSEPAR )
				{
				printf( "\nMissing \")\"" );
		default:
				return TYPE_INVALID;
				}
			return Type1;
		/*--------------------------------------------------------------------*/
		}

	/* Should never reach here. */
	return TYPE_INVALID;
	}
/*============================================================================*/
static type_t EvaluatePowerExpression( const char ** const p, dimensionalvalue_t * const Result, int *Multiplier, int *Style, int Flags )
	{
	const char	*q;
	token_t		Token;
	type_t		Type1, Type2;

	Type1	= EvaluateValueExpression( p, Result, Multiplier, Style, Flags );

	q		= *p;

	while(( Token = GetToken( &q, NULL, Multiplier, Style, Flags )) == TOKEN_CIRCUMFLEX )
		{
		dimensionalvalue_t	Operand2;

		if( Type1 != TYPE_NUMBER && Type1 != TYPE_VALUE )
			{
			printf( "\nInvalid argument for operator" );
			return TYPE_INVALID;
			}

		*p		= q;

		Type2	= EvaluatePowerExpression( &q, &Operand2, Multiplier, Style, Flags );

		if( Type2 != TYPE_NUMBER )
			{
			printf( "\nCan only raise to a dimensionless power." );
			return TYPE_INVALID;
			}

		if( Type1 == TYPE_VALUE )
			{
			if( modf( Operand2.Value, NULL ) != 0 )
				{
				printf( "\nCan only raise a dimensional value to an integer power." );
				return TYPE_INVALID;
				}
			Result->Value				= pow( Result->Value, Operand2.Value );
			Result->Unit.Exp_s		   *= Operand2.Value;
			Result->Unit.Exp_m		   *= Operand2.Value;
			Result->Unit.Exp_kg		   *= Operand2.Value;
			Result->Unit.Exp_A		   *= Operand2.Value;
			Result->Unit.Exp_K		   *= Operand2.Value;
			Result->Unit.Exp_mol	   *= Operand2.Value;
			Result->Unit.Exp_cd		   *= Operand2.Value;
			}
		else
			Result->Value	= pow( Result->Value, Operand2.Value );

		*p		= q;
		}

	return Type1;
	}
/*============================================================================*/
static type_t EvaluateMultiplicativeExpression( const char ** const p, dimensionalvalue_t * const Result, int *Multiplier, int *Style, int Flags )
	{
	const char			*q;
	token_t				Token;
	type_t				Type1, Type2;

	Type1	= EvaluatePowerExpression( p, Result, Multiplier, Style, Flags );

	q		= *p;

	while(( Token = GetToken( &q, NULL, Multiplier, Style, Flags )) == TOKEN_ASTERISK || Token == TOKEN_SLASH )
		{
		dimensionalvalue_t	Operand2;

		if( Type1 != TYPE_NUMBER && Type1 != TYPE_VALUE )
			{
			printf( "\nInvalid argument for operator" );
			return TYPE_INVALID;
			}

		*p	= q;

		Type2	= EvaluatePowerExpression( &q, &Operand2, Multiplier, Style, Flags );

		if( Type2 != TYPE_NUMBER && Type2 != TYPE_VALUE )
			{
			printf( "\nInvalid argument for operator" );
			return TYPE_INVALID;
			}

		switch( Token )
			{
			default:
				printf( "\nInternal error" );
				return TYPE_INVALID;

			case TOKEN_ASTERISK:
				Result->Value			   *= Operand2.Value;
				Result->Unit.Exp_s		   += Operand2.Unit.Exp_s;
				Result->Unit.Exp_m		   += Operand2.Unit.Exp_m;
				Result->Unit.Exp_kg		   += Operand2.Unit.Exp_kg;
				Result->Unit.Exp_A		   += Operand2.Unit.Exp_A;
				Result->Unit.Exp_K		   += Operand2.Unit.Exp_K;
				Result->Unit.Exp_mol	   += Operand2.Unit.Exp_mol;
				Result->Unit.Exp_cd		   += Operand2.Unit.Exp_cd;
				Result->Unit.IsAngle	   |= Operand2.Unit.IsAngle;
				Result->Unit.IsRadiation   |= Operand2.Unit.IsRadiation;
				break;

			case TOKEN_SLASH:
				if( Operand2.Value == 0 )
					{
					printf( "\nDivide by zero" );
					return TYPE_INVALID;
					}
				Result->Value			   /= Operand2.Value;
				Result->Unit.Exp_s		   -= Operand2.Unit.Exp_s;
				Result->Unit.Exp_m		   -= Operand2.Unit.Exp_m;
				Result->Unit.Exp_kg		   -= Operand2.Unit.Exp_kg;
				Result->Unit.Exp_A		   -= Operand2.Unit.Exp_A;
				Result->Unit.Exp_K		   -= Operand2.Unit.Exp_K;
				Result->Unit.Exp_mol	   -= Operand2.Unit.Exp_mol;
				Result->Unit.Exp_cd		   -= Operand2.Unit.Exp_cd;
				Result->Unit.IsAngle	   |= Operand2.Unit.IsAngle;
				Result->Unit.IsRadiation   |= Operand2.Unit.IsRadiation;
				break;
			}

		*p		= q;
		}

	if( Result->Unit.IsRadiation || Result->Unit.IsAngle )
		return TYPE_VALUE;
	for( int i = 0; i < LENGTH( Result->Unit.Exponents ); i++ )
		if( Result->Unit.Exponents[i] != 0 )
			return TYPE_VALUE;

	return TYPE_NUMBER;
	}
/*============================================================================*/
type_t EvaluateExpression( const char ** const p, dimensionalvalue_t * const Result, int *Multiplier, int *Style, int Flags )
	{
	const char	*q;
	token_t		Token;
	type_t		Type1, Type2;

	Type1	= EvaluateMultiplicativeExpression( p, Result, Multiplier, Style, Flags );

	q		= *p;

	while(( Token = GetToken( &q, NULL, Multiplier, Style, Flags )) == TOKEN_PLUS || Token == TOKEN_MINUS )
		{
		dimensionalvalue_t	Operand2;

		if( Type1 != TYPE_NUMBER && Type1 != TYPE_VALUE )
			{
			printf( "\nInvalid argument for operator" );
			return TYPE_INVALID;
			}

		Type2	= EvaluateMultiplicativeExpression( &q, &Operand2, Multiplier, Style, Flags );

		if( Type2 != TYPE_NUMBER && Type2 != TYPE_VALUE )
			{
			printf( "\nInvalid argument for operator" );
			return TYPE_INVALID;
			}

		if( Type1 != Type2 )
			{
			printf( "\nCannot add or subtract a dimensional value with a dimensionless value." );
			return TYPE_INVALID;
			}

		if( Result->Unit.IsRadiation != Operand2.Unit.IsRadiation || Result->Unit.IsAngle != Operand2.Unit.IsAngle )
			{
			printf( "\nCannot add or subtract values that have different units." );
			return TYPE_INVALID;
			}

		for( int i = 0; i < LENGTH( Operand2.Unit.Exponents ); i++ )
			if( Result->Unit.Exponents[i] != Operand2.Unit.Exponents[i] )
				{
				printf( "\nCannot add or subtract values that have different units." );
				return TYPE_INVALID;
				}

		*p		= q;

		switch( Token )
			{
			default:
				printf( "\nInternal error" );
				return TYPE_INVALID;

			case TOKEN_PLUS:
				Result->Value  += Operand2.Value;
				break;
			case TOKEN_MINUS:
				Result->Value  -= Operand2.Value;
				break;
			}

		*p		= q;
		}

	return Type1;
	}
/*============================================================================*/
#if 1
int	PrintExpandedUnit( const unitsignature_t * const Signature, int Mode )
	{
	static const char	*BaseUnits[]			= { "kg", "m", "A", "s", "K", "cd", "mol" };
	static const char	*ExpChars[]				= { "^0", "\xfb", "\xfd", "\xfc" };
	int					PrintedSomethingAlready	= 0, NumberOfNegativeExponents = 0;
	int					UseSuperscript			= 0;
	int					UseNegativeExponents	= ( Mode & MODE_USE_NEGATIVE_EXPONENTS ) != 0;
	int					i;

	if( Signature == NULL )
		return RESULT_ERROR_ARGUMENTS;

	int					Exp[]					=
		{
		Signature->Exp_kg, Signature->Exp_m, Signature->Exp_A, Signature->Exp_s,
		Signature->Exp_K, Signature->Exp_cd, Signature->Exp_mol
		};

	if(( Mode & MODE_USE_SUPERSCRIPT ) == MODE_USE_SUPERSCRIPT )
		for( UseSuperscript = 1, i = 0; i < LENGTH( Exp ); i++ )
			if( Exp[i] < -3 || Exp[i] > 3 || ( Exp[i] < 0 && UseNegativeExponents ))
				UseSuperscript	= 0;

	for( i = 0; i < LENGTH( Exp ); i++ )
		{
		if( Exp[i] < 0 )
			NumberOfNegativeExponents++;
		if( Exp[i] > 0 || ( UseNegativeExponents && Exp[i] < 0 ))
			{
			if( PrintedSomethingAlready )
				printf( "." );
			PrintedSomethingAlready	= 1;
			if( Exp[i] > 1 || Exp[i] < 0 )
				{
				if( UseSuperscript )
					printf( "%s%s", BaseUnits[i], ExpChars[Exp[i]&3] );
				else
					printf( "%s^%d", BaseUnits[i], Exp[i] );
				}
			else
				printf( "%s", BaseUnits[i] );
			}
		}

	if( !UseNegativeExponents && NumberOfNegativeExponents > 0 )
		{
		printf( "/%s", NumberOfNegativeExponents > 1 ? "(" : "" );
		PrintedSomethingAlready	= 0;
		for( i = 0; i < LENGTH( Exp ); i++ )
			{
			if( Exp[i] < 0 )
				{
				if( PrintedSomethingAlready )
					printf( "." );
				PrintedSomethingAlready	= 1;
				if( Exp[i] < -1 )
					{
					if( UseSuperscript )
						printf( "%s%s", BaseUnits[i], ExpChars[-Exp[i]&3] );
					else
						printf( "%s^%d", BaseUnits[i], -Exp[i] );
					}
				else
					printf( "%s", BaseUnits[i] );
				}
			}
		printf( "%s", NumberOfNegativeExponents > 1 ? ")" : "" );
		}
//	printf( "\n" );

	return UseSuperscript;
	}
#else
static void PrintExpandedUnit( int Exp_s, int Exp_m, int Exp_kg, int Exp_A, int Exp_K, int Exp_mol, int Exp_cd )
	{
	int	PrintedSomethingAlready	= 0;

	if( Exp_s != 0 )
		{
		printf( "s^%d", Exp_s );
		PrintedSomethingAlready	= 1;
		}

	if( Exp_m != 0 )
		{
		if( Exp_m != 1 )
			printf( "%sm^%d", PrintedSomethingAlready ? "." : "", Exp_m );
		else
			printf( "%sm", PrintedSomethingAlready ? "." : "" );
		PrintedSomethingAlready	= 1;
		}
	if( Exp_kg != 0 )
		{
		if( Exp_kg != 1 )
			printf( "%skg^%d", PrintedSomethingAlready ? "." : "", Exp_kg );
		else
			printf( "%skg", PrintedSomethingAlready ? "." : "" );
		PrintedSomethingAlready	= 1;
		}
	if( Exp_A != 0 )
		{
		if( Exp_A != 1 )
			printf( "%sA^%d", PrintedSomethingAlready ? "." : "", Exp_A );
		else
			printf( "%sA", PrintedSomethingAlready ? "." : "" );
		PrintedSomethingAlready	= 1;
		}
	if( Exp_K != 0 )
		{
		if( Exp_K != 1 )
			printf( "%sK^%d", PrintedSomethingAlready ? "." : "", Exp_K );
		else
			printf( "%sK", PrintedSomethingAlready ? "." : "" );
		PrintedSomethingAlready	= 1;
		}
	if( Exp_mol != 0 )
		{
		if( Exp_mol != 1 )
			printf( "%smol^%d", PrintedSomethingAlready ? "." : "", Exp_mol );
		else
			printf( "%smol", PrintedSomethingAlready ? "." : "" );
		PrintedSomethingAlready	= 1;
		}
	if( Exp_cd != 0 )
		{
		if( Exp_cd != 1 )
			printf( "%scd^%d", PrintedSomethingAlready ? "." : "", Exp_cd );
		else
			printf( "%scd", PrintedSomethingAlready ? "." : "" );
		PrintedSomethingAlready	= 1;
		}
	printf( "\n" );
	}
#endif
/*============================================================================*/
int TestUnits( void )
	{
	int			i, j;
	char		Str[256];
	const char	*p;
	prefixid_t	PrefixId;
	unitid_t	UnitId;

	for( i = 0; i < LENGTH( UnitsAlias ); i++ )
		for( j = 0; j < LENGTH( PrefixAlias ); j++ )
			{
			sprintf( Str, "%s%s", PrefixAlias[j].String, UnitsAlias[i].String );
			p	= Str;
			if( ParsePrefixedUnit( &p, &PrefixId, &UnitId, 0 ) > 0 )
				{
				//printf( "%s -> %s%s %s%s\n", Str, Prefixes[PrefixId].Name, Units[Equivalence[UnitId].MainUnitId].Name, Prefixes[PrefixId].Symbol, Units[Equivalence[UnitId].MainUnitId].Symbol );
				if( UnitId != UnitsAlias[i].UnitId || PrefixId != PrefixAlias[j].PrefixId )
					printf( "(%s %s) %s -> %s %s\n", PrefixAlias[j].String, UnitsAlias[i].String, Str, Prefixes[PrefixId].Symbol, Units[Equivalence[UnitId].MainUnitId].Symbol );
				}
			else
				printf( "%s -> Erro\n", Str );
			}
	return 0;
	}
/*============================================================================*/
