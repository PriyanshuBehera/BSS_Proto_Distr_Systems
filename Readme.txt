Following is the attached code for CSC-503 coding assignment
To run the code:
1) Download and unzip the CPP file.
2) Open terminal and compile the code using "g++ myBSSProg_21114077.cpp" -o a1.exe
3) Make input.txt file to provide input
4) ./a1.exe to run the code and see the output in output.txt

Sample Input(Given in the tutorial sheet):

begin process p1
send m1
recv_B p2 m2
end process p1
begin process p2
recv_B p1 m1
send m2
end process p2
begin process p3
recv_B p2 m2
recv_B p1 m1
end process p3

Sample output:

begin process p1
send m1(100)
recv_B p2 m2(100)
recv_A p2 m2(110)
end process p1

begin process p2
recv_B p1 m1(000)
recv_A p1 m1(100)
send m2(110)
end process p2

begin process p3
recv_B p2 m2(000)
recv_B p1 m1(000)
recv_A p1 m1(100)
recv_A p2 m2(110)
end process p3

Input(on which checked on):
begin process p1
recv_B p2 m2
send m1
recv_B p4 m4
recv_B p3 m3
end process p1

begin process p2
send m2
recv_B p1 m1
recv_B p4 m4
recv_B p3 m3
end process p2

begin process p3
recv_B p4 m4
send m3
recv_B p1 m1
recv_B p2 m2
end process p3

begin process p4
send m4
recv_B p2 m2
recv_B p1 m1
recv_B p3 m3
end process p4

output:
begin process p1
recv_B p2 m2 (0000)
recv_A p2 m2 (0100)
send m1 (1100)
recv_B p4 m4 (1100)
recv_A p4 m4 (1101)
recv_B p3 m3 (1101)
recv_A p3 m3 (1111)
end process p1

begin process p2
send m2 (0100)
recv_B p1 m1 (0100)
recv_A p1 m1 (1100)
recv_B p4 m4 (1100)
recv_A p4 m4 (1101)
recv_B p3 m3 (1101)
recv_A p3 m3 (1111)
end process p2

begin process p3
recv_B p4 m4 (0000)
recv_A p4 m4 (0001)
send m3 (0011)
recv_B p1 m1 (0011)
recv_B p2 m2 (0011)
recv_A p2 m2 (0111)
recv_A p1 m1 (1111)
end process p3

begin process p4
send m4 (0001)
recv_B p2 m2 (0001)
recv_A p2 m2 (0101)
recv_B p1 m1 (0101)
recv_A p1 m1 (1101)
recv_B p3 m3 (1101)
recv_A p3 m3 (1111)
end process p4

