# 1. Virtual memory management simulator

|                 |                                                              |
| --------------- | ------------------------------------------------------------ |
| Course | Operating Systems |
| Stack           | C                                                            |
| Topic          | Memory management, Page replacement algorithmss |
| Subject | [Homework3](https://github.com/24siefil/Computer_Science/blob/main/OS/os_hw3/subject/Homework3.pdf) |

<br/>

# 2. Summary

본 프로젝트는 Virtual Memory Systems에서 one-level, two-level Page Table 과 Inverted Page Table system을 구현하여 시뮬레이션 하는것이다.

<br/>

# 3. Features

* Virtual Memory System simulator에서 virtual address 크기는 32bits (4Gbytes)로 나타나고, page의 사이즈는 12bits (4Kbytes)로 가정 합니다.
* **One-level Page Table System**
  * Page faults 인 경우 두 가지 page replacement algorithm을 사용합니다. FIFO 와 LRU입니다.

* **Two-level Page Table System**
  - second level page table은 메모리에 접근 하면서 필요한 경우에만 구성하면 됩니다. 
  - second level page table 접근하여 page table entry를 확인하고 해당 page에 이미 physical memory frame 할당 되어 있으면 page hit 이고, 할당 되어 있지 않으면 page faults 인데 이 경우 page replacement algorithm (LRU) 에 따라 frame을 할당하게 됩니다.
  - 기존에 할당 되어 있던 frame을 다른 page 에 할당 할 경우 기존의 매핑 정보를 invalid하게 설정해야 합니다.

* **Inverted Page Table**
   * Hash Table index 값 = (virtual page number + process id) % frame 의 수
   * 같은 Hash table의 entry에 여러 개의 매핑 정보(노드)가 있을 수 있는데 가장 최근에 만들어진 매핑 정보(노드)가 가장 먼저 접근 될 수 있도록 앞쪽에 삽입되어야 합니다.
   * Hash table의 entry에 연결 된 매핑 정보를 다 찾아도 접근하고자하는 virtual page number 와 process id를 발견할 수 없으면 Page faults이고, 이와 달리 발견하면 해당 매핑 정보에 있는 frame number를 사용하여 physical address를 만들어 내면 됩니다.
   * Page faults 가 발생하면 page replacement algorithm (LRU) 에 따라 frame을 할당하게 됩 니다. 기존에 할당 되어 있던 frame을 다른 page 에 할당 할 경우 기존의 매핑 정보를 없애야 합니다.

* **Page replacement Algorithm (LRU)**
   * 매번의 Memory 참조를 수행할 때 마다 (매 한 개의 memory trace 레코드 처리) LRU에 따라 다음에 replace될 frame의 순서를 정하여야 합니다. 
   * Global replacement를 적용하여 process에 상관없이 모든 frame에 적용 됩니다.
   * 단, 초기 메모리 상태에서 Frame의 할당은 낮은 번호(주소)의 Frame부터 할당 합니다.

<br/>

# 4. Demo

```c
$ ./a.out 0 7 14 mtraces/gcc10.trace
process 0 opening mtraces/gcc10.trace
Num of Frames 4 Physical Memory Size 16384 bytes
=============================================================
The Two-Level Page Table Memory Simulation Starts .....
=============================================================
Two-Level procID 0 traceNumber 1 virtual addr 2f8773d8 -> physical addr 3d8
Two-Level procID 0 traceNumber 2 virtual addr 3d729358 -> physical addr 1358
Two-Level procID 0 traceNumber 3 virtual addr 2f8773d8 -> physical addr 3d8
Two-Level procID 0 traceNumber 4 virtual addr 2f8773e0 -> physical addr 3e0
Two-Level procID 0 traceNumber 5 virtual addr 249db050 -> physical addr 2050
Two-Level procID 0 traceNumber 6 virtual addr 2f8773e0 -> physical addr 3e0
Two-Level procID 0 traceNumber 7 virtual addr 2f8773e0 -> physical addr 3e0
Two-Level procID 0 traceNumber 8 virtual addr 249db050 -> physical addr 2050
Two-Level procID 0 traceNumber 9 virtual addr 2f8773e0 -> physical addr 3e0
Two-Level procID 0 traceNumber 10 virtual addr 3d729358 -> physical addr 1358
**** mtraces/gcc10.trace *****
Proc 0 Num of traces 10
Proc 0 Num of second level page tables allocated 3
Proc 0 Num of Page Faults 3
Proc 0 Num of Page Hit 7
```

<br/>

# 5. How to compile and execute the program

* To compile

> gcc memsimhw.c

<br/>

* To execute

프로그램은 다음과 같은 인자를 받아 실행된다.

> ./a.out memsim simType firstLevelBits PhysicalMemorySizeBits TraceFrileNames...

1) 첫 번째 (simType) : 값 0, 1, 2, 3<br/>
	0 일 때 : One-level page table system을 수행 합니다. FIFO Replacement를 사용합니다. <br/>
	1 일 때 : One-level page table system을 수행 합니다. LRU Replacement를 사용합니다. <br/>
	2 일 때 : Two-level page table system을 수행 합니다. LRU Replacement를 사용합니다. <br/>
	3 일 때 : Inverted page table system을 수행 합니다. LRU Replacement를 사용합니다.
2) 두 번째  (firstLevelBits - 1st level 주소 bits 크기) 
3) 세 번째  (PhysicalMemorySizeBits - Physical Memory 크기 bits) 
4) 네 번째 및 그 이후의 인자들  (TraceFileNames, .... ) 
	네 번째부터 그 이후의 인자 수는 제한이 없습니다.

<br/>

# 6. Test result

