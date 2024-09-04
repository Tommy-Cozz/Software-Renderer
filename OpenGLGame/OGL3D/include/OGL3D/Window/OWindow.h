#pragma once
#include <OGL3D/Math/ORect.h>




class OWindow                         //Now if we want to render object wihtin window we need to use the ziue of the inner widnow to retrive an object with the inner area of the window
{

public:
	OWindow();
	~OWindow();

	ORect getInnerSize();

	void makeCurrentContext();
	void present(bool vsync);

private:
	void* m_handle = nullptr;
	void* m_context = nullptr;
};

