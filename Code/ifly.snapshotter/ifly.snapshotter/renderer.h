#pragma once

#include <sstream>
#include <string>

#include "app.h"
#include "util.h"

#include "include/cef_base.h"
#include "include/cef_dom.h"
#include "include/wrapper/cef_message_router.h"

class IflyRenderDelegate : public IflyApp::RenderDelegate {
public:
	IflyRenderDelegate() { }

	void OnWebKitInitialized(CefRefPtr<IflyApp> app) OVERRIDE;
	virtual void OnContextCreated(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual void OnContextReleased(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual bool OnProcessMessageReceived(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
	CefRefPtr<CefMessageRouterRendererSide> message_router_;
	IMPLEMENT_REFCOUNTING(IflyRenderDelegate);
};

