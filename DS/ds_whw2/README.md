# 1. Height of a BST and eifficiency of searching

|        |                                     |
| :----: | ----------------------------------- |
| Course | Data Structures                     |
| Stack  | C                                   |
| Topic  | Binary search tree, Time complexity |

<br/>

# 2. Subject

In class, we discussed the height of a binary search tree is an important measure of its eciency of searching. This homework deals with a few related questions. Use the code for HW5, together with properly completed add(), and **write a program to perform the following experiment that estimates the average height of BST, for N = 50, 100, 200 and 500**

1. (a)  Generate N random data using the standard C library function rand().
2. (b)  Insert the generated data into a BST, and obtain the height of the resulting BST, whose height must be between lg N and N  1.
3. (c)  Repeat Step (b) many times, e.g., ten thousand or one million times. And **calculate the average of results. Estimate the probabilistic distribution of the heights of resulting BSTs.**
4. (d)  Present your program and discuss your experiment result.

 <br/>

# 3. Experiment reusult

```c
==================== Result for 1,000 iterations ====================
About height of BST for N = 50
Average: 9.9700 \ Variance: 2.0851 \ Standard deviation: 1.4440
About height of BST for N = 100
Average: 12.3730 \ Variance: 2.4459 \ Standard deviation: 1.5639
About height of BST for N = 200
Average: 15.0710 \ Variance: 3.1560 \ Standard deviation: 1.7765
About height of BST for N = 500
Average: 18.5480 \ Variance: 3.5617 \ Standard deviation: 1.8872
==================== Result for 10,000 iterations ====================
About height of BST for N = 50
Average: 9.9272 \ Variance: 2.0733 \ Standard deviation: 1.4399
About height of BST for N = 100
Average: 12.3874 \ Variance: 2.4851 \ Standard deviation: 1.5764
About height of BST for N = 200
Average: 14.9924 \ Variance: 2.9621 \ Standard deviation: 1.7211
About height of BST for N = 500
Average: 18.4596 \ Variance: 3.4216 \ Standard deviation: 1.8497
==================== Result for 100,000 iterations ====================
About height of BST for N = 50
Average: 9.9079 \ Variance: 2.0806 \ Standard deviation: 1.4424
About height of BST for N = 100
Average: 12.3816 \ Variance: 2.5584 \ Standard deviation: 1.5995
About height of BST for N = 200
Average: 14.9555 \ Variance: 2.9457 \ Standard deviation: 1.7163
About height of BST for N = 500
Average: 18.4499 \ Variance: 3.3948 \ Standard deviation: 1.8425
```

 <br/>

# 4. Discussion

### Estimate the probabilistic distribution of the heights of resulting BSTs.

50, 100, 200개의 random number로 binary search tree(이하 BST)를 생성하는 것을 각각 1000, 10,000, 100,000회 반복 시행하여 그것들의 (average) height를 추적해 보았다. 50, 100, 200의 각 N값에 대해 height의 분포는, histogram 도시 시 normal distribution의 양상을 보인다. 나아가 실험을 통해 계산한 variance와 standard deviation을 고려할 때, 귀납적 추론을 통해 적당한 N값에 대해 height의 분포는 normal distribution의 양상을 띈다고 estimate할 수 있다. (height는 discrete probability variable이지만, 특정 height의 ­발현 확률은 binomial distribution을 따른다고 할 수 없기에, N값이 충분히 큰 경우라고 해도 height의 분포는 normal distribution에 단순히 근사할 수는 없다.)

 <br/>

### In most (random) cases, the average depth of a BST is O(log n).

본 실험에서 주목해야할 것은 ‘In most (random) cases, the average depth of a BST is O(log n)’임을 확인하는 것이다. 100,000 Iterations 실험결과에 대해 N값과 ‘average depth of a BST’는 다음 표와 같은 관계를 가진다. 이는 ‘average depth of a BST = C * log N, C = 5 ~ 6’로 표현될 수 있으며, iteration 횟수와 무관하게 average 값은 일정한 경향으로 보이므로, ‘average depth of a BST = C * log N’ 는 모든 실험결과에 대해 성립한다.

| N    | Average depth of  a BST | log N  | (Average depth of  a BST) / log N |
| ---- | ----------------------- | ------ | --------------------------------- |
| 50   | 9.9079                  | 1.6990 | **5.8316**                        |
| 100  | 12.3816                 | 2.0000 | **6.1908**                        |
| 200  | 14.9555                 | 2.3010 | **6.5000**                        |
| 500  | 18.4499                 | 2.6990 | **6.8358**                        |

 <br/>

또한 실험결과를 그래프로 도시하면 다음과 같다. Random number의 개수 N과 그에 따른 average height의 증가속도를 비교하면 전자의 경우가 더 빠른것을 확인할 수 있다. 이러한 경향은 N이 증가함에 따라 더욱 뚜렷하게 나타난다. 결과적으로 실험결과를 통해 N과 average height가 추세적으로 log N의 관계를 갖는 것을 확인할 수 있다. 따라서 ‘In most (random) cases, the average depth of a BST is O(log n)’임을 추론할 수 있다.

 <br/>

![IMG_7B2B938DE775-1](https://user-images.githubusercontent.com/83692797/140258485-66671f44-07e7-4843-a0ab-9795bb832b8c.jpeg)

(Samples are from 100,000 iterations)

<br/>

### Height must between lg N and N -1.

한편 실험간 assertion을 통해 ‘height must between lg N and N -1’임을 계속해서 확인하였다. 결과적으로 실험간 발생한 모든 height에 대해 해당 조건은 만족한다. assertion에 사용된 source code는 다음과 같다.

```c
assert(((log((double)n) / log((double)2)) <= height_n[i]) && (height_n[i] <= n - 1));
```

<br/>

### Conclusion

본 실험을 통해 N개의 node를 갖는 BST 생성 시 ‘the average depth of a BST is O(log n)’임 그리고 모든 depth에 대해 ‘between lg N and N -1’임을 확인하였다. 나아가 ‘The height of a binary search tree is an important measure of its efficiency of searching’ 혹은 ‘If we assume that all the keys are searched for with the same probability, then the average search time is proportional to the average depth of the BST’ 이므로, 이를 실험결과와 종합하면, BST를 활용한 탐색이 상당히 효율적이며, 특히 N값이 커짐에 따라 (O(n)의 복잡도를 갖는 sequential search와 비교하여) 그 유용성은 더욱 증대 되는 것을 확인할 수 있었다.

<br/>

# 5. How to compile and execute the project

* To compile

> gcc main.c

<br/>

* To execute

> ./a.out
