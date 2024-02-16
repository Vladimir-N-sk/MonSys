#ifndef TIME_H
#define TIME_H

#include <ctime>
#include <stdexcept>
#include<iostream>

#include "common.h"
#include "globals.h"

const timespec ZEROTIME = {0,0};

ostream& operator<<( ostream& , const timespec&);
istream& operator>>( istream& , timespec&);

string timespec2isotime( const timespec&);
timespec isotime2timespec( const string&);

unsigned long long nanosec( const timespec&);
unsigned long long milisec( const timespec&);
unsigned long long microsec( const timespec&);
timespec nanosec2timespec( unsigned long long);
timespec read_timespec( const string&);
timespec now();
timespec operator+( const timespec&, const timespec&);
timespec operator+=( timespec&, const timespec&);
timespec operator-( const timespec&, const timespec&);
timespec operator-=( timespec&, const timespec&);
timespec operator*( const timespec&, const double);
timespec operator*=( timespec&, const double);
timespec operator/( const timespec&, const double);
timespec operator/=( timespec&, const double);
bool operator>( const timespec&, const timespec&);
bool operator<( const timespec&, const timespec&);
bool operator>=( const timespec&, const timespec&);
bool operator<=( const timespec&, const timespec&);
bool operator==( const timespec&, const timespec&);
bool operator!=( const timespec&, const timespec&);

void nanosleep( const timespec&);

#endif
