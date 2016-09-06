#include "mraa.hpp"

#include <cmath>
#include <signal.h>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include <chrono>
#include <thread>

//#include "ReceiverComms.h"
//#include "json.hpp"
#include <deque>
#include <bitset>
#include <array>

#include "LedSegment.h"
#include "LargeDigitDriver.h"

// opensegment: https://github.com/sparkfun/Serial7SegmentDisplay/wiki/Special-Commands#i2cAddress

constexpr int max_length = 1024;

using namespace std::chrono_literals;

//static struct option long_options[] = {
//	{ "cal-acc",        no_argument,       0, 'a' },
//	{ "cal-mag",        no_argument,       0, 'm' },
//};

volatile sig_atomic_t done = 0;

const float MaxLightCounts = 30;

const int BaseSegmentLedI2cAddress = 0x71;
const int BlueSegmentLedI2cAddress = 0x74;
const int RedSegmentLedI2cAddress = 0x75;

namespace std {
    template<class T>
    const T& clamp(const T& x, const T& lower, const T& upper) {
        return min(upper, max(x, lower));
    }
}









float tween(float t, float start, float end)
{
    return std::clamp((t - start) / (end - start), 0.0f, 1.0f);
}

template <int N>
class LedAnimation
{
public:
    virtual std::array<ColorTriplet, N> GetColors() = 0;
    virtual SegmentLedState GetSegmentLed() = 0;
    virtual bool IsAnimationComplete() = 0;
};

template <int N>
class WooWoosha : public LedAnimation<N>
{
    static constexpr float fadeUpStart = 3.0f;
    static constexpr float fadeUpEnd = 4.5f;
    static constexpr float fadeDownStart = 5.0f;
    static constexpr float fadeDownEnd = 6.0f;
    static constexpr float animEnd = 7.0f;

public:
    WooWoosha() :
        _lastSegmentIdx(0),
        _lastSegmentTime(0),
        _speed(0.040f),
        _dir(1)
    {
        _start = std::chrono::steady_clock::now();
        _lastColorTime.fill(-100);
    }

    virtual std::array<ColorTriplet, N> GetColors() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        while (curTime > _lastSegmentTime + _speed)
        {
            _lastSegmentIdx += _dir;
            if (_lastSegmentIdx == 0 || _lastSegmentIdx == N - 1)
                _dir = -_dir;

            _lastSegmentTime += _speed;
            if (_speed > 0.002f)
                _speed *= 0.99f;

            _lastColorTime[_lastSegmentIdx] = _lastSegmentTime;
        }

        float minBrightness = tween(curTime, fadeUpStart, fadeUpEnd);
        float maxBrightness = 1.0f - tween(curTime, fadeDownStart, fadeDownEnd);

        std::array<ColorTriplet, N> ret;
        for (int i = 0; i < N; i++) {
            auto timeSince = curTime - _lastColorTime[i];
            auto juice = std::clamp(1.0f - (timeSince + 0.1f), 0.0f, 1.0f) * 0.9f;
            auto val = static_cast<uint8_t>(std::clamp(juice, minBrightness, maxBrightness) * MaxLightCounts);
            ret[i] = ColorTriplet{ val, val, val };
        }

        return ret;
    }

    virtual SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{ { ' ',' ',' ',' ' }, 0 };
    }

    virtual bool IsAnimationComplete() override
    {
        return true;
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        return duration.count() > animEnd;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> _start;
    int _lastSegmentIdx;
    float _lastSegmentTime;
    float _speed;
    int _dir;
    std::array<float, N> _lastColorTime;
};


template <int N>
class XmasTree : public LedAnimation<N>
{
public:
    XmasTree()
    {
        _start = std::chrono::steady_clock::now();

        _dootRegion.fill(0);
        const int regionPadding = 3;
        const int outSizeSpacing = 5;
        const int numRegions = 3;
        const int regionSize = (N - outSizeSpacing - ((numRegions - 1) * regionPadding)) / numRegions;
        int startPx = (N / 2) - (regionSize * numRegions + regionPadding * (numRegions - 1)) / 2;
        for (int i = 0; i < numRegions; i++)
        {
            for (int j = 0; j < regionSize; j++)
                _dootRegion[startPx + j] = 3 - i;
            startPx += regionSize + regionPadding;
        }
    }

