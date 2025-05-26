#pragma once

#include <functional>
#include <vector>
#include <algorithm>

namespace Fern
{
    template <typename... Args>
    class Signal{
    public: 
        using ConnectionID = size_t;
        using SlotFunction = std::function<void(Args...)>;
        ConnectionID connect(SlotFunction slot) {
            ConnectionID id = nextID_++;
            slots_.push_back({id, slot});
            return id;
        }
        void emit(Args... args) const {
            for (const auto& slot: slots_){
                slot.second(args...);
            }            
        }
        void disconnect(ConnectionID id) {
            slots_.erase(
                std::remove_if(slots_.begin(), slots_.end(),
                    [id](const auto& pair) { return pair.first == id; }),
                slots_.end()
            );
        }
        private:
            std::vector<std::pair<ConnectionID, SlotFunction>> slots_;
            ConnectionID nextID_ = 0;
    };    
} // namespace Fern
