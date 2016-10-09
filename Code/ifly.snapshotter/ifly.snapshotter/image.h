#pragma once
#include <string>
#include <algorithm>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;

class IflyImage {
public:
	IflyImage() {}
	void Write(int width, int height, const void* buffer, std::string outputFile);
};