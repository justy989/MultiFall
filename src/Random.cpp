#include "Random.h"

Random::Random()
{
	m_iF = 0;
	m_iS = 0;
}

Random::~Random()
{

}

void Random::seed(unsigned int seed)
{
	m_iF = seed;
	m_iS = seed;
}

unsigned int Random::gen(unsigned int min, unsigned int max)
{
	if(max == min)
		return min;

	//Simple RNG by George Marsaglia
	m_iF = 36969 * ( m_iF & 65535 ) + ( m_iF >> 16 );
	m_iS = 18000 * ( m_iS & 65535 ) + ( m_iS >> 16 );
     
	return ( ( ( m_iF << 16 ) + m_iS ) % ( max - min ) ) + min;
}

float Random::genNorm()
{
	m_iF = 36969 * ( m_iF & 65535 ) + ( m_iF >> 16 );
	m_iS = 18000 * ( m_iS & 65535 ) + ( m_iS >> 16 );

	return (float)( ( ( m_iF << 16 ) + m_iS ) % 1000000 ) / (1000000.0f); 
}