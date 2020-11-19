#include "MenuLayer.h"
#include "Button.h"
#include "PuzzleSet.h"
#include "Puzzle.h"
#include "PuzzleIcon.h"
#include "Fractal.h"

namespace sqs {



    MenuLayer::MenuLayer(): Layer() {
        const rose::Sprite startSprite = {{0, 96}, {64, 32}};
        const rose::Sprite quitSprite = {{64, 0}, {64, 32}};
        const rose::Sprite closeSprite = {{96, 96}, {32, 32}};
        const rose::Sprite puzzleSetSprite = {{0, 0}, {32, 32}};

        const glm::vec2 size = glm::vec2(64.0f, 32.0f);
        const glm::vec2 smallsize = glm::vec2(32.0f, 32.0f);
        const glm::vec2 tinysize = glm::vec2(16.0f, 16.0f);
        const glm::vec4 boundingBox = glm::vec4(0.0f, 0.0f, 64.0f, 32.0f);
        const glm::vec4 smallboundingBox = glm::vec4(0.0f, 0.0f, 32.0f, 32.0f);
        const glm::vec4 tinyboundingBox = glm::vec4(0.0f, 0.0f, 16.0f, 16.0f);

        m_StartButton = new Button(startSprite, size, boundingBox, glm::vec2(0.0f, 32.0f));
        m_StartButton->SetAnimationCoords(glm::vec2(0.0f, 32.0f), glm::vec2(m_RightEdge + 96.0f, 32.0f));
        m_QuitButton = new Button(quitSprite, size, boundingBox, glm::vec2(0.0f, -32.0f));
        m_QuitButton->SetAnimationCoords(glm::vec2(0.0f, -32.0f), glm::vec2(m_LeftEdge - 96.0f, -32.0f));
        m_CloseButton = new Button(closeSprite, tinysize, tinyboundingBox, glm::vec2(m_RightEdge + 16.0f, m_TopEdge - 16.0f));
        m_CloseButton->SetAnimationCoords(glm::vec2(m_RightEdge - 16.0f, m_TopEdge - 16.0f), glm::vec2(m_RightEdge + 16.0f, m_TopEdge - 16.0f));


        m_PuzzleSet0 = new PuzzleSet(puzzleSetSprite, smallsize, smallboundingBox, glm::vec2(-32.0f, m_TopEdge + 32.0f));
        m_PuzzleSet1 = new PuzzleSet(puzzleSetSprite, smallsize, smallboundingBox, glm::vec2(0.0f, m_TopEdge + 32.0f));
        m_PuzzleSet2 = new PuzzleSet(puzzleSetSprite, smallsize, smallboundingBox, glm::vec2(32.0f, m_TopEdge + 32.0f));

        m_Entities.push_back((rose::Entity*)m_StartButton);
        m_Entities.push_back((rose::Entity*)m_QuitButton);
        m_Entities.push_back((rose::Entity*)m_CloseButton);
        m_Entities.push_back((rose::Entity*)m_PuzzleSet0);
        m_Entities.push_back((rose::Entity*)m_PuzzleSet1);
        m_Entities.push_back((rose::Entity*)m_PuzzleSet2);

        m_PuzzleSets.push_back(m_PuzzleSet0);
        m_PuzzleSets.push_back(m_PuzzleSet1);
        m_PuzzleSets.push_back(m_PuzzleSet2);

    }

    MenuLayer::~MenuLayer() {
        if(m_QuitButton) delete m_QuitButton;
        if(m_StartButton) delete m_StartButton;
        if(m_CloseButton) delete m_CloseButton;
        if(m_PuzzleSet0) delete m_PuzzleSet0;
        if(m_PuzzleSet1) delete m_PuzzleSet1;
        if(m_PuzzleSet2) delete m_PuzzleSet2;

        for(rose::Entity* e: m_Entities) delete e;
        for(PuzzleSet* ps: m_PuzzleSets) delete ps;
    }

    void MenuLayer::SetAnimationStart() {
        m_Parameter = 0.0f;
        m_Start = true;
    }

    void MenuLayer::OpenPuzzleSetMenu() {
        m_StartButton->GoAway();
        m_StartButton->ScaleTo(glm::vec2(2.0f, 1.0f));
        m_QuitButton->GoAway();
        m_QuitButton->ScaleTo(glm::vec2(1.0f, 2.0f));
        m_CloseButton->ComeBack();
        for(PuzzleSet* ps : m_PuzzleSets) ps->MoveTo(glm::vec2(ps->x(), 0.0f));
        SetAnimationStart();
    }

