#ifndef ANIMATION_TIMER_H
#define ANIMATION_TIMER_H

#include <math.h>


namespace rose {

class AnimationTimer {
    public:
        AnimationTimer(): m_Parameter(0.0f), m_EndAnimation(false), m_Speed(0.0f) {}
        virtual ~AnimationTimer() {}
        void SetSpeed(float speed) {
            m_Speed = speed;
        }
        void SetParameter(float t) {
            m_Parameter = t;
        }
        void ResetParameter() {
            m_Parameter = 0.0f;
        }
        float GetParameter() const {return m_Parameter;}
        float GetSigmoidParameter() const {return Sigmoid(m_Parameter);}
        float GetSpeed() const {return m_Speed;}
        void Update(double deltaTime) {
            if(m_Parameter >= 1.0f) {
                m_EndAnimation = false;
                return;
            }
            else {
                m_Parameter += m_Speed * deltaTime;
                if(m_Parameter >= 1.0f) m_EndAnimation = true;
                else m_EndAnimation = false;
            }
        }
        bool EndAnimation() const {
            return m_EndAnimation;
        }
        float Sigmoid(float _t) const {
            return 1.0f / (1.0f + exp(-15.0f * (_t - 0.5f)));
        }
    private:
        float m_Parameter;
        bool m_EndAnimation;
        float m_Speed;
};

}


#endif //ANIMATION_TIMER_H