    virtual std::array<ColorTriplet, N> GetColors() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        std::array<ColorTriplet, N> ret;
        float timeSinceDoot = std::fmod(curTime, 1.0f) * 1.2;
        float dootiness = std::clamp(1.2f - timeSinceDoot*timeSinceDoot, 0.0f, 1.0f);
        float ambientVal = 0;

        for (int i = 0; i < N; i++) {
            //auto timeSince = curTime - _lastColorTime[i];
            //auto juice = std::clamp(1.0f - (timeSince + 0.1f), 0.0f, 1.0f);
            if (_dootRegion[i] > curTime) {
                auto val = static_cast<uint8_t>(dootiness * MaxLightCounts);
                ret[i] = ColorTriplet{ val, val, 0 };
            }
            else {
                auto val = static_cast<uint8_t>(ambientVal * MaxLightCounts);
                ret[i] = ColorTriplet{ val, val, static_cast<uint8_t>(val / 4) };
            }
        }

        return ret;
    }

    virtual SegmentLedState GetSegmentLed() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();
        if (curTime > 2)
            return SegmentLedState{ { ' ',' ','1',' ' }, 0 };
        if (curTime > 1)
            return SegmentLedState{ { ' ','2',' ',' ' }, 0 };

        return SegmentLedState{ { '3',' ',' ',' ' }, 0 };
    }

    virtual bool IsAnimationComplete() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        return duration.count() > 3.0f;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> _start;
    std::array<int, N> _dootRegion;
};

template <int N>
class Fighting : public LedAnimation<N>
{
public:
    Fighting()
    {
        _start = std::chrono::steady_clock::now();
        _end = _start + std::chrono::seconds(15);
    }

    virtual std::array<ColorTriplet, N> GetColors() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        float timeSinceDoot = curTime / 3.0f;
        float dootiness = std::clamp(1.0f - timeSinceDoot, 0.0f, 1.0f);

        std::array<ColorTriplet, N> ret;
        for (int i = 0; i < N; i++) {
            auto val = static_cast<uint8_t>(dootiness * MaxLightCounts);
            ret[i] = ColorTriplet{ 0, val, 0 };
        }

        return ret;
    }

    virtual SegmentLedState GetSegmentLed() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        if (curTime < 1.0f)
            return SegmentLedState{ { 'F','I','G','T' }, 0 };

        std::chrono::duration<float> remDuration = _end - std::chrono::steady_clock::now();
        int remTime = ceil(remDuration.count());

        if (remTime > 0)
        {
            return SegmentLedState{
                {
                    '0' + static_cast<uint8_t>(remTime / 60),
                    '0' + static_cast<uint8_t>((remTime % 60) / 10),
                    '0' + static_cast<uint8_t>(remTime % 10),
                    ' '
                },
                1
            };
        }

        return SegmentLedState{ { 'E','n','d',' ' }, 0 };
    }

    virtual bool IsAnimationComplete() override
    {
        return false;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> _start;
    std::chrono::time_point<std::chrono::steady_clock> _end;
};

template <int N>
class StagingAnim : public LedAnimation<N>
{
    static constexpr float fadeOutStart = 1.5f;
    static constexpr float fadeOutDuration = 1.5f;
    static constexpr float totalAnimFromLastButton = 3.5f;

public:
    StagingAnim(bool& redRef, bool& blueRef) :
        _redRef(redRef), _blueRef(blueRef)
    {
        _start = std::chrono::steady_clock::now();
        _blueStart = std::chrono::time_point<std::chrono::steady_clock>::max();
        _redStart = std::chrono::time_point<std::chrono::steady_clock>::max();
    }

