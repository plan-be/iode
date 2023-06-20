
<IODE: tests>
IODE: tests
¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯

The needed executables and DLLs should first be rebuilt by a call to ~cmk.bat~C:
&CO 
    c:/usr/iode_src/tests>> mk.bat
&TX

Before recreating a new IODE installer, the following minimal tests must be succesful:

&EN iode api tests: extensive calls to iode API functions
&EN iodecmd tests: execution of some simple reports 
&EN pyiode tests: extensive tests of the IODE python module in the python version currenly implemented (3.9, 3.10...) 

These tests may be run by a simple call to tests/testall.bat:
&CO 
    c:/usr/iode_src/tests>> testall.bat
&TX

The log file ~c./testall.log~C is generated during the process.


&TI Tests directory structure
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
        ./tests/mk.bat: "make" all libs and exes needed for the tests
        ./tests/testall.bat: run all tests
        
        ./tests/cmd : iodecmd tests
            ./tests/cmd/mk.bat: rebuild iode_c_api.lib and iodecmd.exe
            ./tests/cmd/unittests.bat: run the tests on iodecmd
        
        ./tests/test_c_api: iode_c_api.lib tests
            ./tests/test_c_api/mk.bat: rebuild iode_c_api.lib, test1.exe and test1cpp.exe
            ./tests/test_c_api/unittests.bat: run the tests on iode api            
            
        ./tests/pyiode: pyiode tests
            ./tests/pyiode/mk.bat: rebuild iode.pyd for python 3.9 and 3.10
            ./tests/pyiode/unittests.bat: run the tests on iode.pyd under python 3.9 and 3.10
&TX
>
