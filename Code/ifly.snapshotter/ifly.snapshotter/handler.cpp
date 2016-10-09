#include "client.h"

std::string itos_h(int n) {
	char numstr[21];
	std::string str(_itoa(n, numstr, 10));
	return str;
}

void IflyRenderHandler::OnPlaybackStarted() {
	SetAllowPainting(m_outputType == VIDEO);
}

void IflyRenderHandler::OnPlaybackFinished() {
	SetAllowPainting(FALSE);

	if (m_outputType == VIDEO) {
		m_video.Release();
	}
}

void IflyRenderHandler::OnSlideAnimationFinished() {
	if (m_outputType == IMAGE || m_outputType == PDF) {
		SetAllowPainting(TRUE);
	}
}

void IflyRenderHandler::SetAllowPainting(bool value) {
	m_allowPainting = value;
}

bool IflyRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
	int height = m_width / 5 * 3;
    rect = CefRect(0, 0, m_width, height);

    return true;
}

void IflyRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {
	if (m_allowPainting && type == PET_VIEW) {
		std::string f = m_outputFile.ToString();

		if (m_outputType == PDF && m_currentSlide != m_totalSlides) {
			int sep_index = f.find_last_of("\\");
			std::string pdfSlidePrefix = itos_h(m_currentSlide) + "_part_";

			if (sep_index > 0) {
				f = f.substr(0, sep_index + 1) + pdfSlidePrefix + f.substr(sep_index + 1);
			} else {
				f = pdfSlidePrefix + f;
			}
		}

		if (m_outputType == VIDEO) {
			m_video.Write(width, height, buffer, f, m_audioTrackFile);
		} else {
			IflyImage image;
			
			image.Write(width, height, buffer, f);

			SetAllowPainting(FALSE);

			if (m_outputType == PDF) {
				m_currentSlide++;

				if (m_currentSlide > m_totalSlides) {
					CefQuitMessageLoop();
				}
			} else {
				CefQuitMessageLoop();
			}
		}
	} 
}