    virtual std::array<ColorTriplet, N> GetColors() override
    {
        auto now = std::chrono::steady_clock::now();

        if (now < _redStart && _redRef)
            _redStart = now;
        if (now < _blueStart && _blueRef)
            _blueStart = now;

        std::chrono::duration<float> redDuration = now - _redStart;
        std::chrono::duration<float> blueDuration = now - _blueStart;
        float redTime = redDuration.count();
        float blueTime = blueDuration.count();
        float minTime = std::min(redTime, blueTime);

        float maxValue = std::clamp(1.0f - (minTime - fadeOutStart) / fadeOutDuration, 0.0f, 1.0f);

        std::array<ColorTriplet, N> ret;
        for (int i = 0; i < N / 2; i++) {
            float s = redTime > 0 ? std::clamp(redTime*redTime, 0.0f, 1.0f) * 0.9f + 0.1f : 0.1f;
            float s2 = std::min(s, maxValue);
            auto val = static_cast<uint8_t>(s2 * MaxLightCounts);
            ret[i] = ColorTriplet{ val, 0, 0 };
        }
        for (int i = N / 2; i < N; i++) {
            float s = blueTime > 0 ? std::clamp(blueTime*blueTime, 0.0f, 1.0f) * 0.9f + 0.1f : 0.1f;
            float s2 = std::min(s, maxValue);
            auto val = static_cast<uint8_t>(s2 * MaxLightCounts);
            ret[i] = ColorTriplet{ 0, 0, val };
        }

        return ret;
    }

    virtual SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{ { 's','t','r','t' }, 0 };
    }

    virtual bool IsAnimationComplete() override
    {
        std::chrono::duration<float> redDuration = std::chrono::steady_clock::now() - _redStart;
        std::chrono::duration<float> blueDuration = std::chrono::steady_clock::now() - _blueStart;
        return redDuration.count() > totalAnimFromLastButton && blueDuration.count() > totalAnimFromLastButton;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> _start;
    std::chrono::time_point<std::chrono::steady_clock> _blueStart;
    std::chrono::time_point<std::chrono::steady_clock> _redStart;
    bool& _redRef;
    bool& _blueRef;
};

template <int N>
class Boring : public LedAnimation<N>
{
public:
    Boring()
    {
        _start = std::chrono::steady_clock::now();
    }

    virtual std::array<ColorTriplet, N> GetColors() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        std::array<ColorTriplet, N> ret;
        for (int i = 0; i < N; i++) {
            ret[i] = ColorTriplet{ 0, 0, 0 };
        }

        return ret;
    }

    virtual SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{};
    }

    virtual bool IsAnimationComplete() override
    {
        return false;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> _start;
};


void term(int signum)
{
    done = 1;
}


enum class ArenaState {
    Staging,
    Countdown,
    Fighting,
    Paused,
    End
};

constexpr int NUM_LED = 30;

//int main(int argc, char** argv)
//{
//    mraa::Gpio timerSegmentLatch(1);
//    if (timerSegmentLatch.dir(mraa::DIR_OUT) != mraa::SUCCESS) {
//        std::cerr << "Can't set digital pin as output, exiting" << std::endl;
//        return MRAA_ERROR_UNSPECIFIED;
//    }
//    timerSegmentLatch.useMmap(false);
//
//    mraa::Gpio timerSegmentClock(0);
//    if (timerSegmentClock.dir(mraa::DIR_OUT) != mraa::SUCCESS) {
//        std::cerr << "Can't set digital pin as output, exiting" << std::endl;
//        return MRAA_ERROR_UNSPECIFIED;
//    }
//    timerSegmentClock.useMmap(false);
//
//    mraa::Gpio timerSegmentSerial(8);
//    if (timerSegmentSerial.dir(mraa::DIR_OUT) != mraa::SUCCESS) {
//        std::cerr << "Can't set digital pin as output, exiting" << std::endl;
//        return MRAA_ERROR_UNSPECIFIED;
//    }
//    timerSegmentSerial.useMmap(false);
//
//    LargeDigitDriver largeDigits(timerSegmentLatch, timerSegmentClock, timerSegmentSerial);
//
//    uint32_t dig = 0;
//    SegmentLedState s111;
//    while (true) {
//        s111.segments[0] = '0' + (dig / 1000) % 10;
//        s111.segments[1] = '0' + (dig / 100) % 10;
//        s111.segments[2] = '0' + (dig / 10) % 10;
//        s111.segments[3] = '0' + (dig / 1) % 10;
//
//        largeDigits.UpdateSegments(s111);
//
//        dig++;
//        if (dig == 10000) dig = 0;
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    }
//}

