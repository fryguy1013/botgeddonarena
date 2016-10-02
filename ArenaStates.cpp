
#include "ArenaStates.h"

#include "ColorTriplet.h"
#include "SegmentLedState.h"

#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <iostream>

namespace {
    const int MaxLightCounts = 120;
    const int MidLightCounts = 60;
}

namespace std {
    template<class T>
    const T& clamp(const T& x, const T& lower, const T& upper) {
        return std::min(upper, std::max(x, lower));
    }
}

float tween(float t, float start, float end)
{
    return std::clamp((t - start) / (end - start), 0.0f, 1.0f);
}

class WooWoosha : public LedAnimation
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

    void Tick(WorldState& world) override
    {
    }

    std::array<ColorTriplet, N> GetColors() override
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
            auto val = static_cast<uint8_t>(std::min(std::max(juice, minBrightness), maxBrightness) * MaxLightCounts);
            ret[i] = ColorTriplet{ val, val, val };
        }

        return ret;
    }

    SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{ { ' ',' ',' ',' ' }, 0 };
    }

    std::array<bool, 2> GetButtonLeds() override
    {
        return{ false, false };
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> _start;
    int _lastSegmentIdx;
    float _lastSegmentTime;
    float _speed;
    int _dir;
    std::array<float, N> _lastColorTime;
};

class PreCountdownAnimation : public LedAnimation
{
    static constexpr float fadeUpStart = 3.0f;
    static constexpr float fadeUpEnd = 4.5f;
    static constexpr float fadeDownStart = 5.0f;
    static constexpr float fadeDownEnd = 6.0f;
    static constexpr float animEnd = 7.0f;
    static constexpr float particleSpeed = 0.02f;

    struct Particle
    {
        int x;
        int y;
        float startTime;
        float age;
        float speed;
    };

public:
    PreCountdownAnimation(StateMachine* state) :
        _state(state),
        _lastSegmentTime(0),
        _speed(0.700f)
    {
        _start = std::chrono::steady_clock::now();
        _lastColorTime.fill(-100);

        _rng = std::default_random_engine(std::random_device()());
    }

    void Tick(WorldState& world) override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        if (duration.count() > animEnd)
            _state->ChangeState(ArenaState::Countdown, world);
    }

    std::array<ColorTriplet, N> GetColors() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        while (curTime > _lastSegmentTime)
        {
            _particles.push_back(Particle
            {
                std::uniform_int_distribution<int>(0, NumHorizontal - 1)(_rng),
                std::uniform_int_distribution<int>(-NumVertical / 4, NumVertical / 2 - 1)(_rng),
                _lastSegmentTime,
                0,
                std::max(_speed * 0.1f, particleSpeed)
            });

            _lastSegmentTime += _speed;
            if (_speed > 0.100f)
                _speed *= 0.70f;
        }

        for (auto& particle : _particles) {
            while (curTime > particle.startTime + particle.speed) {
                particle.y++;
                particle.startTime += particle.speed;
                particle.age += particle.speed;
            }

            if (particle.y < NumVertical && particle.y >= 0) {
                _lastColorTime[particle.y + particle.x * NumVertical] = curTime - particle.age * 0.3f;
            }
        }

        float minBrightness = tween(curTime, fadeUpStart, fadeUpEnd);
        float maxBrightness = 1.0f - tween(curTime, fadeDownStart, fadeDownEnd);

        std::array<ColorTriplet, N> ret;
        for (int i = 0; i < N; i++) {
            auto timeSince = curTime - _lastColorTime[i];
            auto juice = std::clamp(1.0f - (timeSince + 0.1f), 0.0f, 1.0f) * 0.9f;
            auto val = static_cast<uint8_t>(std::min(std::max(juice, minBrightness), maxBrightness) * MaxLightCounts);
            ret[i] = ColorTriplet{ val, val, val };
        }

        return ret;
    }

    SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{ { ' ',' ',' ',' ' }, 0 };
    }

    std::array<bool, 2> GetButtonLeds() override
    {
        return{ false, false };
    }

private:
    StateMachine* _state;
    std::chrono::time_point<std::chrono::steady_clock> _start;
    float _lastSegmentTime;
    float _speed;
    std::array<float, N> _lastColorTime;
    std::vector<Particle> _particles;

    std::default_random_engine _rng;
};

constexpr float PreCountdownAnimation::particleSpeed;


class XmasTree : public LedAnimation
{
public:
    XmasTree(StateMachine* state) :
        _state(state)
    {
        _start = std::chrono::steady_clock::now();

        _dootRegion.fill(0);
        const int regionPadding = 3;
        const int outSizeSpacing = 5;
        const int numRegions = 3;
        const int regionSize = (NumVertical - outSizeSpacing - ((numRegions - 1) * regionPadding)) / numRegions;
        for (int x = 0; x < NumHorizontal; x++)
        {
            int startPx = (NumVertical / 2) - (regionSize * numRegions + regionPadding * (numRegions - 1)) / 2;
            for (int i = 0; i < numRegions; i++)
            {
                for (int j = 0; j < regionSize; j++)
                    _dootRegion[x * NumVertical + startPx + j] = 3 - i;
                startPx += regionSize + regionPadding;
            }
        }
    }

