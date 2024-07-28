#pragma once

#include <set>
#include <memory>

namespace Utility {
	/* Base Unit : Seconds */
	struct STime {
	public:
		STime(const STime& other) : Time(other.Time) {}
		STime(float time = 0.f) : Time(time) {}

		inline operator float() const { return this->Time; }

		inline const float Seconds()		const { return Time; }
		inline const float MilliSeconds()	const { return Time * 1000.f; }
		inline const float MicroSeconds()	const { return Time * 100000000.f; }
		inline const float NanoSeconds()	const { return Time * 1000000000.f; }

	private:
		float Time;
	};

	class CallOnce {
	public:
		template<typename Callable, typename ...Args>
		void Call(Callable&& func, Args&&... args) {
			if (bCalled) { 
				return; 
			}
			std::forward<Callable>(func)(std::forward<Args>(args)...);
			bCalled = true;
		}

		void Reset() noexcept {
			bCalled = false;
		}

	private:
		bool bCalled = false;
	};

	class State {
	public:
		State() = default;

		State(float seconds) : Duration(seconds) {}
	protected:
		virtual void OnBegin() 				 {}
		virtual void OnTick(STime deltatime) { Elapsed += deltatime.Seconds(); };
		virtual void OnEnd() 				 { Elapsed = 0.f; }
		virtual bool Finished() const 		 { return ((Duration - Elapsed) <= 0.f);}

		float Duration		= INFINITY;
		float Elapsed		= 0.f;
	private:
		size_t Index		= 0;
		size_t Transition	= 0;
	private:
		friend class StateMachine;
	}; 

	class StateMachine {
	public:
		StateMachine() = default;
		virtual ~StateMachine() {}

		void CreateTransition(std::shared_ptr<State> from, std::shared_ptr<State> to);
		void OnTick(STime deltatime);

	private:
		void Transition();

		std::set<std::shared_ptr<State>> m_States;
		std::shared_ptr<State> m_CurrentState;
	};
}

namespace Utility {

	void StateMachine::CreateTransition(std::shared_ptr<State> from, std::shared_ptr<State> to) {
		auto [it_to, inserted_to] = m_States.insert(to);
		if (inserted_to) {
			to->Index = m_States.size();
		}

		auto [it_from, inserted_from] = m_States.insert(from);
		if (inserted_from) {
			from->Index = m_States.size();
		}

		if (!m_CurrentState) {
			m_CurrentState = from; // Initialize root node as first from.
		}

		from->Transition = to->Index;
	}

	void StateMachine::OnTick(STime deltatime) {
		if (m_CurrentState) {
			static CallOnce on_begin_call;
			on_begin_call.Call([&](){ 
				m_CurrentState->OnBegin(); 
			});
			m_CurrentState->OnTick(deltatime);
			if (m_CurrentState->Finished()) {
				m_CurrentState->OnEnd();
				on_begin_call.Reset();
				Transition();
			}
		}
	}

	void StateMachine::Transition() {
		auto next = m_CurrentState->Transition;
		auto it = std::find_if(m_States.begin(), m_States.end(), [next](const std::shared_ptr<State>& state) {
			return state->Index == next;
		});

		if (it != m_States.end()) {
			m_CurrentState = *it;
		}
		else {
			m_CurrentState = nullptr; // No valid transition found
		}
	}

	
}