int main(int argc, char **argv)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, nullptr);

    //mraa_set_priority(99);

    LedSegment<NUM_LED> leds(0);

    mraa::Gpio* redButton = new mraa::Gpio(2);
    if (redButton->dir(mraa::DIR_IN) != mraa::SUCCESS) {
        std::cerr << "Can't set digital pin as input, exiting" << std::endl;
        return MRAA_ERROR_UNSPECIFIED;
    }

    mraa::Gpio* blueButton = new mraa::Gpio(3);
    if (blueButton->dir(mraa::DIR_IN) != mraa::SUCCESS) {
        std::cerr << "Can't set digital pin as input, exiting" << std::endl;
        return MRAA_ERROR_UNSPECIFIED;
    }

    mraa::Gpio* refButton = new mraa::Gpio(4);
    if (refButton->dir(mraa::DIR_IN) != mraa::SUCCESS) {
        std::cerr << "Can't set digital pin as input, exiting" << std::endl;
        return MRAA_ERROR_UNSPECIFIED;
    }

    mraa::Gpio* redButtonLed = new mraa::Gpio(5);
    if (redButtonLed->dir(mraa::DIR_OUT) != mraa::SUCCESS) {
        std::cerr << "Can't set digital pin as output, exiting" << std::endl;
        return MRAA_ERROR_UNSPECIFIED;
    }

    mraa::Gpio* blueButtonLed = new mraa::Gpio(6);
    if (blueButtonLed->dir(mraa::DIR_OUT) != mraa::SUCCESS) {
        std::cerr << "Can't set digital pin as output, exiting" << std::endl;
        return MRAA_ERROR_UNSPECIFIED;
    }

    //mraa::Gpio timerSegmentLatch(1);
    //if (timerSegmentLatch.dir(mraa::DIR_OUT) != mraa::SUCCESS) {
    //    std::cerr << "Can't set digital pin as output, exiting" << std::endl;
    //    return MRAA_ERROR_UNSPECIFIED;
    //}

    //mraa::Gpio timerSegmentClock(0);
    //if (timerSegmentClock.dir(mraa::DIR_OUT) != mraa::SUCCESS) {
    //    std::cerr << "Can't set digital pin as output, exiting" << std::endl;
    //    return MRAA_ERROR_UNSPECIFIED;
    //}

    //mraa::Gpio timerSegmentSerial(8);
    //if (timerSegmentSerial.dir(mraa::DIR_OUT) != mraa::SUCCESS) {
    //    std::cerr << "Can't set digital pin as output, exiting" << std::endl;
    //    return MRAA_ERROR_UNSPECIFIED;
    //}


    //LargeDigitDriver largeDigits(timerSegmentLatch, timerSegmentClock, timerSegmentSerial);

    //uint32_t dig = 0;
    //SegmentLedState s111;
    //while (true) {
    //    s111.segments[0] = '0' + (dig / 1000) % 10;
    //    s111.segments[1] = '0' + (dig / 100) % 10;
    //    s111.segments[2] = '0' + (dig / 10) % 10;
    //    s111.segments[3] = '0' + (dig / 1) % 10;

    //    largeDigits.UpdateSegments(s111);

    //    dig++;
    //    if (dig == 10000) dig = 0;
    //    std::this_thread::sleep_for(100ms);
    //}

    mraa::I2c i2c(6);

    //i2c.address(BaseSegmentLedI2cAddress);
    //i2c.writeReg(0x80, RedSegmentLedI2cAddress);
    //return 0;

    i2c.frequency(mraa::I2cMode::I2C_FAST);

    i2c.address(RedSegmentLedI2cAddress);
    i2c.writeReg(0x79, 0);
    i2c.writeReg(0x77, 0x00);

    i2c.address(BlueSegmentLedI2cAddress);
    i2c.writeReg(0x79, 0);
    i2c.writeReg(0x77, 0x00);

    //uint32_t dig = 0;
    //uint8_t seg[4];
    //while (true) {
    //    seg[0] = '0' + (dig / 1000) % 10;
    //    seg[1] = '0' + (dig / 100) % 10;
    //    seg[2] = '0' + (dig / 10) % 10;
    //    seg[3] = '0' + (dig / 1) % 10;

    //    i2c.address(0x71);
    //    i2c.write(seg, 4);
    //    i2c.address(0x75);
    //    i2c.write(seg, 4);

    //    dig++;
    //    if (dig == 10000) dig = 0;
    //    std::this_thread::sleep_for(100ms);
    //}

    //asio::io_service ioService;
    //ReceiverComms comms(ioService, [&](std::string str)
    //{
    //    try {
    //        auto json = nlohmann::json::parse(str);
    //    }
    //    catch (std::exception) {
    //        std::cout << "exception :(\n";
    //    }
    //});

    bool readyRed = false;
    bool readyBlue = false;

    //ArenaState state = ArenaState::Fighting;
    //std::unique_ptr<LedAnimation<NUM_LED>> fx = std::make_unique<Fighting<NUM_LED>>();

    ArenaState state = ArenaState::Staging;
    std::unique_ptr<LedAnimation<NUM_LED>> fx = std::make_unique<StagingAnim<NUM_LED>>(readyRed, readyBlue);

    blueButtonLed->write(0);
    redButtonLed->write(0);

    while (!done)
    {
        //ioService.poll();

        if (state == ArenaState::Staging)
        {
            if (!readyRed && redButton->read()) {
                readyRed = true;
                std::cout << "Red ready!\n";
                redButtonLed->write(1);
            }
            if (!readyBlue && blueButton->read()) {
                readyBlue = true;
                std::cout << "Blue ready!\n";
                blueButtonLed->write(1);
            }
            if (readyRed && readyBlue && fx->IsAnimationComplete()) {
                if (dynamic_cast<StagingAnim<NUM_LED>*>(fx.get())) {
                    fx = std::make_unique<WooWoosha<NUM_LED>>();
                    std::cout << "woowoosha\n";
                }
                else {
                    state = ArenaState::Countdown;
                    fx = std::make_unique<XmasTree<NUM_LED>>();
                    std::cout << "Getting ready for countdown.\n";
                }
            }
        }
        else if (state == ArenaState::Countdown)
        {
            if (fx->IsAnimationComplete()) {
                fx = std::make_unique<Fighting<NUM_LED>>();
                state = ArenaState::Fighting;
            }
        }
        else if (state == ArenaState::Fighting)
        {
        }

        auto segmentLed = fx->GetSegmentLed();
        i2c.address(BlueSegmentLedI2cAddress);
        i2c.write(segmentLed.segments, 4);
        i2c.writeReg(0x77, segmentLed.dots);
        i2c.address(RedSegmentLedI2cAddress);
        i2c.write(segmentLed.segments, 4);
        i2c.writeReg(0x77, segmentLed.dots);

        //largeDigits.UpdateSegments(segmentLed);

        leds.Refresh(fx->GetColors());

        std::this_thread::sleep_for(2ms);
    }

    std::cout << "exiting safely.\n";

    return mraa::SUCCESS;
}