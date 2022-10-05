#include <chrono>
#include <iostream>
#include <fstream>

// Includes common necessary includes for development using depthai library
#include "depthai/depthai.hpp"

// #include "Windows.h"
#include <windows.h>

#include <filesystem>

using std::cout; using std::cin;
using std::endl; using std::vector;
using std::string;
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;

static const vector<std::string> labelMap = {
    "person",        "bicycle",      "car",           "motorbike",     "aeroplane",   "bus",         "train",       "truck",        "boat",
    "traffic light", "fire hydrant", "stop sign",     "parking meter", "bench",       "bird",        "cat",         "dog",          "horse",
    "sheep",         "cow",          "elephant",      "bear",          "zebra",       "giraffe",     "backpack",    "umbrella",     "handbag",
    "tie",           "suitcase",     "frisbee",       "skis",          "snowboard",   "sports ball", "kite",        "baseball bat", "baseball glove",
    "skateboard",    "surfboard",    "tennis racket", "bottle",        "wine glass",  "cup",         "fork",        "knife",        "spoon",
    "bowl",          "banana",       "apple",         "sandwich",      "orange",      "broccoli",    "carrot",      "hot dog",      "pizza",
    "donut",         "cake",         "chair",         "sofa",          "pottedplant", "bed",         "diningtable", "toilet",       "tvmonitor",
    "laptop",        "mouse",        "remote",        "keyboard",      "cell phone",  "microwave",   "oven",        "toaster",      "sink",
    "refrigerator",  "book",         "clock",         "vase",          "scissors",    "teddy bear",  "hair drier",  "toothbrush"};

static std::atomic<bool> syncNN{true};

