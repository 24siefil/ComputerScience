# 1. Address Book - 5

|        |                    |
| :----: | ------------------ |
| Course | Data Structures    |
| Stack  | C                  |
| Topic  | Binary search tree |

<br/>

# 2. Summary


본 프로젝트는 Binary search tree를 활용하여, 이름과 주소를 입력받아 Address Book을 구축하는 프로그램을 작성하는 것이다.

<br/>

# 3. Features

* 유저와 interact하며 입력을 받고 결과를 표시하는 Frent-end와 DB를 구축하는 Back-end로 나누어 구현하였다.

* Binary search tree를 활용하여 레코드를 관리한다.

* 다음의 commands를 지원한다.

  ```c
  'A' or 'a' : add new record
  'S' or 's' : search numbers by name
  'D' or 'd' : delete record
  'H' or 'h' : print the height of the BST for the Address Book
  'P' or 'p' : print the entire address book
  'Q' or 'q' : quit the program
  ```

* Print 명령어 실행 시 순회 및 출력은 inorder 방식으로 하며, 정렬된 데이터를 열람할 수 있다.

<br/>

# 4. Demo

![ds_hw5 demo](https://user-images.githubusercontent.com/83692797/138581575-82c648f9-a7d0-4194-a7d2-eee90a83d07e.png)

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