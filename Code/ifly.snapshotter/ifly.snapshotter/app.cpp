#include <string>
#include <eh.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "include/cef_app.h"
#include "include/cef_client.h"

#include "app.h"
#include "util.h"
#include "client.h"
#include "handler.h"
#include "renderer.h"

using namespace std;

std::string itos(int n) {
	char numstr[21];
	std::string str(_itoa(n, numstr, 10));
	return str;
}

void IflyApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("off-screen-rendering-enabled");
	command_line->AppendSwitchWithValue("off-screen-frame-rate", "60");
	command_line->AppendSwitch("disable-gpu");
	command_line->AppendSwitch("disable-gpu-compositing");
	command_line->AppendSwitch("enable-begin-frame-scheduling");
	command_line->AppendSwitch("enable-media-stream");
}

void IflyApp::OnContextInitialized() {
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();

	IflyRenderOutputType outputType = IMAGE;
	string outputFile = command_line->GetSwitchValue("outputFile").ToString();
	string audioFile = command_line->GetSwitchValue("audioFile").ToString();
	int slide = atoi(command_line->GetSwitchValue("slide").ToString().c_str());
	int width = atoi(command_line->GetSwitchValue("width").ToString().c_str());
	int presentationId = atoi(command_line->GetSwitchValue("presentationId").ToString().c_str());
	int totalSlides = atoi(command_line->GetSwitchValue("totalSlides").ToString().c_str());

	if (outputFile.find(".png") != string::npos || outputFile.find(".jpg") != string::npos) {
		outputType = IMAGE;
	} else if (outputFile.find(".pdf") != string::npos) {
		outputType = PDF;
	} else {
		outputType = VIDEO;
	}

	if (command_line->GetSwitchValue("debug").ToString() == "video") {
		outputFile = "C:\\Apps\\Ifly\\test-video.mp4";
		audioFile = "";
		presentationId = 9090;
		outputType = VIDEO;
		width = 1600;
	}

	string url("https://spritesapp.com/view/embed/" + itos(presentationId) + "?_cef=1&_cef:progress=0" + 
		(outputType == IMAGE || outputType == PDF ? ("&_cef:animation=0" + (outputType == IMAGE ? "&_cef:slide=" + itos(slide) : "&_cef:fastforward=1")) : ""));

    CefRefPtr<IflyClient> browser_client;
	
	CefWindowInfo window_info;
    CefBrowserSettings browser_settings;
	
	browser_settings.windowless_frame_rate = 60;
	
	window_info.windowless_rendering_enabled = true;
	window_info.transparent_painting_enabled = true;

    browser_client = new IflyClient(new IflyRenderHandler(outputType, outputFile, width, totalSlides, audioFile));
    CefBrowserHost::CreateBrowser(window_info, browser_client.get(), url, browser_settings, nullptr);
}

void IflyApp::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {
	render_delegates_.insert(new IflyRenderDelegate);

	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it)
		(*it)->OnRenderThreadCreated(this, extra_info);
}

void IflyApp::OnWebKitInitialized() {
	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it)
		(*it)->OnWebKitInitialized(this);
}

void IflyApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it)
		(*it)->OnBrowserCreated(this, browser);
}

void IflyApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it)
		(*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> IflyApp::GetLoadHandler() {
	CefRefPtr<CefLoadHandler> load_handler;
	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end() && !load_handler.get(); ++it)
		load_handler = (*it)->GetLoadHandler(this);

  return load_handler;
}

bool IflyApp::OnBeforeNavigation(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, NavigationType navigation_type, bool is_redirect) {
	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it) {
		if ((*it)->OnBeforeNavigation(this, browser, frame, request, navigation_type, is_redirect)) {
			return true;
		}
	}

	return false;
}

void IflyApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it)
		(*it)->OnContextCreated(this, browser, frame, context);
}

void IflyApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it)
		(*it)->OnContextReleased(this, browser, frame, context);
}

void IflyApp::OnUncaughtException(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception, CefRefPtr<CefV8StackTrace> stackTrace) {
	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end(); ++it) {
		(*it)->OnUncaughtException(this, browser, frame, context, exception, stackTrace);
	}
}

bool IflyApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
	ASSERT(source_process == PID_BROWSER);

	bool handled = false;

	RenderDelegateSet::iterator it = render_delegates_.begin();
	for (; it != render_delegates_.end() && !handled; ++it) {
		handled = (*it)->OnProcessMessageReceived(this, browser, source_process, message);

		if (handled) {
			break;
		}
	}

	return handled;
}