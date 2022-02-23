# 1. Address Book - 6

|        |                       |
| :----: | --------------------- |
| Course | Data Structures       |
| Stack  | C                     |
| Topic  | Hashing with chaining |

<br/>

# 2. Summary


본 프로젝트는 hashing with chaining 기법을 활용하여, 이름과 주소를 입력받아 Address Book을 구축하는 프로그램을 작성하는 것이다.

<br/>

# 3. Features

* 유저와 interact하며 입력을 받고 결과를 표시하는 Frent-end와 DB를 구축하는 Back-end로 나누어 구현하였다.

* Hash table을 활용하여 레코드를 관리하며, Hash 함수는 다음과 같다.

  ```c
  int name_to_num(char name[3])
  {
    int x;
  
    x = name[0];
    x = x << 8;
    x = x + name[1];
    x = x << 8;
    return (x + name[2]);
  }
  
  int hash(char name[3])
  {
    return (name_to_num(name) % HASH_PRIME);
  }
  ```

* 다음의 commands를 지원한다.

  ```c
  'A' or 'a' : add new record
  'S' or 's' : search numbers by name
  'D' or 'd' : delete record
  'P' or 'p' : print out the number of keys in each chain. 
  For example, the output
  0:0 1:0 2:2 3:0 4:0 5:2 6:1 7:0 8:0 9:0 10:0 11:0 12:0 13:0 14:0 15:0 16:0
  'Q' or 'q' : quit the program
  ```

<br/>

# 4. Demo

![image](https://user-images.githubusercontent.com/83692797/141444574-0f55e14b-80d9-4134-9800-db525e8a2cc7.png)

<br/>

# 5. How to compile the project

본 프로젝트의 결과물은  `Makefile`을 포함한다. 해당 `Makefile`은 `addr1`, `test`, `clean` rules를 지원한다. 모든 소스코드들이 컴파일된 후에 프로그램이 생성된다.

* For the address book program

> make addr

* For the test

> make test

<br/>

# 6. How to execute the program

* For the execution

> ./addr