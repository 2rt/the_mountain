## My Solution implies three methods.

### First Method

It checks for the highest value in the 11x11 matrix, then jumps 5,5 extra so the last highest value is now,
in the top left, this gives more room to check for the second method

### Second Method

It checks every possible line in the matrix horizontally, and vertically except from the index 0 and 10, row and column.

This is because, because they do not have rows, or columns next to them.

What I had discovered is that once you get to the correct x or y co-ordinate the row below and above are identical. Below is an Example

```diff
  299.34 302.68 306.02 309.36 312.69 316.03 319.37 322.71 326.04 329.37 332.71
+  299.50 302.85 306.20 309.54 312.89 316.23 319.58 322.92 326.27 329.61 332.96
  299.56 302.91 306.26 309.60 312.95 316.30 319.65 323.00 326.34 329.69 333.04
+  299.50 302.85 306.20 309.54 312.89 316.23 319.58 322.92 326.27 329.61 332.96
  299.34 302.68 306.02 309.36 312.69 316.03 319.37 322.71 326.04 329.37 332.71
```

Above you can see that the line starting in 299.50 happens twice, this means that Y is found, Goto line 188 there is a vote taken because of the plateaus, and false positives.

### Third Method

Plateaus are only a problem if they are the largest value in the matrix, sometimes we have to randomly jump but doing that always is wasteful,
So what we do instead, is take another vote. We vote where the plateau is located within the 11x11 matrix and we jump in the direction it was going, because
we've already got here by the largest value so the only way is forward not backward, this way we cut down the amount of jumps needed.

All this gives me 10.18.

Thank you for reading my solution.
