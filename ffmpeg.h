#ifndef FFMPEG_H
#define FFMPEG_H

#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

class ffmpeg{
    public:
        static ffmpeg* getInstance();

        void setFFMPEGDir(string dir);

        void setTmpImageFileName(string filename);

        void setEmotionUrl(string url);

        string getTmpImageFileName();

        string getFFMPEGDir();

        bool saveRTMPtoImageFile(string rtmpUrl);

    private:
        ffmpeg();

        static ffmpeg *instance;

        string ffmpegDir;
        string tmpImageFileName;

        bool executePackage(LPCWSTR fileName, LPCWSTR args, bool wait);

        LPCWSTR stringToLPCWSTR(std::string orig);
};


#endif // FFMPEG_H
