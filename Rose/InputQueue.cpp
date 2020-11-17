#include "InputQueue.h"

#include <SDL_events.h>

namespace rose {


void InputQueue::PollEvents() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT: 
                m_Inputs.push(InputType::Close);
                break;
            case SDL_MOUSEBUTTONDOWN: 
                if(event.button.button == SDL_BUTTON_LEFT)
                    m_Inputs.push(InputType::LeftTap);
                else if(event.button.button == SDL_BUTTON_RIGHT)
                    m_Inputs.push(InputType::RightTap);
                break;
        }
    }
}


InputType InputQueue::NextInput(){
    InputType input = m_Inputs.front();
    m_Inputs.pop();
    return input;
}


glm::ivec2 InputQueue::GetMouseCoords() {
    glm::ivec2 mouseCoords;
    SDL_GetMouseState(&mouseCoords.x, &mouseCoords.y);
    mouseCoords.x -= m_Window->GetWidth() / 2.0f;
    mouseCoords.y -= m_Window->GetHeight() / 2.0f;
    mouseCoords.y *= -1.0f;
    return mouseCoords;
}

}