# 1. Address Book - 1

|        |         |
| :----: | ------- |
| Course | Data Structures |
| Stack  | C       |
| Topic | Array |

<br/>

# 2. Summary


본 프로젝트는 이름과 주소를 입력받아 Address Book을 구축하는 프로그램을 구현하는 것이다.

<br/>

# 3. Features

* 유저와 interact하며 입력을 받고 결과를 표시하는 Frent-end와 DB를 구축하는 Back-end로 나누어 구현하였다.

* 다음의 commands를 지원한다.

  ```c
  'A' or 'a' : add new record
  'S' or 's' : search numbers by name
  'D' or 'd' : delete record
  'Q' or 'q' : quit the program
  'P' or 'p' : print the entire address book

* 다음의 방법으로 입력에 대한 예외처리를 진행한다.

  ```c
  int prompt_name(char *s, char *p)
  {
    char *q;
    char c;
  
    while (1) {
      printf("%s ", s);
      fflush(stdout);
      q = p;
      while (1) {
        c = getchar();
        if (c == EOF)
          exit(-1);
        if (c == '\n')
          break;
        if (q < p + 3)
          *q = c;
        ++q;
      }
      if (q == p + 3) // valid input
        return (0);
      if (q == p) // c == '\n'
        return (1);
      printf("Please type a three-letter name.\n"); // invalid input
    }
  }
  ```

<br/>

# 4. Demo
<img width="357" alt="addr1_demo" src="https://user-images.githubusercontent.com/83692797/132805030-15c7a9c7-d22b-419b-ad37-19a906cd2275.png">

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
