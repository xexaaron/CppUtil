#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <cstring>
#include <cassert>

namespace Utility {

    class UndoActionBase {
    public:
        virtual void Undo() = 0;
        virtual void Redo() = 0;
        virtual ~UndoActionBase() = default; // Virtual destructor
    };

    template <typename T>
    class UndoAction : public UndoActionBase {
    public:
        UndoAction(T* data) :
            Original(*data), 
            Data(data) 
        {
        #ifndef NDEBUG
            assert(data && "UndoAction Data is null");
        #endif //  NDEBUG
        }

        virtual void Undo() override {
        #ifndef NDEBUG
            assert(data && "UndoAction Data is null");
        #endif //  NDEBUG
            if (Data) {
                T temp = *Data; 
                *Data = Original;
                Original = temp;
            }
        }

        virtual void Redo() override {
            Undo();
        }

    private:
        T Original;
        T* Data;
    };

    class UndoStack {
    public:
        void Push(const std::shared_ptr<UndoActionBase>& action) {
            m_Actions.push_back(action);
        }

        template <typename T, typename ...Args>
        void Push(Args&&... args) {
            m_Actions.push_back(std::make_shared<UndoAction<T>>(std::forward<Args>(args)...));
        }

        void Pop(bool undo = true) {
            if (!m_Actions.empty()) {
                if (undo) {
                    m_Actions.back()->Undo();
                } else {
                    m_Actions.back()->Redo();
                }
                m_Actions.pop_back();
            }
        }

        void Clear() {
            m_Actions.clear();
        }

        void Flush(bool undo = true) {
            if (undo) {
                for (auto& action : m_Actions) {
                    action->Undo();
                }
            } else {
                for (auto& action : m_Actions) {
                    action->Redo();
                }
            }
            m_Actions.clear();
        }

    private:
        std::vector<std::shared_ptr<UndoActionBase>> m_Actions;
    };

} 