int main(int argc, char** argv) {
    using namespace std;
    using namespace std::chrono;
    // string nnPath(BLOB_PATH);
    string nnPath;

    std::fstream fs;
    // vector<directory_entry> files_list;
    vector<string> files_list;

    // If path to blob specified, use that
    if(argc > 1) {
        files_list.push_back(string(argv[1]));
    }
    else
    {
        string path = "c:/depthai_blob_files/test_dir/";

        fs.open(path + "test.txt", std::fstream::out | std::fstream::trunc);

        for (const auto & file : directory_iterator(path))
        {
            if (file.is_regular_file() && file.path().extension() == ".blob")
                files_list.push_back(file.path().string());
            cout << file.path().extension() << endl;
        }
    }

    for (const auto filepath : files_list)
    {
        nnPath = filepath;
        

        // Print which blob we are using
        printf("Using blob at path: %s\n", nnPath.c_str());
        fs << nnPath << endl;

        const uint16_t nn_shape = 417;
        // const uint16_t nn_shape = 640;

        const auto output_shape = int(3 * (pow((nn_shape/8.0), 2) + pow((nn_shape/16.0), 2) + pow((nn_shape/32.0), 2)));

        // Create pipeline
        dai::Pipeline pipeline;

        // Define source and output
        auto camRgb = pipeline.create<dai::node::ColorCamera>();
        auto detectionNetwork = pipeline.create<dai::node::NeuralNetwork>();
        auto xoutRgb = pipeline.create<dai::node::XLinkOut>();
        auto nnOut = pipeline.create<dai::node::XLinkOut>();

        detectionNetwork->setBlobPath(nnPath);
        detectionNetwork->setNumPoolFrames(4);
        detectionNetwork->input.setBlocking(false);
        detectionNetwork->setNumInferenceThreads(2);

        xoutRgb->setStreamName("rgb");
        xoutRgb->input.setBlocking(false);

        nnOut->setStreamName("nn");

        // Properties
        // camRgb->setPreviewSize(129, 97);
        camRgb->setPreviewSize(nn_shape, nn_shape);
        camRgb->setBoardSocket(dai::CameraBoardSocket::RGB);
        camRgb->setResolution(dai::ColorCameraProperties::SensorResolution::THE_1080_P);
        camRgb->setInterleaved(false);
        camRgb->setColorOrder(dai::ColorCameraProperties::ColorOrder::BGR);
        // camRgb->setFps(30);
        camRgb->setFps(60);

        // Linking
        camRgb->preview.link(detectionNetwork->input);
        if(syncNN) {
            detectionNetwork->passthrough.link(xoutRgb->input);
        } else {
            camRgb->preview.link(xoutRgb->input);
        }

        detectionNetwork->out.link(nnOut->input);

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
        auto qNN = device.getOutputQueue("nn", 4, false);

        // LARGE_INTEGER time1, time2, freq, starttime, stoptime;
        // QueryPerformanceFrequency(&freq);

        // uint64_t frm_cnt = 0;
        // QueryPerformanceCounter(&starttime);

        cv::Mat frame;
        // vector<dai::ImgDetection> detections;
        vector<float> output, output2, output3;

        cv::namedWindow("rgb", cv::WINDOW_NORMAL);
        cv::resizeWindow("rgb", cv::Size(nn_shape, nn_shape));
        // cv::resizeWindow("rgb", cv::Size(2*129, 2*97));

        auto startTime = steady_clock::now();
        int counter = 0;
        auto startTime_loop = startTime;
        int counter_loop = 0;
        float fps = 0;
        auto color2 = cv::Scalar(255, 255, 255);
        float duration_loop = 60;

        startTime_loop = steady_clock::now();

        // while(true) {
        while(duration_cast<duration<float>>(steady_clock::now() - startTime_loop).count() < duration_loop ) {
            std::shared_ptr<dai::ImgFrame> inRgb;
            std::shared_ptr<dai::NNData> inNN;

            if(syncNN) {
                inRgb = qRgb->get<dai::ImgFrame>();
                inNN = qNN->get<dai::NNData>();
            } else {
                inRgb = qRgb->tryGet<dai::ImgFrame>();
                inNN = qNN->tryGet<dai::NNData>();
            }

            counter++;
            counter_loop++;
            auto currentTime = steady_clock::now();
            auto elapsed = duration_cast<duration<float>>(currentTime - startTime);
            if(elapsed > seconds(1)) {
                fps = counter / elapsed.count();
                counter = 0;
                startTime = currentTime;
            }

            if(inRgb) {
                frame = inRgb->getCvFrame();
                std::stringstream fpsStr;
                fpsStr << "NN fps: " << std::fixed << std::setprecision(2) << fps;
                cv::putText(frame, fpsStr.str(), cv::Point(2, inRgb->getHeight() - 4), cv::FONT_HERSHEY_TRIPLEX, 0.4, color2);
            }

            if(inNN) {
                output = inNN->getLayerFp16("cif");
                output2 = inNN->getLayerFp16("caf");
                // output3 = inNN->getLayerFp16("710");
                // cout << output.size() << " " << output2.size() << " " << output3.size() << endl;
                cout << output[0] << " " << output2[0] /*<< " " << output3[0]*/ << endl;
            }

            if(!frame.empty()) {
                cv::imshow("rgb", frame);
            }

            int key = cv::waitKey(1);
            if(key == 'q' || key == 'Q') {

                // for (auto item : output){
                //     cout << item << " ";
                // }
                // cout << endl;
                // for (auto item : output2){
                //     cout << item << " ";
                // }
                // cout << endl;
                // for (auto item : output3){
                //     cout << item << " ";
                // }
                break;
                // return 0;
            }
        }

        // QueryPerformanceCounter(&stoptime);
        // cout << frm_cnt << "   " << frm_cnt / (double(stoptime.QuadPart - starttime.QuadPart) / freq.QuadPart) << endl;
        
        cout << counter_loop << "   " << counter_loop / duration_cast<duration<double>>(steady_clock::now() - startTime_loop).count() << endl;
        fs << counter_loop << "   " << counter_loop / duration_cast<duration<double>>(steady_clock::now() - startTime_loop).count() << endl;

    }

    if (fs.is_open())
        fs.close();

    return 0;
}