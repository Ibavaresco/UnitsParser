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
#if			!defined __UNITSPARSER_H__
#define __UNITSPARSER_H__
/*============================================================================*/
typedef enum
	{
	PREFIX_ERROR	= -1,
	PREFIX_NONE		=  0,
	PREFIX_QUECTO,
	PREFIX_RONTO,
	PREFIX_YOCTO,
	PREFIX_ZEPTO,
	PREFIX_ATTO,
	PREFIX_FEMTO,
	PREFIX_PICO,
	PREFIX_NANO,
	PREFIX_MICRO,
	PREFIX_MILLI,
	PREFIX_CENTI,
	PREFIX_DECI,
	PREFIX_DECA,
	PREFIX_HECTO,
	PREFIX_KILO,
	PREFIX_MEGA,
	PREFIX_GIGA,
	PREFIX_TERA,
	PREFIX_PETA,
	PREFIX_EXA,
	PREFIX_ZETTA,
	PREFIX_YOTTA,
	PREFIX_RONNA,
	PREFIX_QUETTA
	} prefixid_t;
/*============================================================================*/
double		GetPrefixValue			( prefixid_t PrefixId );
const char	*GetPrefixSymbol		( prefixid_t PrefixId );
const char	*GetPrefixName			( prefixid_t PrefixId );
/*----------------------------------------------------------------------------*/
#define	ACCEPT_BOTH_ENCODINGS		0
#define	REJECT_UTF8					1
#define	REJECT_ANSI					2
#define	REJECT_U_AS_MICRO			4
/*----------------------------------------------------------------------------*/
prefixid_t	ParsePrefix				( const char **p, int Flags );
/*============================================================================*/
typedef enum
	{
	UNIT_INVALID	= -2,
	UNIT_CUSTOM		= -1,

	UNIT_NONE		=  0,

	UNIT_SECOND,
	UNIT_METER,
	UNIT_KILOGRAM,
	UNIT_AMPERE,
	UNIT_KELVIN,
	UNIT_MOLE,
	UNIT_CANDELA,

	UNIT_RADIAN,
	UNIT_STERADIAN,
	UNIT_HERTZ,
	UNIT_NEWTON,
	UNIT_PASCAL,
	UNIT_JOULE,
	UNIT_WATT,
	UNIT_COULOMB,
	UNIT_VOLT,
	UNIT_OHM,
	UNIT_SIEMENS,
	UNIT_FARAD,
	UNIT_HENRY,
	UNIT_TESLA,
	UNIT_WEBER,
	UNIT_BECQUEREL,
	UNIT_GRAY,
	UNIT_SIEVERT,
	UNIT_KATAL,

	UNIT_CANDELA_PER_SQUARE_METER,
	UNIT_PASCAL_SECOND,
	UNIT_SQUARE_METER_PER_SECOND,
	UNIT_CUBIC_METER_PER_SECOND,
	UNIT_COULOMB_PER_KG,

	UNIT_SQUARE_METER,
	UNIT_CUBIC_METER,

	UNIT_METER_PER_SECOND,
	UNIT_METER_PER_SECOND_SQUARED,

	UNIT_POISE,
	UNIT_STOKES,

	UNIT_LUMEN,

	UNIT_DEGREE,
	UNIT_GRADE,
	UNIT_ARC_MINUTE,
	UNIT_ARC_SECOND,

	UNIT_DEGREE_CELSIUS,
	UNIT_DEGREE_FAHRENHEIT,
	UNIT_DEGREE_RANKINE,
	UNIT_DEGREE_REAUMUR,

	UNIT_FERMI,
	UNIT_ANGSTROM,
	UNIT_MICRON,
	UNIT_MIL,
	UNIT_POINT,
	UNIT_PICA,
	UNIT_INCH,
	UNIT_FOOT,
	UNIT_SURVEY_FOOT,
	UNIT_YARD,
	UNIT_FATHOM,
	UNIT_ROD,
	UNIT_CHAIN,
	UNIT_FURLONG,
	UNIT_MILE,
	UNIT_NAUTICAL_MILE,
	UNIT_LAND_LEAGUE,
	UNIT_MARINE_LEAGUE,
	UNIT_BRAZILIAN_LEAGUE,
	UNIT_ASTRONOMICAL_UNIT,
	UNIT_LIGHT_YEAR,
	UNIT_PARSEC,

	UNIT_BARN,
	UNIT_SQUARE_INCH,
	UNIT_SQUARE_FOOT,
	UNIT_SQUARE_YARD,
	UNIT_ARE,
	UNIT_ACRE,
	UNIT_ALQUEIRE_PAULISTA,
	UNIT_ALQUEIRE_GOIANO,
	UNIT_SQUARE_MILE,

	UNIT_TEASPOON,
	UNIT_TABLESPOON,
	UNIT_CUBIC_INCH,
	UNIT_UK_FLUID_OUNCE,
	UNIT_US_FLUID_OUNCE,
	UNIT_CUP,
	UNIT_US_PINT,
	UNIT_UK_PINT,
	UNIT_QUART,
	UNIT_LITER,
	UNIT_BOARDFOOT,
	UNIT_US_GALLON,
	UNIT_CANADA_GALLON,
	UNIT_UK_GALLON,
	UNIT_PECK,
	UNIT_CUBIC_FOOT,
	UNIT_BUSHEL,
	UNIT_BARREL,
	UNIT_CUBIC_YARD,
	UNIT_STERE,

	UNIT_KNOT,
	UNIT_KILOMETER_PER_HOUR,
	UNIT_MILE_PER_HOUR,
	UNIT_SPEED_OF_LIGHT,

	UNIT_STANDARD_GRAVITY,

	UNIT_GRAM,
	UNIT_CARAT,
	UNIT_GRAIN,
	UNIT_POUND,
	UNIT_POUND_TROY,
	UNIT_OUNCE,
	UNIT_OUNCE_TROY,
	UNIT_SLUG,
	UNIT_METRIC_TONNE,
	UNIT_SHORT_TONNE,
	UNIT_LONG_TONNE,
	UNIT_ATOMIC_MASS_UNIT,
	UNIT_STONE,
	UNIT_US_HUNDREDWEIGHT,
	UNIT_UK_HUNDREDWEIGHT,
	UNIT_ARROBA,
	UNIT_PORTUGAL_ARROBA,
	UNIT_CASTELA_ARROBA,
	UNIT_CATALUNA_ARROBA,
	UNIT_ARAGON_ARROBA,

	UNIT_JULIAN_YEAR,
	UNIT_GREGORIAN_YEAR,
	UNIT_TROPICAL_YEAR,
	UNIT_FORTNIGHT,
	UNIT_WEEK,
	UNIT_DAY,
	UNIT_HOUR,
	UNIT_MINUTE,

	UNIT_RPM,

	UNIT_ELECTRON_VOLT,
	UNIT_ERG,
	UNIT_CALORIE,
	UNIT_BRITISH_THERMAL_UNIT,
	UNIT_WATT_HOUR,
	UNIT_THERM,

	UNIT_TORRICELLI,
	UNIT_POUND_PER_SQUARE_INCH,
	UNIT_MILLIMETER_OF_MERCURY,
	UNIT_INCH_OF_MERCURY,
	UNIT_INCH_OF_WATER,
	UNIT_METER_OF_WATER,
	UNIT_BAR,
	UNIT_STANDARD_ATMOSPHERE,

	UNIT_HORSEPOWER,

	UNIT_POUND_FORCE,
	UNIT_POUNDAL,
	UNIT_GRAM_FORCE,
	UNIT_DYNE,
	UNIT_KIP,

	UNIT_CURIE,
	UNIT_ROENTGEN,
	UNIT_RAD,

	UNIT_LUX,
	UNIT_STILB,
	UNIT_PHOT,
	UNIT_LAMBERT,
	UNIT_FOOTCANDLE,
	UNIT_FOOTLAMBERT,

	UNIT_FARADAY,
	UNIT_AMPERE_HOUR,

	UNIT_MHO,
	UNIT_GAUSS,
	UNIT_MAXWELL
	} unitid_t;
