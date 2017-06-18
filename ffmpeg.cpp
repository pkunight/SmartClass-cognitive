#include "ffmpeg.h"

ffmpeg* ffmpeg::instance = new ffmpeg();

ffmpeg* ffmpeg::getInstance(){
    return ffmpeg::instance;
}

ffmpeg::ffmpeg(){
    ffmpegDir = "C:/QtProjects/SmartClass-qt5.6/ffmpeg-bin/";
    tmpImageFileName = "tmp.jpg";
}

void ffmpeg::setFFMPEGDir(string dir){
    ffmpegDir = dir;
}

string ffmpeg::getFFMPEGDir(){
    return ffmpegDir;
}

void ffmpeg::setTmpImageFileName(string filename){
    tmpImageFileName = filename;
}

string ffmpeg::getTmpImageFileName(){
    return tmpImageFileName;
}

bool ffmpeg::executePackage(LPCWSTR fileName, LPCWSTR args, bool wait)
{
    SHELLEXECUTEINFOW sei = { sizeof(SHELLEXECUTEINFOW) };

    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;

    sei.lpFile = fileName;
    sei.lpParameters = args;

    if (!ShellExecuteExW(&sei)) {
        return false;
    }

    if (wait) {
        HANDLE hProcess = sei.hProcess;
        if (hProcess != 0) {
            WaitForSingleObject(hProcess, INFINITE);
            CloseHandle(hProcess);
        }
    }

    return true;
}

LPCWSTR ffmpeg::stringToLPCWSTR(std::string orig)
{
    size_t origsize = orig.length() + 1;
    size_t convertedChars = 0;
    wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
    return wcstring;
}

bool ffmpeg::saveRTMPtoImageFile(string rtmpUrl) {
    string exeFile = ffmpegDir + "ffmpeg.exe";
    LPCWSTR lp_exeFile = stringToLPCWSTR(exeFile);
    string execPara = "-i \"" + rtmpUrl + "\" -y -f image2 -vframes 1 " + ffmpegDir + tmpImageFileName;
    LPCWSTR lp_execPara = stringToLPCWSTR(execPara);
    return executePackage(lp_exeFile, lp_execPara, true);
}
