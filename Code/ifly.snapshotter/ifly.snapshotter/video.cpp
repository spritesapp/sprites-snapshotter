#include "video.h"
#include "audio.h"

std::string itos_v(int n) {
	char numstr[21];
	std::string str(_itoa(n, numstr, 10));
	return str;
}

std::string exec_v(std::string cmd) {
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    _pclose(pipe);
    return result;
}

std::string add_timestamp_v(std::string file, int extra) {
	int sep_index = file.find_last_of("\\");

	clock_t t = clock();
	
	if (sep_index < 0) {
		return itos_v(t) + "_" + itos_v(extra) + "_" + file;
	} else {
		return file.substr(0, sep_index + 1) + itos_v(t) + "_" + 
			itos_v(extra) + "_" + file.substr(sep_index + 1);
	}
}

void IflyVideo::Write(int width, int height, const void* buffer, std::string outputFile, std::string audioFile) {
	std::string cmd;

	if (!m_writerInitialized) {
		m_width = width;
		m_height = height;
		m_videoFile = add_timestamp_v(outputFile, 0);
		m_audioFile = audioFile;
		m_finalOutputFile = outputFile;

		cmd = "ffmpeg -f rawvideo -vcodec rawvideo  -s " + itos_v(width) + "x" + itos_v(height) + " -r 60 -pix_fmt rgb32 -i - -c:v libx264 -preset ultrafast -qp 0 -b:v 1M -r 60 -y \"" + m_videoFile + "\""; 

		/*cmd = "ffmpeg -r 60 -f rawvideo -vcodec rawvideo -pix_fmt rgb32 -s " + itos_v(width) + "x" + itos_v(height) + " -i - " + 
			  "-threads 0 -q:v 1 -y \"" + m_videoFile + "\"";*/

		m_outputFile = _popen(cmd.c_str(), "wb");

		m_writerInitialized = true;
	}

	fwrite(buffer, sizeof(int)*width*height, 1, m_outputFile);
}

void IflyVideo::Release() {
	std::string cmd;
	std::string m_reencodedFile;

	if (m_outputFile) {
		_pclose(m_outputFile);
	}
	
	if (m_width < 1600) {
		m_reencodedFile = add_timestamp_v(m_videoFile, 1);

		cmd = "ffmpeg -i \"" + m_videoFile + "\" -c:v libx264 -pix_fmt yuv420p \"" + m_reencodedFile + "\"";

		exec_v(cmd.c_str());
		remove(m_videoFile.c_str());

		m_videoFile = m_reencodedFile;
	}

	if (m_audioFile.length() > 0) {
		IflyAudio audio;

		audio.Mix(m_videoFile, m_audioFile, m_finalOutputFile);
	} else {
		rename(m_videoFile.c_str(), m_finalOutputFile.c_str());
	}
}