#include "client.h"

void IflyClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	if (!m_message_router) {
		CefMessageRouterConfig config;

		m_message_router = CefMessageRouterBrowserSide::Create(config);
		m_message_handler_set.insert(new IflyQueryHandler(this));

		MessageHandlerSet::const_iterator it = m_message_handler_set.begin();
		for (; it != m_message_handler_set.end(); ++it)
			m_message_router->AddHandler(*(it), false);
	}
}

void IflyClient::OnQueryForwarded(CefString query) {
	if (query == "Ifly.PlaybackStarted") {
		m_render_handler->OnPlaybackStarted();
	} else if (query == "Ifly.SlideAnimationFinished") {
		m_render_handler->OnSlideAnimationFinished();
	} else if (query == "Ifly.PlaybackFinished") {
		m_render_handler->OnPlaybackFinished();

		CefQuitMessageLoop();
	} 
}

bool IflyClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
	return m_message_router->OnProcessMessageReceived(browser, source_process, message);
}

bool IflyQueryHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
	const std::string& message_name = request;

	if (message_name.find("Ifly.Cef.") == 0) {
		if (message_name.find("Ifly.Cef.PlaybackStarted") == 0) {
			m_client->OnQueryForwarded("Ifly.PlaybackStarted");
		} else if (message_name.find("Ifly.Cef.SlideAnimationFinished") == 0) {
			m_client->OnQueryForwarded("Ifly.SlideAnimationFinished");
		} else if (message_name.find("Ifly.Cef.PlaybackFinished") == 0) {
			m_client->OnQueryForwarded("Ifly.PlaybackFinished");
		}

		callback->Success("ok");

		return true;
	}

	return false;
}