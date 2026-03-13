#include "pxpch.h"

#include "Test.h"
#include <iostream>

namespace Paradox {

	void Print()
	{
#ifdef PX_DEBUG
		std::cout << "Hello! Debug" << std::endl;
#endif

#ifdef PX_RELEASE
		std::cout << "Hello! Release" << std::endl;
#endif

#ifdef PX_DIST
		std::cout << "Hello! Dist" << std::endl;
#endif
	}

}