```c
[B617065@localhost tester]$ ./memsim 0 7 14 mtraces/sixpack.trace mtraces/swim.trace mtraces/gcc.trace mtraces/bzip.trace
process 0 opening mtraces/sixpack.trace
process 1 opening mtraces/swim.trace
process 2 opening mtraces/gcc.trace
process 3 opening mtraces/bzip.trace
Num of Frames 4 Physical Memory Size 16384 bytes
=============================================================
The One-Level Page Table with FIFO Memory Simulation Starts .....
=============================================================
**** mtraces/sixpack.trace *****
Proc 0 Num of traces 1000000
Proc 0 Num of Page Faults 842939
Proc 0 Num of Page Hit 157061
**** mtraces/swim.trace *****
Proc 1 Num of traces 1000000
Proc 1 Num of Page Faults 825327
Proc 1 Num of Page Hit 174673
**** mtraces/gcc.trace *****
Proc 2 Num of traces 1000000
Proc 2 Num of Page Faults 803157
Proc 2 Num of Page Hit 196843
**** mtraces/bzip.trace *****
Proc 3 Num of traces 1000000
Proc 3 Num of Page Faults 766049
Proc 3 Num of Page Hit 233951

[B617065@localhost tester]$ ./memsim 1 7 14 mtraces/sixpack.trace mtraces/swim.trace mtraces/gcc.trace mtraces/bzip.trace
process 0 opening mtraces/sixpack.trace
process 1 opening mtraces/swim.trace
process 2 opening mtraces/gcc.trace
process 3 opening mtraces/bzip.trace
Num of Frames 4 Physical Memory Size 16384 bytes
=============================================================
The One-Level Page Table with LRU Memory Simulation Starts .....
=============================================================
**** mtraces/sixpack.trace *****
Proc 0 Num of traces 1000000
Proc 0 Num of Page Faults 792379
Proc 0 Num of Page Hit 207621
**** mtraces/swim.trace *****
Proc 1 Num of traces 1000000
Proc 1 Num of Page Faults 763444
Proc 1 Num of Page Hit 236556
**** mtraces/gcc.trace *****
Proc 2 Num of traces 1000000
Proc 2 Num of Page Faults 716106
Proc 2 Num of Page Hit 283894
**** mtraces/bzip.trace *****
Proc 3 Num of traces 1000000
Proc 3 Num of Page Faults 629737
Proc 3 Num of Page Hit 370263

[B617065@localhost tester]$ ./memsim 2 7 14 mtraces/sixpack.trace mtraces/swim.trace mtraces/gcc.trace mtraces/bzip.trace
process 0 opening mtraces/sixpack.trace
process 1 opening mtraces/swim.trace
process 2 opening mtraces/gcc.trace
process 3 opening mtraces/bzip.trace
Num of Frames 4 Physical Memory Size 16384 bytes
=============================================================
The Two-Level Page Table Memory Simulation Starts .....
=============================================================
**** mtraces/sixpack.trace *****
Proc 0 Num of traces 1000000
Proc 0 Num of second level page tables allocated 37
Proc 0 Num of Page Faults 792379
Proc 0 Num of Page Hit 207621
**** mtraces/swim.trace *****
Proc 1 Num of traces 1000000
Proc 1 Num of second level page tables allocated 34
Proc 1 Num of Page Faults 763444
Proc 1 Num of Page Hit 236556
**** mtraces/gcc.trace *****
Proc 2 Num of traces 1000000
Proc 2 Num of second level page tables allocated 35
Proc 2 Num of Page Faults 716106
Proc 2 Num of Page Hit 283894
**** mtraces/bzip.trace *****
Proc 3 Num of traces 1000000
Proc 3 Num of second level page tables allocated 9
Proc 3 Num of Page Faults 629737
Proc 3 Num of Page Hit 370263

[B617065@localhost tester]$ ./memsim 3 7 14 mtraces/sixpack.trace mtraces/swim.trace mtraces/gcc.trace mtraces/bzip.trace
process 0 opening mtraces/sixpack.trace
process 1 opening mtraces/swim.trace
process 2 opening mtraces/gcc.trace
process 3 opening mtraces/bzip.trace
Num of Frames 4 Physical Memory Size 16384 bytes
=============================================================
The Inverted Page Table Memory Simulation Starts .....
=============================================================
**** mtraces/sixpack.trace *****
Proc 0 Num of traces 1000000
Proc 0 Num of Inverted Hash Table Access Conflicts 1098256
Proc 0 Num of Empty Inverted Hash Table Access 265057
Proc 0 Num of Non-Empty Inverted Hash Table Access 734943
Proc 0 Num of Page Faults 792379
Proc 0 Num of Page Hit 207621
**** mtraces/swim.trace *****
Proc 1 Num of traces 1000000
Proc 1 Num of Inverted Hash Table Access Conflicts 1492035
Proc 1 Num of Empty Inverted Hash Table Access 152430
Proc 1 Num of Non-Empty Inverted Hash Table Access 847570
Proc 1 Num of Page Faults 763444
Proc 1 Num of Page Hit 236556
**** mtraces/gcc.trace *****
Proc 2 Num of traces 1000000
Proc 2 Num of Inverted Hash Table Access Conflicts 1167377
Proc 2 Num of Empty Inverted Hash Table Access 229777
Proc 2 Num of Non-Empty Inverted Hash Table Access 770223
Proc 2 Num of Page Faults 716106
Proc 2 Num of Page Hit 283894
**** mtraces/bzip.trace *****
Proc 3 Num of traces 1000000
Proc 3 Num of Inverted Hash Table Access Conflicts 1456597
Proc 3 Num of Empty Inverted Hash Table Access 171256
Proc 3 Num of Non-Empty Inverted Hash Table Access 828744
Proc 3 Num of Page Faults 629737
Proc 3 Num of Page Hit 370263
```
