#pragma once

#include <cmath>

namespace Tangram {

struct FadeEffect {

public:

    enum class Interpolation {
        linear, pow, sine
    };

    FadeEffect() {}

    FadeEffect(bool _in, Interpolation _interpolation, float _duration)
        : m_interpolation(_interpolation), m_duration(_duration), m_in(_in)
    {}

    float update(float _dt) {
        m_step += _dt;
        float st = m_step / m_duration;

        switch (m_interpolation) {
            case Interpolation::linear:
                return m_in ? st : -st + 1;
            case Interpolation::pow:
                return m_in ? st * st : -(st * st) + 1;
            case Interpolation::sine:
                return m_in ? sin(st * M_PI * 0.5) : cos(st * M_PI * 0.5);
        }

        return st;
    }

    bool isFinished() {
        return m_step > m_duration;
    }

private:

    Interpolation m_interpolation = Interpolation::linear;
    float m_duration = 0.5;
    float m_step = 0.0;
    bool m_in;
};

}
