#include "device.hpp"

#include <format>

#include "deviceConfig.hpp"
#include "exceptions.hpp"

using namespace device;

/**
 * @brief Construct a new Device:: Device object
 *
 * @details This constructor is used to initialize the device object. The device
 * object is used to handle all device API calls.
 * The constructor initializes the following variables:
 *      - _deviceID: the device ID of the device
 *      - _deviceCount: the number of devices in the system
 *      - _deviceProp: the device properties of the device
 *
 *
 * @note If the device ID is not given, the constructor will get the device ID
 * from the device API. If the device ID is given, the constructor will use the
 * given device ID. The constructor will also get the device count from the
 * device API.
 *
 * @throw DeviceException if an error occurred during the device API calls or
 * if the device ID is out of range.
 *
 * @param deviceID
 */
Device::Device()
{
    int                 deviceID = -1;
    const deviceError_t error    = __getDevice(&deviceID);

    addDeviceError(error, "Getting the device ID");

    *this = Device(_deviceID);
}

Device::Device(const int deviceID) : _deviceID(deviceID), _deviceCount(0)
{
    deviceError_t error = __getDeviceCount(&_deviceCount);
    addDeviceError(error, "Getting the device count");

    if (_deviceID >= _deviceCount)
        _errorMsgs.push_back(std::format(
            "The device ID is out of range. The device ID is {} and the "
            "device count is {}",
            _deviceID,
            _deviceCount
        ));

    error = __getDeviceProperties(&_deviceProp, _deviceID);
    addDeviceError(error, "Getting the device properties");

    error = __setDevice(_deviceID);
    addDeviceError(
        error,
        std::format("Setting the device with the device ID {}", _deviceID)
    );

    checkErrors("Device initialization");
}

/**
 * @brief function to add an error message to the error message list
 *
 * @details This function is used to add an error message to the error message
 * list. The error message list is used to store all error messages that
 * occurred during the device API calls.
 *
 * @throw DeviceException if the error code is not __deviceSuccess__
 *
 * @param error the error code that occurred
 * @param msg the message that should be added to the error message list
 */
void device::Device::addDeviceError(
    const deviceError_t error,
    const std::string&  msg
)
{
    if (error != __deviceSuccess__)
        _errorMsgs.push_back(
            msg + " failed with the following error:\n\n" +
            __deviceGetErrorString(error)
        );
}

/**
 * @brief function to check for errors in the device API
 *
 * @note This function is used to check if in the latest device API calls an
 * error occurred.
 *
 * @param msg optional message to print in case of error. Default is no
 * additional message. The msg parameter will be inserted in a string "Error
 * in " + msg + ":\n" + error messages.
 */
void Device::checkErrors() { checkErrors("Device API call"); }

void Device::checkErrors(const std::string& msg)
{
    if (!_errorMsgs.empty())
    {
        std::string errorMsg = "Error in " + msg + ":\n\n";

        for (const auto& e : _errorMsgs) errorMsg = errorMsg + e + "\n";

        throw customException::DeviceException(errorMsg);
    }
}