# Workspace wheel for i3
A quick hack to add list of workspaces to the center of each monitor as a wheel.
![alt text](http://i.imgur.com/BHZI2f7.gif "Logo Title Text 1")

## Installation
Requires python (with evdev and i3ipc packages), libcairo, ...

1. Run make to compile overlay.c.
2. Run main.py to list input devices.
3. Modify main.py to include your keyboard input device (dev=InputDevice('/dev/input/event*'))
4. Run main.py
