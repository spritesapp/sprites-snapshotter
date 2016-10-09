Sprites Snapshotter
===================

Sprites Snashotter is a command-line utility used to take image/video snapshots of Sprites presentations. The program uses [Chromium Embedded Framework (CEF)](https://bitbucket.org/chromiumembedded/cef) for loading the page and capturing render buffer.

General-Purpose Use
===================

The snapshotter is certainly capable of capturing output of an arbitrary page. Take a look at the description below and I'm sure you'll be able to adopt the implementation. If you have any questions, don't hesitate to send me an email to [volpav@gmail.com](mailto:volpav@gmail.com).

Setup
=====

First of all, you'll need Visual C++ 2010 Express for building some of the components (like, CEF). You won't need IDE itself, only the build tools. You can download Visual C++ 2010 from here: http://www.visualstudio.com/downloads/download-visual-studio-vs.

- Download and install CEF: http://www.magpcss.net/cef_downloads/. Make sure you choose CEF3 for x86. You don't need to build the project, just take everything from `\Release` folder and copy it to `C:\Program Files (x86)\CEF`.
- Download and install Windows SDK ([Windows 8](http://msdn.microsoft.com/en-us/windows/hh852363.aspx), [Windows 8.1](http://msdn.microsoft.com/en-us/windows/bg162891.aspx)).
- Download and install FFmpeg: http://ffmpeg.zeranoe.com/builds/win32/dev/ffmpeg-20140818-git-3c19744-win32-dev.7z. Unzip it to C:\Program Files (x86)\FFmpeg and add the path to the "bin" folder into the "PATH" environment variable.
- ~~Download and install OpenCV: http://opencv.org/downloads.html. It's recommended to extract the package contents under `C:\Program Files (x86)\OpenCV`. After this is done, set OpenCV environment variables as outlined here: [Setting OpenCV environment variables](http://docs.opencv.org/doc/tutorials/introduction/windows_install/windows_install.html#set-the-opencv-enviroment-variable-and-add-it-to-the-systems-path). Make sure your variables point to x86 version!~~
- Download and install x264vfw: http://sourceforge.net/projects/x264vfw/. After this is done, configure x264vfw (just type "x264vfw" when you're at the start screen to quickly find the executable). set following settings:
 - Tuning: "Film"
 - Check "Zero latency"
 - Ratefactor: 1.0
- Clone this project, open the .sln file in Visual Studio (make sure to run it as Administrator) and build.

Command-Line Arguments
======================

There's a number of command-line arguments that Sprites Snapshotter expects (arguments are passed in a format `--[argument name]=[argument value]`):

- `outputFile` - physical path to the output file. For image-based snapshots specify the path to .jpg file and for video-based use .avi extension (note that file extension is required - otherwise, OpenCV will be upset and won't generate any files).
- `presentationId` - Presentation Id.
- `width` - Viewport width, in pixels (height is computed automatically based on 5x3 ratio).
- `slide` - zero-based index of the target slide for cases when exporting to an image (video export ignores this parameter).

Example:

```
ifly.snapshotter.exe 
    --presentationId=2465 
    --slide=2 
    --width=1024 
    --outputFile="C:\Apps\Ifly\screenshot.jpg"
````

The snapshotter supports three output formats:

- Image (use `.jpg` file extension);
- Video (use `.avi` file extension. During post-processing you'll get `.mpg` file).
- PDF file (use `.pdf` file extension. Note that the snapshotter itself won't generate PDF file - it will simply output a set of images that should correspond to multiple pages in a resulting PDF file and it's your responsibility to compile them. Sprites Export Service is capable of doing it using [iTextSharp](https://sourceforge.net/projects/itextsharp/)).

Distribution
============

You need to distribute the contents of `out\Release` folder. Please note that you'll probably need to install [Visual C++ Redistributable for Visual Studio 2012 Update 4](http://www.microsoft.com/en-us/download/details.aspx?id=30679) (or in whichever version of Visual Studio you've built the project). 

Communication with the Page
===========================

The snapshotter will pass a few query-string parameters when loading a page and can be communicated with via a simple JavaScript API call. Here are the query-string parameters:

- `_cef` - when set to "1", this is how you know that your page is being loaded by snapshotter.
- `_cef:progress` - inicates whether to enable progress indicator for the presentation. It's always set to "0" (disable).
- `_cef:animation` - indicates whether to enable animations/transitions ("1" - enable, "0" - disable).
- `_cef:slide` - 1-based index of a slide to render.
- `_cef:fastforward` - indicates whether to show each slide for just a couple of seconds (enough to capture output buffer).

For communicating back with the snapshotter (e.g. to tell it when to start/stop capturing the output), make a call to `cefQuery` method which is available on the `window` object (it's only there when `_cef` query-string parameter is set to "1"). For example (here we use `window.top` in case of making a call from a frame):

```javascript
window.top.cefQuery({
    request: 'Ifly.Cef.PlaybackStarted',
    onSuccess: function() { },
    onFailure: function() { }
});
```

The code above notifies the snapshotter that the content of interest is about to start appearing and so the output buffer needs to be captured from this point on. An actual example from Sprites can be found in ["CEFAdapter:transmit" method](https://github.com/volpav/sprites/blob/master/Code/Ifly.Web.Editor/Assets/js/Models/Embed/Player.ts#L724) file (you can also search the project for "CEFAdapter").

Here's the list of all the available requests to use with `cefQuery` method:

- `Ifly.Cef.PlaybackStarted` - starts the recording.
- `Ifly.Cef.PlaybackFinished` - stops the recording and finalizes the output (e.g. encodes the video file).
- `Ifly.Cef.SlideAnimationFinished` - notifies the snapshotter that the current slide has finished animating (is used during PDF/image exports).

License
=======

The source code is available under [GPL v3](https://www.gnu.org/licenses/gpl-3.0.en.html) license.
