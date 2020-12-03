//#include <algorithm>

#include "Rose.h"
#include "Puzzle.h"
#include "Fractal.h"

namespace sqs {



//Puzzle::Puzzle(FractalElement* elements, const glm::ivec2& dimensions, const glm::vec2& pos, int index) 
Puzzle::Puzzle(FractalElement* elements, const glm::ivec2& dimensions, int index) 
   : Entity(GetSprite(), GetObjectSize(), GetBoundingBox(), glm::vec2(GetInitOffset() + GetSpacing() * index, 0.0f)) {
        m_Index = index;
        m_Dimensions = dimensions;
        m_MaxTransformations = 5; //temp: should load this value in from default puzzle data

        //undo icons 
        for(int i = 0; i < m_MaxTransformations; ++i) {
            m_UndoIcons.emplace_back(new UndoIcon(glm::vec2(UndoIcon::s_Margin * ((1 - m_MaxTransformations) / 2.0f + i) + x(), -110.0f), this));
        }

        //make a bunch of 1x1 fractals as starting point for all puzzles
        for(int row = 0; row < dimensions.y; ++row) {
            for(int col = 0; col < dimensions.x; ++col) {
                FractalElement element = elements[row * dimensions.x + col];
                if(element != FractalElement::Block) {
                    glm::vec2 startCoords = BaseFractal::GetCoords({col, row}, 1, m_Dimensions, glm::vec2(x(), y()));
                    m_Fractals.emplace_back(new Fractal<int>((int)element, {col, row}, glm::vec2(startCoords.x, startCoords.y), GetIndex()));
                }
            }
        }
}


Puzzle::~Puzzle() {
    for(BaseFractal* f: m_Fractals) {
        if(f) delete f;
    }

    for(UndoIcon* i: m_UndoIcons) {
        if(i) delete i;
    }
}

void Puzzle::Open() {
    m_IsOpen = true;
}

void Puzzle::Close() {
    m_IsOpen = false;
}

void Puzzle::MoveTo(const glm::vec2& pos) {
    Entity::MoveTo(pos);

    float deltaX = pos.x - x();
    float deltaY = pos.y - y();

    for(BaseFractal* f: m_Fractals) {
        if(f) f->MoveBy(glm::vec2(deltaX, deltaY));
    }

    for(UndoIcon* i: m_UndoIcons) {
        if(i) i->MoveBy(glm::vec2(deltaX, deltaY));
    }
}

void Puzzle::MoveBy(const glm::vec2& shift) {
    Entity::MoveBy(shift);
    for(BaseFractal* f: m_Fractals) {
        if(f) f->MoveBy(shift);
    }

    for(UndoIcon* i: m_UndoIcons) {
        if(i) i->MoveBy(shift);
    }
}


void Puzzle::OnAnimationEnd() {
    Entity::OnAnimationEnd();
    for(BaseFractal* f: m_Fractals) {
        if(f) f->OnAnimationEnd();
    }
    for(UndoIcon* i: m_UndoIcons) {
        if(i) i->OnAnimationEnd();
    }


    if(m_FractalCorners.TopLeft) {
    
        glm::ivec2 index = m_FractalCorners.TopLeft->GetIndex();
        int size = GetFractalSize(m_FractalCorners.TopLeft);

        glm::vec2 coords = BaseFractal::GetCoords(index, size * 2, GetDimensions(), glm::vec2(x(), y()));


        if(size == 1) {
            Fractal<int>* topLeft = dynamic_cast<Fractal<int>*>(m_FractalCorners.TopLeft);
            Fractal<int>* topRight = dynamic_cast<Fractal<int>*>(m_FractalCorners.TopRight);
            Fractal<int>* bottomLeft = dynamic_cast<Fractal<int>*>(m_FractalCorners.BottomLeft);
            Fractal<int>* bottomRight = dynamic_cast<Fractal<int>*>(m_FractalCorners.BottomRight);
            
            glm::imat2 mat{ topLeft->GetSubElementsI({0, 0}),
                            topRight->GetSubElementsI({0, 0}),
                            bottomLeft->GetSubElementsI({0, 0}),
                            bottomRight->GetSubElementsI({0, 0})};

            m_Fractals.emplace_back(new Fractal<glm::imat2>(mat, index, coords, GetIndex()));
        }else if(size == 2) {

            glm::imat2 mat00 = dynamic_cast<Fractal<glm::imat2>*>(m_FractalCorners.TopLeft)->GetSubElementsIMat2({0, 0});
            glm::imat2 mat10 = dynamic_cast<Fractal<glm::imat2>*>(m_FractalCorners.TopRight)->GetSubElementsIMat2({0, 0});
            glm::imat2 mat01 = dynamic_cast<Fractal<glm::imat2>*>(m_FractalCorners.BottomLeft)->GetSubElementsIMat2({0, 0});
            glm::imat2 mat11 = dynamic_cast<Fractal<glm::imat2>*>(m_FractalCorners.BottomRight)->GetSubElementsIMat2({0, 0});

            int arr[16];

            for(int row = 0; row < 2; ++row) {
                for(int col = 0; col < 2; ++col) {
                    arr[(0 + col) + 4 * (0 + row)] = glm::value_ptr(mat00)[row * 2 + col];  
                    arr[(2 + col) + 4 * (0 + row)] = glm::value_ptr(mat10)[row * 2 + col];  
                    arr[(0 + col) + 4 * (2 + row)] = glm::value_ptr(mat01)[row * 2 + col];  
                    arr[(2 + col) + 4 * (2 + row)] = glm::value_ptr(mat11)[row * 2 + col];  
                }
            }

            glm::imat4 mat = glm::make_mat4(arr);

            m_Fractals.emplace_back(new Fractal<glm::imat4>(mat, index, coords, GetIndex()));
        }else{
            assert(false);
        }


        std::vector<BaseFractal*>::iterator topLeft = GetFractalIterator(m_FractalCorners.TopLeft);
        m_Fractals.erase(topLeft);
        delete m_FractalCorners.TopLeft; 
        m_FractalCorners.TopLeft = nullptr;

        std::vector<BaseFractal*>::iterator topRight = GetFractalIterator(m_FractalCorners.TopRight);
        m_Fractals.erase(topRight);
        delete m_FractalCorners.TopRight; 
        m_FractalCorners.TopRight = nullptr;

        std::vector<BaseFractal*>::iterator bottomLeft = GetFractalIterator(m_FractalCorners.BottomLeft);
        m_Fractals.erase(bottomLeft);
        delete m_FractalCorners.BottomLeft; 
        m_FractalCorners.BottomLeft = nullptr;

        std::vector<BaseFractal*>::iterator bottomRight = GetFractalIterator(m_FractalCorners.BottomRight);
        m_Fractals.erase(bottomRight);
        delete m_FractalCorners.BottomRight; 
        m_FractalCorners.BottomRight = nullptr;
    }
}
void Puzzle::OnAnimationUpdate(float t) {
    Entity::OnAnimationUpdate(t);
    for(BaseFractal* f: m_Fractals) {
        if(f) f->OnAnimationUpdate(t);
    }
    for(UndoIcon* i: m_UndoIcons) {
        if(i) i->OnAnimationUpdate(t);
    }
}

void Puzzle::Draw() const {
//    Entity::Draw(); //puzzle doesn't need/have a visual element
    for(BaseFractal* f: m_Fractals) {
        if(f) f->Draw();
    }
    for(int i = 0; i < GetTransformationCount(); ++i) {
        UndoIcon* icon = m_UndoIcons.at(i);
        if(icon) icon->Draw();
    }
}

BaseFractal* Puzzle::GetFractal(const glm::ivec2& index) const {
    for(BaseFractal* f: m_Fractals) {
        if(f->GetIndex() == index) {
            if(Fractal<int>* fractal = dynamic_cast<Fractal<int>*>(f)) {
                if(fractal->Contains(index)) return f;
            }
            if(Fractal<glm::imat2>* fractal = dynamic_cast<Fractal<glm::imat2>*>(f)) {
                if(fractal->Contains(index)) return f;
            }
            if(Fractal<glm::imat4>* fractal = dynamic_cast<Fractal<glm::imat4>*>(f)) {
                if(fractal->Contains(index)) return f;
            }
        }
    }
    return nullptr;
}


void Puzzle::SplitFractal(BaseFractal* fractal) {
    //calcualte start coordinates of subfractals
    glm::ivec2 index = fractal->GetIndex();
    //dynamic cast to find size
    int fractalSize = GetFractalSize(fractal);
    int subFractalSize = fractalSize / 2;
    glm::vec2 startCoords0 = BaseFractal::GetCoordsForTarget(glm::ivec2(index.x, index.y), subFractalSize, index, fractalSize, m_Dimensions, glm::vec2(x(), y()));
    glm::vec2 startCoords1 = BaseFractal::GetCoordsForTarget(glm::ivec2(index.x + subFractalSize, index.y) , subFractalSize, index, fractalSize, m_Dimensions, glm::vec2(x(), y()));
    glm::vec2 startCoords2 = BaseFractal::GetCoordsForTarget(glm::ivec2(index.x, index.y + subFractalSize) , subFractalSize, index, fractalSize, m_Dimensions, glm::vec2(x(), y()));
    glm::vec2 startCoords3 = BaseFractal::GetCoordsForTarget(glm::ivec2(index.x + subFractalSize, index.y + subFractalSize) , subFractalSize, index, fractalSize, m_Dimensions, glm::vec2(x(), y()));


    BaseFractal* fractal0;
    BaseFractal* fractal1;
    BaseFractal* fractal2;
    BaseFractal* fractal3;

    if(subFractalSize == 1) {
        Fractal<glm::imat2>* f = dynamic_cast<Fractal<glm::imat2>*>(fractal);
        fractal0 = new Fractal(f->GetSubElementsI({0, 0}), {index.x, index.y}, 
                               glm::vec2(startCoords0.x, startCoords0.y), GetIndex());
        fractal1 = new Fractal(f->GetSubElementsI({subFractalSize, 0}), {index.x + subFractalSize, index.y}, 
                               glm::vec2(startCoords1.x, startCoords1.y), GetIndex());
        fractal2 = new Fractal(f->GetSubElementsI({0, subFractalSize}), {index.x, index.y + subFractalSize}, 
                               glm::vec2(startCoords2.x, startCoords2.y), GetIndex());
        fractal3 = new Fractal(f->GetSubElementsI({subFractalSize, subFractalSize}), {index.x + subFractalSize, index.y + subFractalSize},
                               glm::vec2(startCoords3.x, startCoords3.y), GetIndex());
    }else if(subFractalSize == 2) {
        Fractal<glm::imat4>* f = dynamic_cast<Fractal<glm::imat4>*>(fractal);
        fractal0 = new Fractal(f->GetSubElementsIMat2({0, 0}), {index.x, index.y}, 
                               glm::vec2(startCoords0.x, startCoords0.y), GetIndex());
        fractal1 = new Fractal(f->GetSubElementsIMat2({subFractalSize, 0}), {index.x + subFractalSize, index.y},
                               glm::vec2(startCoords1.x, startCoords1.y), GetIndex());
        fractal2 = new Fractal(f->GetSubElementsIMat2({0, subFractalSize}), {index.x, index.y + subFractalSize}, 
                               glm::vec2(startCoords2.x, startCoords2.y), GetIndex());
        fractal3 = new Fractal(f->GetSubElementsIMat2({subFractalSize, subFractalSize}), {index.x + subFractalSize, index.y + subFractalSize}, 
                               glm::vec2(startCoords3.x, startCoords3.y), GetIndex());
    }else {
        std::cout << "Subfractal size" << std::endl;
        assert(false);
    }
    
    glm::vec2 endCoords0 = BaseFractal::GetCoords(glm::ivec2(index.x, index.y), subFractalSize, m_Dimensions, glm::vec2(x(), y()));
    glm::vec2 endCoords1 = BaseFractal::GetCoords(glm::ivec2(index.x + subFractalSize, index.y), subFractalSize, m_Dimensions, glm::vec2(x(), y()));
    glm::vec2 endCoords2 = BaseFractal::GetCoords(glm::ivec2(index.x, index.y + subFractalSize), subFractalSize, m_Dimensions, glm::vec2(x(), y()));
    glm::vec2 endCoords3 = BaseFractal::GetCoords(glm::ivec2(index.x + subFractalSize, index.y + subFractalSize), subFractalSize, m_Dimensions, glm::vec2(x(), y()));
    
    fractal0->MoveTo(endCoords0);
    fractal1->MoveTo(endCoords1);
    fractal2->MoveTo(endCoords2);
    fractal3->MoveTo(endCoords3);

    m_Fractals.push_back(fractal0);
    m_Fractals.push_back(fractal1);
    m_Fractals.push_back(fractal2);
    m_Fractals.push_back(fractal3);

    std::vector<BaseFractal*>::iterator iter = GetFractalIterator(fractal);
    m_Fractals.erase(iter);
    delete fractal; 
}


std::vector<BaseFractal*>::iterator Puzzle::GetFractalIterator(BaseFractal* fractal) {
    std::vector<BaseFractal*>::iterator iter = m_Fractals.end();

    for(std::vector<BaseFractal*>::iterator i = m_Fractals.begin(); i < m_Fractals.end(); ++i) {
        if(&(*(*i)) == &(*fractal)) iter = i;
    }

    return iter;
}


BaseFractal* Puzzle::GetClosestFractal(float mousex, float mousey) const {
    BaseFractal* closestF = nullptr;

    for(BaseFractal* f: m_Fractals) {
        if(!closestF) {
            closestF = f;
        }else{
            float closestDis = rose::PointDistance(glm::vec2(closestF->x(), closestF->y()), glm::vec2(mousex, mousey));
            float thisDis = rose::PointDistance(glm::vec2(f->x(), f->y()), glm::vec2(mousex, mousey));
            if(thisDis < closestDis) closestF = f;
        }
    }


    return closestF;
}

FractalCorners Puzzle::FindFractalCorners(float mousex, float mousey) const {
    FractalCorners fc;

    BaseFractal* closestF = GetClosestFractal(mousex, mousey);

    if(!closestF) return fc;


    //determine which corner that fractal belongs to
    glm::ivec2 index = closestF->GetIndex();
    int size = GetFractalSize(closestF);
    BaseFractal* topLeft = nullptr;
    BaseFractal* topRight = nullptr;
    BaseFractal* bottomLeft = nullptr;
    BaseFractal* bottomRight = nullptr;

    if(closestF->x() - mousex < 0) { //closest fractal is on left of mouse
        if(closestF->y() - mousey < 0) { //closest fractal is below mouse
            bottomLeft = closestF;
            bottomRight = GetFractal(glm::ivec2(index.x + size, index.y));
            topLeft = GetFractal(glm::ivec2(index.x, index.y - size));
            topRight = GetFractal(glm::ivec2(index.x + size, index.y - size));
        }else{
            topLeft = closestF;
            topRight = GetFractal(glm::ivec2(index.x + size, index.y));
            bottomLeft = GetFractal(glm::ivec2(index.x, index.y + size));
            bottomRight = GetFractal(glm::ivec2(index.x + size, index.y + size));
        }
    }else{ //closest fractal is on right side of mouse
        if(closestF->y() - mousey < 0) { //closest fractal is below mouse
            bottomRight = closestF;
            bottomLeft = GetFractal(glm::ivec2(index.x - size, index.y));
            topLeft = GetFractal(glm::ivec2(index.x - size, index.y - size));
            topRight = GetFractal(glm::ivec2(index.x, index.y - size));
        }else{
            topRight = closestF;
            topLeft = GetFractal(glm::ivec2(index.x - size, index.y));
            bottomLeft = GetFractal(glm::ivec2(index.x - size, index.y + size));
            bottomRight = GetFractal(glm::ivec2(index.x, index.y + size));
        }
    }

    if(topLeft && GetFractalSize(topLeft) == size) fc.TopLeft = topLeft;
    if(topRight && GetFractalSize(topRight) == size) fc.TopRight = topRight;
    if(bottomLeft && GetFractalSize(bottomLeft) == size) fc.BottomLeft = bottomLeft;
    if(bottomRight && GetFractalSize(bottomRight) == size) fc.BottomRight = bottomRight;

    return fc;
}

void Puzzle::FormFractal(FractalCorners fc) {
    glm::ivec2 targetIndex = fc.TopLeft->GetIndex();

    int subFractalSize = GetFractalSize(fc.TopLeft);
    int targetSize = subFractalSize * 2;

    glm::vec2 topLeftEndCoords = BaseFractal::GetCoordsForTarget(fc.TopLeft->GetIndex(), subFractalSize, targetIndex, targetSize, m_Dimensions, glm::vec2(x(), y()));
    fc.TopLeft->MoveTo(topLeftEndCoords);
    glm::vec2 topRightEndCoords = BaseFractal::GetCoordsForTarget(fc.TopRight->GetIndex(), subFractalSize, targetIndex, targetSize, m_Dimensions, glm::vec2(x(), y()));
    fc.TopRight->MoveTo(topRightEndCoords);
    glm::vec2 bottomLeftEndCoords = BaseFractal::GetCoordsForTarget(fc.BottomLeft->GetIndex(), subFractalSize, targetIndex, targetSize, m_Dimensions, glm::vec2(x(), y()));
    fc.BottomLeft->MoveTo(bottomLeftEndCoords);
    glm::vec2 bottomRightEndCoords = BaseFractal::GetCoordsForTarget(fc.BottomRight->GetIndex(), subFractalSize, targetIndex, targetSize, m_Dimensions, glm::vec2(x(), y()));
    fc.BottomRight->MoveTo(bottomRightEndCoords);

    m_FractalCorners.TopLeft = fc.TopLeft;
    m_FractalCorners.TopRight = fc.TopRight;
    m_FractalCorners.BottomLeft = fc.BottomLeft;
    m_FractalCorners.BottomRight = fc.BottomRight;
}


void Puzzle::SwapFractals(BaseFractal* fractalA, BaseFractal* fractalB) {
    glm::ivec2 indexA = fractalA->GetIndex();
    fractalA->SetIndex(fractalB->GetIndex());
    fractalB->SetIndex(indexA);

    fractalA->MoveTo({fractalB->x(), fractalB->y()});
    fractalB->MoveTo({fractalA->x(), fractalA->y()});

    //how to transform A to get back to original position
    glm::ivec2 newIndexA = fractalA->GetIndex();
    glm::ivec2 newIndexB = fractalB->GetIndex();

    if(newIndexA.y == newIndexB.y) { //horizontal transformation
        if(newIndexA.x < newIndexB.x) m_TransformationStack.push_back({TransformationType::TranslatePosX, fractalA->GetIndex(), GetFractalSize(fractalA)});
        else m_TransformationStack.push_back({TransformationType::TranslateNegX, fractalA->GetIndex(), GetFractalSize(fractalA)});
    }else{ //vertical transformation
        if(newIndexA.y < newIndexB.y) m_TransformationStack.push_back({TransformationType::TranslatePosY, fractalA->GetIndex(), GetFractalSize(fractalA)});
        else m_TransformationStack.push_back({TransformationType::TranslateNegY, fractalA->GetIndex(), GetFractalSize(fractalA)});
    }

}

void Puzzle::RotateFractalCW(BaseFractal* fractal) {
    fractal->RotateBy(-1.5708);
    m_TransformationStack.push_back({TransformationType::RotateCCW, fractal->GetIndex(), GetFractalSize(fractal)});
}

void Puzzle::RotateFractalCCW(BaseFractal* fractal) {
    fractal->RotateBy(1.5708);
    m_TransformationStack.push_back({TransformationType::RotateCW, fractal->GetIndex(), GetFractalSize(fractal)});
}

void Puzzle::ReflectFractalX(BaseFractal* fractal) {
    fractal->ScaleTo({1.0f, -1.0f});
    m_TransformationStack.push_back({TransformationType::ReflectX, fractal->GetIndex(), GetFractalSize(fractal)});
}

void Puzzle::ReflectFractalY(BaseFractal* fractal) {
    fractal->ScaleTo({-1.0f, 1.0f});
    m_TransformationStack.push_back({TransformationType::ReflectY, fractal->GetIndex(), GetFractalSize(fractal)});
}

bool Puzzle::UndoResizeFractals() {
    //temp:
    //if a transformation is needed, call all transformations and return true
    //otherwise just return false
    return false;
}

void Puzzle::UndoTransformation() {

    struct TransformationData tData = m_TransformationStack.back();

    BaseFractal* f = GetFractal(tData.FractalIndex);

    //temp: should only undo transformation after making sure fractal sizes are matching/correct
    if(tData.FractalSize != GetFractalSize(f)) return;

    BaseFractal* otherF;
    glm::ivec2 index = f->GetIndex();

    switch(tData.Transformation) {
        case TransformationType::TranslatePosX:
            otherF = GetFractal(glm::ivec2(index.x + GetFractalSize(f), index.y)); 
            if(otherF && GetFractalSize(f) == GetFractalSize(otherF)) SwapFractals(f, otherF);
            break;
        case TransformationType::TranslateNegX:
            otherF = GetFractal(glm::ivec2(index.x - GetFractalSize(f), index.y)); 
            if(otherF && GetFractalSize(f) == GetFractalSize(otherF)) SwapFractals(f, otherF);
            break;
        case TransformationType::TranslatePosY:
            otherF = GetFractal(glm::ivec2(index.x, index.y + GetFractalSize(f))); 
            if(otherF && GetFractalSize(f) == GetFractalSize(otherF)) SwapFractals(f, otherF);
            break;
        case TransformationType::TranslateNegY:
            otherF = GetFractal(glm::ivec2(index.x, index.y - GetFractalSize(f))); 
            if(otherF && GetFractalSize(f) == GetFractalSize(otherF)) SwapFractals(f, otherF);
            break;
        case TransformationType::RotateCW:
            RotateFractalCW(f);
            break;
        case TransformationType::RotateCCW:
            RotateFractalCCW(f);
            break;
        case TransformationType::ReflectX: 
            ReflectFractalX(f);
            break;
        case TransformationType::ReflectY:
            ReflectFractalY(f);
            break;
    }

    //the undo transformation pushes another transformation onto the transformation stack, 
    //so pop_back is called twice - the first undo removes the transoformation pushed onto the 
    //stack in the switch statement above, and the second is the actual transformation we want to undo
    m_TransformationStack.pop_back();
    m_TransformationStack.pop_back();
}


}
