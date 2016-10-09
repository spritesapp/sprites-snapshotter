#include "renderer.h"

void IflyRenderDelegate::OnWebKitInitialized(CefRefPtr<IflyApp> app) {
	CefMessageRouterConfig config;
	message_router_ = CefMessageRouterRendererSide::Create(config);
}

void IflyRenderDelegate::OnContextCreated(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
	message_router_->OnContextCreated(browser,  frame, context);
}

void IflyRenderDelegate::OnContextReleased(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
	message_router_->OnContextReleased(browser,  frame, context);
}

bool IflyRenderDelegate::OnProcessMessageReceived(CefRefPtr<IflyApp> app, CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
	return message_router_->OnProcessMessageReceived(browser, source_process, message);
}
