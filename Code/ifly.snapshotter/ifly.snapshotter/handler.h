#pragma once

#include "image.h"
#include "video.h"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include "include/cef_process_message.h"
#include "include/wrapper/cef_message_router.h"

enum IflyRenderOutputType {
	IMAGE,
	VIDEO,
	PDF
};

class IflyRenderHandler : public CefRenderHandler {
public:
	IflyRenderHandler::IflyRenderHandler(IflyRenderOutputType outputType, CefString outputFile, int width, int totalSlides, CefString audioTrackFile)
		: m_outputType(outputType), m_outputFile(outputFile), m_width(width), m_totalSlides(totalSlides), m_currentSlide(1), m_allowPainting(FALSE), m_audioTrackFile(audioTrackFile) {}

	~IflyRenderHandler() {};

	virtual void SetAllowPainting(bool value);
	virtual void OnPlaybackStarted();
	virtual void OnPlaybackFinished();
	virtual void OnSlideAnimationFinished();

	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE;
	virtual void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) OVERRIDE;
private:
	int m_width;
	int m_totalSlides;
	int m_currentSlide;
	IflyVideo m_video;
	bool m_allowPainting;
	CefString m_outputFile;
	CefString m_audioTrackFile;
	IflyRenderOutputType m_outputType;

    IMPLEMENT_REFCOUNTING(IflyRenderHandler);
};