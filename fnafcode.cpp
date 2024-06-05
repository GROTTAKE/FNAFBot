/*

LEFT LIGHT
X:20
Y:400
WIDTH:72
HEIGHT:135

LEFT DOOR
X:17
Y:278
WIDTH:72
HEIGHT:129


RIGHT LIGHT
X:1184
Y:412
WIDTH:78
HEIGHT:143


RIGHT DOOR
X:1173
Y:293
WIDTH:99
HEIGHT:135


BONNIE
X:415
Y:200
WIDTH:107
HEIGHT:231


CHICA
X:785
Y:161
WIDTH:129
HEIGHT:373

7
X:1150
Y:406
WIDTH:94
HEIGHT:63


























*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <cmath>

//SLEEP
#include <thread>
#include <chrono>

using namespace cv;
using namespace std;

/////Screen Dimensions
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

//variables
// Path to the target image
std::string targetImageString = "G:\\Projects\\FNAFBOT\\CODE\\ROK.jpg";
int sleepTime = 1;
int shortPause = 10;
int screenshotCounter = 0;

//has Foxy Reached Stage 3
bool foxyThree = false;
int bonnieInt = 0;
bool bonnieBool = false;
int chicaInt = 0;
bool chicaBool = false;
double targetValue = 0.9;

// Set the time duration after which the bonnieTimeMet should become true
const std::chrono::seconds bonnieThresholdDuration(19); // Change this to your desired time
bool bonnieTimeMet = false; // Initial bonnieTimeMet
auto bonnieTime = std::chrono::steady_clock::now(); // Get the start time
auto currentTime = std::chrono::steady_clock::now();

// Set the time duration after which the chicaTimeMet should become true
const std::chrono::seconds chicaThresholdDuration(14); // Change this to your desired time
bool chicaTimeMet = false; // Initial bonnieTimeMet
auto chicaTime = std::chrono::steady_clock::now(); // Get the start time
const int lowerBound = 5010; // 4.8 seconds in milliseconds


void showCursor() {
    while (true) {
        POINT cursorPos;
        if (GetCursorPos(&cursorPos)) {
            std::cout << "Mouse Cursor Position - X: " << cursorPos.x << ", Y: " << cursorPos.y << std::endl;
        }
        else {
            std::cout << "Failed to get the mouse cursor position." << std::endl;
        }
        if (GetAsyncKeyState('H') & 0x8000) {
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            Sleep(500);
            break;
        }
    }
}


/////////////////////MAIN Function to look for the image in question/////////////////////////////////////////
bool targetImageMatches(int x1, int y1, int x2, int y2) {

    //showCursor();
    //Sleep(sleepTime);

	if (GetAsyncKeyState('Z') & 0x8000) {
		showCursor();
	}


	// Load the target image
	Mat targetImage = imread(targetImageString, IMREAD_COLOR);

	// Check if the image was successfully loaded
	if (targetImage.empty()) {
		cout << "Error: Could not read the target image." << endl;
        
		return false;
	}

	// Get the size of the target image
	Size targetSize = targetImage.size();

    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create a screenshot object
    if (!x1) { // this is the NULL check
       x1 = 0;
    }
    if (!y1) { // this is the NULL check
        y1 = 0;
    }
    if (!x2 || x2 == 0) { // this is the NULL check
        
        x2 = screenWidth;
    }
    else if(x2 < 256){
        x2 = 256;
    }
    if (!y2 || y2 == 0) { // this is the NULL check
        
        y2 = screenHeight;
    }
    else if(y2 < 256){
        y2 = 256;
    }

	HWND hwndDesktop = GetDesktopWindow();
	HDC hScreenDC = GetDC(hwndDesktop);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, x2, y2);
	HGDIOBJ hOldBitmap = SelectObject(hMemoryDC, hBitmap);
	BitBlt(hMemoryDC, 0, 0, x2, y2, hScreenDC, x1, y1, SRCCOPY);

	// Convert the screenshot to a Mat object
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = x2;
	bmi.bmiHeader.biHeight = -y2;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	Mat screenshot(y2, x2, CV_8UC3);

	// Capture the screenshot using BitBlt
	if (!BitBlt(hMemoryDC, 0, 0, x2, y2, hScreenDC, x1, y1, SRCCOPY)) {
		cout << "Error: Failed to capture the screen." << endl;
        Sleep(10000);
		// Add any additional error handling code here
	}

	// Get the captured screenshot data
	int captureResult = GetDIBits(hMemoryDC, hBitmap, 0, y2, screenshot.data, &bmi, DIB_PAL_COLORS);
	if (captureResult == 0) {
		cout << captureResult << endl;
		cout << "Error: Failed to retrieve screenshot data." << endl;
        Sleep(10000);
		// Add any additional error handling code here
	}

    /*
	// Display the captured screenshot dimensions
	cout << "Captured screenshot dimensions: " << screenshot.cols << " x " << screenshot.rows << endl;
    cout << "Target image dimensions: " << targetSize.width << " x " << targetSize.height << endl;
    */
	// Release the resources
	SelectObject(hMemoryDC, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(hwndDesktop, hScreenDC);

	
	
	//waitKey(0);

	GetDIBits(hMemoryDC, hBitmap, 0, y2, screenshot.data, &bmi, DIB_PAL_COLORS);

	//DIAGNOSTICS
	
	// Check if the screenshot and target image have the same number of channels
	if (screenshot.channels() != targetImage.channels()) {
		cout << "Error: Screenshot and target image have different number of channels." << endl;
        Sleep(10000);
		//return -1;
	}
	// Check if the target image is smaller than or equal to the screenshot
	if (targetSize.width > screenshot.cols || targetSize.height > screenshot.rows) {
		cout << "Error: Target image is larger than the screenshot." << endl;
        Sleep(10000);
		return false;
	}
	// Check if the depth and type of the input images match
	if (screenshot.depth() != targetImage.depth() || screenshot.type() != targetImage.type()) {
		cout << "screenshot channels:" << screenshot.depth() << endl;
		cout << "targetImage channels: " << targetImage.depth() << endl;
		cout << "Error: Depth and type of input images do not match." << endl;
        Sleep(10000);
		//return -1;
	}


    /*
    //make the screenshot smaller//////////
    int lowerResolutionFactor = 8;  // You can adjust this factor as needed
    resize(screenshot, screenshot, Size(screenWidth / lowerResolutionFactor, screenHeight / lowerResolutionFactor));
    ///////////////////////////////////////
    */

    /*
    //SHOW AND PRINT SCREENSHOT//////////////////////////////////
    //imshow("screenshot", screenshot);
    screenshotCounter++;
    std::string folderPath = "ScreenshotFolder/"; // Add your folder path here
    std::string fileName = folderPath + std::to_string(screenshotCounter) + "screenshot.jpg";
    imwrite(fileName, screenshot);
    /////////////////////////////////////////////////////////////
    */
    
	// Perform template matching
    Mat result;
    if (screenshot.empty()) {
        cout << "Error: NO SCREENSHOT." << endl;
        Sleep(10000);
        return false;
    }
    if (screenshot.cols <= 0 || screenshot.rows <= 0) {
        cout << "Error: Invalid screenshot dimensions." << endl;
        Sleep(1000);
        return false;
    }
	matchTemplate(screenshot, targetImage, result, TM_CCOEFF_NORMED);
    
    double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    result.release();
    screenshot.release();
    targetImage.release();
	// Check if the target image was found
	if (maxVal < targetValue) {
		cout << maxVal << ": Image not found." << endl;
		//return false;
	}
	else {
		cout << maxVal << ": Image successfully found." << endl;
		//return false;
	}

	// Release the resources
	SelectObject(hMemoryDC, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(hwndDesktop, hScreenDC);


	return (maxVal > targetValue);
}




/// <Button Functions>
/// /////////////////////////////////////////////////////////////BUTTONS////////////////////////////////////////////////////////////////////////////////
/// </summary>
void turnOnLeftDoor() {
    int mouseGotoX = 55;
    int mouseGotoY = 343;
    SetCursorPos(10, 200);
    Sleep(sleepTime);
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Left\\Left_Door_Off.png";
        if (targetImageMatches(17, 278, 72, 129) == true) {
            // Move the cursor to the center of the screen and left click
            int centerX = mouseGotoX;
            int centerY = mouseGotoY;
            SetCursorPos(centerX, centerY);
            mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
            //cout << maxVal << endl;
            //break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Left\\Left_Door_On.png";
        if (targetImageMatches(17, 278, 72, 129) == true) {
            break;
        }
    }
    
}

void turnOnLeftLight() {
    int mouseGotoX = 55;
    int mouseGotoY = 455;
    SetCursorPos(10, 200);
    Sleep(sleepTime);
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Left\\Left_Light_Off.png";
        if (targetImageMatches(20, 400, 72, 135) == true) {
            // Move the cursor to the center of the screen and left click
            int centerX = mouseGotoX;
            int centerY = mouseGotoY;
            SetCursorPos(centerX, centerY);
            mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
            //cout << maxVal << endl;
            //break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Left\\Left_Light_On.png";
        if (targetImageMatches(20, 400, 72, 135) == true) {
            break;
        }
    }

}

void turnOnRightDoor() {
    int mouseGotoX = 1216;
    int mouseGotoY = 353;
    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Right\\Right_Door_Off.png";
        if (targetImageMatches(1173, 293, 200, 200) == true) {
            // Move the cursor to the center of the screen and left click
            int centerX = mouseGotoX;
            int centerY = mouseGotoY;
            SetCursorPos(centerX, centerY);
            mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
            //cout << maxVal << endl;
            //break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Right\\Right_Door_On.png";
        if (targetImageMatches(1173, 293, 200, 200) == true) {
            break;
        }
    }

}

void turnOnRightLight() {
    int mouseGotoX = 1216;
    int mouseGotoY = 466;
    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Right\\Right_Light_Off.jpg";
        if (targetImageMatches(1184, 412, 256, 256) == true) {
            // Move the cursor to the center of the screen and left click
            int centerX = mouseGotoX;
            int centerY = mouseGotoY;
            SetCursorPos(centerX, centerY);
            mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
            //cout << maxVal << endl;
            //break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Right\\Right_Light_On.jpg";
        if (targetImageMatches(1184, 412, 256, 256) == true) {
            break;
        }
    }

}

void turnOffLeftDoor() {
    int mouseGotoX = 55;
    int mouseGotoY = 343;
    SetCursorPos(10, 200);
    Sleep(sleepTime);
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Left\\Left_Door_On.png";
        if (targetImageMatches(17, 278, 200, 200) == true) {
            // Move the cursor to the center of the screen and left click
            int centerX = mouseGotoX;
            int centerY = mouseGotoY;
            SetCursorPos(centerX, centerY);
            mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
            //cout << maxVal << endl;
            //break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Left\\Left_Door_Off.png";
        if (targetImageMatches(17, 278, 200, 200) == true) {
            break;
        }
    }

}

void turnOffLeftLight() {
    int mouseGotoX = 55;
    int mouseGotoY = 455;
    SetCursorPos(10, 200);
    Sleep(sleepTime);
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Left\\Left_Light_On.png";
        if (targetImageMatches(20, 400, 72, 135) == true) {
            // Move the cursor to the center of the screen and left click
            int centerX = mouseGotoX;
            int centerY = mouseGotoY;
            SetCursorPos(centerX, centerY);
            mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
            //cout << maxVal << endl;
            //break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Left\\Left_Light_Off.png";
        if (targetImageMatches(20, 400, 72, 135) == true) {
            break;
        }
    }

}

void turnOffRightDoor() {
    int mouseGotoX = 1216;
    int mouseGotoY = 353;
    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Right\\Right_Door_On.png";
        if (targetImageMatches(1173, 293, 200, 200) == true) {
            // Move the cursor to the center of the screen and left click
            int centerX = mouseGotoX;
            int centerY = mouseGotoY;
            SetCursorPos(centerX, centerY);
            mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
            //cout << maxVal << endl;
            //break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Right\\Right_Door_Off.png";
        if (targetImageMatches(1173, 293, 200, 200) == true) {
            break;
        }
    }

}

void turnOffRightLight() {
    int mouseGotoX = 1216;
    int mouseGotoY = 466;
    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Right\\Right_Light_On.jpg";
        if (targetImageMatches(1184, 412, 300, 300) == true) {
            // Move the cursor to the center of the screen and left click
            int centerX = mouseGotoX;
            int centerY = mouseGotoY;
            SetCursorPos(centerX, centerY);
            mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
            //cout << maxVal << endl;
            //break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Right\\Right_Light_Off.jpg";
        if (targetImageMatches(1184, 412, 300, 300) == true) {
            break;
        }
    }

}
///////////////////////////////////////////////////////////////////////////////BUTTONS//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Open Cameras
//scroll right > look for button > cursor up > cursor down > cursor up > look for Cam 7
void openCams() {
    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    int cameraInt = 0;

    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    //cursor up
    SetCursorPos(555, 600);
    Sleep(sleepTime);
    //cursor down
    SetCursorPos(555, 680);
    Sleep(sleepTime);
    //cursor up
    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    //break;

    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Switch\\RoomRight.png";
        if (targetImageMatches(0, 0, 0, 0) == true) {
            SetCursorPos(1270, 200);
            Sleep(sleepTime);
            //cursor up
            SetCursorPos(555, 600);
            Sleep(sleepTime);
            //cursor down
            SetCursorPos(555, 680);
            Sleep(sleepTime);
            //cursor up
            SetCursorPos(1270, 200);
            Sleep(sleepTime);
            //break;
            cameraInt = 0;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\CamButtons\\7.png";
        if (targetImageMatches(1150, 406, 116, 116) == true) {
            break;
        }
        //cameraInt++;
        SetCursorPos(1270, 200);
    }

}

//Close Cameras
void closeCams() {
    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    int cameraInt = 0;

    SetCursorPos(1270, 200);
    Sleep(sleepTime);
    //cursor up
    SetCursorPos(555, 600);
    Sleep(sleepTime);
    //cursor down
    SetCursorPos(555, 680);
    Sleep(sleepTime);
    //cursor up
    SetCursorPos(555, 600);
    Sleep(sleepTime);
    //break;

    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\CamButtons\\7.png";
        if (targetImageMatches(1150, 406, 116, 116) == true) {
            SetCursorPos(1270, 200);
            Sleep(sleepTime);
            //cursor up
            SetCursorPos(555, 600);
            Sleep(sleepTime);
            //cursor down
            SetCursorPos(555, 680);
            Sleep(sleepTime);
            //cursor up
            SetCursorPos(555, 600);
            Sleep(sleepTime);
            //break;
            cameraInt = 0;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Switch\\RoomRight.png";
        if (targetImageMatches(0, 0, 0, 0) == true) {
            break;
        }
        //cameraInt++;
        SetCursorPos(1270, 200);
    }

}

void camera1C() {
    Sleep(sleepTime); 
    int mouseGotoX = 932;
    int mouseGotoY = 490;
    int centerX = mouseGotoX;
    int centerY = mouseGotoY;
    SetCursorPos(centerX, centerY);
    mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
    Sleep(sleepTime);
}

void camera4B() {
    Sleep(sleepTime);
    int mouseGotoX = 1090;
    int mouseGotoY = 646;
    int centerX = mouseGotoX;
    int centerY = mouseGotoY;
    SetCursorPos(centerX, centerY);
    mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
    Sleep(sleepTime);
}

void camera2A() {
    Sleep(sleepTime);
    int mouseGotoX = 983;
    int mouseGotoY = 605;
    int centerX = mouseGotoX;
    int centerY = mouseGotoY;
    SetCursorPos(centerX, centerY);
    mouse_event(MOUSEEVENTF_LEFTDOWN, centerX, centerY, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, centerX, centerY, 0, 0);
    Sleep(sleepTime);
}

void checkBonnie() {
    int counter = 0;
    while (true) {
        SetCursorPos(10, 200);

        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Bonnie\\NoBonnieShadow.png";
        if (targetImageMatches(415, 200, 300, 300) == true) {
            if (bonnieBool == true) {
                bonnieInt = 0;
                bonnieBool = false;

                bonnieTimeMet = false;
            }
            cout << "NO BONNIE" << endl;
            turnOffLeftDoor();
            break;
        }

        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Bonnie\\BonnieShadow.png";
		if (targetImageMatches(415, 200, 300, 300) == true) {
			std::cout << "BONNIE" << std::endl;
			turnOnLeftDoor();
            bonnieBool = true;
            bonnieTime = currentTime; // Reset the start time
			break;
		}
        if(counter > 10){
            break;
        }
        turnOnLeftLight();
        counter++;
	}
}

void checkChica() {
    int counter = 0;
    while (true) {
        SetCursorPos(1270, 200);

        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Chica\\NoChicaDoor.jpg";
        if (targetImageMatches(785, 161, 129, 373) == true) {
            if (chicaBool == true) {
                chicaInt = 0;
                chicaBool = false;

                chicaTimeMet = false;
            }
            std::cout << "NO CHICA" << std::endl;
            turnOffRightDoor();
            break;
        }

		targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Chica\\ChicaDoor.jpg";
		if (targetImageMatches(785, 161, 151, 373) == true) {
			std::cout << "CHICA" << std::endl;
			turnOnRightDoor();
            chicaBool = true;
            chicaTime = currentTime; // Reset the start time
			break;
		}
        if (counter > 10) {
            break;
        }
        turnOnRightLight();
        counter++;
    }
}

void checkFoxy() {
    targetValue = 0.75;
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage1\\Stage11.jpg";
        if (targetImageMatches(0, 100, 0, 620) == true) {
            break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage2\\Stage21.png";
        if (targetImageMatches(0, 100, 0, 620) == true) {
            foxyThree = false;
            break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage3\\Stage31.png";
        if (targetImageMatches(0, 100, 0, 620) == true) {
            foxyThree = true;
            break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage4\\Stage43.jpg";
        if (targetImageMatches(0, 100, 0, 620) == true) {
            break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage1\\Stage12.jpg";
        if (targetImageMatches(0, 100, 0, 620) == true) {
            break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage2\\Stage22.png";
        if (targetImageMatches(0, 100, 0, 620) == true) {
            foxyThree = false;
            break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage3\\Stage32.png";
        if (targetImageMatches(0, 100, 0, 620) == true) {
            foxyThree = true;
            break;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage4\\Stage44.jpg";
        if (targetImageMatches(0, 100, 0, 620) == true) {
            break;
        }
    }
    targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage4\\Stage43.jpg";
    if (targetImageMatches(0, 100, 0, 620) == true) {
        //Go to CAM 2A FOXY HALL
        camera2A();
        targetValue = 0.9;
        Sleep(sleepTime);
        closeCams();
        Sleep(sleepTime);
        turnOnLeftDoor();
        foxyThree = false;
    }
    targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Foxy\\Stage4\\Stage44.jpg";
    if (targetImageMatches(0, 100, 0, 620) == true) {
        //Go to CAM 2A FOXY HALL
        camera2A();
        targetValue = 0.9;
        Sleep(sleepTime);
        closeCams();
        Sleep(sleepTime);
        turnOnLeftDoor();
        foxyThree = false;
    }
    targetValue = 0.9;
}

bool checkFreddy() {
    targetValue = 0.6;
    while (true) {
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Freddy\\Freddy.png";
        if (targetImageMatches(0, 0, 0, 0) == true) {
            targetValue = 0.9;
            return true;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Freddy\\CamChica.png";
        if (targetImageMatches(0, 0, 0, 0) == true) {
            targetValue = 0.9;
            return true;
        }
        targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Cams\\Freddy\\Hall.png";
        if (targetImageMatches(0, 0, 0, 0) == true) {
            targetValue = 0.9;
            return false;
        }
        camera4B();
        Sleep(sleepTime);
    }
}

void checkPower() {
    targetImageString = "G:\\Projects\\FNAFBOT\\IMAGES\\Room\\Power\\4Percent.png";
    if (targetImageMatches(180, 610, 250, 250) == true) {
        cout << "Right Door On" << endl;
        turnOnRightDoor();
        cout << "Open Cams" << endl;
        openCams();
        cout << "4B" << endl;
        camera4B();
        cout << "1C" << endl;
        camera1C();
        cout << "Check Foxy" << endl;
        checkFoxy();
        cout << "4B" << endl;
        camera4B();

        cout << "Close Cams" << endl;
        closeCams();


        turnOffRightDoor();
        turnOffLeftDoor();
        Sleep(30000);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int main()
{
    int foxyCounter = -1;
    turnOffLeftDoor();

    bonnieTime = currentTime; // Reset the start time
    chicaTime = currentTime; // Reset the start time
    auto startTime = std::chrono::steady_clock::now();

    //Stuff
    while (true) {
        /*
        if (GetAsyncKeyState('S') & 0x8000) {
            break;
            return 0;
        }
        if (GetAsyncKeyState('Z') & 0x8000) {
            showCursor();
        }

        //TEST PROGRAM
        turnOnLeftDoor();
        Sleep(sleepTime);
        turnOnLeftLight();
        Sleep(sleepTime);
        turnOnRightDoor();
        Sleep(sleepTime);
        turnOnRightLight();
        Sleep(sleepTime);
        turnOffLeftDoor();
        Sleep(sleepTime);
        turnOffLeftLight();
        Sleep(sleepTime);
        turnOffRightDoor();
        Sleep(sleepTime);
        turnOffRightLight();
        */


        //MAIN PROGRAM/////////////////////////////////////////////////////////////////////////////////
        /*
        cout << "Start Cycle" << endl;
        cout << "Left Light On" << endl;
        turnOnLeftLight();
        cout << "Check Bonnie." << endl;
        checkBonnie();
        cout << "Left Light Off" << endl;
        turnOffLeftLight();

        cout << "Right Door On" << endl;
        turnOnRightDoor();
        cout << "Open Cams" << endl;
        openCams();
        cout << "4B" << endl;
        camera4B();
        if (foxyCounter == 3) {
            cout << "1C" << endl;
            camera1C();
            cout << "Check Foxy" << endl;
            checkFoxy();
            cout << "4B" << endl;
            camera4B();
            foxyCounter = 0;
        }
        cout << "Close Cams" << endl;
        closeCams();
        cout << "Check Power" << endl;
        checkPower();

        cout << "Right Door Off" << endl;
        turnOffRightDoor();
        cout << "Right Light On" << endl;
        turnOnRightLight();
        cout << "Check Chica" << endl;
        checkChica();
        cout << "Right Light Off" << endl;
        turnOffRightLight();

        cout << "Rest" << endl;
        Sleep(500);
        //checks on bonnie and chica
        cout << "Checks on bonnie and chica" << endl;
        cout << "Left Door Off" << endl;
        turnOffLeftDoor();
        cout << "Left Light On" << endl;
        turnOnLeftLight();
        cout << "Check Bonnie" << endl;
        checkBonnie();
        cout << "Left Light Off" << endl;
        turnOffLeftLight();
        cout << "Rest" << endl;
        Sleep(500);
        //and chica
        cout << "Right Door Off." << endl;
        turnOffRightDoor();
        cout << "Right Light On" << endl;
        turnOnRightLight();
        cout << "Check Chica" << endl;
        checkChica();
        cout << "Right Light Off" << endl;
        turnOffRightLight();
        cout << "Check Power" << endl;
        checkPower();
        cout << "Rest" << endl;
        Sleep(500);
        //checks on bonnie and chica
        cout << "Checks on bonnie and chica" << endl;
        cout << "Left Door Off" << endl;
        turnOffLeftDoor();
        cout << "Left Light On" << endl;
        turnOnLeftLight();
        cout << "Check Bonnie." << endl;
        checkBonnie();
        cout << "Left Light Off" << endl;
        turnOffLeftLight();
        cout << "Rest" << endl;
        Sleep(500);
        //and chica
        cout << "Right Door Off" << endl;
        turnOffRightDoor();
        cout << "Right Light On" << endl;
        turnOnRightLight();
        cout << "Check Chica." << endl;
        checkChica();
        cout << "Right Light Off" << endl;
        turnOffRightLight();
        cout << "Check Power" << endl;
        checkPower();


        foxyCounter++;
        cout << "Rest" << endl;
        Sleep(500);
        */
        //MAIN PROGRAM/////////////////////////////////////////////////////////////////////////////////

        //AGRESSIVE PROGRAM////////////////////////////////////////////////////////////////////////////
        /*
        cout << "Start Cycle" << endl;
        cout << "Left Light On" << endl;
        turnOnLeftLight();
        cout << "Check Bonnie." << endl;
        checkBonnie();
        cout << "Left Light Off" << endl;
        turnOffLeftLight();

        cout << "Open Cams" << endl;
        openCams();
        cout << "4B" << endl;
        camera4B();
        if (foxyCounter == 3) {
            cout << "1C" << endl;
            camera1C();
            cout << "Check Foxy" << endl;
            checkFoxy();
            cout << "4B" << endl;
            camera4B();
            foxyCounter = 0;
        }
        cout << "Close Cams" << endl;
        closeCams();
        cout << "Check Power" << endl;
        checkPower();

        cout << "Right Light On" << endl;
        turnOnRightLight();
        cout << "Check Chica" << endl;
        checkChica();
        cout << "Right Light Off" << endl;
        turnOffRightLight();

        cout << "Check Power" << endl;
        checkPower();

        foxyCounter++;
        */
        //AGRESSIVE PROGRAM////////////////////////////////////////////////////////////////////////////

        //FOUR TWENTY PROGRAM////////////////////////////////////////////////////////////////////////////
        
        cout << "Start Cycle" << endl;
        cout << "Foxy Counter: " << foxyCounter << endl;
        if (bonnieInt >= 5) {
            cout << "Left Light On" << endl;
            turnOnLeftLight();
            cout << "Check Bonnie." << endl;
            checkBonnie();
            cout << "Left Light Off" << endl;
            turnOffLeftLight();
        }
        else {
            Sleep(700);
            foxyCounter--;
        }
        bonnieInt++;

        if (foxyCounter >= 25) {
            cout << "Right Door On" << endl;
            turnOnRightDoor();
            cout << "Open Cams" << endl;
            openCams();
            cout << "4B" << endl;
            camera4B();
            cout << "1C" << endl;
            camera1C();
            cout << "Check Foxy" << endl;
            checkFoxy();
            cout << "4B" << endl;
            camera4B();
            foxyCounter = 0;

            cout << "Close Cams" << endl;
            closeCams();

            bonnieInt++;
            chicaInt++;
        }
        else {
            cout << "Open Cams" << endl;
            openCams();
            cout << "4B" << endl;
            camera4B();
            cout << "Close Cams" << endl;
            closeCams();
        }

        if (chicaInt >= 5) {
            cout << "Right Light On" << endl;
            turnOnRightLight();
            cout << "Check Chica" << endl;
            checkChica();
            cout << "Right Light Off" << endl;
            turnOffRightLight();
        }
        else {
            cout << "Right Door Off" << endl;
            turnOffRightDoor();
            Sleep(200);
            foxyCounter--;
        }
        chicaInt++;
        
        cout << "Check Power" << endl;
        checkPower();

        foxyCounter = foxyCounter + 2;
        
        //FOUR TWENTY PROGRAM////////////////////////////////////////////////////////////////////////////

        //FOUR TWENTY PROGRAM SCUFFED////////////////////////////////////////////////////////////////////////////
        /*



        cout << "Start Cycle" << endl;
        cout << "Foxy Counter: " << foxyCounter << endl;


        auto currentTime = std::chrono::steady_clock::now();
        // Calculate the elapsed time
        auto bonnieElapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - bonnieTime);
        auto chicaElapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - chicaTime);
        // Check if the elapsed time has reached the threshold
        if (bonnieElapsedTime >= bonnieThresholdDuration) {
            bonnieTimeMet = true; // Set the bonnieTimeMet to true
        }
        if (bonnieTimeMet || bonnieBool) {
            cout << "Left Light On" << endl;
            turnOnLeftLight();
            cout << "Check Bonnie." << endl;
            checkBonnie();
            cout << "Left Light Off" << endl;
            turnOffLeftLight();
            bonnieTimeMet = false;
        }

        /asterisk
        if (foxyCounter >= 15 && !bonnieTimeMet && !chicaTimeMet) {
            cout << "Right Door On" << endl;
            turnOnRightDoor();
            cout << "Open Cams" << endl;
            openCams();
            cout << "4B" << endl;
            camera4B();
            cout << "1C" << endl;
            camera1C();
            cout << "Check Foxy" << endl;
            checkFoxy();
            cout << "4B" << endl;
            camera4B();
            foxyCounter = 0;

            cout << "Close Cams" << endl;
            closeCams();

        }
        
        else {
            cout << "Open Cams" << endl;
            openCams();
            cout << "4B" << endl;
            camera4B();
            cout << "Close Cams" << endl;
            closeCams();
        }
        asterisk/

        if (chicaElapsedTime >= chicaThresholdDuration) {
            chicaTimeMet = true; // Set the bonnieTimeMet to true
        }
        if (chicaTimeMet || chicaBool) {
            cout << "Right Light On" << endl;
            turnOnRightLight();
            cout << "Check Chica" << endl;
            checkChica();
            cout << "Right Light Off" << endl;
            turnOffRightLight();
            chicaTimeMet = false;
        }


        cout << "Check Power" << endl;
        checkPower();

        // Get the current time
        currentTime = std::chrono::steady_clock::now();

        // Calculate the elapsed time
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);

        while (true) {
            elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            if (elapsedTime.count() >= lowerBound) {
                cout << "Open Cams" << endl;
                openCams();
                cout << "4B" << endl;
                camera4B();
                cout << "Close Cams" << endl;
                closeCams();
                break; // Exit the loop when condition is met
            }
        }
        
        */
        //FOUR TWENTY PROGRAM SCUFFED////////////////////////////////////////////////////////////////////////////

    }
        //SLEEP
        //this_thread::sleep_for(1s);
        Sleep(5000); // Sleep for 100 milliseconds (adjust the value as needed)
    return 0;
}