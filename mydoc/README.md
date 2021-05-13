install gtest

    sudo apt  install cmake
    git clone https://github.com/google/googletest
    cd googletest
    cmake CMakeLists.txt
    make
    sudo cp ./lib/libgtest*.a  /usr/lib
    sudo cp -r googletest/include/gtest /usr/include

gtest demo

    vi test.cc

        #include<gtest/gtest.h>
        int add(int a,int b){
            return a+b;
        }
        TEST(testCase,test0){
            EXPECT_EQ(add(2,3),5);
        }
        int main(int argc,char **argv){
          testing::InitGoogleTest(&argc,argv);
          return RUN_ALL_TESTS();
        }
    g++ test.cc -lgtest -lpthread
    ./a.out
        [==========] Running 1 test from 1 test suite.
        [----------] Global test environment set-up.
        [----------] 1 test from testCase
        [ RUN      ] testCase.test0
        [       OK ] testCase.test0 (3 ms)
        [----------] 1 test from testCase (3 ms total)

        [----------] Global test environment tear-down
        [==========] 1 test from 1 test suite ran. (3 ms total)
        [  PASSED  ] 1 test.

ctags install and use

	sudo apt  install exuberant-ctags
	ctags --fields=+iaS --extra=+q -R -f ~/.vim/systags /usr/include /usr/local/include
	ctags -R

	vim ~/.vimrc
		set tags=tags
		set tags+=~/.vim/systags

	vim test.cc
		ctrl+]
		ctrl+o
