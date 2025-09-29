# MMS Simulation Code for the Robot

This repository contains simulation code for advanced maze-solving algorithms, implemented in C. The key focus is on dynamic and heuristic flood-fill approaches for efficient and robust maze exploration, as used in micromouse or similar robotic maze challenges.

## Algorithms

### 1. Dynamic Floodfill with Heuristic Approach

This algorithm is goal-oriented and uses a classic flood-fill (BFS) to compute distances from all cells to the goal. It works as follows:
- **Initial pass:** Assumes no internal maze walls and computes the shortest path.
- **Dynamic updates:** If the robot encounters a wall it did not know about, it updates its internal map and recomputes the flood-fill distances.
- **Convergence:** The process repeats, and after 3-5 iterations the path usually converges to the true shortest path.
- **Heuristic:** Uses Manhattan distance in the absence of wall data; as walls are discovered, the real distances are used.

The robot only fully recalculates distances if it finds its lowest-cost next step is blocked. This makes it robust against early incomplete information; initial slow runs help reveal more maze structure for future, faster runs.

---

### 2. Full Maze Exploration with Dynamic FloodFill

This algorithm extends dynamic floodfill, aiming to explore the entire maze as efficiently as possible:
- **Phased exploration:** 
  1. Visits all four corners (edges) of the maze.
  2. Visits the goal area.
  3. Systematically visits all remaining unvisited cells, always choosing the nearest one.
- **Flood-fill for navigation:** Uses flood-fill to navigate to the current target.
- **Benefit:** After this full exploration, the robot can compute and follow the shortest path from start to goal in a single, optimal run.

The approach minimizes backtracking by targeting corners/edges first, which tend to maximize new area revealed with each move. The robot marks cells as visited and avoids unnecessary traversal.

---

### 3. Neighbour Focused Full Maze Exploration (Heuristic-Based)

This approach further improves full exploration by using a custom heuristic and dynamic goal selection:
- **Neighbour priority:** The robot always tries to move to unvisited adjacent cells, especially those in nearly-completed sections, to reduce missed spots and avoid backtracking.
- **Section-based heuristic:** The maze is divided into sub-sections ("sixteenths"), and the robot weights targets based on proximity and how many unvisited cells remain in each section.
- **Line-of-sight & corridor logic:** The heuristic favors cells with fewer obstacles and rewards clearing corridors and tight sections.
- **Flood-fill for pathfinding:** The robot uses flood-fill for actual movement, recalculating as new walls are discovered or the optimal route changes.
- **Outcome:** Achieves full maze mapping with fewer steps and turns, minimizing error and improving exploration efficiency.

---