/*============================================================================*/
typedef struct
	{
	double		ScaleFactor;
	union
		{
		struct
			{
			int	Exp_s;
			int	Exp_m;
			int	Exp_kg;
			int	Exp_A;
			int	Exp_K;
			int	Exp_mol;
			int	Exp_cd;
			int	IsAngle;
			int	IsRadiation;
			};
		int		Exponents[7];
		};
	} unitsignature_t;
/*============================================================================*/
const char	*GetUnitName			( unitid_t UnitId );
const char	*GetUnitSymbol			( unitid_t UnitId );
const char	*GetUnitDescription		( unitid_t UnitId );
unitid_t	ParseUnit				( const char **p );
/*============================================================================*/
typedef struct
	{
	unitsignature_t	Unit;
	double			Value;
	} dimensionalvalue_t;
/*============================================================================*/
#define	STYLE_ANY			0
#define	STYLE_DIVISION_BAR	1
#define	STYLE_SUPERSCRIPT	2
#define	STYLE_CIRCUMFLEX	3
#define	STYLE_UNSIGNED		4
#define	STYLE_SIGNED		5
/*============================================================================*/
#define	REJECT_PREFIX		1
#define REJECT_DIVIDER		2
#define REJECT_MILTIPLIER	4
/*============================================================================*/
#define	RESULT_OK										  1
#define	RESULT_NO_UNIT									  0
#define	RESULT_NO_EXPONENT								  0
#define	RESULT_ERROR_ARGUMENTS							 -1
#define	RESULT_ERROR_UNIT_NOT_FOUND						 -2
#define RESULT_ERROR_MIXING_EXPONENT_STYLES				 -3
#define	RESULT_ERROR_MIXING_DOT_AND_ASTERISK			 -4
#define RESULT_ERROR_DIVISION_WITH_NEGATIVE_EXPONENT	 -5
#define	RESULT_ERROR_MORE_THAN_ONE_DIVISION				 -6
#define	RESULT_ERROR_INVALID_EXPONENT_VALUE				 -7
#define	RESULT_ERROR_EXPONENT_WITH_MORE_THAN_ONE_DIGIT	 -8
#define	RESULT_ERROR_UNMATCHED_PARENTHESIS				 -9
#define RESULT_ERROR_MISSING_SPACE						-10

