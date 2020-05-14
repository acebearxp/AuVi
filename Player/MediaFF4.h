/* Processing the media content (video and audio) via FFmpeg 4.2.2 */

#pragma once

// ffmpeg 4.2.2
extern "C" {
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

using namespace std;

class MediaFF4
{
public:
	// open the media file, return 0 if succeeded
	int Open(wstring path);
	// last error
	const wstring& GetLastError() { return m_wstrError; }
	// set target
	void SetTarget(HWND hwnd) { m_hwnd = hwnd; }
private:
	wstring m_wstrError;
	HWND m_hwnd;

	// read some frames
	void readFrames(AVFormatContext* pFmtCtx, AVCodecContext* pCodecCtx, int nVideoIdx);
	void render(AVFrame* pFrame, int width, int height);
};