    void MenuLayer::OpenMainMenu() {
        m_StartButton->ComeBack();
        m_StartButton->ScaleTo(glm::vec2(1.0f, 1.0f));
        m_QuitButton->ComeBack();
        m_QuitButton->ScaleTo(glm::vec2(1.0f, 1.0f));
        m_CloseButton->GoAway();
        for(PuzzleSet* ps : m_PuzzleSets) ps->MoveTo(glm::vec2(ps->x(), m_TopEdge + 32.0f));
        SetAnimationStart();
    }

    void MenuLayer::ClosePuzzleSet(PuzzleSet* openPuzzleSet) {
        for(PuzzleSet* ps : m_PuzzleSets) ps->MoveTo(glm::vec2(ps->x(), 0.0f));
        openPuzzleSet->Close();
        SetAnimationStart();
    }

    void MenuLayer::OpenPuzzleSet(PuzzleSet* puzzleSet) {
        puzzleSet->Open();
        for(PuzzleSet* ps : m_PuzzleSets) ps->MoveTo(glm::vec2(ps->x(), m_TopEdge + 32.0f));
        SetAnimationStart();
    }

    void MenuLayer::OpenPuzzle(Puzzle* puzzle) {
        PuzzleSet* puzzleSet = GetOpenPuzzleSet();
        puzzleSet->OpenPuzzle(puzzle);
        SetAnimationStart();
    }

    bool MenuLayer::Update(double deltaTime, rose::InputType rawInput, const glm::ivec2& mousePos) {
        rose::InputType input = rawInput;
        //process raw inputs here to accept double taps/flicks

        glm::vec2 mouse = {static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};

        if(m_Parameter <1.0f) input = rose::InputType::None; //temp: to avoid retting animations


        //process inputs - return true to quit application
        if(m_QuitButton->LeftTap(input, mouse.x, mouse.y) || input == rose::InputType::Quit) return true;

        if(m_StartButton->LeftTap(input, mouse.x, mouse.y)) {
            OpenPuzzleSetMenu();
        }


        if(m_CloseButton->LeftTap(input, mouse.x, mouse.y)) {
            PuzzleSet* openPuzzleSet = GetOpenPuzzleSet();

            if(openPuzzleSet) {
                ClosePuzzleSet(openPuzzleSet);
            }else{
                OpenMainMenu();
            }

        }


        for(PuzzleSet* ps : m_PuzzleSets) {
            if(ps->LeftTap(input, mouse.x, mouse.y)) {
                OpenPuzzleSet(ps);
                break;
            }
            for(PuzzleIcon* icon: ps->GetPuzzleIcons()) {
                if(icon->LeftTap(input, mouse.x, mouse.y)) {
                    OpenPuzzle(icon->GetPuzzle());
                    break;
                }
            }


        }

        if(Puzzle* puzzle = GetOpenPuzzle(m_PuzzleSets)) {
            for(Fractal* fractal: puzzle->GetFractals()) {
                if(fractal->LeftFlick(input, mouse.x, mouse.y)) {
                    glm::ivec2 index = fractal->GetIndex();
                    Fractal* otherFractal = puzzle->GetFractal(glm::ivec2(index.x - 1, index.y)); //to get fractal to the left of current
                    if(otherFractal) {
                        puzzle->SwapFractals(fractal, otherFractal);
                        SetAnimationStart();
                        break;
                    }
                }
            } 
        }



        if(m_Parameter < 1.0f && m_Start) {
            m_Parameter += static_cast<float>(deltaTime) * .0010f;
            if(m_Parameter >= 1.0f) {
                for(rose::Entity* e: m_Entities) e->OnAnimationEnd();
            }else{
                for(rose::Entity* e: m_Entities) e->OnAnimationUpdate(Sigmoid(m_Parameter));
            }
        }

        return false;
    }


    Puzzle* MenuLayer::GetOpenPuzzle(const std::vector<PuzzleSet*>& puzzleSets) const {
        PuzzleSet* puzzleSet = GetOpenPuzzleSet();
        if(puzzleSet) {
            for(Puzzle* puzzle: puzzleSet->GetPuzzles()) {
                if(puzzle && puzzle->IsOpen()) return puzzle;
            }
        }

        return nullptr;
    }


    PuzzleSet* MenuLayer::GetOpenPuzzleSet() const {
        for(PuzzleSet* ps: m_PuzzleSets) {
            if(ps && ps->IsOpen()) return ps;
        }
        return nullptr;
    }


    void MenuLayer::Draw() {
        for(rose::Entity* e: m_Entities) {
            e->Draw();
        }
    }

    float MenuLayer::Sigmoid(float t) const {
        return 1.0f / (1.0f + exp(-15.0f * (t - 0.5f)));
    }


}
