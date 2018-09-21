## 製作日程總表
https://docs.google.com/spreadsheets/d/13Cj-VZlZ8MZo7MhLsoFPpzRJiK4N8sjay6_cyU0qCDs/edit#gid=403743726

## Platform

Openframeworks 0.10.0 (Windows 10, VS 2017)

## Addons

### Download
All Addons + HPV Sample Videos [https://drive.google.com/drive/folders/1Zdq8oVpw829NrxYX8vIKh-zvHiIY4i0p?usp=sharing]

* ofxNetwork
* ofxGUI
* ofxJSON
* ofxKinectForWindows2
* ofxHPVPlayer

### Install Details

#### ofxKinectForWindows2 [https://github.com/elliotwoods/ofxKinectForWindows2]

1. Make your project with Project Generator, including the ofxKinectForWindows2 addon, and open in IDE (Visual Studio 2017)

2. In `Property Manager` (open it from `View` -> `Other Windows` -> `Property Manager`), right click on your project to select Add Existing Property Sheet... and select the `ofxKinectForWindows2.props` file located in `ofxKinectForWindows2` master folder

#### ofxHPVPlayer [https://github.com/vjacob/ofxHPVPlayer]

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


#### ofxPostProcessing [https://github.com/neilmendoza/ofxPostProcessing/tree/stable]

* (To be adapted, seems not working on 0.10.0)


##
