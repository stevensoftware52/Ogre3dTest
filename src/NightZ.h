#ifndef __OgreApp1_h_
#define __OgreApp1_h_

#include "stdafx.h"
#include "BaseApplication.h"
#include "res/resource.h"

class NightZ : public BaseApplication
{
	public:
	    NightZ(void);
	    virtual ~NightZ(void);
	
	protected:
	    virtual void createScene(void);
};

#endif
