# 1. Properties of the ladders graph

|        |                                        |
| :----: | -------------------------------------- |
| Course | Data Structures                        |
| Stack  | C                                      |
| Topic | Graph |

 <br/>

# 2. Subject

For this homework, we will try to figure out some properties of the “ladders” graph based on words.dat. Based on the data file and programs in whw4.zip, write programs to answer the following questions and submit a report that includes your code, answers, and any of your comments.

1. On the “ladders” graph based on words.dat
<br/>(a) print out all the words adjacent to hello. What is the degree of hello?
<br/>(b) print out all the words adjacent to graph. What is the degree of graph?

2. Compute the table of distribution of degrees. That is, make a table of the number of degree-0 vertices, the number of degree-1 vertices, . . . , etc. For example, the table should look like

```
   0: 671
   1: 774
   2: 727
   3: 638
   .
   .
   .
   23: 2
   24: 3
   25: 2
   .
   .
   .

```

3. What is the maximum degree?

4. What are the words that have the maximum degree?

5. What is the average degree?

6. How many nodes does our adjacency list have?

7. What is the minimum possible size required of POOL SIZE in backend.c?

 <br/>

# 3. Solution
```c
==================== Question 1-(a) ====================
All the words adjacent to hello: cello hallo hells hullo jello 
The degree of hello: 5


==================== Question 1-(b) ====================
All the words adjacent to hello: grape grapy 
The degree of graph: 2


==================== Question 2 ====================
The table of distribution of degrees
0:      671
1:      774
2:      727
3:      638
4:      523
5:      428
6:      329
7:      280
8:      249
9:      213
10:     188
11:     162
12:     120
13:     116
14:     102
15:     75
16:     53
17:     32
18:     32
19:     20
20:     8
21:     6
22:     4
23:     2
24:     3
25:     2


==================== Question 3 ====================
The maximum degree: 25


==================== Question 4 ====================
bares cores 


==================== Question 5 ====================
The average degree: 4.910544


==================== Question 6 ====================
Our adjacency list has 28270 nodes
```

* Question 7

  minimum possible size required of POOL SIZE in backend.c는 adjacency_list의 노드의 총 개수인 28270이다.

 <br/>

# 4. How to compile and execute the project

* To compile

> make ladders

<br/>

* To execute

> ./ladders
