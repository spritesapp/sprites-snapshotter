#pragma once

#include <set>
#include <string>
#include <iostream>

#include "handler.h"

class IflyClient : public CefClient, public CefLifeSpanHandler {
	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;

public:
	IflyClient(CefRefPtr<IflyRenderHandler> handler) : m_render_handler(handler) {}
	~IflyClient() {};
	
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return this; }
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() { return (CefRenderHandler*)m_render_handler; }
	virtual void OnQueryForwarded(CefString query);

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;
private:
	CefRefPtr<CefMessageRouterBrowserSide> m_message_router;
	MessageHandlerSet m_message_handler_set;
	CefRefPtr<IflyRenderHandler> m_render_handler;

	IMPLEMENT_REFCOUNTING(IflyClient);
};

class IflyQueryHandler : public CefMessageRouterBrowserSide::Handler {
public:
	IflyQueryHandler(IflyClient* client):m_client(client) {}
	~IflyQueryHandler() {};

	virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) OVERRIDE;
private:
	CefRefPtr<IflyClient> m_client;
	IMPLEMENT_REFCOUNTING(IflyQueryHandler);
};