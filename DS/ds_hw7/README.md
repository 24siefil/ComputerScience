# 1. The laders game

|        |                         |
| :----: | ----------------------- |
| Course | Data Structures         |
| Stack  | C                       |
| Topic | Graph, BFS |

<br/>

# 2. Summary

본 프로젝트는 BFS를 활용하여 ladders game 프로그램을 작성하는 것이다.

<br/>

# 3. Features

* ladders game은 5개의 알파벳으로 이루어진 임의의 단어에 대해, 단 1개의 알파벳만 다른 단어를 adjacent word로 정의한다.

* ladders 프로그램은 5757개의 단어를 가진 words.dat (dictionary)파일을 읽고 각 단어의 인접관계를 파악하여 adjacent matrix와 adjacent list를 제작한다.

* ladders 프로그램은 제작한 matrix와 list를 활용하여 임의의 start와 goal 단어입력 시 두 단어간 shortest path를 BFS algorithm을 활용하여 출력한다.

<br/>

# 4. Demo

![image](https://user-images.githubusercontent.com/83692797/146143142-6317bd2d-6c90-4317-8185-b48e27223f39.png)

![image](https://user-images.githubusercontent.com/83692797/146143210-b8fb7934-633a-4dd2-8120-bdaeda98a4d7.png)

<br/>

# 5. How to compile the project

본 프로젝트의 결과물은  `Makefile`을 포함한다. 해당 `Makefile`은 `ladders`, `test`, `clean` rules를 지원한다. 모든 소스코드들이 컴파일된 후에 프로그램이 생성된다.

* For the address book program

> make ladders

* For the test

> make test

<br/>

# 6. How to execute the program

* For the execution

> ./ladders
