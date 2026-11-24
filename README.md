The routines can parse and calculate complex expressions with values and units and give the result in S.I. units.

It implements the operations +, -, *, / and ^ (power).

It can calculate expressions like: s = s0 + v0 * t + ( ac * t ^2 ) / 2

Suppose:
s0 = 50 m
v0 = 22 m/s
t  = 10 s
ac = 9.8 m/s^2

Input:
<p>
50 m + 22 m/s * 10 s + (9.8 m/s² * (10 s) ^ 2) / 2
<p>
Result:
<p>
Value = 760 m ( "meter" = m = distance/length )
<p>
<p>
Voltage divider:
<p>
<p>
Input:
<p>
30 V * 3.3 kohm / ( 30 kohm + 3.3 kohm )
<p>
Result:
<p>
Value = 2.97297 V ( "volt" = kg.m²/(A.s³) = electric potential )
<p>


It understands all the S.I. prefixes and units, and exponents.
The units it understands:

Second,
Meter,
Kilogram,
Ampere,
Kelvin,
Mole,
Candela,
Radian,
Steradian,
Hertz,
Newton,
Pascal,
Joule,
Watt,
Coulomb,
Volt,
Ohm,
Siemens,
Farad,
Henry,
Tesla,
Weber,
Becquerel,
Gray,
Sievert,
Katal,
Poise,
Stokes,
Lumen,
Degree,
Grade,
Arc-Minute,
Arc-Second,
Degree Celsius,
Degree Fahrenheit,
Degree Rankine,
Degree Reaumur,
Fermi,
Angstrom,
Micron,
mil,
Point,
Pica,
Inch,
Foot,
Survey Foot,
Yard,
Fathom,
Rod,
Chain,
Furlong,
Mile,
Nautical Mile,
Land League,
Marine League,
Brazilian League,
Astronomical Unit,
Light-Year,
Parsec,
Barn,
Are,
Acre,
Alqueire Paulista,
Alqueire Goiano,
Teaspoon,
Tablespoon,
UK Fluid Ounce,
US Fluid Ounce,
Cup,
US Pint,
UK Pint,
Quart,
Liter,
Boardfoot,
US Gallon,
Canada Gallon,
UK Gallon,
Peck,
Bushel,
Barrel,
Stere,
Knot,
Kilometer Per Hour,
Mile Per Hour,
Speed Of Light,
Standard Gravity,
Gram,
Carat,
Grain,
Pound,
Pound-Troy,
Ounce,
Ounce-Troy,
Slug,
Metric Tonne,
Short Tonne,
Long Tonne,
Atomic Mass Unit,
Stone,
US Hundredweight,
UK Hundredweight,
Arroba,
Portugal Arroba,
Castela Arroba,
Cataluna Arroba,
Aragon Arroba,
Julian Year,
Gregorian Year,
Tropical Year,
Fortnight,
Week,
Day,
Hour,
Minute,
Rpm,
Electron-Volt,
Erg,
Calorie,
British Thermal Unit,
Watt-Hour,
Therm,
Torricelli,
Pound Per Square Inch,
Millimeter Of Mercury,
Inch Of Mercury,
Inch Of Water,
Meter Of Water,
Bar,
Standard Atmosphere,
Horsepower,
Pound-Force,
Poundal,
Gram-Force,
Dyne,
Kip,
Curie,
Roentgen,
Rad,
Lux,
Stilb,
Phot,
Lambert,
Footcandle,
Footlambert,
Faraday,
Ampere-Hour,
Mho,
Gauss,
Maxwell
