#ifndef MENU_LAYER_H
#define MENU_LAYER_H

#include "Rose.h"

namespace sqs {

    class Button;
    class PuzzleSet;
    class Puzzle;

    class MenuLayer: public rose::Layer {
        public:
            MenuLayer();
            virtual ~MenuLayer();
            virtual bool Update(double deltaTime, rose::InputType input, const glm::ivec2& mousePos) override;
            Puzzle* GetOpenPuzzle(const std::vector<PuzzleSet*>& puzzleSets);
            virtual void Draw() override;

        private:
            Button* m_QuitButton;
            Button* m_StartButton;
            Button* m_CloseButton;
            PuzzleSet* m_PuzzleSet0;
            PuzzleSet* m_PuzzleSet1;
            PuzzleSet* m_PuzzleSet2;

            std::vector<rose::Entity*> m_Entities;
            std::vector<PuzzleSet*> m_PuzzleSets;

            float m_Parameter {1.0f};
            bool m_Start {false};

            float m_LeftEdge {-240.0f};
            float m_RightEdge {240.0f};
            float m_TopEdge {135.0f};
            float m_BottomEdge {-135.0f};
    };

}


#endif //MENU_LAYER_H