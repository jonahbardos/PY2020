#pragma once

#include <memory>
#include <chrono>
#include <math.h>
#include <vector>
#include <urg_c/urg_sensor.h>
#include <urg_c/urg_utils.h>

struct Polar2D;

typedef struct Polar2D
{
    double r, theta;
    int asCartesianX()
    {
        (long)(r * cos(theta));
    }
    int asCartesianY()
    {
        (long)(r * sin(theta));
    }
} Polar2D;

class URGLidar
{
private:
    urg_t urg;
    int error = 0;
    const char connect_device[13] = "/dev/ttyACM0";
    const long connect_baudrate = 115200;
    std::vector<std::shared_ptr<Polar2D>> lastFrame;                  //The last completed frame.
    std::chrono::time_point<std::chrono::steady_clock> lastFrameTime; //The system time at which the last frame was completed.

public:
    /**
     * Uses the Hokuyo URG sensor to read Lidar data and build frames.
     */
    URGLidar();
    /**
     * Opens a connection with the sensor on /dev/ttyACM0 and sets up
     * scan range and skip defaults.
     * @returns True if this operation was successful.
     */
    bool open();
    /**
     * @returns The last completed frame of nullptr if there is none.
     */
    std::vector<std::shared_ptr<Polar2D>> getLastFrame();
    /**
     * @returns the steady clock timestamp just before the sensor was
     * last queried for a new frame, or the time this URGLidar was
     * created if no frame have been created.
     */
    auto getLastFrameTime();
    /**
     * Builds the next frame and updates the last frame and time variables
     * @returns True if this operation was successful.
     */
    bool createFrame();
    /**
     * @returns The last error code from the sensor. Generated by open, close,
     * and getLastFrame.
     */
    int getError();
    /**
     * Closes and releases the connection to the sensor.
     * @returns True if this operation was successful.
     */
    bool close();
};