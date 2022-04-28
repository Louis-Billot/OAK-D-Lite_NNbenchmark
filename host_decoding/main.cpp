#include <iostream>

// Includes common necessary includes for development using depthai library
#include "depthai/depthai.hpp"

// #include "Windows.h"
#include <windows.h>

int main() {
    using namespace std;
    // Create pipeline
    dai::Pipeline pipeline;

    // Define source and output
    auto camRgb = pipeline.create<dai::node::ColorCamera>();
    auto xoutRgb = pipeline.create<dai::node::XLinkOut>();

    xoutRgb->setStreamName("rgb");

    // Properties
    camRgb->setPreviewSize(300, 300);
    camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
    camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
    camRgb->setInterleaved(false);
    camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::RGB);

    // Linking
    camRgb->preview.link(xoutRgb->input);

    // Connect to device and start pipeline
    dai::Device device(pipeline, dai::UsbSpeed::SUPER);

    cout << "Connected cameras: ";
    for(const auto& cam : device.getConnectedCameras()) {
        cout << cam << " ";
    }
    cout << endl;

    // Print USB speed
    cout << "Usb speed: " << device.getUsbSpeed() << endl;

    // Output queue will be used to get the rgb frames from the output defined above
    auto qRgb = device.getOutputQueue("rgb", 4, false);

    LARGE_INTEGER time1, time2, freq, starttime, stoptime;
    QueryPerformanceFrequency(&freq);

    uint64_t frm_cnt = 0;
    QueryPerformanceCounter(&starttime);

    while(true) {
        QueryPerformanceCounter(&time1);

        auto inRgb = qRgb->get<dai::ImgFrame>();

        QueryPerformanceCounter(&time2);

        double delay = double(time2.QuadPart - time1.QuadPart) / freq.QuadPart;

        cout << delay << "   " << 1/delay << endl;

        // Retrieve 'bgr' (opencv format) frame
        cv::imshow("rgb", inRgb->getCvFrame());

        frm_cnt ++;

        int key = cv::waitKey(1);
        if(key == 'q' || key == 'Q') {
            break;
        }
    }

    QueryPerformanceCounter(&stoptime);
    cout << frm_cnt << "   " << frm_cnt / (double(stoptime.QuadPart - starttime.QuadPart) / freq.QuadPart) << endl;

    return 0;
}