#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "include/cef_app.h"

class IflyApp : public CefApp, public CefBrowserProcessHandler, public CefRenderProcessHandler {
public:
	class RenderDelegate : public virtual CefBase {
	public:
		virtual CefRefPtr<CefLoadHandler> GetLoadHandler(CefRefPtr<IflyApp> app) { return NULL; }
		virtual void OnRenderThreadCreated(CefRefPtr<IflyApp> app, CefRefPtr<CefListValue> extra_info) {}
		virtual void OnWebKitInitialized(CefRefPtr<IflyApp> app) {}
		virtual void OnBrowserCreated(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser) {}
		virtual void OnBrowserDestroyed(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser) {}
		virtual bool OnBeforeNavigation(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, cef_navigation_type_t navigation_type, bool is_redirect) { return false; }
		virtual void OnContextCreated(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {}
		virtual void OnContextReleased(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {}
		virtual void OnUncaughtException(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace) {}
		virtual bool OnProcessMessageReceived(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) { return false; }
	};

	typedef std::set<CefRefPtr<RenderDelegate> > RenderDelegateSet;

	IflyApp() {}

	virtual void OnContextInitialized() OVERRIDE;
	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE { return this; }

	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
	virtual void OnWebKitInitialized() OVERRIDE;
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
	virtual bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, NavigationType navigation_type, bool is_redirect) OVERRIDE;
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;
  
	RenderDelegateSet render_delegates_;
private:
	IMPLEMENT_REFCOUNTING(IflyApp);
};