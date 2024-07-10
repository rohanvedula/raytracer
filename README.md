# cs488

## Compilation
Compilation is same a default

## Specification
### Task 1: 
For this task, a position based integration scheme (Verlet integration was used)
based on the previous position and current position to find the new position (see lines 1607-1609).
We use the formula from the in the course notes to find the next position. Change line 1767 to `.step()` in order to test this task.

<img src="/imagesa3/task1a.gif" alt="drawing" width="500"/>

### Task 2: 
To run task 2, change line 1767 to `.step_box()`. For this task, Verlet integration
was used again. At each movement, we check the position of each of the points to see if the x,y or z coordinate are
over/under 0.5/-0.5 and if they are, then we reflect the previous position over the current position along the
axis of intersection by finding the distance between positon and previous position and subtracting that from the current position
so that we will be on the opposite side as before. Then we shift both the previous position and current position by an amount to align
it with the axis. Since it is possible to have an intersection with multiple axises, we have a loop to consecutively check the intersection
with the axises. We sent an upper limit of 10 for the loop so we won't have an infinite loop. After this, we have the following:

<img src="/imagesa3/task2.gif" alt="drawing" width="500"/>

### Task 3:
For this task, we use the formula from the course note for a bead on a wire
to adjust the position to account for a sphere of radius 0.5. To run this part,
change lien 1766 to `.step_sphere`. After this, this is the output:

<img src="/imagesa3/task3.gif" alt="drawing" width="500"/>

### Task 4:
To run this task, comment out line 1767 and uncomment line 1766. Then, change the 
intial velocity (line 1596) to `float3(0.0f)`. Essentially, we turn off the intial velocity
of the particles. Firstly, we have a function `get_force()` in the particle system which reutrns
the force between 2 particles using the formula discussed in the course notes. Next, we have a function
`accumulate_gravity()` also in particle system which finds the total force exerted on a particle by accumulating
the force from all the particles. Lastly, we have the `step_gravity()` function in the particle class.
This is similar to the normal step function except instead of gravity, we apply the total force to each particle and
divide by the mass of the particle. The mass used is 0.2. This was found experimentally to produce the result
that looked closest to the example. Initlally, I started with a pass caclulating using the gravitational constant
and the value 2e-3 that was given but this produced a result that didn't look accurate so 0.2 was used instead. Here 
is the output from this part:

<img src="/imagesa3/task4.gif" alt="drawing" width="500"/>

### Task 5:
To run this part, change `globalNumParticles` to 20, comment line 1980 (used for shading the spheres), uncomment line 1768, comment out line 1766
and change line 1767 to `step_sphere()`. Then, run as normal. Unfortunately, my laptop is really bad so it can't handle processing
more than 20 balls smoothly so that was all that was used for this example. Feel free to test with more balls.  Firstly, the `step_sphere()` is the same as in task 3,
the `intersect_collisions()` function iterates through all pairs of balls 10 times add calls the `resolve_collision()` function. This function
first checks if there is a collision between two balls, if not then it returns. If there is, then first we find the direction vector between the centres
of the two balls and the amount of intersection. To resolve these collision, we find the component of velocity (difference between prevPositon and position )v1, v2 along the direction vector 
of both the balls 1 and 2. We subtract the amount v1 from the velocity of ball 1 then add v2. Doing this removes the velocity of the ball along
that direction (effectively making it a stop) and since the balls have the same weight, we know that the velocity gained is equal to the
velocity of the other ball along the direction vector by property of momentum conservation. After this, we finish
setting the previous position of the balls using the velocity and current positon. Lastly, we can adjust the new prevPositoin and position
be the distance d/2 like done in the course notes to resolve all collisions between the balls.

<img src="/imagesa3/task5.gif" alt="drawing" width="500"/>


