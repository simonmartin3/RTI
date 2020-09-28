//---------------------------------
// libraryServer.h
//---------------------------------
// MARTIN Simon
//---------------------------------


#ifndef LIBRARYSERVER

#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

#define LIBRARYSERVER

class libraryServer {

	private :
		struct CMMP 
		{
			int type;
			char parametres[100];
		};

	public :
		void setCMMP(int t, char msg)
		{
			type = t;
			parametres = msg;
		}

		CMMP getCMMP(){return CMMP;}
};
#endif
