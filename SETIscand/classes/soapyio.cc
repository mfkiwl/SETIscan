#include "soapyio.h"

SoapyIO::SoapyIO(QObject *parent) : QObject(parent)
	{
	SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
	SoapySDR::Kwargs::iterator it;

	for( int i = 0; i < results.size(); ++i)
		{
		printf("Found device #%d: ", i);
		for( it = results[i].begin(); it != results[i].end(); ++it)
			{
			printf("%s = %s\n", it->first.c_str(), it->second.c_str());
			}
		printf("\n");
		}
	}
