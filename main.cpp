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
#include "ArenaStates.h"

// opensegment: https://github.com/sparkfun/Serial7SegmentDisplay/wiki/Special-Commands#i2cAddress

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
const int BigSegmentLedI2cAddress = 0x61;




void term(int signum)
{
    done = 1;
}



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

    LedSegment<LedAnimation::N> leds(0);

    mraa::Gpio* redButton = new mraa::Gpio(2);
    if (redButton->dir(mraa::DIR_IN) != mraa::SUCCESS) {
        std::cerr << "Can't set digital pin as input, exiting" << std::endl;
        return MRAA_ERROR_UNSPECIFIED;
    }
    redButton->mode(mraa::MODE_PULLDOWN);

    mraa::Gpio* blueButton = new mraa::Gpio(3);
    if (blueButton->dir(mraa::DIR_IN) != mraa::SUCCESS) {
        std::cerr << "Can't set digital pin as input, exiting" << std::endl;
        return MRAA_ERROR_UNSPECIFIED;
    }
    blueButton->mode(mraa::MODE_PULLDOWN);

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


    //mraa::Gpio sda(18);
    //mraa::Gpio scl(19);
    //sda.mode(mraa::MODE_PULLUP);
    //scl.mode(mraa::MODE_PULLUP);


    //i2c.address(BaseSegmentLedI2cAddress);
    //i2c.writeReg(0x80, RedSegmentLedI2cAddress);
    //return 0;



    i2c.frequency(mraa::I2cMode::I2C_FAST);

    //i2c.address(RedSegmentLedI2cAddress);
    //i2c.writeReg(0x79, 0);
    //i2c.writeReg(0x77, 0x00);

    //i2c.address(BlueSegmentLedI2cAddress);
    //i2c.writeReg(0x79, 0);
    //i2c.writeReg(0x77, 0x00);



    //uint32_t dig = 0;
    //uint8_t seg[4];
    //while (true) {
    //    seg[0] = '0' + (dig / 100) % 10;
    //    seg[1] = '0' + (dig / 10) % 10;
    //    seg[2] = '0' + (dig / 1) % 10;

    //    i2c.address(BigSegmentLedI2cAddress);
    //    i2c.write(seg, 3);

    //    dig++;
    //    if (dig == 1000) dig = 0;
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
    std::unique_ptr<LedAnimation> fx = GetAnimation(ArenaState::Staging);

    blueButtonLed->write(0);
    redButtonLed->write(0);

    while (!done)
    {
        //ioService.poll();

        fx->Tick(readyRed, readyBlue);

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
                fx = GetAnimation(ArenaState::PreCountdown);
                state = ArenaState::PreCountdown;
                std::cout << "woowoosha\n";
            }
        }
        else if (state == ArenaState::PreCountdown)
        {
            if (fx->IsAnimationComplete()) {
                fx = GetAnimation(ArenaState::Countdown);
                state = ArenaState::Countdown;
                std::cout << "Getting ready for countdown.\n";
            }
        }
        else if (state == ArenaState::Countdown)
        {
            if (fx->IsAnimationComplete()) {
                fx = GetAnimation(ArenaState::Fighting);
                state = ArenaState::Fighting;
            }
        }
        else if (state == ArenaState::Fighting)
        {
        }

        auto segmentLed = fx->GetSegmentLed();
        i2c.address(BlueSegmentLedI2cAddress);
        i2c.write((uint8_t*)segmentLed.segments, 4);
        i2c.writeReg(0x77, segmentLed.dots);
        i2c.address(RedSegmentLedI2cAddress);
        i2c.write((uint8_t*)segmentLed.segments, 4);
        i2c.writeReg(0x77, segmentLed.dots);

        //largeDigits.UpdateSegments(segmentLed);
        i2c.address(BigSegmentLedI2cAddress);
        i2c.write((uint8_t*)segmentLed.segments, 3);

        leds.Refresh(fx->GetColors());

        std::this_thread::sleep_for(2ms);
    }

    std::cout << "exiting safely.\n";

    return mraa::SUCCESS;
}