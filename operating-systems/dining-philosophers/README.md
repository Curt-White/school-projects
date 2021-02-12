## Dining Philosophers
There were many solutions on the internet that
implemented elaborate solutions to this problem.
However, almost all of the solutions which achieved
good performance involved asking the neighbours if they
were hungry or asking them for the chop sticks (Chandy/Misra). With
this in mind it didnt seem like a huge violation of the problem to keep 
track of whose eating the most by saying it is an "arbitrator" whom
the thread must notify of its completion of eating. Surely this is not the best
but accidentally spent 20 hours doing part 2 incorrectly so this is what I could
 do with the left over time.

The most notable major problem with this solution is
that the integer will likely roll over after a while. Well
in about 40,000 years if the philosopher eats every 10 microseconds. 
Hopefully that is not a huge issue.

Lastly, it is impossible for there to be deadlock because since every
philosopher picks up one a single side (left if odd, right if even), then
philosopher #4 will always have the opportunity to pick up the last fork
since nobody is contending for it. Since 4 tends to be favoured in this
model, if the threads report how much they eat we can suppress the threads
which get to eat more often by making them wait longer and preventing
starvation.
