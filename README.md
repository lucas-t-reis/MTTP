<p align="center">
  <img src="https://freesvg.org/img/burglar.png" width="128" height="128">
</p>
<h1 align="center">MTTP</h1>
<p align="center">
  <img src="https://img.shields.io/github/last-commit/lucas-t-reis/MTTP">
  <img src="https://img.shields.io/github/repo-size/lucas-t-reis/MTTP">
</p>



# Compiling & Running
The parameters used are:\
\
**1** - Metaheuristic to be used [GRASP, ILS]\
**2** - # of thieves [1,2,3,4,5]\
**3** - safe or not [0,1] - means considering or not the cost of going back with current weight\
**4** - # of moves each thief has\
**5** - VND neighborhood size [1 ~ 1000]\
**6** - # of iterations in each neighborhood [1 ~ 1000]\
**7** - Random factor to be used

```bash
~$ g++ -std=c++11 -O3 MTTP.cpp -o MTTP
~$ ./MTTP GRASP 2 0 2 100 100 20 < path/to/input
```

The output consists of the route taken in one line, followed by the items taken in the next line. That is repeated for each thief
# Multiple Traveling Thieves Problem - MTTP
Proposed by Chand and Wagner<sup>1</sup> and based on the Traveling Thief Problem (TTP) by Bonyad,Michalewicz and Barone<sup>2</sup>, the MTTP is a combination of the Knapsack problem and the Traveling Salesman Problem (TSP). 
Initially you are given a set of cities, each containing items with a certain value and weight. Additionaly, you begin the problem with a number **t** of Thieves. The goal of the Thieves is to maximize profit by stealing items from the cities while minimizing the penalty, which in this problem is a 'fee' paid to use the backpack which stores the items - that is the tricky part, since each item stolen reduces your traveling speed, so you must choose wisely the citiy order and items to steal. To achieve that goal, they must never exceed the groups weight carrying limit. 

## Example
In the figure below, each city - except the first one - has a set of items with (value, weight). Consider then the knapsack capacity as 3, the usage fee as 1, the max speed 1 and minimum 0.1. The optimal value **Z**(Route, Items) = 50.

<p align="center">
<img src="https://raw.githubusercontent.com/lucas-t-reis/MTTP/master/assets/sample.svg">
</p>

In this case, considering 1 thief for simplicity sake, the thief travels without stealing items from city *1* to city *2* and after that goes to city *4* and *3*. This part of the route has a time cost of 5 + 6 + 4 = 15. In city *3*  it takes the first two items, making a profit of 80. After that it travels back to the orgin, again with the cost of 15 - and in that part of the heist he travels back with a velocity of 0.4, because of the weight of the items in the backpack. Therefore we have **Z**([1,2,4,3], [40,40]) = 80 -1\*(5+6+4+15) = 50. Note that the solution isn't the knapsack optimum, which is to take the item with value 100 from city 3, nor the TSP solution - visit 1,2,3,4. It is a combination of those independent problems, hence the complex nature of the task at hand.


## Problem definition
* **N** = {1,2,....,n} is the set of cities
* d<sub>ij</sub> is the distance from city *i* to city *j*
* **M**<sub>i</sub> = {1,.....,m<sub>i</sub>} is the set of items of city i
* Each item *k* at city *i* gives a profit *p*<sub>ik</sub> and a weight w<sub>ik</sub> associated to it
* The speed of each thief varies according to the number of items it's carrying
* y<sub>ik</sub> is 1 if item *k* was taken from city *i* and 0 otherwise
* The maximum capacity of the gang is denoted **W**
* The weight carried by a thief leaving city i is denoted W<sub>i</sub>
* The constant v is defined as being v = (V<sub>max</sub> - V<sub>min</sub>)/**W**
* **R** is the fee that must be paid to use the backpack
* Any item that isn't already taken and doesn't exceed the capacity of the backpack can be taken
* The first city - origin - doesn't have items
* Each city can only be visited once
* After stealing from the last city, the thief must return to the origin
* The graph of the cities has the Euclidian distance as edge cost between cities

## Cost function

<p align="center">
<img src="https://raw.githubusercontent.com/lucas-t-reis/MTTP/master/assets/cost.svg">
</p>

### References

**1** - S. Chand, M. Wagner. Fast Heuristics for the Multiple Traveling Thieves Problem. Proceedings of GECCO’16, pp. 293–300, 2016.\
**2** - M. R. Bonyadi, Z. Michalewicz, L. Barone. The travelling thief problem: the first step in the transition from theoretical problems to realistic problems. Proceedings of CEC’13, pp. 1037–1044, 2013.

## Project authors
* [Matheus Aguilar](https://github.com/Matheus-Aguilar) 
* [Lucas Reis](https://github.com/lucas-t-reis)

### Credits
* Thief image by <a href="https://pixabay.com/users/OpenClipart-Vectors-30363/?utm_source=link-attribution&amp;utm_medium=referral&amp;utm_campaign=image&amp;utm_content=157142">OpenClipart-Vectors</a> from <a href="https://pixabay.com/?utm_source=link-attribution&amp;utm_medium=referral&amp;utm_campaign=image&amp;utm_content=157142">Pixabay</a>
