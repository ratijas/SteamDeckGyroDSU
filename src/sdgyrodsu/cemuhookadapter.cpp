#include "cemuhookadapter.h"
#include "sdhidframe.h"

using namespace kmicki::cemuhook::protocol;

#define SD_SCANTIME_US 4000;
#define ACC_1G 16383;
#define GYRO_1DEGPERSEC 16;

namespace kmicki::sdgyrodsu
{
    MotionData GetMotionData(SdHidFrame const& frame)
    {
        MotionData data;

        SetMotionData(frame,data);

        return data;
    }

    void SetMotionData(SdHidFrame const& frame, MotionData &data)
    {
        static const float acc1G = (float)ACC_1G;
        static const float gyro1dps = (float)GYRO_1DEGPERSEC;

        uint64_t timestamp = (uint64_t)frame.Increment*SD_SCANTIME_US;

        data.timestampL = (uint32_t)(timestamp & 0xFFFFFFFF);
        data.timestampH = (uint32_t)(timestamp >> 32);
        
        data.accX = -(float)frame.AccelAxisRightToLeft/acc1G;
        data.accY = -(float)frame.AccelAxisFrontToBack/acc1G;
        data.accZ = (float)frame.AccelAxisTopToBottom/acc1G;

        data.pitch = (float)frame.GyroAxisRightToLeft/gyro1dps;
        data.yaw = -(float)frame.GyroAxisFrontToBack/gyro1dps;
        data.roll = (float)frame.GyroAxisTopToBottom/gyro1dps;
    }

    CemuhookAdapter::CemuhookAdapter(hiddev::HidDevReader & _reader)
    : reader(_reader)
    {
        ;
    }

    void CemuhookAdapter::StartFrameGrab() {}

    MotionData const& CemuhookAdapter::GetMotionDataNewFrame()
    {
        SetMotionData(GetSdFrame(reader.GetNewFrame(this)),data);
        reader.UnlockFrame(this);
        return data;
    }

    void CemuhookAdapter::StopFrameGrab() {}

    bool CemuhookAdapter::IsControllerConnected()
    {
        return true;
    }
}