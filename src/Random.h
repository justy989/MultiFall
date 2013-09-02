#ifndef RANDOM_H
#define RANDOM_H

/*Random - used to generate random values*/
class Random
{
public:

	Random();
	~Random();

	//seed the generator
	void seed(unsigned int seed);

	//generate value within the specified range
	unsigned int gen(unsigned int min, unsigned int max);

	//Generates a value between 0.0 and 1.0
	float genNorm();

protected:

	//LOL not too sure, just part of the func i found online :D
	unsigned int m_iF;
	unsigned int m_iS;
};

#endif