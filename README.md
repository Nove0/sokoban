# Sokuban – Game & Level Builder  

This is a **Sokuban** game written in **C++ with SFML**. There are two parts to this project:  

- The **game**, where you play levels.  
- The **level builder**, where you create and edit levels, save them as files, and then play them in the game.  

Both have their own directories:  

- `app/` – The application to play levels.  
- `builder/` – The application to create and modify levels.  

Each directory has its own file explaining how to **build and run** the application, so check those for details.  

## Gameplay  

### **Game**  
- Move with the **arrow keys** or **WASD**.  
- Push the boxes to the **targets**.  

### **Level Builder**  
- Increase/decrease board size with **Up/Down Arrow Keys**.  
- Load a level to edit with **L**.  
- Save a level with **S**, then type a number to name it.  
- Saved levels can be played in the game.  

## Issues & Things That Could Be Improved  

Didn't spend too much time polishing this, so there are some rough edges:  

- **Floating points instead of integers** – Should probably be using integers everywhere, but right now there are floating-point values in places where they’re not really needed.  
- **Graphics alignment** – If the square size isn’t a clean divisor of the window size (or the other way around), things don’t always align properly.  

That’s pretty much it. It works, but if anyone wants to improve it, these are the main things that could be better.

