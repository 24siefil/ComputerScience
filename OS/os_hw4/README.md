# 1. File System Simulator

|                 |                                                              |
| --------------- | ------------------------------------------------------------ |
| Course | Operating Systems |
| Stack           | C                                                            |
| Topic          | File systems, Shell |
| Subject | [Homework4](https://github.com/24siefil/Computer_Science/blob/main/OS/os_hw4/Homework4.pdf) |

<br/>

# 2. Summary

본 프로젝트는 파일 시스템을 간략화 하여 구현하는 것이다. 구현한 프로그램은 실제 디스크를 사용하는 것이 아닌, 디스크와 동등한 형태의 이미지 파일을 활용하며, 다수의 명령어를 시뮬레이션 한다.

<br/>

# 3. Features

* 이미지 파일의 데이터는 특정한 레이아웃 형태로 포맷되어 있다. `disk_read()`, `disk_write()` 함수를 통하여 실제 디스크와 같이 블록단위로 엑세스한다.
* 디렉토리 구조나 파일을 관리하기 위해 i-node를 사용하고, 가용한 블록(free blocks)들을 관리하기 위하여 비트맵을 사용한다.
* 비트맵의 블록 수는 디스크의 전체 블록 수에 따라 유동적으로 할당된다.
* 모든 명령어의 구현에는 실제 쉘과 동일한 형식으로 에러 상황에 대한 처리를 진행한다.
* 한 명령어에 대한 동작 완료 시 모든 변경 사항이 디스크 이미지로 쓰여 져야 한다. 버퍼캐시를 사용하지 않는다.
* 비트맵 등을 메모리상에서 관리하지 않는다. 필요 시 디스크에서 읽은 후 다시 디스크에 쓰는 형태로 데이터를 관리한다.

* 지원하는 명령어는 다음과 같다.

```
mount [disk_image_file_name]
umount
dump
ls [path]
cd [path]
tocuh [path]
mkdir [path]
rmdir [path]
mv [path1] [path2]
rm [path]
```

<br/>

# 4. Demo

* ls, cd, mount [disk_image_file_name], umount
```shell
./a.out
OS SFS shell
os_shell> mount DISK1.img
Disk image: DISK1.img
Superblock magic: abadf001
Number of blocks: 10241
Volume name: HW3_DISK_IMG1
HW3_DISK_IMG1, mounted
os_shell> ls
./      ../     os/     congratulation_on_successful_ls/
os_shell> mkdir aaa
os_shell> ls
./      ../     os/     congratulation_on_successful_ls/        aaa/
os_shell> mkdir aaa
mkdir: aaa: Already exists
os_shell> cd os  
os_shell> ls
./      ../     hw3_submit/     os_grade.txt
os_shell> cd os_grade.txt
cd: os_grade.txt: Not a directory
os_shell> cd asdf
cd: asdf: No such file or directory
os_shell> umount
HW3_DISK_IMG1, unmounted
os_shell> exit
bye
```
* touch [path]
```shell
os_shell> ls  
./      ../     os/     congratulation_on_successful_ls/
os_shell> touch aaa
os_shell> ls
./      ../     os/     congratulation_on_successful_ls/        aaa
os_shell> touch aaa
touch: aaa: Already exists
```
* mkdir [path]
```shell
os_shell> ls
./      ../     os/     congratulation_on_successful_ls/
os_shell> mkdir aaa
os_shell> ls
./      ../     os/     congratulation_on_successful_ls/        aaa/
os_shell> mkdir aaa
mkdir: aaa: Already exists
```
* rmdir [path]
```shell
os_shell> ls
./      ../     hw3_submit/     os_grade.txt
os_shell> rmdir hw3_submit
os_shell> ls
./      ../     os_grade.txt
os_shell> rmdir .
rmdir: .: Invalid argument
os_shell> rmdir os_grade.txt
rmdir: os_grade.txt: Not a directory
os_shell> cd ..
os_shell> ls
./      ../     os/     congratulation_on_successful_ls/
os_shell> rmdir os
rmdir: os: Directory not empty
os_shell> rmdir aaa
rmdir: aaa: No such file or directory
```
* mv [path1] [path2]
```shell
os_shell> ls
./      ../     hw3_submit/     os_grade.txt
os_shell> mv os_grade.txt aaa
os_shell> ls
./      ../     hw3_submit/     aaa
os_shell> mv aaa hw3_submit
mv: hw3_submit: Already exists
os_shell> mv asdf jkl;
mv: asdf: No such file or directory
os_shell> mv . aaa
mv: .: Invalid argument
os_shell> mv aaa .. 
mv: ..: Invalid argument
```
* rm [path]
```shell
s_shell> ls
./      ../     hw3_submit/     os_grade.txt
os_shell> rm os_grade.txt
os_shell> ls   
./      ../     hw3_submit/
os_shell> rm hw3_submit
rm: hw3_submit: Is a directory
os_shell> rm aaa
rm: aaa: No such file or directory
```

<br/>

# 5. How to compile and execute the program

본 프로젝트의 결과물은  `Makefile`을 포함한다. 해당 `Makefile`은 `all`, `clean` rules를 지원한다. 모든 소스코드들이 컴파일된 후에 프로그램이 생성된다.

* To compile and execute

> make all
