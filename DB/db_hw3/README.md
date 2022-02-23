# 1.  Spring Boot and REST Interface

|        |                                        |
| :----: | -------------------------------------- |
| Course | Data Base                |
| Stack | SQL, MariaDB, Java, JDBC, Spring Boot |
| Topic | JDBC, Programmatic Query, REST Interface |
| Report | [Notion Link](https://www.notion.so/24siefil/Spring-Boot-and-REST-Interface-7c34698285124789847ce861f652a326) |

 <br/>

# 2. Subject

* Spring Boot 프레임워크에 따라서 JDBC를 통해 MariaDB로부터 반환받은 쿼리 결과 값을 REST Interface로 확인하는 체계를 구축한다.

* 교과서 126 페이지의 테이블을 활용하여, 18세 이상인 두 명 이상의 Sailors를 가지는 등급에 대해, 각 등급별로 (투표가 가능한) Sailors의 average age를 구하는 질의를 MariaDB에 전송하여 반환 결과를 REST Client에 넘겨준다.

* 다음의 명령어를 통해 결과를 확인하고, port는 각자의 환경에 맞게 설정하라.

  > Get localhost:<port>/step1

* 정상작동 여부를 REST GUI Client를 통해 증빙하여라.

 <br/>

# 3. Structure

```
                ---Spring Boot--------------------------
                | --------Tomcat Server--------------- |
                | |                                  | |
GET, PVT, POST -----> Rest Controller                | |
                | |     |-> Function Mapping         | |
                | |              {                   | |
                | |                JDBC - MariaDB    | |
                | |              }                   | |
                | ------------------------------------ |
                ----------------------------------------
```

- HTML: annotation
- Server → Client: HTTP(REST의 개념 구현)
  - REST: 원격의 resource에 대한 요청(HTTP로 구현)
    - GET, PVT, POST + 파라미터 명시
    - Spring Boot는 Client단에서 명령어를 처리

 <br/>


# 4. Demo

 18세 이상인 두 명 이상의 Sailors를 가지는 등급에 대해, 각 등급별로 (투표가 가능한) Sailors의 average age를 구하는 질의의 결과(응답)은 다음과 같다.

 ![image](https://user-images.githubusercontent.com/83692797/147033587-bdd6bde8-d6b5-470b-ad43-eae1b6431b1b.png)
