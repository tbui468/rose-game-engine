#ifndef INPUT_TYPE_H
#define INPUT_TYPE_H

namespace rose {

enum class InputType {
    None,
    Quit,
    LeftTap, 
    RightTap,
    DoubleTap, 
    FlickLeft,
    FlickRight,
    FlickUp,
    FlickDown
};

}

#endif //INPUT_TYPE
