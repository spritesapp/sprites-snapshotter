#pragma once
#include <string>
#include <time.h>
#include <fstream>

class IflyAudio {
public:
	IflyAudio() {}

	void Mix(std::string videoFile, std::string audioFile, std::string outputFile);
};