    void Tick(WorldState& world) override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        if (duration.count() > 3.0f)
            _state->ChangeState(ArenaState::Fighting, world);
    }

    std::array<ColorTriplet, N> GetColors() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        std::array<ColorTriplet, N> ret;
        float timeSinceDoot = std::fmod(curTime, 1.0f) * 1.2f;
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

    SegmentLedState GetSegmentLed() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();
        if (curTime > 2)
            return SegmentLedState{ { ' ',' ','1',' ' }, 0 };
        if (curTime > 1)
            return SegmentLedState{ { ' ','2',' ',' ' }, 0 };

        return SegmentLedState{ { '3',' ',' ',' ' }, 0 };
    }

    std::array<bool, 2> GetButtonLeds() override
    {
        return{ false, false };
    }

private:
    StateMachine* _state;
    std::chrono::time_point<std::chrono::steady_clock> _start;
    std::array<int, N> _dootRegion;
};

class Fighting : public LedAnimation
{
public:
    Fighting(StateMachine* state, WorldState& world) :
        _state(state)
    {
        _start = std::chrono::steady_clock::now();
        _end = _start + std::chrono::milliseconds(static_cast<int>(world.timeRemaining * 1000));
    }

    void Tick(WorldState& world) override
    {
        if (world.redButton || world.blueButton)
            _state->ChangeState(ArenaState::End, world);
        if (world.refButton) {
            std::chrono::duration<float> duration = _end - std::chrono::steady_clock::now();
            world.timeRemaining = duration.count();
            _state->ChangeState(ArenaState::Paused, world);
        }
    }

    std::array<ColorTriplet, N> GetColors() override
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

    SegmentLedState GetSegmentLed() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        //if (curTime < 1.0f)
        //    return SegmentLedState{ { 'F','I','G','T' }, 0 };

        std::chrono::duration<float> remDuration = _end - std::chrono::steady_clock::now();
        int remTime = static_cast<int>(ceil(remDuration.count()));

        if (remTime > 0)
        {
            return SegmentLedState {
                {
                    '0' + static_cast<char>(remTime / 60),
                    '0' + static_cast<char>(remTime % 60) / 10,
                    '0' + static_cast<char>(remTime % 10),
                    ' '
                },
                static_cast<uint8_t>(1)
            };
        }

        return SegmentLedState{ { 'E','n','d',' ' }, 0 };
    }

    std::array<bool, 2> GetButtonLeds() override
    {
        return{ true, true };
    }

private:
    StateMachine* _state;
    std::chrono::time_point<std::chrono::steady_clock> _start;
    std::chrono::time_point<std::chrono::steady_clock> _end;
};

class StagingAnim : public LedAnimation
{
    static constexpr float fadeOutStart = 1.5f;
    static constexpr float fadeOutDuration = 1.5f;
    static constexpr float totalAnimFromLastButton = 3.5f;

public:
    StagingAnim(StateMachine* state) :
        _state(state)
    {
        _start = std::chrono::steady_clock::now();
        _blueStart = std::chrono::time_point<std::chrono::steady_clock>::max();
        _redStart = std::chrono::time_point<std::chrono::steady_clock>::max();
    }

    void Tick(WorldState& world) override
    {
        auto now = std::chrono::steady_clock::now();
        if (now < _redStart && world.redButton)
            _redStart = now;
        if (now < _blueStart && world.blueButton)
            _blueStart = now;

        std::chrono::duration<float> redDuration = std::chrono::steady_clock::now() - _redStart;
        std::chrono::duration<float> blueDuration = std::chrono::steady_clock::now() - _blueStart;
        if (redDuration.count() > totalAnimFromLastButton && blueDuration.count() > totalAnimFromLastButton)
            _state->ChangeState(ArenaState::PreCountdown, world);
    }

    std::array<ColorTriplet, N> GetColors() override
    {
        auto now = std::chrono::steady_clock::now();

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

    SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{ { 's','t','r','t' }, 0 };
    }

    std::array<bool, 2> GetButtonLeds() override
    {
        return { _redStart == std::chrono::time_point<std::chrono::steady_clock>::max(),
            _blueStart == std::chrono::time_point<std::chrono::steady_clock>::max() };
    }

private:
    StateMachine* _state;
    std::chrono::time_point<std::chrono::steady_clock> _start;
    std::chrono::time_point<std::chrono::steady_clock> _blueStart;
    std::chrono::time_point<std::chrono::steady_clock> _redStart;
};

class PreStagingAnim : public LedAnimation
{
    static constexpr float fadeOutStart = 1.5f;

public:
    PreStagingAnim(StateMachine* state) :
        _state(state)
    {
        _start = std::chrono::steady_clock::now();
        _startingTime = std::chrono::time_point<std::chrono::steady_clock>::max();
    }


