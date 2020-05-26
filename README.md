# Mimic-adduser-on-Linux

Write a program in C to mimic the "adduser" command in Linux. This command will add either an ordinary user or a system 
user. It has to handle 2 files "passwd" and "shadow". both these files will be in some folder specified by an environment 
variable PFILE.The program has to take all arguments as command line arguments (Refer man pages)

$ make
$ sudo ./tanu -u 1005 -g 15 -c "Helloworld" test2
