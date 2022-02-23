# 1.  Programmatic Query with MariaDB Java Connector

|        |                                        |
| :----: | -------------------------------------- |
| Course | Data Base                |
| Stack | SQL, MariaDB, Java, JDBC, Ecplise |
| Topic | JDBC, Programmatic Query |
| Report | [Notion Link](https://www.notion.so/24siefil/MariaDB-Java-Connector-63dbfdd78b864b9291dfcd94ae2bca66) |

 <br/>

# 2. Subject

* 교과서 126 페이지의 테이블을 활용하여 다음의 절차를 순차적으로 수행하는 자바 프로그램을 구현 하시오.

  * **Step 1)** 18세 이상인 두 명 이상의 Sailors를 가지는 등급에 대해, 각 등급별로 (투표가 가능한) Sailors의 average age를 구하는 질의를 MariaDB에 전송하여 반환 결과를 출력하시오.

  * **Step 2)** Andy의 나이를 30으로 업데이트 한 후 Step1을 반복하고 Step1의 결과와 비교하여, average age가 변화한 등급을 출력하시오.

    (참고: Step 1의 결과를 HashMap 등의 자료 구조에 저장 한 후, 신규 ResultSet에 반환된 쿼리 결과들을 순회하여 확인하시오.)

 <br/>


# 3. SQL Query

```sql
-- SQL Query for Step1

SELECT S.rating, AVG(S.age) AS average_age
FROM Sailors S
WHERE S.age >= 18
GROUP BY S.rating
HAVING COUNT(*) >= 2;
```

```sql
-- SQL Query for Step2

UPDATE Sailors SET age = 30.0
WHERE sname = 'Andy';
```

 <br/>

# 4. Demo

```
========== Step1 ==========
rating	| average_age
--------+------------
3	| 44.5
7	| 40.0
8	| 40.5


========== Step2 ==========
Successfully updated!
Diff. rating: 8
```
