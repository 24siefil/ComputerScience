all	:
		rm -rf DISK1.img
		rm -rf DISK2.img
		rm -rf DISKFull.img
		cp ./disk/DISK1.img .
		cp ./disk/DISK2.img .
		cp ./disk/DISKFull.img .
		clear
		gcc sfs_disk.c sfs_func_hw.c sfs_main.c
		./a.out

clean :
		rm -rf DISK1.img
		rm -rf DISK2.img
		rm -rf DISKFull.img
		rm -rf a.out

# gcc sfs_disk.c sfs_func_hw.c sfs_main.c sfs_func_ext.o

rst :
		rm -rf DISK1.img
		rm -rf DISK2.img
		rm -rf DISKFull.img
		cp ./disk/DISK1.img .
		cp ./disk/DISK2.img .
		cp ./disk/DISKFull.img .

lscd :
		make rst
		gcc sfs_disk.c sfs_func_hw.c sfs_main.c
		clear
		./a.out < ./tester/test_lscd
# cp DISK1.img my_DISK1.img
# make rst
# ./sfs < ./tester/test_lscd > a_cout
# cp DISK1.img a_DISK1.img
# clear
# diff my_cout a_cout
# diff my_DISK1.img a_DISK1.img
# rm -rf my_cout a_cout
# rm -rf my_DISK1.img a_DISK1.img

rmdir :
		make rst
		gcc sfs_disk.c sfs_func_hw.c sfs_main.c
		clear
		./a.out < ./tester/test_rmdir

mv :
		make rst
		gcc sfs_disk.c sfs_func_hw.c sfs_main.c
		clear
		./a.out < ./tester/test_mv

touch :
		make rst
		gcc sfs_disk.c sfs_func_hw.c sfs_main.c
		clear
		./a.out < ./tester/test_touch

rm :
		make rst
		gcc sfs_disk.c sfs_func_hw.c sfs_main.c
		clear
		./a.out < ./tester/test_rm

mkdir :
		make rst
		gcc sfs_disk.c sfs_func_hw.c sfs_main.c
		clear
		./a.out < ./tester/test_mkdir
