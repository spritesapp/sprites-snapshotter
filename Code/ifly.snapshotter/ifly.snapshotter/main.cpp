#include "app.h"

int main(int argc, char *argv[]) {
	CefMainArgs args;
	void* sandbox_info = NULL;

	CefRefPtr<IflyApp> app(new IflyApp);

	int exit_code = CefExecuteProcess(args, app.get(), sandbox_info);
	if (exit_code >= 0) {
		return exit_code;
	}
	
	CefSettings settings;
	
	settings.no_sandbox = true;
	settings.windowless_rendering_enabled = true;
	settings.ignore_certificate_errors = true;
	
	CefInitialize(args, settings, app.get(), sandbox_info);

	CefRunMessageLoop();

	CefShutdown();
	
	return 0;
}