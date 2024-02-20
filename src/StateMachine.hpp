//
//  StateMachine.hpp
//  Simple game
//
//  Created by M Power on 2019-05-07.
//  Copyright © 2019 M Power. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <memory>
#include <stack>
#include "State.hpp"

namespace mp {
    typedef std::unique_ptr<State> StateRef;
    
    class StateMachine {
    public:
        StateMachine() {}
        ~StateMachine() {}
        
        void AddState(StateRef newState, bool isReplacing = true);
        void RemoveState();
        // Run at start of each loop in Game.cpp
        void ProcessStateChanges();
        StateRef &GetActiveState();
        
    private:
        std::stack<StateRef> states;
        StateRef new_state;
        bool is_removing, is_adding, is_replacing;
    };
}
