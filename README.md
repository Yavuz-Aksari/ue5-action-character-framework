# UE5 Action Character Controller (Graybox)

A high-performance, event-driven C++ character controller designed for a Dark Fantasy Action RPG. Built in Unreal Engine 5 with a strict focus on AAA architectural standards, memory safety, and maintainability.

## Core Architecture

* **Zero-Tick Optimization:** The `Tick()` function is completely disabled. All mechanics operate on an Event-Driven architecture using Enhanced Input and `FTimerHandle` for maximum CPU efficiency.
* **Stateless Transition Validation (Guard Clauses):** State changes are strictly validated through stateless `switch-case` guard clauses (`CanChangeMovementState`, `CanChangeActionState`). This prevents physics exploits and blocks invalid transitions (e.g.attacking or dashing while falling).
* **Modern UE5 Memory Management:** 100% compliance with UE5 pointer standards. Utilizes `TObjectPtr<T>` for all `UObject` references, eliminating raw pointers. Integrated comprehensive `nullptr` validation blocks and Editor assertions to prevent dangling pointer memory leaks and runtime crashes.
* **Separation of Concerns:** Movement States (Legs/Physics) and Action States (Upper Body/Combat) are decoupled, allowing modular combinations without logic overlap.

## Controls & Input Bindings

| Action | Input | Trigger Event | Validation |
| :--- | :--- | :--- | :--- |
| **Move** | WASD | Triggered | Active |
| **Camera** | Mouse | Triggered | N/A |
| **Jump** | Spacebar | Started / Completed | Active |
| **Dash** | Shift | Started | Guard Clause + Cooldown |
| **Attack** | Left Mouse | Started | Guard Clause + Timer |
| **Parry** | Q | Started | Guard Clause + Timer |

## Technical Showcase
<video src="showcase.mp4" autoplay loop muted playsinline width="35%"></video>