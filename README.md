# SanSheng-Riverside-Interactive

## Platform

Openframeworks 0.10.0 (Windows 10, VS 2017)

## Addons

* ofxKinectForWindows2 [https://github.com/elliotwoods/ofxKinectForWindows2]
	
1. Make your project with Project Generator, including the ofxKinectForWindows2 addon, and open in IDE (Visual Studio 2017)
	
2. In `Property Manager` (open it from `View` -> `Other Windows` -> `Property Manager`), right click on your project to select Add Existing Property Sheet... and select the `ofxKinectForWindows2.props` file located in `ofxKinectForWindows2` master folder

* ofxHPVPlayer [https://github.com/vjacob/ofxHPVPlayer]

	- for oF ver 0.10.0, use the following func in `main.cpp` to set OpenGL ver. 4.1

```C++
int main( )
{
    ofGLWindowSettings settings;
    settings.setSize(1024, 512);
    settings.windowMode = OF_WINDOW;
    settings.setGLVersion(4, 1);

    ofCreateWindow(settings);
	
    ofRunApp(new ofApp());
}
```
	
(To be adapted)
* ofxPostProcessing [https://github.com/neilmendoza/ofxPostProcessing/tree/stable]

##