#define	RESULT_ERROR_VALUE_NOT_FOUND					-11
#define	RESULT_INVALID_NUMBER							-12
#define	RESULT_ERROR_WRONG_UNIT							-13
#define	RESULT_ERROR_UNIT_NOT_SUPPLIED					-14
/*============================================================================*/
int			ParsePrefixedUnit			( const char ** const p, prefixid_t * const PrefixId, unitid_t * const UnitId, int Flags );
int			ParseUnitsExpression		( const char ** const p, unitsignature_t * const Signature );
unitid_t	FindUnitId					( const unitsignature_t * const Signature );

int			ParseNumber					( const char ** const p, double *Value );
/*----------------------------------------------------------------------------*/
#define	REQUIRE_UNIT					1
#define	REQUIRE_SPACE					2
/*----------------------------------------------------------------------------*/
int			ParseValueWithUnitExpr		( const char ** const p, double * const Value, unitid_t UnitId, int Flags );
int			ParseValueWithPrefixedUnit	( const char ** const p, double * const Value, unitid_t UnitId, int Flags );
/*============================================================================*/
typedef enum
	{
	TYPE_INVALID	= -1,
	TYPE_UNDEFINED,
	TYPE_NUMBER,
	TYPE_VALUE
	} type_t;
/*----------------------------------------------------------------------------*/
type_t		EvaluateExpression			( const char ** const p, dimensionalvalue_t * const Result, int *Multiplier, int *Style, int Flags );
/*============================================================================*/
#define	MODE_USE_CARET					0
#define	MODE_USE_SUPERSCRIPT			1
#define	MODE_USE_DIVISION_BAR			0
#define	MODE_USE_NEGATIVE_EXPONENTS		2
/*----------------------------------------------------------------------------*/
int			PrintExpandedUnit			( const unitsignature_t * const Signature, int Mode );
/*============================================================================*/
int			TestUnits					( void );
/*============================================================================*/
#endif	/*	!defined __UNITSPARSER_H__ */
/*============================================================================*/
