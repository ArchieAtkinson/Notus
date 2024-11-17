# Demo of Type State Pattern

Taken from Rust, this is a demonstration of using Types create a stateful abstraction.

This example uses GPIO pins, which can be in one of three states:
- Un-configured
- Input
- Output

Using three separate objects that return the other states as a result of an action. Ensuring only the `Gpio<Input>` class get request the current pins state. 

