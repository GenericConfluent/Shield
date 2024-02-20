#include "StateMachine.hpp"

namespace mp {
    void StateMachine::AddState(StateRef newState, bool isReplacing) {
        this->is_adding = true;
        this->is_replacing = isReplacing;
        this->new_state = std::move(newState);
    }
    
    void StateMachine::RemoveState() {
        this->is_removing = true;
    }
    
    void StateMachine::ProcessStateChanges() {
        if (this->is_removing && !this->states.empty()) {
            this->states.pop();
            
            if (!this->states.empty()) {
                this->states.top()->resume();
            }
            
            this->is_removing = false;
        }
        
        if (!this->is_adding) return;
        if (!this->states.empty()) {
            if (this->is_replacing) {
                this->states.pop();
            } else {
                this->states.top()->pause();
            }
        }
        
        this->states.push(std::move(this->new_state));
        this->states.top()->init();
        this->is_adding = false;
    }
    
    StateRef& StateMachine::GetActiveState() {
        return this->states.top();
    }
}
