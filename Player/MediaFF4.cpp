#include "stdafx.h"
#include "MediaFF4.h"

using namespace std;

int MediaFF4::Open(wstring path)
{
    size_t size;
    char szBuf[MAX_PATH*2];
    wcstombs_s(&size, szBuf, path.c_str(), sizeof(szBuf));

    AVFormatContext* pFmtCtx = NULL;
    if (avformat_open_input(&pFmtCtx, szBuf, NULL, NULL) != 0) {
        m_wstrError = L"打开文件失败";
        return -1;
    };

    if (avformat_find_stream_info(pFmtCtx, NULL) < 0) {
        m_wstrError = L"检索媒体信息失败";
        return -2;
    }

    // 找到视频流
    int nVideoIdx = -1;
    AVStream* pVideo = NULL;
    for (int i = 0; i < static_cast<int>(pFmtCtx->nb_streams); i++) {
        if (pFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            nVideoIdx = i;
            pVideo = pFmtCtx->streams[i];
            break;
        }
    }

    if (pVideo == NULL) {
        m_wstrError = L"没有找到视频流";
        return -3;
    }

    AVCodec* pCodec = NULL;
    pCodec = avcodec_find_decoder(pVideo->codecpar->codec_id);
    if (pCodec == NULL) {
        m_wstrError = L"不支持的视频";
        return -4;
    }

    // 复制编解码器环境
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, pVideo->codecpar) != 0) {
        m_wstrError = L"分配解码器失败";
        return -5;
    }

    // 打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        m_wstrError = L"打开解码器失败";
        return -6;
    }

    // read frame
    readFrames(pFmtCtx, pCodecCtx, nVideoIdx);

    // free
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFmtCtx);

    return 0;
}

void MediaFF4::readFrames(AVFormatContext *pFmtCtx, AVCodecContext *pCodecCtx, int nVideoIdx)
{
    // 准备缓冲区
    AVFrame* pFrame = av_frame_alloc();
    AVFrame* pFrameRGB = av_frame_alloc();

    int align = 4; // windows要求每行4字节对齐
    int width = pCodecCtx->width / 2, height = pCodecCtx->height / 2; // scale to 1/2
    int size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, width, height, align);
    uint8_t* pBuf = static_cast<uint8_t*>(av_malloc(size));
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, pBuf, AV_PIX_FMT_BGR24, width, height, align);

    // 准备拷贝帧图像
    SwsContext* pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        width, height, AV_PIX_FMT_BGR24,
        SWS_FAST_BILINEAR, NULL, NULL, NULL);

    int nRet = 0, i = 0;
    AVPacket packet;
    while (av_read_frame(pFmtCtx, &packet) >= 0) {
        if (packet.stream_index == nVideoIdx) {
            // 只处理视频信息
            do {
                nRet = avcodec_send_packet(pCodecCtx, &packet);
                nRet = avcodec_receive_frame(pCodecCtx, pFrame);
            } while (nRet == AVERROR(EAGAIN));
            
            //if (i == 500) {
                sws_scale(pSwsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                render(pFrameRGB, width, height);
            //    break;
            //}
            i++;
        }
        av_packet_unref(&packet);
    }

    av_free(pBuf);
    av_frame_free(&pFrameRGB);
    av_frame_free(&pFrame);
}

void MediaFF4::render(AVFrame* pFrame, int width, int height)
{
    HDC hdc = GetDC(m_hwnd);

    // pixel data
    /*for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t *pBuf = pFrame->data[0] + y * pFrame->linesize[0] + x * 3;
            SetPixel(hdc, x, y, RGB(pBuf[0], pBuf[1], pBuf[2]));
        }
    }*/

    BITMAPINFO bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 8 * 3; // 每像素3字节
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    BYTE* pBuf = NULL;
    bmpInfo.bmiHeader.biHeight *= -1; // BMP 方向调整
    HBITMAP hBmp = CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS, (void**)&pBuf, NULL, 0);
    bmpInfo.bmiHeader.biHeight *= -1;
    int size = pFrame->linesize[0] * height;
    memcpy_s(pBuf, size, pFrame->data[0], size);

    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectBitmap(hdcMem, hBmp);
    BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

    DeleteDC(hdcMem);
    DeleteBitmap(hBmp);
    ReleaseDC(m_hwnd, hdc);
}