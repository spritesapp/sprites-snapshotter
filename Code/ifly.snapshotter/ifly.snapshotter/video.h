#pragma once
#include <string>

class IflyVideo {

public:
	IflyVideo():m_writerInitialized(false) {}

	void Write(int width, int height, const void* buffer, std::string outputFile, std::string audioFile);
	void Release();
public:
	FILE* m_outputFile;
	std::string m_audioFile;
	std::string m_videoFile;
	std::string m_finalOutputFile;
	bool m_writerInitialized;
	int m_width;
	int m_height;
};