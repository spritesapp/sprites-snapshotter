#include "audio.h"

std::string itos_a(long n) {
	char numstr[21];
	std::string str(_itoa(n, numstr, 10));
	return str;
}

std::string exec(std::string cmd) {
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

std::string add_timestamp(std::string file, int extra) {
	int sep_index = file.find_last_of("\\");

	clock_t t = clock();
	
	if (sep_index < 0) {
		return itos_a(t) + "_" + itos_a(extra) + "_" + file;
	} else {
		return file.substr(0, sep_index + 1) + itos_a(t) + "_" + 
			itos_a(extra) + "_" + file.substr(sep_index + 1);
	}
}

int get_duration(std::string file) {
	std::string output = exec("ffprobe -show_entries format=duration -i \"" + file + "\" | findstr /G [0-9]");

	if (output.length() <= 0) {
		return 0;
	}

	int start_index = output.find("="), end_index = output.find(".");

	if (start_index <= 0 || start_index == (int)(output.length() - 1)) {
		return 0;
	}

	return atoi((end_index > 0 ? output.substr(start_index + 1, end_index - start_index) : output.substr(start_index	+ 1)).c_str());
}

void IflyAudio::Mix(std::string videoFile, std::string audioFile, std::string outputFile) {
	if (std::ifstream(audioFile.c_str())) {
		int fade_out = 5, video_duration = get_duration(videoFile), audio_duration = get_duration(audioFile);

		std::string new_audio_file = audioFile;
		std::string new_video_file = videoFile;

		if (audio_duration > video_duration) {
			audio_duration = video_duration;
			new_audio_file = add_timestamp(audioFile, 0);

			exec("ffmpeg -i \"" + audioFile + "\" -t " + itos_a(audio_duration) + " -y \"" + new_audio_file + "\"");

			remove(audioFile.c_str());
		}

		audioFile = new_audio_file;
		new_audio_file = add_timestamp(audioFile, 1);

		exec("ffmpeg -i \"" + audioFile + "\" -af \"afade=t=out:st=" + itos_a(audio_duration - 5) + ":d=5\" -y \"" + new_audio_file + "\"");
		remove(audioFile.c_str());

		new_video_file = add_timestamp(videoFile, 0);

		exec("ffmpeg -i \"" + videoFile + "\" -i \"" + new_audio_file + 
			"\" -map 0:0 -map 1:0 -codec copy -shortest -y \"" + new_video_file + "\"");

		remove(videoFile.c_str());
		remove(new_audio_file.c_str());
		rename(new_video_file.c_str(), outputFile.c_str());
	}
}