    void Tick(WorldState& world) override
    {
        auto now = std::chrono::steady_clock::now();
        if (now < _startingTime && world.refButton)
            _startingTime = now;

        std::chrono::duration<float> startingDuration = std::chrono::steady_clock::now() - _startingTime;
        if (startingDuration.count() > fadeOutStart) {
            _state->ChangeState(ArenaState::Staging, world);
            world.timeRemaining = 120;
        }
    }

    std::array<ColorTriplet, N> GetColors() override
    {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> duration = now - _start;
        float curTime = duration.count();

        float timeSinceDoot = curTime / 3.0f;
        float dootiness = std::clamp(timeSinceDoot / fadeOutStart, 0.0f, 1.0f);

        std::chrono::duration<float> startingDuration = now - _startingTime;
        float startingTime = startingDuration.count();
        if (startingTime > 0)
            dootiness = std::clamp(1.0f - startingTime / fadeOutStart, 0.0f, 1.0f);

        std::array<ColorTriplet, N> ret;
        for (int i = 0; i < N; i++) {
            auto val = static_cast<uint8_t>(dootiness * MidLightCounts);
            ret[i] = ColorTriplet{ val, val, val };
        }

        return ret;
    }

    SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{};
    }

    std::array<bool, 2> GetButtonLeds() override
    {
        return { false, false };
    }

private:
    StateMachine* _state;
    std::chrono::time_point<std::chrono::steady_clock> _start;
    std::chrono::time_point<std::chrono::steady_clock> _startingTime;
};

class PausedAnim : public LedAnimation
{
public:
    PausedAnim(StateMachine* state) :
        _state(state)
    {
        _start = std::chrono::steady_clock::now();
        _releasedPause = false;
    }

    void Tick(WorldState& world) override
    {
        _remTime = static_cast<int>(ceil(world.timeRemaining));
            
        if (!world.refButton)
            _releasedPause = true;
        if (world.refButton && _releasedPause)
            _state->ChangeState(ArenaState::Countdown, world);

        if (world.resetButton && _releasedPause)
            _state->ChangeState(ArenaState::PreStaging, world);
    }

    std::array<ColorTriplet, N> GetColors() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        float timeSinceDoot = curTime / 3.0f;
        float dootiness = std::clamp(1.0f - timeSinceDoot, 0.0f, 1.0f);

        std::array<ColorTriplet, N> ret;
        for (int i = 0; i < N; i++) {
            auto val = static_cast<uint8_t>(dootiness * MaxLightCounts);
            ret[i] = ColorTriplet{ val, val, 0 };
        }

        return ret;
    }

    SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{
            {
                '0' + static_cast<char>(_remTime / 60),
                '0' + static_cast<char>(_remTime % 60) / 10,
                '0' + static_cast<char>(_remTime % 10),
                ' '
            },
            static_cast<uint8_t>(1)
        };
    }

    std::array<bool, 2> GetButtonLeds() override
    {
        return { false, false };
    }

private:
    StateMachine* _state;
    std::chrono::time_point<std::chrono::steady_clock> _start;
    bool _releasedPause;
    int _remTime;
};

class End : public LedAnimation
{
public:
    End(StateMachine* state) :
        _state(state)
    {
        _start = std::chrono::steady_clock::now();
    }


    void Tick(WorldState& world) override
    {
        if (world.resetButton)
            _state->ChangeState(ArenaState::PreStaging, world);
    }

    std::array<ColorTriplet, N> GetColors() override
    {
        std::chrono::duration<float> duration = std::chrono::steady_clock::now() - _start;
        float curTime = duration.count();

        float timeSinceDoot = curTime / 3.0f;
        float dootiness = std::clamp(1.0f - timeSinceDoot, 0.0f, 1.0f);

        std::array<ColorTriplet, N> ret;
        for (int i = 0; i < N; i++) {
            auto val = static_cast<uint8_t>(dootiness * MaxLightCounts);
            ret[i] = ColorTriplet{ val, val, 0 };
        }

        return ret;
    }

    SegmentLedState GetSegmentLed() override
    {
        return SegmentLedState{};
    }

    std::array<bool, 2> GetButtonLeds() override
    {
        return { false, false };
    }

private:
    StateMachine* _state;
    std::chrono::time_point<std::chrono::steady_clock> _start;
};


void StateMachine::ChangeState(ArenaState state, WorldState& world)
{
    _state = GetAnimation(state, world);
}

std::unique_ptr<LedAnimation> StateMachine::GetAnimation(ArenaState state, WorldState& world)
{
    switch (state)
    {
    case ArenaState::PreStaging:
        return std::make_unique<PreStagingAnim>(this);
    case ArenaState::Staging:
        return std::make_unique<StagingAnim>(this);
    case ArenaState::PreCountdown:
        return std::make_unique<PreCountdownAnimation>(this);
    case ArenaState::Countdown:
        return std::make_unique<XmasTree>(this);
    case ArenaState::Fighting:
        return std::make_unique<Fighting>(this, world);
    case ArenaState::Paused:
        return std::make_unique<PausedAnim>(this);
    case ArenaState::End:
    default:
        return std::make_unique<End>(this);
    }
}
