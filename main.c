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
#include <stdio.h>
#include <string.h>
#include "UnitsParser.h"
/*============================================================================*/
static void TestAllUnits( void )
	{
	unitid_t	Unit;

	for( Unit = UNIT_SECOND; Unit <= UNIT_METER_PER_SECOND_SQUARED; Unit++ )
		{
		char		Str[128];

		printf( "\nTesting %s. Enter values with units or unit expressions that convert to \"%s\" (%s).\n\n", GetUnitDescription( Unit ), GetUnitName( Unit ), GetUnitSymbol( Unit ));

		while( fgets( Str, sizeof Str, stdin ) != NULL && strlen( Str ) > 0 && Str[0] != '\n' )
			{
			double	Value;
			const char	*p	= Str;

			if( ParseValueWithUnitExpr( &p, &Value, Unit, REQUIRE_UNIT ) >= RESULT_OK )
				printf( " => %lg %s\n", Value, GetUnitSymbol( Unit ));
			else
				printf( "Error\n" );
			}
		}
	}
/*============================================================================*/
static void TestUnitExpressions( void )
	{
	char		Str[128];

	printf( "\nEnter a valid unit expression optionally (empty input to stop):" );

	while( fgets( Str, sizeof Str, stdin ) != NULL && strlen( Str ) > 0 && Str[0] != '\n' )
		{
		unitsignature_t	Signature;
//		double			Value;
		const char		*p	= Str;

		if( Str[strlen( Str ) - 1] == '\n' )
			Str[strlen( Str ) - 1] = 0;

		if( ParseUnitsExpression( &p, &Signature ) < RESULT_OK )
			printf( "Error\n" );
		else
			{
			unitid_t	UnitId;

			UnitId	= FindUnitId( &Signature );
			printf( "\n%s => ", Str );
			if( UnitId > UNIT_CUSTOM )
				printf( "%s = ", GetUnitSymbol( UnitId ));
			PrintExpandedUnit( &Signature, MODE_USE_CARET );//| MODE_USE_NEGATIVE_EXPONENTS );
			printf( "\n" );
			}
		printf( "\nEnter a valid unit expression optionally (empty input to stop):" );
		}
	}
/*============================================================================*/
int main( void )
	{
	const char		*p;
	char			Str[128];

	TestUnitExpressions();

#if 0
	int				i, j;
	prefixid_t		PrefixId;
	unitid_t		UnitId;
	unitsignature_t	Signature;

	for( i = 0; i < LENGTH( UnitsAlias ); i++ )
		for( j = 0; j < LENGTH( PrefixAlias ); j++ )
			{
			sprintf( Str, "%s%s", PrefixAlias[j].String, UnitsAlias[i].String );
			p	= Str;
			if( ParsePrefixedUnit( &p, &PrefixId, &UnitId ) > 0 )
				{
				//printf( "%s -> %s%s %s%s\n", Str, Prefixes[PrefixId].Name, Units[Equivalence[UnitId].MainUnitId].Name, Prefixes[PrefixId].Symbol, Units[Equivalence[UnitId].MainUnitId].Symbol );
				if( UnitId != Equivalence[UnitsAlias[i].UnitId].MainUnitId || PrefixId != PrefixAlias[j].PrefixId )
					printf( "%s (%s %s) -> %s %s\n", Str, PrefixAlias[j].String, UnitsAlias[i].String, Prefixes[PrefixId].Symbol, Units[Equivalence[UnitId].MainUnitId].Symbol );
				}
			else
				printf( "%s -> Erro\n", Str );
			}
	return 0;
#endif

#if 1
	while( 1 )
		{
		printf( "\nEnter a value representing a volume and a valid unit of volume optionally preceded by an S.I. prefix:" );
		fflush( stdout );
		fgets( Str, sizeof Str, stdin );

		if( strlen( Str ) == 0 || Str[0] == '\n' || Str[0] == '\r' )
			return 0;
/*
		for( i = 0; i <= strlen( Str ); i++ )
			printf( "%02X ", (unsigned char)Str[i] );
		printf( "\n" );
*/
		p	= Str;

		double	Value;

		if( ParseValueWithUnitExpr( &p, &Value, UNIT_CUBIC_METER, REQUIRE_UNIT ) >= RESULT_OK )
			printf( "\n=> %lg %s\n", Value, GetUnitSymbol( UNIT_CUBIC_METER ));
		else
			printf( "Error\n" );
/*
		if( ParseValueWithPrefixedUnit( &p, &Value, UNIT_WATT, 0 ) >= RESULT_OK )
			printf( "\n=> %lg %s\n", Value, GetUnitSymbol( UNIT_WATT ));
		else
			printf( "Erro\n" );
*/

/*
		if( ParseUnitsExpression( &p, &Signature ) > 0 )
			{
			UnitId	= FindUnitId( &Signature );
			if( UnitId >= UNIT_NONE )
				printf( " %s %s\n", GetUnitName( UnitId ), GetUnitSymbol( UnitId ));
			else
				printf( "%s -> CustomUnit\n", Str );
			}
		else
			printf( "%s -> Erro\n", Str );
*/
		}

#else

	for( i = 0; i < LENGTH( UnitsAlias ); i++ )
		{
		p	= UnitsAlias[i].String;

		if(( r = ParseUnit( &p )) != i )
			printf( "Nao achou %d (%s) -> %d (%s)\n", i, UnitsAlias[i].String, r, UnitsAlias[r].String );
		}
#endif
	}
/*============================================================================*/
